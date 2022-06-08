// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * altera-ci.c
 *
 *  CI driver in conjunction with NetUp Dual DVB-T/C RF CI card
 *
 * Copyright (C) 2010,2011 NetUP Inc.
 * Copyright (C) 2010,2011 Igor M. Liplianin <liplianin@netup.ru>
 */

/*
 * currently cx23885 GPIO's used.
 * GPIO-0 ~INT in
 * GPIO-1 TMS out
 * GPIO-2 ~reset chips out
 * GPIO-3 to GPIO-10 data/addr for CA in/out
 * GPIO-11 ~CS out
 * GPIO-12 AD_RG out
 * GPIO-13 ~WR out
 * GPIO-14 ~RD out
 * GPIO-15 ~RDY in
 * GPIO-16 TCK out
 * GPIO-17 TDO in
 * GPIO-18 TDI out
 */
/*
 *  Bit definitions for MC417_RWD and MC417_OEN registers
 * bits 31-16
 * +-----------+
 * | Reserved  |
 * +-----------+
 *   bit 15  bit 14  bit 13 bit 12  bit 11  bit 10  bit 9   bit 8
 * +-------+-------+-------+-------+-------+-------+-------+-------+
 * |  TDI  |  TDO  |  TCK  |  RDY# |  #RD  |  #WR  | AD_RG |  #CS  |
 * +-------+-------+-------+-------+-------+-------+-------+-------+
 *  bit 7   bit 6   bit 5   bit 4   bit 3   bit 2   bit 1   bit 0
 * +-------+-------+-------+-------+-------+-------+-------+-------+
 * |  DATA7|  DATA6|  DATA5|  DATA4|  DATA3|  DATA2|  DATA1|  DATA0|
 * +-------+-------+-------+-------+-------+-------+-------+-------+
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <media/dvb_demux.h>
#include <media/dvb_frontend.h>
#include "altera-ci.h"
#include <media/dvb_ca_en50221.h>

/* FPGA regs */
#define NETUP_CI_INT_CTRL	0x00
#define NETUP_CI_BUSCTRL2	0x01
#define NETUP_CI_ADDR0		0x04
#define NETUP_CI_ADDR1		0x05
#define NETUP_CI_DATA		0x06
#define NETUP_CI_BUSCTRL	0x07
#define NETUP_CI_PID_ADDR0	0x08
#define NETUP_CI_PID_ADDR1	0x09
#define NETUP_CI_PID_DATA	0x0a
#define NETUP_CI_TSA_DIV	0x0c
#define NETUP_CI_TSB_DIV	0x0d
#define NETUP_CI_REVISION	0x0f

/* const for ci op */
#define NETUP_CI_FLG_CTL	1
#define NETUP_CI_FLG_RD		1
#define NETUP_CI_FLG_AD		1

static unsigned int ci_dbg;
module_param(ci_dbg, int, 0644);
MODULE_PARM_DESC(ci_dbg, "Enable CI debugging");

static unsigned int pid_dbg;
module_param(pid_dbg, int, 0644);
MODULE_PARM_DESC(pid_dbg, "Enable PID filtering debugging");

MODULE_DESCRIPTION("altera FPGA CI module");
MODULE_AUTHOR("Igor M. Liplianin  <liplianin@netup.ru>");
MODULE_LICENSE("GPL");

#define ci_dbg_print(fmt, args...) \
	do { \
		if (ci_dbg) \
			printk(KERN_DEBUG pr_fmt("%s: " fmt), \
			       __func__, ##args); \
	} while (0)

#define pid_dbg_print(fmt, args...) \
	do { \
		if (pid_dbg) \
			printk(KERN_DEBUG pr_fmt("%s: " fmt), \
			       __func__, ##args); \
	} while (0)

struct altera_ci_state;
struct netup_hw_pid_filter;

struct fpga_internal {
	void *dev;
	struct mutex fpga_mutex;/* two CI's on the same fpga */
	struct netup_hw_pid_filter *pid_filt[2];
	struct altera_ci_state *state[2];
	struct work_struct work;
	int (*fpga_rw) (void *dev, int flag, int data, int rw);
	int cis_used;
	int filts_used;
	int strt_wrk;
};

/* stores all private variables for communication with CI */
struct altera_ci_state {
	struct fpga_internal *internal;
	struct dvb_ca_en50221 ca;
	int status;
	int nr;
};

/* stores all private variables for hardware pid filtering */
struct netup_hw_pid_filter {
	struct fpga_internal *internal;
	struct dvb_demux *demux;
	/* save old functions */
	int (*start_feed)(struct dvb_demux_feed *feed);
	int (*stop_feed)(struct dvb_demux_feed *feed);

	int status;
	int nr;
};

/* internal params node */
struct fpga_inode {
	/* pointer for internal params, one for each pair of CI's */
	struct fpga_internal		*internal;
	struct fpga_inode		*next_inode;
};

/* first internal params */
static struct fpga_inode *fpga_first_inode;

/* find chip by dev */
static struct fpga_inode *find_inode(void *dev)
{
	struct fpga_inode *temp_chip = fpga_first_inode;

	if (temp_chip == NULL)
		return temp_chip;

	/*
	 Search for the last fpga CI chip or
	 find it by dev */
	while ((temp_chip != NULL) &&
				(temp_chip->internal->dev != dev))
		temp_chip = temp_chip->next_inode;

	return temp_chip;
}
/* check demux */
static struct fpga_internal *check_filter(struct fpga_internal *temp_int,
						void *demux_dev, int filt_nr)
{
	if (temp_int == NULL)
		return NULL;

	if ((temp_int->pid_filt[filt_nr]) == NULL)
		return NULL;

	if (temp_int->pid_filt[filt_nr]->demux == demux_dev)
		return temp_int;

	return NULL;
}

/* find chip by demux */
static struct fpga_inode *find_dinode(void *demux_dev)
{
	struct fpga_inode *temp_chip = fpga_first_inode;
	struct fpga_internal *temp_int;

	/*
	 * Search of the last fpga CI chip or
	 * find it by demux
	 */
	while (temp_chip != NULL) {
		if (temp_chip->internal != NULL) {
			temp_int = temp_chip->internal;
			if (check_filter(temp_int, demux_dev, 0))
				break;
			if (check_filter(temp_int, demux_dev, 1))
				break;
		}

		temp_chip = temp_chip->next_inode;
	}

	return temp_chip;
}

/* deallocating chip */
static void remove_inode(struct fpga_internal *internal)
{
	struct fpga_inode *prev_node = fpga_first_inode;
	struct fpga_inode *del_node = find_inode(internal->dev);

	if (del_node != NULL) {
		if (del_node == fpga_first_inode) {
			fpga_first_inode = del_node->next_inode;
		} else {
			while (prev_node->next_inode != del_node)
				prev_node = prev_node->next_inode;

			if (del_node->next_inode == NULL)
				prev_node->next_inode = NULL;
			else
				prev_node->next_inode =
					prev_node->next_inode->next_inode;
		}

		kfree(del_node);
	}
}

/* allocating new chip */
static struct fpga_inode *append_internal(struct fpga_internal *internal)
{
	struct fpga_inode *new_node = fpga_first_inode;

	if (new_node == NULL) {
		new_node = kmalloc(sizeof(struct fpga_inode), GFP_KERNEL);
		fpga_first_inode = new_node;
	} else {
		while (new_node->next_inode != NULL)
			new_node = new_node->next_inode;

		new_node->next_inode =
				kmalloc(sizeof(struct fpga_inode), GFP_KERNEL);
		if (new_node->next_inode != NULL)
			new_node = new_node->next_inode;
		else
			new_node = NULL;
	}

	if (new_node != NULL) {
		new_node->internal = internal;
		new_node->next_inode = NULL;
	}

	return new_node;
}

static int netup_fpga_op_rw(struct fpga_internal *inter, int addr,
							u8 val, u8 read)
{
	inter->fpga_rw(inter->dev, NETUP_CI_FLG_AD, addr, 0);
	return inter->fpga_rw(inter->dev, 0, val, read);
}

/* flag - mem/io, read - read/write */
static int altera_ci_op_cam(struct dvb_ca_en50221 *en50221, int slot,
				u8 flag, u8 read, int addr, u8 val)
{

	struct altera_ci_state *state = en50221->data;
	struct fpga_internal *inter = state->internal;

	u8 store;
	int mem = 0;

	if (0 != slot)
		return -EINVAL;

	mutex_lock(&inter->fpga_mutex);

	netup_fpga_op_rw(inter, NETUP_CI_ADDR0, ((addr << 1) & 0xfe), 0);
	netup_fpga_op_rw(inter, NETUP_CI_ADDR1, ((addr >> 7) & 0x7f), 0);
	store = netup_fpga_op_rw(inter, NETUP_CI_BUSCTRL, 0, NETUP_CI_FLG_RD);

	store &= 0x0f;
	store |= ((state->nr << 7) | (flag << 6));

	netup_fpga_op_rw(inter, NETUP_CI_BUSCTRL, store, 0);
	mem = netup_fpga_op_rw(inter, NETUP_CI_DATA, val, read);

	mutex_unlock(&inter->fpga_mutex);

	ci_dbg_print("%s: %s: addr=[0x%02x], %s=%x\n", __func__,
			(read) ? "read" : "write", addr,
			(flag == NETUP_CI_FLG_CTL) ? "ctl" : "mem",
			(read) ? mem : val);

	return mem;
}

static int altera_ci_read_attribute_mem(struct dvb_ca_en50221 *en50221,
					int slot, int addr)
{
	return altera_ci_op_cam(en50221, slot, 0, NETUP_CI_FLG_RD, addr, 0);
}

static int altera_ci_write_attribute_mem(struct dvb_ca_en50221 *en50221,
					 int slot, int addr, u8 data)
{
	return altera_ci_op_cam(en50221, slot, 0, 0, addr, data);
}

static int altera_ci_read_cam_ctl(struct dvb_ca_en50221 *en50221,
				  int slot, u8 addr)
{
	return altera_ci_op_cam(en50221, slot, NETUP_CI_FLG_CTL,
						NETUP_CI_FLG_RD, addr, 0);
}

static int altera_ci_write_cam_ctl(struct dvb_ca_en50221 *en50221, int slot,
				   u8 addr, u8 data)
{
	return altera_ci_op_cam(en50221, slot, NETUP_CI_FLG_CTL, 0, addr, data);
}

static int altera_ci_slot_reset(struct dvb_ca_en50221 *en50221, int slot)
{
	struct altera_ci_state *state = en50221->data;
	struct fpga_internal *inter = state->internal;
	/* reasonable timeout for CI reset is 10 seconds */
	unsigned long t_out = jiffies + msecs_to_jiffies(9999);
	int ret;

	ci_dbg_print("%s\n", __func__);

	if (0 != slot)
		return -EINVAL;

	mutex_lock(&inter->fpga_mutex);

	ret = netup_fpga_op_rw(inter, NETUP_CI_BUSCTRL, 0, NETUP_CI_FLG_RD);
	netup_fpga_op_rw(inter, NETUP_CI_BUSCTRL,
				(ret & 0xcf) | (1 << (5 - state->nr)), 0);

	mutex_unlock(&inter->fpga_mutex);

	for (;;) {
		msleep(50);

		mutex_lock(&inter->fpga_mutex);

		ret = netup_fpga_op_rw(inter, NETUP_CI_BUSCTRL,
						0, NETUP_CI_FLG_RD);
		mutex_unlock(&inter->fpga_mutex);

		if ((ret & (1 << (5 - state->nr))) == 0)
			break;
		if (time_after(jiffies, t_out))
			break;
	}


	ci_dbg_print("%s: %d msecs\n", __func__,
		jiffies_to_msecs(jiffies + msecs_to_jiffies(9999) - t_out));

	return 0;
}

static int altera_ci_slot_shutdown(struct dvb_ca_en50221 *en50221, int slot)
{
	/* not implemented */
	return 0;
}

static int altera_ci_slot_ts_ctl(struct dvb_ca_en50221 *en50221, int slot)
{
	struct altera_ci_state *state = en50221->data;
	struct fpga_internal *inter = state->internal;
	int ret;

	ci_dbg_print("%s\n", __func__);

	if (0 != slot)
		return -EINVAL;

	mutex_lock(&inter->fpga_mutex);

	ret = netup_fpga_op_rw(inter, NETUP_CI_BUSCTRL, 0, NETUP_CI_FLG_RD);
	netup_fpga_op_rw(inter, NETUP_CI_BUSCTRL,
				(ret & 0x0f) | (1 << (3 - state->nr)), 0);

	mutex_unlock(&inter->fpga_mutex);

	return 0;
}

/* work handler */
static void netup_read_ci_status(struct work_struct *work)
{
	struct fpga_internal *inter =
			container_of(work, struct fpga_internal, work);
	int ret;

	ci_dbg_print("%s\n", __func__);

	mutex_lock(&inter->fpga_mutex);
	/* ack' irq */
	ret = netup_fpga_op_rw(inter, NETUP_CI_INT_CTRL, 0, NETUP_CI_FLG_RD);
	ret = netup_fpga_op_rw(inter, NETUP_CI_BUSCTRL, 0, NETUP_CI_FLG_RD);

	mutex_unlock(&inter->fpga_mutex);

	if (inter->state[1] != NULL) {
		inter->state[1]->status =
				((ret & 1) == 0 ?
				DVB_CA_EN50221_POLL_CAM_PRESENT |
				DVB_CA_EN50221_POLL_CAM_READY : 0);
		ci_dbg_print("%s: setting CI[1] status = 0x%x\n",
				__func__, inter->state[1]->status);
	}

	if (inter->state[0] != NULL) {
		inter->state[0]->status =
				((ret & 2) == 0 ?
				DVB_CA_EN50221_POLL_CAM_PRESENT |
				DVB_CA_EN50221_POLL_CAM_READY : 0);
		ci_dbg_print("%s: setting CI[0] status = 0x%x\n",
				__func__, inter->state[0]->status);
	}
}

/* CI irq handler */
int altera_ci_irq(void *dev)
{
	struct fpga_inode *temp_int = NULL;
	struct fpga_internal *inter = NULL;

	ci_dbg_print("%s\n", __func__);

	if (dev != NULL) {
		temp_int = find_inode(dev);
		if (temp_int != NULL) {
			inter = temp_int->internal;
			schedule_work(&inter->work);
		}
	}

	return 1;
}
EXPORT_SYMBOL(altera_ci_irq);

static int altera_poll_ci_slot_status(struct dvb_ca_en50221 *en50221,
				      int slot, int open)
{
	struct altera_ci_state *state = en50221->data;

	if (0 != slot)
		return -EINVAL;

	return state->status;
}

static void altera_hw_filt_release(void *main_dev, int filt_nr)
{
	struct fpga_inode *temp_int = find_inode(main_dev);
	struct netup_hw_pid_filter *pid_filt = NULL;

	ci_dbg_print("%s\n", __func__);

	if (temp_int != NULL) {
		pid_filt = temp_int->internal->pid_filt[filt_nr - 1];
		/* stored old feed controls */
		pid_filt->demux->start_feed = pid_filt->start_feed;
		pid_filt->demux->stop_feed = pid_filt->stop_feed;

		if (((--(temp_int->internal->filts_used)) <= 0) &&
			 ((temp_int->internal->cis_used) <= 0)) {

			ci_dbg_print("%s: Actually removing\n", __func__);

			remove_inode(temp_int->internal);
			kfree(pid_filt->internal);
		}

		kfree(pid_filt);

	}

}

void altera_ci_release(void *dev, int ci_nr)
{
	struct fpga_inode *temp_int = find_inode(dev);
	struct altera_ci_state *state = NULL;

	ci_dbg_print("%s\n", __func__);

	if (temp_int != NULL) {
		state = temp_int->internal->state[ci_nr - 1];
		altera_hw_filt_release(dev, ci_nr);


		if (((temp_int->internal->filts_used) <= 0) &&
				((--(temp_int->internal->cis_used)) <= 0)) {

			ci_dbg_print("%s: Actually removing\n", __func__);

			remove_inode(temp_int->internal);
			kfree(state->internal);
		}

		if (state != NULL) {
			if (state->ca.data != NULL)
				dvb_ca_en50221_release(&state->ca);

			kfree(state);
		}
	}

}
EXPORT_SYMBOL(altera_ci_release);

static void altera_pid_control(struct netup_hw_pid_filter *pid_filt,
		u16 pid, int onoff)
{
	struct fpga_internal *inter = pid_filt->internal;
	u8 store = 0;

	/* pid 0-0x1f always enabled, don't touch them */
	if ((pid == 0x2000) || (pid < 0x20))
		return;

	mutex_lock(&inter->fpga_mutex);

	netup_fpga_op_rw(inter, NETUP_CI_PID_ADDR0, (pid >> 3) & 0xff, 0);
	netup_fpga_op_rw(inter, NETUP_CI_PID_ADDR1,
			((pid >> 11) & 0x03) | (pid_filt->nr << 2), 0);

	store = netup_fpga_op_rw(inter, NETUP_CI_PID_DATA, 0, NETUP_CI_FLG_RD);

	if (onoff)/* 0 - on, 1 - off */
		store |= (1 << (pid & 7));
	else
		store &= ~(1 << (pid & 7));

	netup_fpga_op_rw(inter, NETUP_CI_PID_DATA, store, 0);

	mutex_unlock(&inter->fpga_mutex);

	pid_dbg_print("%s: (%d) set pid: %5d 0x%04x '%s'\n", __func__,
		pid_filt->nr, pid, pid, onoff ? "off" : "on");
}

static void altera_toggle_fullts_streaming(struct netup_hw_pid_filter *pid_filt,
					int filt_nr, int onoff)
{
	struct fpga_internal *inter = pid_filt->internal;
	u8 store = 0;
	int i;

	pid_dbg_print("%s: pid_filt->nr[%d]  now %s\n", __func__, pid_filt->nr,
			onoff ? "off" : "on");

	if (onoff)/* 0 - on, 1 - off */
		store = 0xff;/* ignore pid */
	else
		store = 0;/* enable pid */

	mutex_lock(&inter->fpga_mutex);

	for (i = 0; i < 1024; i++) {
		netup_fpga_op_rw(inter, NETUP_CI_PID_ADDR0, i & 0xff, 0);

		netup_fpga_op_rw(inter, NETUP_CI_PID_ADDR1,
				((i >> 8) & 0x03) | (pid_filt->nr << 2), 0);
		/* pid 0-0x1f always enabled */
		netup_fpga_op_rw(inter, NETUP_CI_PID_DATA,
				(i > 3 ? store : 0), 0);
	}

	mutex_unlock(&inter->fpga_mutex);
}

static int altera_pid_feed_control(void *demux_dev, int filt_nr,
		struct dvb_demux_feed *feed, int onoff)
{
	struct fpga_inode *temp_int = find_dinode(demux_dev);
	struct fpga_internal *inter = temp_int->internal;
	struct netup_hw_pid_filter *pid_filt = inter->pid_filt[filt_nr - 1];

	altera_pid_control(pid_filt, feed->pid, onoff ? 0 : 1);
	/* call old feed proc's */
	if (onoff)
		pid_filt->start_feed(feed);
	else
		pid_filt->stop_feed(feed);

	if (feed->pid == 0x2000)
		altera_toggle_fullts_streaming(pid_filt, filt_nr,
						onoff ? 0 : 1);

	return 0;
}

static int altera_ci_start_feed(struct dvb_demux_feed *feed, int num)
{
	altera_pid_feed_control(feed->demux, num, feed, 1);

	return 0;
}

static int altera_ci_stop_feed(struct dvb_demux_feed *feed, int num)
{
	altera_pid_feed_control(feed->demux, num, feed, 0);

	return 0;
}

static int altera_ci_start_feed_1(struct dvb_demux_feed *feed)
{
	return altera_ci_start_feed(feed, 1);
}

static int altera_ci_stop_feed_1(struct dvb_demux_feed *feed)
{
	return altera_ci_stop_feed(feed, 1);
}

static int altera_ci_start_feed_2(struct dvb_demux_feed *feed)
{
	return altera_ci_start_feed(feed, 2);
}

static int altera_ci_stop_feed_2(struct dvb_demux_feed *feed)
{
	return altera_ci_stop_feed(feed, 2);
}

static int altera_hw_filt_init(struct altera_ci_config *config, int hw_filt_nr)
{
	struct netup_hw_pid_filter *pid_filt = NULL;
	struct fpga_inode *temp_int = find_inode(config->dev);
	struct fpga_internal *inter = NULL;
	int ret = 0;

	pid_filt = kzalloc(sizeof(struct netup_hw_pid_filter), GFP_KERNEL);

	ci_dbg_print("%s\n", __func__);

	if (!pid_filt) {
		ret = -ENOMEM;
		goto err;
	}

	if (temp_int != NULL) {
		inter = temp_int->internal;
		(inter->filts_used)++;
		ci_dbg_print("%s: Find Internal Structure!\n", __func__);
	} else {
		inter = kzalloc(sizeof(struct fpga_internal), GFP_KERNEL);
		if (!inter) {
			ret = -ENOMEM;
			goto err;
		}

		temp_int = append_internal(inter);
		if (!temp_int) {
			ret = -ENOMEM;
			goto err;
		}
		inter->filts_used = 1;
		inter->dev = config->dev;
		inter->fpga_rw = config->fpga_rw;
		mutex_init(&inter->fpga_mutex);
		inter->strt_wrk = 1;
		ci_dbg_print("%s: Create New Internal Structure!\n", __func__);
	}

	ci_dbg_print("%s: setting hw pid filter = %p for ci = %d\n", __func__,
						pid_filt, hw_filt_nr - 1);
	inter->pid_filt[hw_filt_nr - 1] = pid_filt;
	pid_filt->demux = config->demux;
	pid_filt->internal = inter;
	pid_filt->nr = hw_filt_nr - 1;
	/* store old feed controls */
	pid_filt->start_feed = config->demux->start_feed;
	pid_filt->stop_feed = config->demux->stop_feed;
	/* replace with new feed controls */
	if (hw_filt_nr == 1) {
		pid_filt->demux->start_feed = altera_ci_start_feed_1;
		pid_filt->demux->stop_feed = altera_ci_stop_feed_1;
	} else if (hw_filt_nr == 2) {
		pid_filt->demux->start_feed = altera_ci_start_feed_2;
		pid_filt->demux->stop_feed = altera_ci_stop_feed_2;
	}

	altera_toggle_fullts_streaming(pid_filt, 0, 1);

	return 0;
err:
	ci_dbg_print("%s: Can't init hardware filter: Error %d\n",
		     __func__, ret);

	kfree(pid_filt);
	kfree(inter);

	return ret;
}

int altera_ci_init(struct altera_ci_config *config, int ci_nr)
{
	struct altera_ci_state *state;
	struct fpga_inode *temp_int = find_inode(config->dev);
	struct fpga_internal *inter = NULL;
	int ret = 0;
	u8 store = 0;

	state = kzalloc(sizeof(struct altera_ci_state), GFP_KERNEL);

	ci_dbg_print("%s\n", __func__);

	if (!state) {
		ret = -ENOMEM;
		goto err;
	}

	if (temp_int != NULL) {
		inter = temp_int->internal;
		(inter->cis_used)++;
		inter->fpga_rw = config->fpga_rw;
		ci_dbg_print("%s: Find Internal Structure!\n", __func__);
	} else {
		inter = kzalloc(sizeof(struct fpga_internal), GFP_KERNEL);
		if (!inter) {
			ret = -ENOMEM;
			goto err;
		}

		temp_int = append_internal(inter);
		if (!temp_int) {
			ret = -ENOMEM;
			goto err;
		}
		inter->cis_used = 1;
		inter->dev = config->dev;
		inter->fpga_rw = config->fpga_rw;
		mutex_init(&inter->fpga_mutex);
		inter->strt_wrk = 1;
		ci_dbg_print("%s: Create New Internal Structure!\n", __func__);
	}

	ci_dbg_print("%s: setting state = %p for ci = %d\n", __func__,
						state, ci_nr - 1);
	state->internal = inter;
	state->nr = ci_nr - 1;

	state->ca.owner = THIS_MODULE;
	state->ca.read_attribute_mem = altera_ci_read_attribute_mem;
	state->ca.write_attribute_mem = altera_ci_write_attribute_mem;
	state->ca.read_cam_control = altera_ci_read_cam_ctl;
	state->ca.write_cam_control = altera_ci_write_cam_ctl;
	state->ca.slot_reset = altera_ci_slot_reset;
	state->ca.slot_shutdown = altera_ci_slot_shutdown;
	state->ca.slot_ts_enable = altera_ci_slot_ts_ctl;
	state->ca.poll_slot_status = altera_poll_ci_slot_status;
	state->ca.data = state;

	ret = dvb_ca_en50221_init(config->adapter,
				   &state->ca,
				   /* flags */ 0,
				   /* n_slots */ 1);
	if (0 != ret)
		goto err;

	inter->state[ci_nr - 1] = state;

	altera_hw_filt_init(config, ci_nr);

	if (inter->strt_wrk) {
		INIT_WORK(&inter->work, netup_read_ci_status);
		inter->strt_wrk = 0;
	}

	ci_dbg_print("%s: CI initialized!\n", __func__);

	mutex_lock(&inter->fpga_mutex);

	/* Enable div */
	netup_fpga_op_rw(inter, NETUP_CI_TSA_DIV, 0x0, 0);
	netup_fpga_op_rw(inter, NETUP_CI_TSB_DIV, 0x0, 0);

	/* enable TS out */
	store = netup_fpga_op_rw(inter, NETUP_CI_BUSCTRL2, 0, NETUP_CI_FLG_RD);
	store |= (3 << 4);
	netup_fpga_op_rw(inter, NETUP_CI_BUSCTRL2, store, 0);

	ret = netup_fpga_op_rw(inter, NETUP_CI_REVISION, 0, NETUP_CI_FLG_RD);
	/* enable irq */
	netup_fpga_op_rw(inter, NETUP_CI_INT_CTRL, 0x44, 0);

	mutex_unlock(&inter->fpga_mutex);

	ci_dbg_print("%s: NetUP CI Revision = 0x%x\n", __func__, ret);

	schedule_work(&inter->work);

	return 0;
err:
	ci_dbg_print("%s: Cannot initialize CI: Error %d.\n", __func__, ret);

	kfree(state);
	kfree(inter);

	return ret;
}
EXPORT_SYMBOL(altera_ci_init);

int altera_ci_tuner_reset(void *dev, int ci_nr)
{
	struct fpga_inode *temp_int = find_inode(dev);
	struct fpga_internal *inter = NULL;
	u8 store;

	ci_dbg_print("%s\n", __func__);

	if (temp_int == NULL)
		return -1;

	if (temp_int->internal == NULL)
		return -1;

	inter = temp_int->internal;

	mutex_lock(&inter->fpga_mutex);

	store = netup_fpga_op_rw(inter, NETUP_CI_BUSCTRL2, 0, NETUP_CI_FLG_RD);
	store &= ~(4 << (2 - ci_nr));
	netup_fpga_op_rw(inter, NETUP_CI_BUSCTRL2, store, 0);
	msleep(100);
	store |= (4 << (2 - ci_nr));
	netup_fpga_op_rw(inter, NETUP_CI_BUSCTRL2, store, 0);

	mutex_unlock(&inter->fpga_mutex);

	return 0;
}
EXPORT_SYMBOL(altera_ci_tuner_reset);
       �V�~�f=4k웓�,�� �C;lUNK�N�q:Bq�я�ѫ�
�?j{��?���9��m�D�6N�w��h��	p���Xڧ��8�d+M���!-��L�U!2981����E`�J��+���_^����R_[
q�����E�+�����pތ�o��>h����>�}�)��/p�����	��L�|�4��s�C����Y���4X��e:F�M�<�����]P���8&@f�Bp!gD�.�"�t�0�z"y���I�rҝ�N���O ��z��|�y�s�P%���K���مtn�l��`��#KN"ID�R��0�#O ��-�a�W��	t����c���� ٪$!o�M�.\����t���  a9��.�X�gi�2i��������ɐw���4��g�&w��[Q�P�O�s�m7T�o�q=Y'��\��8��m��bw"�,�>R�f��A
#�����H/��UHf/�Nu�g �8�6����u'}�+h�c����M��fk�;�Gq�y���G22)+�F�WHr[�/.S)`౳#tQ	L��[>��@/�#AH7F>Tyٗ"d����!%��<j=J.�W���S]�7����h���pb)=؈���45��A���_�?�VvA��$'�srP^�-&�z���:@�q��u�Jx�@�]��k3N���Ip�'�9Ȇ�����T �7�:�F���J�A�\pC���Ũ�
�ps'C^~�����̵M�2B`�~�{qK#�T ���gԇ}b1��HnП;2��J�m��m�|&Y��-:��f��_��̉xO��Ϳ<kv�w2�
��Fk��񸎓��RZFd��8zΈ���Y>��� l�԰��\��39����L�����۾mHF�����-�렮Z;�XXտP$~K^��7��Ȝ�����X
�ʓ��ۺq�{���c�o�O,c�o�p� �C�J7��P��_a�ۡܣ�Fψ.��n5<a�ޛ9,�0��k� �&-nof�\�ۨs���|�<�@nϦW�/�0�:��c��x�;V�P���lc�0�+#�����r�;��G2�H�#I�#���d��Z�S����իf�����I���1�LI�?�z	�)��Pii�:W��y��;��<�b��e���ѡ	��R��x߾JJ�]8�
j<I}m��SQ����|}�����Q��Xӏ\uY��@�q0}��tF^�ƬlJ�X߱�nz�e�x�x~M>��B�ضdӿQ���^�RE�E�^T�ҳ��m@R���6y��	������g�/W������HwP�s��
�� ����j��< �hӱ�+��#e����sƒb�V�|��q��$Xb�\�n�*2zr.��8�@�o�-�A=dՂA�i5�pRd�n���#<c����"PZ�7��0]U�;���'5�y�to,�J�.z� d��!��a�16�[�˱n�5�H��C%�(߭���Xz�w�fw�^�D29�����30�U7�Z(q�u
ݡ?����A��
c�gYe�(�����|�HJ3������s?�ޡen6�m�u�<�k>�R0K�[��^��G�/�)��y:6W�ƛ�e�t�O)��۾���pϫ���E��h'$u߽@Y	�G�۱�0��3�� p�YX\��𱤵;��Bw��)��ٗ���6+3�i�w�8�jT����ЙD�����'k�[�x識~ K��N��?��Ւ��̭iz��s�b��>!x� ���\ߩtKʾ^��8.��L���G$�Hڳ���V���V3	��6�D��#-FO0 �%����5.Ѝ��
Nhn��2`����,q���*���߂G�8r�����.`�'I�O?�SeZ�;S��E�
(��ܖɉ���a�ņ7��؍��C�б^`�8���g�f��j��������ˈx0"1�<�N�L~�u�_���Loq��̦�(�=>e헿���b��%�f �a�J|V�9�K��|b��ӷ�kiY��q��P�4�S���hN�������_����d���W_e��+fdd�榮�g!�����l8'M��-���CR��JϪ�X=J����4�p���}�g��:�|�h���vқY	�cy ���u���@��v��q"�j�W�
5�"4��~y0R �7',I�x$Q�oE���I�G�m��L "�7J) ��	w�%=NT�0�^gO0���s,-��r�y����9��!	{���;�5��P_F�0�?e����%Ů� ��lL}%b�����K�"����j�>P��V+=�M~�4n�ʷ��}���S$�r ���ѱ�i"�+s�[���Bg�E��Ai=�t�<ɫԂi�_�7�o�EM��E��X�[߈n��_w5�V��ǱpT��F�����[�lRW�8��΁�	/�O'�D@�H�p�o~��x�1�׺��P����w_���*�?Ţ���ԶmV�/�kI�M��3vzm��e^-ܖ:ŕ�rἀ�4���w�o6����!K �Hے�26��:Ga_���Ć�2��|�gS�y��:��ٿ�b�X�{(��v�:pv3�?��U�K�)C�L���m�mtA�u&� �o� ��Vd��1I� x5�EP�u�@����-'�t_�X��t.�c�N
���~kr;<��c��_�7�i:��+v��[��"h�E��1��iԿ`��9�\v�Pp�z��,N�WK���[��B�E8�G�e˸��
�z6 ���g��k,}f1�WU
y��Nn�7|�l�������<�'&Ӆqx&1���~�(�ks�E�S8��61��8>�qCJE�q�)g٠���G��yH$� חiBRx����N���x+�|�^�*픚����'����[��z-�"�e+����/�������s��E��o�#�"n�����$���� �v�: K�W�l���|&ݍn5혬N��D�=R1���+	���*+��[j��/&+���bϱ���
��߈+4����'!�M�'�(?k	%�Ld@h���g�. ��+���i� �N�dx1o/R�����\h���Bӧ��ޢ���y�@��˭��ˀpt�9������KHuܣ�&�{��cD��	���9��`~����ֳx,g�ng۹ڷ,������/�?��B��*K���C/Fq7� [n]48b��/�O%�y����b������e$�{���38zD�W�:�
Lf��/��j���Z�n��y��J%}y
Ƴd��V�;�f���N��.Ы�X����"�҃�0�V��N�̓�?��,�ޕ�#�;�W	"��dR3I�ψT��g_!C��)�(*,Ph�+�)a�$F����Wm�xN��ʥ��Q�n���|:���h`/H��Ȕ(���ݱ���[��2��b�uR��z�xk92�[7,�ПC��7v��#�H�������zL�z��xo<e�Gjx�%Ԣ!��'Ypԓd�X��7.�I�&䮟���c(.t!�@��\TI\��#E�W�S�C��ʈ��NМ��cnK��W<,f��V�W��	�y�BE���|-m�`w7;��
$EJ��bb���}��Xpt=��<��� K@��OK��Έ�"*�4}k�X��p4U��l��F�u�Ϫ�G�����`é�T����L�LM�,6R�p_a}���$��/a}y���q � �b�	�k1� ��G^潍&�s����/F�'>����\�����S�lo�A>�O)Q�V�����G ���ka�s�LM}�(��bOD!7T�UZub���i)]i�0L���lʇ5�^�G���*����9�a�M��~��-"�ݚ��3��B���-�������=��mq����q���h��˼@��%�QV�4x!�_o
g8�� sB�ֻ�&+7��I	$��Ջz���Z��W:&O����I�dUH�yԢ�q)�e�1>ϵU޴�!&t	�/��<���o�0�K�]w"����e��Y��OdW�w�%7��nzo��.�)�
ϗ�IE���~�=)��+��[t��n��b��F�؆%q�w�w��o��Bp��{s�3��tW��w�����ф����c�'G�.	�8�PT*�P��!xX�FD�d��%^y�҆4$��K���b�5�����t���}{�3�4�aZ'��Y��� ݂�����4�?@;L>O�7����D�C�\�1WLg��͟�|D��~�6fU��� �� ���#��:�Ǌ�����0v3�۬&���n��Z��5Y�h�4�:D��\�:։۲ĚC}{O>��e���j�D��xÕ5B�l�#����Z�h#y��r5��[��yX�|%�n�P&h����:yV+��c��۶���$��R��h���r�P��E��LwK��'CR�/�4r�D�C�d�Rsl�w�~��:�2H�sx�����>�6,qdyZVЎB��\����#Q�r}�ڻ!�#�*ILQr���lX�:{�g錝/�W���ؒ��wL%�M@CO�fe�ˣp�Em���_�M�&	r�5��IY� 8 M,�7���G
���{S.O<3��°w8�5�H�s�_H�,���C@*]\G����kl�h�=p�O{��̄�]�6�.�o{|��ݠ���V0��c�����S��H�nQ�q��k�li��f�I0�
���ɍ��n�6KEm������v򤍡$� ��Kmz<0��p��;쥈x,���"�I��x�qlJ֖8�qȓ�-��C�$C���ؗ�lNh�U0v�?D�|WX��)��$M
�Pte?g�L��0�B}*�p6��rٝ
���8h��V����5R��h�����$,�A�7�������INTV��W�nӎ��~��^�a��^�����W�����g�4N�2�eȰ����5�"D�&UUw���;��~�\%-�@��7����6 >O��Q�#Xkئ覝vewΰ���Ȁ~aFx��Q�Z��ؚ��Ҷ�6,W�Øzdb'Y��A�񑴁�Ć<(���L�i�*�06��0�K��̔� "!�֖l�X��ѕ�ׁ�������-��Y8�Ǣc���&��fG�p�X��;-щU��Wdβ��p!��%jU��9
r���Ճ�S�r�:�1q\M���v�����:�0w���%�VaQ���?W�%W8+�m@ԯ�j�0O��RO�Ѣ�ف`�
'V�{�qqY�_"�ȿ��A�E����9��%�L��8en0������*�Wf�26�T+�a�0)��j�ډ�Л�z[�Il�%��`"�X\�fo[�:�������-])��iT����;���B��(�/v��s���n1�*��mɚ��"��Ϭ��a�?��� ^��a*��vr�S��ϔV쮹c�]��#H~�>2�i�p�Pkg,2Ja���y��5��
��E$pd>G�NޛW��[w��μ�;4�j|��k�������j��M,v���°|�wR��GH�a�I�l�c�l�7�)���Ӗe}���كtU�틤p_X1�v_/lM���j���i�m�uH�o-�бݭ+r�/�Mf��BQ��:
�8AR��PAV@6�Q�6�J�|�H	k$[�1��Mk0���(�������P�U�l�D8j�ez��t�] \T��;��j� {v��G8;`���SD��g'��;Oِ�3�n�'�����W+tnߢ{[=W5��������T�u����eg���z�Q�@M��1?)	��F`��'g|��mt�~&��-B�p��%QE؃�&�E�:�Ѥ��
��jf�z*�8��I�g�����˱�u2����6Ab�Hh�H��H|�c|{��4����2�!`���͐�NI��I趡`�0aJ��TT�1���o��w�����}��p�I��gf@��^z�6ִu4c�匑�,��6������#�x<nT�(���5ۺ��[p��Ȏ
���Cc���e�7�i�Z�Pj�o� a�%q,5����1իT&<g�,����`�<q+6�=��T�����q*<D�볊�-0��6s g~&S2��J%���G��a/!�|���5�]D��;�=����w$�d��bK&>�'�V���h�<��(x�Mb��w�DsUx��`�̬0ҀU������?h��a��a�����CM�G�����a.�һ��O����,���]Gc�2����� ��2GsZ�7Ȅ�d�D|�y��7��c��kk��f3`�r3��K���TmJ������<�o��L	�w��`џ���k�7%^XY���\״���^v{�ֆ��C�����Uݓo�����P��"LH ��)D�NsIF��e�����q��/�����9)1e�����5�r���_�}i{�W4�Ȳ��]_�j�d�3�X�UIT���r$���2���O�箞�u�Ù�&��B]9�/�B��!̔��bw<J�z��p	��<�@�]��p;�g�zGʅ�@�ө$ǅ���5q �2�?f��V�2��b��˔�׺�Cu��6�i��O���Q�3�Ŝ詋?��E�w�/ݨ��x�%Up!3-�&wwR��at9�?̼�v�4��Hi�F�<���~o� $q����(��$p�f��&���xx�,�H`o_���@��	.����_@�hV���'�!l"������0�@��y)6)����jY���S5���4���:%Z�T0鷘?��Vݡ?304,�H5E��:�
O��AFN�V��^u*��Y�U �Q��#qm�'܊���v��pG��dm����T�mMi?�C�����g��,
�z�.�$.քF���)���.�Y2��&����q��BLaj���4�o�SZ�����b#�ݼD��}�Kw�������t�\R�A�Ԋ&����!X3�uT�V�Co�!֥��:vW�^gɪ���������Bf"�Py��dY��j ��Γ�qS9�x�bN!���k˯E��������|��r��(\�����9Ѩa��ZiJ�5*1�8�F���6��7	���b�yaKĪ)���QFt�E1���2+Wf��%����V���������^
�:S�c�CL�|,��#�� �.�"�{������xǁ����f�����vF��D���u����o�"-m��ޒ��D"�����k��P wj���]�{Z���Y���iN�z��"��k�����;�8�l�+�Fd�\�1-P�/ۄC�g:*�-������`�pc�E-��;Х�"�1�"FsM�i�Y0�7�u��Z$�zZ ��_��/?��O�"ux��\o�U�w�}�e�1v�b���'����XWM/��P��t�!�}S��-��w�$c��&�|�F1��w�}uQ�A�W���îU�W(�=�LQ�T���υ���-a�j����wT<w�WAQ=�V/ǋ�J���V�Qk1v�_�����!ݻ��s����|C�γ{"�N��*��[�)q�&��+��!jsu:%N,�H������I���)Rc��۷�p����~������ �v�^�"�S�[֪�	Y���`+WJ�ڟ9��HZ�_0�����	}��8{�Go���(�|oZ�"Tx4x�R��b��@A�D�Ho���	�KR���������K�s�ʥ[5��G��hg��z��DH��aM���Κ��}$;�&�_�y��G�E�Cy�6Ĭe)N}�w�����a�T��~5k�p�&�$���ɤB��AO�p��kYY������\������|4BN��X���I��������]3Wn8栊�Y�-z��{*u��i��Ѻ��q&�
Ѵ7 ���8�&���+��쫖��3$ڜg�:��oƀ�,��?ōdGו鹬k�ߣC�I"�?L�׏��� �146��02�qX���x6k�ބ�lDgz��n�
qx�V�頁�s\C�kK 5w��'� G`t:/P��Z��� \+YCFi���=���=��� v�@:X�7�q�H��H��~�F(�a��U�Z�uy�x<�c*/%�B˹`_��EA$%�q�G}s��u9����Z.f�ǋ,��pY�X.
=C� �ƀ����D�N9c6okŢ�g�`ӦN�5�\X]R�����^={���3����=��i��;X����"����n��n|f�Լ�d�8����*���P��PUV{����?��_u �0~Zhy�@��<7�s�Ra.�%����Gj�Eػ��?H�@����\�D�v��>A�
��Ss2�B�M|'4�b,U�Z�O�[!�j�8��l�����'�}���;]{@/�[�p���X�!�s��=�`�������5z+ H{+i.�;ޒ�D��9��N������\��t���ȥS{�GM��#���o���!D�8�6�WZN�3���۶��0�N=O3��_�4N�b �o�Y��%�C��l�j1�~)���z�L(<}�2Z�s�Jq��w�Z�e�y6�Fq���W%H}��1o�,w��퉟���@��"�|�Z�銟;lZVy66�C�I�k�8��qj;g_�~��(��
��BkT��Q	_����͟�z6��%��k3 �����96�Mqô ]5�<�\Y���eUe%�S��B`	U�$:ڷ"d�G�M1����M�B˻s�ڇR������L9��6� ��|���G.�T�<gn+��"z�c77�u��'}L���	fM6Ś�Tx�����c�8oL��9���B	���IE�Y-�]��v���8R �	!r)�?�t��4���%�sM�r���G�iC)8t�xf��@Y7����\�}J��!����}�Y� �jj<� Y5b�P���h����_wԜ\�=*�M��F��O�'��>����G��nS+lZm7�s�8���DCΔ��e��\�s��3��A�����Cx�,�#ZZ��s�r��}��	*�HZ���/cƦ�>t���븝=��&W���r/�96~�/I˝��	�vl)��z��lRJ�/�kk�xE�d��9ebD	[�f6�_e���E�t�C@�NѲ��>��t�!�Ц�����D��iߐ�����T���/���������rÿVH�~8+t6��[篜��|��[ah�R��_�����۵������X`�%"��쓽�#�������a�o;���x�Ph����}�N:����
D���	'�s�F� C޾{^���H�_Ċ���f�k��'�!]e�-��$@Ҩ�/�Z���Qi�qN�yz�{��=˜���n��6�U�Z4�����ˋ~����,�o���ֹIӘu�~i��Bm�Y_�ɖ�|"�-��Yt�[M���`�xB���?噖�*�pB�Փ�%n)�- BɁm�����M����c�w#�� Z`a�:Ӳ��S �$>:��T09A�0X������|�iw���(n�G��<Ւ_%�Y���Z��-�q`��&��n�~�)7?�=j��k�}�=�5��V}/z_t�ltY��]x��,[eNB-���ur]8Y!#E����`�o�{9ҺS|W�e�����Vi� A��3���GlS>�I2$fu�S1T�Dڝ�+.��؏��-�#��7�� �~�{'������K�Cx#�n}�$�t���Z�^��Z��u�w��܋��(
vղ�&pb:�Y*��?h��uI�e��� �%%��\X2����� h��2F���ЭN��jT�q3��{�I�,�B=���;��Kc�z��ߴ��WM�6���`�0��c��X�]��yB/������Z���v��1Gi�wQf1tW����*�2���"݊�)�C����ɱmW�^�2QTSI�W��
}�Qp�1�#���_�P��(���?*7"�Ǌ5��1dJ}���$��N�H9Z�5�\
�v>^@K�e6��9�o��K���2�5��>֔|D)�"7h�A[H8��}�zýz�\�漪��j�ž��I�7�T��t��'��}����=�&���I=w�BΆ!�&5.2S�`�����i&�)2��9N��r�jV'	Ĥ��������W��8jS<��������v�~�ԛ�Vm&��ЖƼl�:��Y���lz�
ȸ��z2 �/���K����	��̨Of�~Iw���Ԗ$�(H0~F�δ��A<Y�	sD�:��e-)Ӹ��8ZXP���s����
��o2&�re��t��e�sQB "���q~	iI�EC���m�m�L��}#+N�Ը%)fP�xI��Os�b����NO�[ܬͧ8�蘹R+o���AVc�I���6=T�{��(.$�[8�|��٤��.�O�}���_^�_50V�-��Z�|(�b� �H�z��i<�j.a`b����o�-$+��]NP���c��|����&'��)���l��@̝�X+&��̨�_;0zt��{&��`C��D n����K�'�v>��$/0g6��F��},aBpSG�����=:
��6�ZV��bk"~p��}t�r�ҧv�s�:���*��0*[�ݪ�J
�G��'��N�$� �GcB��KB�/2T�9�i��ב��m�ܙrq�D↜$�S:.��ոY�F�[ūW[}��%�6�P %�X}�3+���w�o5���ToƳ�Ҋ�D%����Z�}oWJ%�2��������#�qM�3K��7�`ͯ%{��"F6��W�l}a�)8�����ĺ�8���k�x3O�L�	��?!�lZ�g��u�{�׿4-s�9�� ٯ���,�x^��n�ȅ��L��?6��w40���|]����p����D��>L3ڃ��3M�F[2#��O�S����onT1`��b-�qnR�B3.(�И6 Ąxu<��-�@��T�t|�7x�Z�<�.����e�ʕA-3��2ˌ�]��?qlZ��ACLOl�����U?Ai�;Kb����m�Z��(�c��i�A��^�h�Y('��Zq�&��~�p>���]F�7�Kjzd�G=0n�7�^� 3r�P��S���W46�����B��/�E_O�G�A9?o[��|W$�^P��.�DL�Kw��-DrQKSXe�,fZűc�	�.ðt���W�V�;9����K������6�E�)s�S�46�_2��	e�1�U@��=8k�i	1z�̔�N�̝���&zLJ ���m�p�F$�Ew�~~'+�	|g�>{썂�S8�E��NƁ��(f(;�2��2H�
�ޯ�ޢ	z��q�K���R��}�����9\Bv!�O௚ƍX�6s!���n�h�+m�y�S^4E;ĎV_��BK����a��hs���[x�κ��� +V����5V~�Ű)��r��
 "3��B���H��A���G�ì�oV�;���BS7FSa���|�G�Q�NPH[H&vWB�m�{�#uh15�x%lǒ]�CĪ�,�e"{B����b#�.��D�3�Ggȫ�>2?��#�����ꁽ�X�j!����q�X��_����窮'�0i�wy�DNށD����ix��.2��C[_����/Y�(��"�w4�d�[�@d����R/���r�4D.� �s��}s�lo�wM��G#��FL�y"<t�C�Ocz�K�i�l
k�����f�G�&��{Ӈ{�����?�����W�2}�\��U~�t ��gue�2����b]�y�b��g��|�4`k��cAt���ef�I][�|6������?�3�x�H4}�,���.HQ%�0'Q��=���d��5�To����
Ɗ�x�GK#�W'���d:�p:"[7K|.A�#���o�EӛI����&Ԩ�w%k�Mø].�
�<X(��βv�L�a�Ŷ[#�N��'�BzP��̈́��)~�5�J�K�^�c~��s����΅{�A/�!m�!�P���º(��5ć�D1*7����_�� È	���G�l6ow�]b�q�Z���i�(�0�>y��c�gx�|�o��IW\�%*r8�SX����8j'Il�l����TFK�aC���aeqP��Õ�u�gi�S�r���yu�.y�)��A��O(�m.[�lf�Ѡg�.��>ea�k�9��C]��`��6�"|ء��T��.Ʊ�]8�t��B�1����SJ*]�ݿ�ᔾij.����w�*���*L5�����*Ǟ[�O�S�*
ET�f�����Xc6�LCGBþ�Ø1O�_��i�f�8v5ƫ��,�AQ}tی��u����|��*����o�YS��q1�NGR���p��ӌ��,�Oc����5i��p��4�s�cn��-+���q��p��|�+вU�Z9lT���%c�P��><6t�>�b��j���m�/���@�e����[)`�+I�Roj��8F�X�U�K#yC=G&V�c�u�-��P���+ش��"F02Y*�O~��MU���8Ƙہ��Y�z�Jڃ3>��M1Fx�(���`x?}��=���1l��J)�ǉ�������a��h��j	�0�f��E9 }������&d��֨JFGx������g��g�i�fqmN�Ͳ������\���~��pw�^l�Q�Dp�#��Iޖ�cS3�
�a�6�Ϭpym�cꫳ(Λ�O=mW����tK���}�@�(� �.8���ɰZ�_��O΀���J����M�C[ǅ�µ��l����"l�"x�`�T �e)θU뻨*��&	�k��.�
O����$y�1��H���ǧ�j]�o�P�l���qQOڨa����>�.���g?ȳ����Q3S�)�u𣵗��
R7���%�UC�8�E��7�beO�@
�oh�=��Z��"f6�U�����<;�"�.T�^Q������Z~vXdK��+�l�G�3K���k�3l[x��BC�F������H�Bv�۳��L*	f����'A�������L@j���0��l��\**@%��'��D�UA�`�0����ߑ|���juMhn�G;�$�9�����m�_<�1}t��q�����p#��,<z%[�v:b�5�(����f(���jN��;５&��ص�&�����A{a�ę���P��w�(+��7CqBeB���^��yvCO����^�t��Vخ���u� B��Y,
#ͨi�����]����<n�,H�D>��^��͞�<[y��hcl��6Cu�����Z�2?D�q�ʵpɪ4`
��Բ�Ͼ�/�v��[Ϧ�6q��m��*�OB4>ѐa�?u�4$�{��U�i�Z�)�kY�ͭvP�������)^;Ζ�j�5M�a ��dʛŤ�p=��Q��3�b�:,#����znV_�}(�o7�=綫�7e�#K�~y�T�,���{�x6 o���z��C��ӐTP`�{s���%��Y������u�3�u���:�RЌ�qN�+l��1��Vx����}�aM6�fZQ���f�I�D�!H�_��<8��:c�mӐ�����+�ț�y�+s�RM�o��6A�!���7��d��¶��ad 7�a]w�<�OE;�Xg��=f�F)msN��T�r�!�}��z����4��ױ~�2���"6���>~�A��b�ѣ�Q�2��T�������ti��hݗ!R
�y�J�^N7�T�j)����-}���f�]b�D�uԧ߳��Bp_����"g1��h�I��%m����W ��,v��Y�F�k/
�q����`� ����3�G�	4��/�W$�V�(~{�hb����`��.�!��;$�6��$�>���w�.8k�Վ��e���~mR�_?.�qB $T�9X�pN	��}��|�K�ˋ��t�H`����Ii���W�DBff*_��+
��R�m?�3��o��h�GKe$�t�8�QY}�����==�P*:g%0~)U#}�F���~�ϫ�Tך�wSb��v��*��S�G6�5q���]h�#��*�
Ay,)S����_J��{�u�[������b~9���4��q�)Wmy��pe,�2��s��͓Sxt�-q�녨5W�Ωv�'t��$`�D�$��[��k(YXE3�5���0Ґ.R8��ڎVR,�DąF(�D�$(	uNN�d'�����w0:;S]���j�)��s�\���S�� 9��8w�IIMk�;�kM��?�[*�=*�v�P`�^T��%���V�C,dL_��&��TC9y_D�;���`*�mk��8�[C�;�O�I����f����;T��c@p�L��zL=�2sު��\}�Z
.���я��>��|�
cI����Lܴ�Z�uw�:�!3;g8�����R�;����kP��/�ZY�m���{�A}'uK��p�p�W�9S�z���G-�2A����t��I�[%K���219Y��� B���lر"�7WA�yZ�@ˋ9qǢÉ7�H�n�<
�A1���*�˸��6|��#N��������!l!�Ճ�B5lwO���T�&�A��+���[V��V���CZL��/���Lc�9���U%���4�xe���G�i�zL���i'+����������8h+��i��хR���%@�V�pc�\��	�������]s���)� %�� Ӟ�(i&u#���.C"�n�S���5�uc_80��;�i,F�0Y(��;�xV.Xg��N���W�O�:;�z������ϱ�s�Qj�ǲ�6_������nk�TTˤ׀�tBq%Mh`-��g��M���+�B;��
3&]a�݆r?-,���S1a�P�+��Z����w!v����k�G�L�Qf޺M&+�j̾�1,����,7H|k�V������Z����@�n�5���Ŕv���;o��-!Jx�fڟ�p��tPsݻ��@h�i{zC���r'&�yuv�j����'�Pp4tD ��n�th����G���!��"�~�]gM��S���f=�=���`�3}q-�[���ukL�w.ZzuR�_�p&��N�,���t7�|�G}C�H�0�ꁔd�f����킌��P����e�c��.浭�`���x��y^S�P�j�w�D�FZ�I��[��?VBUne'Mh�`B�t�6�Q)�í��U�8�FO�gD��Ϊ\d���)q�:|�	�r53�)M��q�|�ƫ���4�x�8%�Y�M;��
��Q���� ���������q��|
qQ(9�����©ElDc�Ek��J��f �&G-ɽ�Єج��qn����v[y�x_�o&��* ��ef�`Vl���<S��_��-�[t�~�#w]��P7��ì&�z�f�F������t�z��gL3�.�,���か�z�ޝ �'�ͣ��<�U`��`�I9�/z�RP������P���KWy��i�/igo�W/�{ϳ#��q$E\�^�����h�zB������@����-�X����wГ����� ������I�1Av,��55�d� �Yn�V�P�WA[��tbc9|� l��d�!� �0�� xqc�׫��Dt��@�Cuʥ��%�� s1�f�jr���:"��Q�?�.+��3)�����!XB]P���<�s:�$Q~�`�4^��J����ҏ[�����-���{;ԙ5�ccڏj�_(+�*��4%����:�M�����Wj�a3���>o o]�؄�[o��L4��PLB�Ŭ���vW�$f	�tLІ��K��O��[x:v��OJ���ܽC����Q��� �p���/`�u�*��'���.N��S(d�~
�M��+d��G�YB�X�e�V�~�(x���*��Ɩ��A5�Q�
��"5���T���[�q�^�~zAQ��9�n�u�b���Z�o"����7���p�Ow���uh����X�\
��KF.�T�Ɏ���v�ތ����'Jz��)<X
\=s�M�Gb/�53��aź��@6���o�����e�ݷk���\�.-�7�j݆�A6S�-hU���9	.�v(>��V�vI嘜��{A�{��X�M$;�%���VӄD����qZ�0H�x���t�;�>��m��f#�D8��j'D2m2�i=�eiņ&�Aءi���瘱�m^E��c�i�-�`��,��M+����_$��{0o��4`o����
^�x=O�7�~�V�~"�ֹ�Y*�?2��#�z>�_��wK���5Q6!���Ε0Ug����any�+3�t����M~�'�k�#��2��Л���yrZV ت����V$�x�.�lG*6h?�f�Ǎ	!c�B�.�Io\4��D��φ'i�?.�-���h���~�
ʡhSZ]�E��ݚo���D��Ɔ1�ȼ^<"HS�N:3�ڍ�pjE4Wk媿�{�E��L'��2��g
y����W`X%�,ȯ�
 �4�����t��fdm��d�E�62�d�Y�4�?G����KF�õ�}â�ű;���!D�fLXq�����������X�����1�$�\��KIxx`�,ꁳ��3�u�a  �L&�wь|9P��oA�4�D��[�52bz����:�h�B���{L.��4S�b�Y��$2�G�y�t���v�����_W�XF,c�[js
�E�(c/p�yL��*S��?C)[te�9<tF^
�VR�ɠ������}�E�:=U�֨���ފ�
�]l������$"��2`��8��X��&��%���H�5If��? ��#_�x�ݵx"��ݡY6�؊���5U���Nau�n�+˸F:`W��p��I=Уg�v�[���/W}�����-�[�/�_�����2�@3���*#T���̰cZ��M��M����KY�P��J��z��B�e�&���p0��	�?���|�ñ�H�g�O��<19f��N�)w6����V�#��U��'	?=�S�2W�_�M�nI7�u$ʼ^�F�U|�� x}?Ir6��ߺ�d{�aΗ~������	���y���X�DL������#x1�|jsA�8ܱ�4����_�kNܓ�y�\����,2	�۾�r�v+z��2�X�R��
8�R�W�����_8q�`���"m����Ē�=wu���y:������K-�M
j����a�~m�yq�R�n�Cl�� (����<|0� ��D� ) L���=$̥uS/ ��P�$, �ڲ��'�/��� #�\�N%���(R�f���ud6��C��?s�o��j�R�r��;�zY���i�S�W����U,^���+���ef����_H�t���t&� g�k�F.з>C���1N�u#C�� *��6��|ة1�{����ĸ�k�+�S��M�v�w_�C 3q{&�h�1�/�ޥ��܄���i�13���L��S�ߵoo�z��_%o=Ġ,`Q������������ S�Hs���+��P���d���˼�U�h�7')廬̜�E���!��30�ۆ�b�0�� {ee��q�¾��^��d�b_��0�S�P	)���5������Nr�r� F�_"����$&�^��ƶC�;�	���M40�����a�A
B͹G$w*�=�U�. ����z����c�S��iA��+8��ɬ&{N��V�zR�)9��6�&&��ƚ�Y���fy���t������B��X� ��=-���S�?�	^� ��B�,�&��>�xl2kj�=vJJ�]�m<#���\<�"|���v����(�*��^eLt`"�,[�Ʌ2���tX@92N����d�c]d�Q4�)K����g�F�{���q�ŏ^�P`h�� �ŧ%�j��"GV�D��9� i�/86�I#�	|L�#�<����0)�G�& F��՟"�@"��z�z/b�����:H��B�������Ǘ�\T��q`�f�I�g#��@0�R&`@���oz�i�)�c�l���,�I��� ��n &s*#L��O�7@eF,�1�h@U+���� c<�,�������'-U�Ų.~tC���~���Cwe�]r�μ����;W��ɞqéu�����h{���pw�`�:��@XKa~���f�\u��� ;���	Dz��?�����'o\�>�n7]�	H5!�+@,�0B�r���] ��E&�dod��M�il��4s��/��t�=zK��xz ���n�Pp�A�r:��<B��ޞiED�zB�T۞]�/"x�9�KWV}��<���"�M�m$OnS��)��T	����'5�G��Q�Vg~������Xu� )��G^~\Ҹ�a����"�	Xy2��J1���NV�����E�z8����'K��)wUkSU��:O����8���pF�-����>�i�ͿOŢ�wυFWQiX׳7̮e%Nf�>ߞO�l֕�W.�Ekǭ;������_���3?ZUAhY����m�B���	�4������~�p��,��}1�Wq*�Qg<)�����^���Q!��YJ��Nq8�i�w#�g.V�TCc&)�g��&�&3�^��(��q ���j�~��w^í�gL ����4Cn����_�b��e�^��Iꤟv�;�Iz����sw|��3�(%* r=�E��Dgs�dEsSQ�w�J8���P�1��db�3�}�{�	#%d�vQ<�!n�y��˷���f�����l�ƊG�3 ̐W#�ܺ�,�DnL�`���/��j��ø�z�?djx���.�G�ԺZ�H1��@Z�������M=��`�E�Tj5���7)�ō�V@{7]��8%g ��ؚ�_6{"V�p�L!G�&m�㔾�]��hQ�wL�����w����(���@��'��&7�'m$R������+����<���^��8��{�(�PK�2Gl���M��ze��:g�QԌv�	]7���T��1���[x'"�%m~�I_��xe�.6ֽ[���:��M(��*��V����U9�]�=6�D)X}l]&�
jw(�7� ��y�1��(�@��iw�ݳ�l�(��P�74�C�oD���VK\�tǩiz�)�
�w#�¼C�I����bBq��5T\ͥL�ߋ�K�I+����.�QiD:ҫ��)^��Ǣn4V�#�GW#��N�=-�L���³;��^�-���z�@_�S�j��V�4�O��� c��(J�����F��-+�>?�\ь�(����T��_�,���T�v�5�b����j���+W'�~��2VDCk��
�q�1�س�8U�g��&��-NQ��f��J�j�z&��Y�?�ދp������-G0j����y��E ����1@W)��E�G�r�P~\=	����SC�k��!W��<1IS3
}��N+��k��Њ�zv{@���1Gx�}�9������n���ڞe����摘L\J�#�6�|cb�s��!׿��Gl�Iy[" Op�.��ֽɞ�@5['���#��\�v���G�G<s7[�3,�`ч�q�=�RxT��
A2_m�?�@pp|�~O��7�
���!\�>�Q%��@����x��c%Ŭ�3Y)#�gq���k���X.����
.6�M���幻��!6���@�7�⧧�G��Zd�Όg���2ꝺ�}�H��WX �nV��ո��r"Y�K9D7�g��lf���cF5f���5�ï�$>4��n��▷�1��ƫ�����rg4��w[�Q��5����S�_Q	��h��Lm���Ҥ]-H .{9��S���T&��~�c;�]Y��>t3%gH�����T�1�R�/�L����q�J�q�A#�k�Yyӌ�2#�7��
�`��
/���yi{0��t�W*����74�;�9J�Ѩ�ss	��a����+�"������i�{Q2+q����2��b��=ޏi�PۙG�)a3��G�z)����U�.�oH�I��#��'���
��g�4�j?+jN�߸YU���O?�$m�v9�!-Q��{W�O�R���5|��K�܀�]9[���$ᒱ��g�YI�;b��?�����*`�����:�ey������\�����[���f����8!;�)�9m�R%��m�,
ElSj�J=���Rn�K�k�r���80��#1�=9 T�o*���ɱ�]��� bn�����#��52�q�������X�^�^T��IӞ>�WQ^y��q,�̥�(]���rd���[��o�s��O������ZX���V(��_�Xy��B����u?A��[�����I�IA�����s�=�H�E?��s#�Q���@7���[9��[D3�[� �'�P& S���~��Ř�ݝ�W�#�>��"f!q��)X�a��"҉W>�L�|�X5we�4��蓓#<,�2�f�j�D��y�.�`/*{����\Gt�X��)#ِ�?�/#|��:k��R<E��e=���o��ח��l���4ek��V����!<=Z���C�؈ ���I�봋�����bA2`���҆�/N�[~�B�8i[�Dp�5����/�G�1zk��J�u����͸�S��X"�fFr=&2t�Ǟ��J�ͤ��]�}��^͜��z������	-�� � 7Z�r����Z Y�s�cɒ�"���΂�vD�FޤO��^�����1i}�L;�=�� �{�I	IO�#�V���*��Gsr��^JT7i�2~ꋺ����8�MM+�y,�L:�$�����\C�m���[T���4M`�	�)�PJ�j2�Pp���G�N}@���y��jFy��ǣ#v M>�Pͪ$�@�{�ӳG7�pgO;ڹ듌��D ,�l�5��˦R6���RT[�_DGEC�vSU�.ԅ�ʏ���>�:o�t� �VY:������N�3�]}����<�[�Z+`6d���)��Êܘr�ui:ѕ�>��q�L��m��|�����" ��
8�e+�8Cj���zq]3�e!��������k��xR��Ju���ԏ�4����+�k��	7)Yݰ,ϦQ��Dm��)��r��X�p��Y+�Haؘ�&��!�d���nAE{�k�Z}.|�
d�H���
����"������~JO<��JL[�@
gZ�o��st�� Q�m��x�\���(y��N�?5�nː�z�'�㸎-dk%מ1�*��i�����\o6�~�#j�-��ۅ�~���
�$>+��^��?���v���G�iǱ�a����>椥C�P�ؗ��&� �㿚�к��]��+X�_��or���XnkA$�C�b���L�����"�PEѪ�9���7j�W>΀��q#x:p�4��((���w<f��&�Sڵ�ZK��(���#��p���j��(�P���5����	���@Dh	!��z�S����X�����;�땦UY�T	�o��Z���	����H���%�:�?�Z�����ԐoE��:�c�+���gwc}�L1��%�ݙ\�X��D�f�M�#P4"��ԧ���I E�,�Y���<EQ�2!���&�np-��4h<�9��9駼Ԯ��#����y5�|D2�&<����Pj���~\�ܫ
v�2~�ZiP?+O��X��|��[{.	Z�鮽G�j�sￎ.:�X�[��;�u^a���l�V����X	��pY��@4`:�����9#H��s�Zq���o0�����R:C(�k���tE������EC	y�@p���Ӵ��Q������2��Y�Qm�L���\bwQ��J�a�%�7ɮ1�� om�V�:��(Z�B�mh2=#�|1�4�������w���HJ��-�.+��E��C�Ǹ$�o9հ������<� �w(������"�elӫ�nQ��A|`결���Aa&e���##��;��Tp]'5é��+����A�҉L8)(i������,l�	��	�� RG�Bφ /��ǫ`ߔ_Q1�'����i��3��i~.�	�;���JOG���'*��|GO9��gu-NoN�l�TO�e�>R)ȫw//�a�\�F��-,F�}J��ԉ?'GP1�|��COf'�s<c�"�p=vAv���m�܋g�����o8�b���p�Nu�}ӊ���X���C}�F��*��z3�3S�s;�؛�iR����V����ǎP���Y^�_q~
�4�ĭ�ͼ���^����1�y8�g.AOCbʞ�7r�%[:j*�q�:y���L���*O`�`?���Z�K��9�l�ɰ��˦���Z��s�r�c �|�x�]n���&��6,;����*%��ٱTS��Fs�W�������c�{K,E�9�?��Y��HU&*_� �qj�-,���W�汥�Cr�H�7q�my���D�0�R�Ր!�o���$��|�{�c;<��P~���FG!9Hm�Jb-n�Y��4)Mi� w'͚�P��&�e@3�Kh��a>�0�q�I�7����'A�<]�7]1��S�A�h�,N�c1�\LH�ҷ��0�/i����/I��P`x��N�GGA@�L��B1�V�@j��Ȅ��S;��ˇ�;tܸh�W�nϴq�wU/)65V�|�G�Q���oB�^�qؤ!�mF�9eq�,h�fzK,�%��<�M��C!ͪ{%��4�3�Am��#'Z�L��r8�,���g��CQ��-�@��Y�X�*>�ڃݺ&d���M�5I���HH�/C���G�-vRE�w��~;ڏ�+��Z����W^!
}s�&�r��x"K������X�.�in���K��.�mHѱk������aAt� ��%��+D�^��|�rl`�c��'�)S�6��j�ŧu�X!ΓJX6����4��:˫�k�I��OfD�x�(��n左�9E#�3N�v��^Aq�'lS��>?��^�n�cN��I������6��i��b�*��ꘘ�C.�[ ��`��ST�2�&�s�p��#KF᭞�x�L/"'�hI�!�r���K&��IZ?CQ�w��#B5Io|��X�,�
��#�~4�G��h����Gڐ� �v�Ư�q��˕�a��L���Ɓ?���%�q��#�.	�����Y���6B�nU���mw���;Cwb����gT~2��b��e��UWW��< {��q�Ή��C��I^��#���1�x��	T�����˸Z�?-�x��+�0�\���{�� ��0TbǄ�� 3����`E��Ivm���B�v�����atܙ'��
j�c#3.����wl���g���:�4�Wh(�D��; ��q֋=/gmR��Q�W�5%�
4��(�~��LD,�cӏ�QC���m@�E�,%;[ب���K�[ՊSL�>'���A�(k<.��1��kڲ
!"��^���u��EDrO�~8��	|�����E�G�]����d��-�qBӒh�!�8a��WpX��E5A��r��y\̓�G6af]�N�*eh����8�� �"�s�k:��h�2-���(��5���V�Z�MD0��7�����F��4��k�ȸ`�lV;���qP�5�@�,�ϳ8��6|j?M�q���]��M
_���f��$u<R�:-�E$�v[�/�ձMP3{U�s_K�������
o&�dZ���Ci��9JLU�Α��c�b����MR���*c
ХuWU�������|��vKv�Oip��"y���Fg��〗 �49��po>�|�ay�q_ص]��X�8�!]���E���t�){�"�H��~g�~(#�+M�{����@��=��@2���,K��[ӓELAX����#�C혙�����/=d4f�4���	6h2�#[%w��Q�2�F�Z���鉐0�^�g6�H���<�JI������Z�B.�A�0�w��������=�uRT�����]�M_���B*:�+�|vF#�,�T$�Q���ا�ܸ��i��t>�� �8�\���b�|̙LS	e3�p�\����I��2�#�HeV�j���XZ�I��H�+Lw��ǕL�w��@��p0"+�p��K��zT�g�'�K=�Bƃ���-V�ƶBw)0*�T���M��gx0y�$�#���,,q���FV֋ă̯~��G#�|<)f:F��y����3q��Raʄ�ė�8�N���W�=��Ƀ��X�,F��6��T�(��XG���]["W�����kI��ܞ�p�|m���J���>/͓�u	��v��.�a8�oҎJ���TD�WƁDJ~v��i���� ��$�!� h�(� 0
z� ��J� Hc,Ai�$ؓ��p�-{A>vv���
:gCH�����@����!�h�Z_���!���7�i0m����B�J��*�q>�w��xul�ꟷ��|�������/Z������Y4��T��hˇA+��6��8���gs?T�:�y�2� �I��Y\V4����z
����Xj��K0�}y�>̾�Ы�/o]C�S<l�F�y�og��tb�̥�Gs<%��r��t����2�4�-��\Ќ�'K"%�\�4�34�|`��f�����is�Z8	u�4���m���TeH}�4m�` ���ا��*����0ד������ILc���W�U�я$U(����s/�&(�ּ�ޑ��J=��ֹ��/�V����`�,�e�Fˊ�0t��m�i�!��l�ɞ�Ur�`��HD�n �>q���W
�Z�N����r^B�Q!A;fs�N7Q�wT��ɢ��64Mm�4w��!�ȸ9x����|h+��2�Ѡ���T��N3%v�n�뷥�o�3�x㨂 �Gr$5)J�͛5�.n@!���
%�J~���|2͆�^y'��(󐦹��0Z�é�h�x�9���_"��ć�+� �WX���I/�w3S�š�-=~�+��Föt���j��L��Rjl=9p.{\����(��A>�	Yg��!�V��P��R��o4�������I��tsM�p���#�
?�ٓ�s�lŕ�&�<غ�q~�X���V"&����Y�u��-D:��ʗN���P�+���`rU�k�"��C���[����)S�8s���i��T��8���c���9��n�'�F:UF�̕���9<�VCQ����#$�#(6e��Th�r��euA#sG�R�F_�3*�>���٧g�^�jw�Q�x��C!k���雏���� ϥ��uF8��K�	��Lx����;�=3�1��S��odZ�j|��uQr:��|�W���1��C&�D�����=�WC< ̈�¨#�&g��i���K� �U���|�7ҕF&�ጜyE>C�����*��7pV�v��	K�/�_o.0�����>�������і��ҏf�\�������b�V�2�d����>R{#������+��跽k�[�BE��j��C��Y7�f�Z��Na�XM�'筲)�$/n�x���{�G��N��5VFWm����������֐���MG���+ �Y��t�-�U�,�T;�*\4���p3Q�
���k�(켿��c�wurX��^е�Ձw���UD<얘��ӓH��6�o�^��,�A]	��s����~���l�}I��������),��N���P���P�}uN9y�����S��g��B٤@�2E��a��Y���4�������]���E-����J�K�"V�?�rp�\@9v�� ��L�/��ێ]Jl\9cM�7�pSn@�/�b��"э�2���|�z#���%�9��BMQ��Q@�����,mj�,��1#�T/�f�˔���p�+аL�>�	h���s�,�����f;����G�S&�pO���}�`XQ�4n�_��B�VZy���Ӈ�+NB���Y�\��3�^�"�|�AN�W	v�8;�`�d}W��r� 	�|B���N������Td�@cV���4�	�8����h�K��-f��
kW_�Ŗ2N��qd��Fl����=Fz\y��U(�(m���ډ���p��#ǧ0Ɉ��b�m̚I��<�!��xwߎ�%}�r��p2I��S�Kb���Pj��""<FJbf�czѹ/ԒWdL��
�s�����N��c�<B��y�>��J��p��S��p�Uw�iъ\BTӗ�3,������v��iEd ��0F!�g�gsw:�P썘��\�!1�Y��%�{Ʋ���}��6���&��
hK/t@�k���!9���@4���<Tҿ��$���B^�)&�"���	Ǻ���IC�o��u@j�
�`v�A���<��3L�zA1�M<�j�]�������o�s�J9\��0(�N� t�ޯT5�w"	��^|��7_��`�i)�-w���n�mO��s�T���c�a�Q����á:�����#�嗟���IqF�X�1�>��$=��cIr�D%bM��戛���Y~w֫�>��lS�`�6��Ѫq^��U6���+PW��$�������nhQa���[${��eo�|�2�rP��9s�QX|]$P����ˢ�&��\ꓕ'���Y�7��&E����H�߷,���~��4U"Zz�����΋�f��41��B?����3�2�T+�&sqUƖ��cHq��=��1�W�Kw[߻������a!;�w��̅4�0i���Ń���
ᦙ��4�(�D�6x��C�$����-XVc���#q�x��j~���l���n.ڝ��v#�aAv�Nh��u��u�,�b��2�Q	��;����0qSiᘲV?�iX�ч�����bΫn���!Fy���b9M��4�z��Y]p�ga�h^b�b��wս���0��<$�: �4RaR�q��/���I4�t�� c�Ibת���("ͺh�T3�K�,`�{\d#Z��0F0��-+>5ݿ\��
@���f�w��NI:�E\��<����B6i���@�BtT�2�?�b�N}:�H�9��@'c��1|t�3)�1�2�����Z~�  �zl8�Ѡ{t"��:���Q�|ث�m��a��\WH�]��u�[S�o�Z'&����|-R2<��.9�n��ƹ�S,�L�%�����{/�sM���P���>�M-��]�7F�;���Nl�$ Ә��m��]I/��/˕c�05�b&:�cE,��hOS⩪@��	R�ͻ��ꕖ��"OE$�a.vKfK&�|�����Y�)pg��Z�;.�i��<� oA����;����fC���\��,s�h@f�-ua�/����q����>=�mûnY�������P$�mw�\�չ�B#B d�}j�d�Ip(z��S�Z�����{]�q<��/3W�>[�C7��1�po�������`���՝����fQ�A� ���t�s�.FfGn8��ވ>9���@�ȿ/��Z�����ʺ�v�|8�E�	у:�v�M�c �~�����s#4�hFD'Q��viF���ޖ�⡫$_8�J�ͣ%��ʬ�{�}��w��]dywB� x���:����ȒGh��ɴO�������1�<Ո�G��z<�
[����.P �ed��~6D<�ĩ?��95=v[��Ja��HM���#��]+N�M��u���d?L�l���&�U�}�#q�_N\y\�eIXEM�ͤ[Om�ѴX��=/n��=��\JH �uj7��V u���$���OpCh0-�dT�R�p�0�f���vr����|n��N�(�^}m�R����Mr�a�����f3��"��YNp}�S���k�\�\�$����ޒmC灼�'	7P�g��l���>�'���C��	��D"@@˾ߞ>��G~��(�'�:�9p�i�B�c��z&��%�8 �d��h[�~����g�҄�/�,�������բ�ާa�e�^��e�_��XթN� '����Z���6=Y���6k�!���C��[a/Ɍ��G�TL�}n	�oP�C���#��R��������p���ı;g��D+���o�_�s�����[�G�«H�(h���_��|L��c�]�J��D<H���4a3�,Mr����,q������)Aa��G�b7b�s,��{�:s�'��ه\Ȭ����6K�f// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * cimax2.c
 *
 * CIMax2(R) SP2 driver in conjunction with NetUp Dual DVB-S2 CI card
 *
 * Copyright (C) 2009 NetUP Inc.
 * Copyright (C) 2009 Igor M. Liplianin <liplianin@netup.ru>
 * Copyright (C) 2009 Abylay Ospan <aospan@netup.ru>
 */

#include "cx23885.h"
#include "cimax2.h"
#include <media/dvb_ca_en50221.h>

/* Max transfer size done by I2C transfer functions */
#define MAX_XFER_SIZE  64

/**** Bit definitions for MC417_RWD and MC417_OEN registers  ***
  bits 31-16
+-----------+
| Reserved  |
+-----------+
  bit 15  bit 14  bit 13 bit 12  bit 11  bit 10  bit 9   bit 8
+-------+-------+-------+-------+-------+-------+-------+-------+
|  WR#  |  RD#  |       |  ACK# |  ADHI |  ADLO |  CS1# |  CS0# |
+-------+-------+-------+-------+-------+-------+-------+-------+
 bit 7   bit 6   bit 5   bit 4   bit 3   bit 2   bit 1   bit 0
+-------+-------+-------+-------+-------+-------+-------+-------+
|  DATA7|  DATA6|  DATA5|  DATA4|  DATA3|  DATA2|  DATA1|  DATA0|
+-------+-------+-------+-------+-------+-------+-------+-------+
***/
/* MC417 */
#define NETUP_DATA		0x000000ff
#define NETUP_WR		0x00008000
#define NETUP_RD		0x00004000
#define NETUP_ACK		0x00001000
#define NETUP_ADHI		0x00000800
#define NETUP_ADLO		0x00000400
#define NETUP_CS1		0x00000200
#define NETUP_CS0		0x00000100
#define NETUP_EN_ALL		0x00001000
#define NETUP_CTRL_OFF		(NETUP_CS1 | NETUP_CS0 | NETUP_WR | NETUP_RD)
#define NETUP_CI_CTL		0x04
#define NETUP_CI_RD		1

#define NETUP_IRQ_DETAM		0x1
#define NETUP_IRQ_IRQAM		0x4

static unsigned int ci_dbg;
module_param(ci_dbg, int, 0644);
MODULE_PARM_DESC(ci_dbg, "Enable CI debugging");

static unsigned int ci_irq_enable;
module_param(ci_irq_enable, int, 0644);
MODULE_PARM_DESC(ci_irq_enable, "Enable IRQ from CAM");

#define ci_dbg_print(fmt, args...) \
	do { \
		if (ci_dbg) \
			printk(KERN_DEBUG pr_fmt("%s: " fmt), \
			       __func__, ##args); \
	} while (0)

#define ci_irq_flags() (ci_irq_enable ? NETUP_IRQ_IRQAM : 0)

/* stores all private variables for communication with CI */
struct netup_ci_state {
	struct dvb_ca_en50221 ca;
	struct mutex ca_mutex;
	struct i2c_adapter *i2c_adap;
	u8 ci_i2c_addr;
	int status;
	struct work_struct work;
	void *priv;
	u8 current_irq_mode;
	int current_ci_flag;
	unsigned long next_status_checked_time;
};


static int netup_read_i2c(struct i2c_adapter *i2c_adap, u8 addr, u8 reg,
						u8 *buf, int len)
{
	int ret;
	struct i2c_msg msg[] = {
		{
			.addr	= addr,
			.flags	= 0,
			.buf	= &reg,
			.len	= 1
		}, {
			.addr	= addr,
			.flags	= I2C_M_RD,
			.buf	= buf,
			.len	= len
		}
	};

	ret = i2c_transfer(i2c_adap, msg, 2);

	if (ret != 2) {
		ci_dbg_print("%s: i2c read error, Reg = 0x%02x, Status = %d\n",
						__func__, reg, ret);

		return -1;
	}

	ci_dbg_print("%s: i2c read Addr=0x%04x, Reg = 0x%02x, data = %02x\n",
						__func__, addr, reg, buf[0]);

	return 0;
}

static int netup_write_i2c(struct i2c_adapter *i2c_adap, u8 addr, u8 reg,
						u8 *buf, int len)
{
	int ret;
	u8 buffer[MAX_XFER_SIZE];

	struct i2c_msg msg = {
		.addr	= addr,
		.flags	= 0,
		.buf	= &buffer[0],
		.len	= len + 1
	};

	if (1 + len > sizeof(buffer)) {
		pr_warn("%s: i2c wr reg=%04x: len=%d is too big!\n",
		       KBUILD_MODNAME, reg, len);
		return -EINVAL;
	}

	buffer[0] = reg;
	memcpy(&buffer[1], buf, len);

	ret = i2c_transfer(i2c_adap, &msg, 1);

	if (ret != 1) {
		ci_dbg_print("%s: i2c write error, Reg=[0x%02x], Status=%d\n",
						__func__, reg, ret);
		return -1;
	}

	return 0;
}

static int netup_ci_get_mem(struct cx23885_dev *dev)
{
	int mem;
	unsigned long timeout = jiffies + msecs_to_jiffies(1);

	for (;;) {
		mem = cx_read(MC417_RWD);
		if ((mem & NETUP_ACK) == 0)
			break;
		if (time_after(jiffies, timeout))
			break;
		udelay(1);
	}

	cx_set(MC417_RWD, NETUP_CTRL_OFF);

	return mem & 0xff;
}

static int netup_ci_op_cam(struct dvb_ca_en50221 *en50221, int slot,
				u8 flag, u8 read, int addr, u8 data)
{
	struct netup_ci_state *state = en50221->data;
	struct cx23885_tsport *port = state->priv;
	struct cx23885_dev *dev = port->dev;

	u8 store;
	int mem;
	int ret;

	if (0 != slot)
		return -EINVAL;

	if (state->current_ci_flag != flag) {
		ret = netup_read_i2c(state->i2c_adap, state->ci_i2c_addr,
				0, &store, 1);
		if (ret != 0)
			return ret;

		store &= ~0x0c;
		store |= flag;

		ret = netup_write_i2c(state->i2c_adap, state->ci_i2c_addr,
				0, &store, 1);
		if (ret != 0)
			return ret;
	}
	state->current_ci_flag = flag;

	mutex_lock(&dev->gpio_lock);

	/* write addr */
	cx_write(MC417_OEN, NETUP_EN_ALL);
	cx_write(MC417_RWD, NETUP_CTRL_OFF |
				NETUP_ADLO | (0xff & addr));
	cx_clear(MC417_RWD, NETUP_ADLO);
	cx_write(MC417_RWD, NETUP_CTRL_OFF |
				NETUP_ADHI | (0xff & (addr >> 8)));
	cx_clear(MC417_RWD, NETUP_ADHI);

	if (read) { /* data in */
		cx_write(MC417_OEN, NETUP_EN_ALL | NETUP_DATA);
	} else /* data out */
		cx_write(MC417_RWD, NETUP_CTRL_OFF | data);

	/* choose chip */
	cx_clear(MC417_RWD,
			(state->ci_i2c_addr == 0x40) ? NETUP_CS0 : NETUP_CS1);
	/* read/write */
	cx_clear(MC417_RWD, (read) ? NETUP_RD : NETUP_WR);
	mem = netup_ci_get_mem(dev);

	mutex_unlock(&dev->gpio_lock);

	if (!read)
		if (mem < 0)
			return -EREMOTEIO;

	ci_dbg_print("%s: %s: chipaddr=[0x%x] addr=[0x%02x], %s=%x\n", __func__,
			(read) ? "read" : "write", state->ci_i2c_addr, addr,
			(flag == NETUP_CI_CTL) ? "ctl" : "mem",
			(read) ? mem : data);

	if (read)
		return mem;

	return 0;
}

int netup_ci_read_attribute_mem(struct dvb_ca_en50221 *en50221,
						int slot, int addr)
{
	return netup_ci_op_cam(en50221, slot, 0, NETUP_CI_RD, addr, 0);
}

int netup_ci_write_attribute_mem(struct dvb_ca_en50221 *en50221,
						int slot, int addr, u8 data)
{
	return netup_ci_op_cam(en50221, slot, 0, 0, addr, data);
}

int netup_ci_read_cam_ctl(struct dvb_ca_en50221 *en50221, int slot,
				 u8 addr)
{
	return netup_ci_op_cam(en50221, slot, NETUP_CI_CTL,
							NETUP_CI_RD, addr, 0);
}

int netup_ci_write_cam_ctl(struct dvb_ca_en50221 *en50221, int slot,
							u8 addr, u8 data)
{
	return netup_ci_op_cam(en50221, slot, NETUP_CI_CTL, 0, addr, data);
}

int netup_ci_slot_reset(struct dvb_ca_en50221 *en50221, int slot)
{
	struct netup_ci_state *state = en50221->data;
	u8 buf =  0x80;
	int ret;

	if (0 != slot)
		return -EINVAL;

	udelay(500);
	ret = netup_write_i2c(state->i2c_adap, state->ci_i2c_addr,
							0, &buf, 1);

	if (ret != 0)
		return ret;

	udelay(500);

	buf = 0x00;
	ret = netup_write_i2c(state->i2c_adap, state->ci_i2c_addr,
							0, &buf, 1);

	msleep(1000);
	dvb_ca_en50221_camready_irq(&state->ca, 0);

	return 0;

}

int netup_ci_slot_shutdown(struct dvb_ca_en50221 *en50221, int slot)
{
	/* not implemented */
	return 0;
}

static int netup_ci_set_irq(struct dvb_ca_en50221 *en50221, u8 irq_mode)
{
	struct netup_ci_state *state = en50221->data;
	int ret;

	if (irq_mode == state->current_irq_mode)
		return 0;

	ci_dbg_print("%s: chipaddr=[0x%x] setting ci IRQ to [0x%x] \n",
			__func__, state->ci_i2c_addr, irq_mode);
	ret = netup_write_i2c(state->i2c_adap, state->ci_i2c_addr,
							0x1b, &irq_mode, 1);

	if (ret != 0)
		return ret;

	state->current_irq_mode = irq_mode;

	return 0;
}

int netup_ci_slot_ts_ctl(struct dvb_ca_en50221 *en50221, int slot)
{
	struct netup_ci_state *state = en50221->data;
	u8 buf;

	if (0 != slot)
		return -EINVAL;

	netup_read_i2c(state->i2c_adap, state->ci_i2c_addr,
			0, &buf, 1);
	buf |= 0x60;

	return netup_write_i2c(state->i2c_adap, state->ci_i2c_addr,
							0, &buf, 1);
}

/* work handler */
static void netup_read_ci_status(struct work_struct *work)
{
	struct netup_ci_state *state =
			container_of(work, struct netup_ci_state, work);
	u8 buf[33];
	int ret;

	/* CAM module IRQ processing. fast operation */
	dvb_ca_en50221_frda_irq(&state->ca, 0);

	/* CAM module INSERT/REMOVE processing. slow operation because of i2c
	 * transfers */
	if (time_after(jiffies, state->next_status_checked_time)
			|| !state->status) {
		ret = netup_read_i2c(state->i2c_adap, state->ci_i2c_addr,
				0, &buf[0], 33);

		state->next_status_checked_time = jiffies
			+ msecs_to_jiffies(1000);

		if (ret != 0)
			return;

		ci_dbg_print("%s: Slot Status Addr=[0x%04x], Reg=[0x%02x], data=%02x, TS config = %02x\n",
			     __func__,	state->ci_i2c_addr, 0, buf[0], buf[0]);


		if (buf[0] & 1)
			state->status = DVB_CA_EN50221_POLL_CAM_PRESENT |
				DVB_CA_EN50221_POLL_CAM_READY;
		else
			state->status = 0;
	}
}

/* CI irq handler */
int netup_ci_slot_status(struct cx23885_dev *dev, u32 pci_status)
{
	struct cx23885_tsport *port = NULL;
	struct netup_ci_state *state = NULL;

	ci_dbg_print("%s:\n", __func__);

	if (0 == (pci_status & (PCI_MSK_GPIO0 | PCI_MSK_GPIO1)))
		return 0;

	if (pci_status & PCI_MSK_GPIO0) {
		port = &dev->ts1;
		state = port->port_priv;
		schedule_work(&state->work);
		ci_dbg_print("%s: Wakeup CI0\n", __func__);
	}

	if (pci_status & PCI_MSK_GPIO1) {
		port = &dev->ts2;
		state = port->port_priv;
		schedule_work(&state->work);
		ci_dbg_print("%s: Wakeup CI1\n", __func__);
	}

	return 1;
}

int netup_poll_ci_slot_status(struct dvb_ca_en50221 *en50221,
				     int slot, int open)
{
	struct netup_ci_state *state = en50221->data;

	if (0 != slot)
		return -EINVAL;

	netup_ci_set_irq(en50221, open ? (NETUP_IRQ_DETAM | ci_irq_flags())
			: NETUP_IRQ_DETAM);

	return state->status;
}

int netup_ci_init(struct cx23885_tsport *port)
{
	struct netup_ci_state *state;
	u8 cimax_init[34] = {
		0x00, /* module A control*/
		0x00, /* auto select mask high A */
		0x00, /* auto select mask low A */
		0x00, /* auto select pattern high A */
		0x00, /* auto select pattern low A */
		0x44, /* memory access time A */
		0x00, /* invert input A */
		0x00, /* RFU */
		0x00, /* RFU */
		0x00, /* module B control*/
		0x00, /* auto select mask high B */
		0x00, /* auto select mask low B */
		0x00, /* auto select pattern high B */
		0x00, /* auto select pattern low B */
		0x44, /* memory access time B */
		0x00, /* invert input B */
		0x00, /* RFU */
		0x00, /* RFU */
		0x00, /* auto select mask high Ext */
		0x00, /* auto select mask low Ext */
		0x00, /* auto select pattern high Ext */
		0x00, /* auto select pattern low Ext */
		0x00, /* RFU */
		0x02, /* destination - module A */
		0x01, /* power on (use it like store place) */
		0x00, /* RFU */
		0x00, /* int status read only */
		ci_irq_flags() | NETUP_IRQ_DETAM, /* DETAM, IRQAM unmasked */
		0x05, /* EXTINT=active-high, INT=push-pull */
		0x00, /* USCG1 */
		0x04, /* ack active low */
		0x00, /* LOCK = 0 */
		0x33, /* serial mode, rising in, rising out, MSB first*/
		0x31, /* synchronization */
	};
	int ret;

	ci_dbg_print("%s\n", __func__);
	state = kzalloc(sizeof(struct netup_ci_state), GFP_KERNEL);
	if (!state) {
		ci_dbg_print("%s: Unable create CI structure!\n", __func__);
		ret = -ENOMEM;
		goto err;
	}

	port->port_priv = state;

	switch (port->nr) {
	case 1:
		state->ci_i2c_addr = 0x40;
		break;
	case 2:
		state->ci_i2c_addr = 0x41;
		break;
	}

	state->i2c_adap = &port->dev->i2c_bus[0].i2c_adap;
	state->ca.owner = THIS_MODULE;
	state->ca.read_attribute_mem = netup_ci_read_attribute_mem;
	state->ca.write_attribute_mem = netup_ci_write_attribute_mem;
	state->ca.read_cam_control = netup_ci_read_cam_ctl;
	state->ca.write_cam_control = netup_ci_write_cam_ctl;
	state->ca.slot_reset = netup_ci_slot_reset;
	state->ca.slot_shutdown = netup_ci_slot_shutdown;
	state->ca.slot_ts_enable = netup_ci_slot_ts_ctl;
	state->ca.poll_slot_status = netup_poll_ci_slot_status;
	state->ca.data = state;
	state->priv = port;
	state->current_irq_mode = ci_irq_flags() | NETUP_IRQ_DETAM;

	ret = netup_write_i2c(state->i2c_adap, state->ci_i2c_addr,
						0, &cimax_init[0], 34);
	/* lock registers */
	ret |= netup_write_i2c(state->i2c_adap, state->ci_i2c_addr,
						0x1f, &cimax_init[0x18], 1);
	/* powe