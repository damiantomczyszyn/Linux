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
       /* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * altera-ci.c
 *
 *  CI driver in conjunction with NetUp Dual DVB-T/C RF CI card
 *
 * Copyright (C) 2010 NetUP Inc.
 * Copyright (C) 2010 Igor M. Liplianin <liplianin@netup.ru>
 */
#ifndef __ALTERA_CI_H
#define __ALTERA_CI_H

#define ALT_DATA	0x000000ff
#define ALT_TDI		0x00008000
#define ALT_TDO		0x00004000
#define ALT_TCK		0x00002000
#define ALT_RDY		0x00001000
#define ALT_RD		0x00000800
#define ALT_WR		0x00000400
#define ALT_AD_RG	0x00000200
#define ALT_CS		0x00000100

struct altera_ci_config {
	void *dev;/* main dev, for example cx23885_dev */
	void *adapter;/* for CI to connect to */
	struct dvb_demux *demux;/* for hardware PID filter to connect to */
	int (*fpga_rw) (void *dev, int ad_rg, int val, int rw);
};

#if IS_REACHABLE(CONFIG_MEDIA_ALTERA_CI)

extern int altera_ci_init(struct altera_ci_config *config, int ci_nr);
extern void altera_ci_release(void *dev, int ci_nr);
extern int altera_ci_irq(void *dev);
extern int altera_ci_tuner_reset(void *dev, int ci_nr);

#else

static inline int altera_ci_init(struct altera_ci_config *config, int ci_nr)
{
	pr_warn("%s: driver disabled by Kconfig\n", __func__);
	return 0;
}

static inline void altera_ci_release(void *dev, int ci_nr)
{
	pr_warn("%s: driver disabled by Kconfig\n", __func__);
}

static inline int altera_ci_irq(void *dev)
{
	pr_warn("%s: driver disabled by Kconfig\n", __func__);
	return 0;
}

static inline int altera_ci_tuner_reset(void *dev, int ci_nr)
{
	pr_warn("%s: driver disabled by Kconfig\n", __func__);
	return 0;
}

#endif
#if 0
static inline int altera_hw_filt_init(struct altera_ci_config *config,
							int hw_filt_nr)
{
	pr_warn("%s: driver disabled by Kconfig\n", __func__);
	return 0;
}

static inline void altera_hw_filt_release(void *dev, int filt_nr)
{
	pr_warn("%s: driver disabled by Kconfig\n", __func__);
}

static inline int altera_pid_feed_control(void *dev, int filt_nr,
		struct dvb_demux_feed *dvbdmxfeed, int onoff)
{
	pr_warn("%s: driver disabled by Kconfig\n", __func__);
	return 0;
}

#endif /* CONFIG_MEDIA_ALTERA_CI */

#endif /* __ALTERA_CI_H */
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        /* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * altera-ci.c
 *
 *  CI driver in conjunction with NetUp Dual DVB-T/C RF CI card
 *
 * Copyright (C) 2010 NetUP Inc.
 * Copyright (C) 2010 Igor M. Liplianin <liplianin@netup.ru>
 */
#ifndef __ALTERA_CI_H
#define __ALTERA_CI_H

#define ALT_DATA	0x000000ff
#define ALT_TDI		0x00008000
#define ALT_TDO		0x00004000
#define ALT_TCK		0x00002000
#define ALT_RDY		0x00001000
#define ALT_RD		0x00000800
#define ALT_WR		0x00000400
#define ALT_AD_RG	0x00000200
#define ALT_CS		0x00000100

struct altera_ci_config {
	void *dev;/* main dev, for example cx23885_dev */
	void *adapter;/* for CI to connect to */
	struct dvb_demux *demux;/* for hardware PID filter to connect to */
	int (*fpga_rw) (void *dev, int ad_rg, int val, int rw);
};

#if IS_REACHABLE(CONFIG_MEDIA_ALTERA_CI)

extern int altera_ci_init(struct altera_ci_config *config, int ci_nr);
extern void altera_ci_release(void *dev, int ci_nr);
extern int altera_ci_irq(void *dev);
extern int altera_ci_tuner_reset(void *dev, int ci_nr);

#else

static inline int altera_ci_init(struct altera_ci_config *config, int ci_nr)
{
	pr_warn("%s: driver disabled by Kconfig\n", __func__);
	return 0;
}

static inline void altera_ci_release(void *dev, int ci_nr)
{
	pr_warn("%s: driver disabled by Kconfig\n", __func__);
}

static inline int altera_ci_irq(void *dev)
{
	pr_warn("%s: driver disabled by Kconfig\n", __func__);
	return 0;
}

static inline int altera_ci_tuner_reset(void *dev, int ci_nr)
{
	pr_warn("%s: driver disabled by Kconfig\n", __func__);
	return 0;
}

#endif
#if 0
static inline int altera_hw_filt_init(struct altera_ci_config *config,
							int hw_filt_nr)
{
	pr_warn("%s: driver disabled by Kconfig\n", __func__);
	return 0;
}

static inline void altera_hw_filt_release(void *dev, int filt_nr)
{
	pr_warn("%s: driver disabled by Kconfig\n", __func__);
}

static inline int altera_pid_feed_control(void *dev, int filt_nr,
		struct dvb_demux_feed *dvbdmxfeed, int onoff)
{
	pr_warn("%s: driver disabled by Kconfig\n", __func__);
	return 0;
}

#endif /* CONFIG_MEDIA_ALTERA_CI */

#endif /* __ALTERA_CI_H */
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        // SPDX-License-Identifier: GPL-2.0-or-later
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
	/* power on slots */
	ret |= netup_write_i2c(state->i2c_adap, state->ci_i2c_addr,
						0x18, &cimax_init[0x18], 1);

	if (0 != ret)
		goto err;

	ret = dvb_ca_en50221_init(&port->frontends.adapter,
				   &state->ca,
				   /* flags */ 0,
				   /* n_slots */ 1);
	if (0 != ret)
		goto err;

	INIT_WORK(&state->work, netup_read_ci_status);
	schedule_work(&state->work);

	ci_dbg_print("%s: CI initialized!\n", __func__);

	return 0;
err:
	ci_dbg_print("%s: Cannot initialize CI: Error %d.\n", __func__, ret);
	kfree(state);
	return ret;
}

void netup_ci_exit(struct cx23885_tsport *port)
{
	struct netup_ci_state *state;

	if (NULL == port)
		return;

	state = (struct netup_ci_state *)port->port_priv;
	if (NULL == state)
		return;

	if (NULL == state->ca.data)
		return;

	dvb_ca_en50221_release(&state->ca);
	kfree(state);
}
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       // SPDX-License-Identifier: GPL-2.0-or-later
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
	/* power on slots */
	ret |= netup_write_i2c(state->i2c_adap, state->ci_i2c_addr,
						0x18, &cimax_init[0x18], 1);

	if (0 != ret)
		goto err;

	ret = dvb_ca_en50221_init(&port->frontends.adapter,
				   &state->ca,
				   /* flags */ 0,
				   /* n_slots */ 1);
	if (0 != ret)
		goto err;

	INIT_WORK(&state->work, netup_read_ci_status);
	schedule_work(&state->work);

	ci_dbg_print("%s: CI initialized!\n", __func__);

	return 0;
err:
	ci_dbg_print("%s: Cannot initialize CI: Error %d.\n", __func__, ret);
	kfree(state);
	return ret;
}

void netup_ci_exit(struct cx23885_tsport *port)
{
	struct netup_ci_state *state;

	if (NULL == port)
		return;

	state = (struct netup_ci_state *)port->port_priv;
	if (NULL == state)
		return;

	if (NULL == state->ca.data)
		return;

	dvb_ca_en50221_release(&state->ca);
	kfree(state);
}
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       /* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * cimax2.h
 *
 * CIMax(R) SP2 driver in conjunction with NetUp Dual DVB-S2 CI card
 *
 * Copyright (C) 2009 NetUP Inc.
 * Copyright (C) 2009 Igor M. Liplianin <liplianin@netup.ru>
 * Copyright (C) 2009 Abylay Ospan <aospan@netup.ru>
 */

#ifndef CIMAX2_H
#define CIMAX2_H
#include <media/dvb_ca_en50221.h>

extern int netup_ci_read_attribute_mem(struct dvb_ca_en50221 *en50221,
						int slot, int addr);
extern int netup_ci_write_attribute_mem(struct dvb_ca_en50221 *en50221,
						int slot, int addr, u8 data);
extern int netup_ci_read_cam_ctl(struct dvb_ca_en50221 *en50221,
						int slot, u8 addr);
extern int netup_ci_write_cam_ctl(struct dvb_ca_en50221 *en50221,
						int slot, u8 addr, u8 data);
extern int netup_ci_slot_reset(struct dvb_ca_en50221 *en50221, int slot);
extern int netup_ci_slot_shutdown(struct dvb_ca_en50221 *en50221, int slot);
extern int netup_ci_slot_ts_ctl(struct dvb_ca_en50221 *en50221, int slot);
extern int netup_ci_slot_status(struct cx23885_dev *dev, u32 pci_status);
extern int netup_poll_ci_slot_status(struct dvb_ca_en50221 *en50221,
						int slot, int open);
extern int netup_ci_init(struct cx23885_tsport *port);
extern void netup_ci_exit(struct cx23885_tsport *port);

#endif
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        /* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * cimax2.h
 *
 * CIMax(R) SP2 driver in conjunction with NetUp Dual DVB-S2 CI card
 *
 * Copyright (C) 2009 NetUP Inc.
 * Copyright (C) 2009 Igor M. Liplianin <liplianin@netup.ru>
 * Copyright (C) 2009 Abylay Ospan <aospan@netup.ru>
 */

#ifndef CIMAX2_H
#define CIMAX2_H
#include <media/dvb_ca_en50221.h>

extern int netup_ci_read_attribute_mem(struct dvb_ca_en50221 *en50221,
						int slot, int addr);
extern int netup_ci_write_attribute_mem(struct dvb_ca_en50221 *en50221,
						int slot, int addr, u8 data);
extern int netup_ci_read_cam_ctl(struct dvb_ca_en50221 *en50221,
						int slot, u8 addr);
extern int netup_ci_write_cam_ctl(struct dvb_ca_en50221 *en50221,
						int slot, u8 addr, u8 data);
extern int netup_ci_slot_reset(struct dvb_ca_en50221 *en50221, int slot);
extern int netup_ci_slot_shutdown(struct dvb_ca_en50221 *en50221, int slot);
extern int netup_ci_slot_ts_ctl(struct dvb_ca_en50221 *en50221, int slot);
extern int netup_ci_slot_status(struct cx23885_dev *dev, u32 pci_status);
extern int netup_poll_ci_slot_status(struct dvb_ca_en50221 *en50221,
						int slot, int open);
extern int netup_ci_init(struct cx23885_tsport *port);
extern void netup_ci_exit(struct cx23885_tsport *port);

#endif
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        // SPDX-License-Identifier: GPL-2.0-or-later
/*
 *
 *  Support for a cx23417 mpeg encoder via cx23885 host port.
 *
 *    (c) 2004 Jelle Foks <jelle@foks.us>
 *    (c) 2004 Gerd Knorr <kraxel@bytesex.org>
 *    (c) 2008 Steven Toth <stoth@linuxtv.org>
 *      - CX23885/7/8 support
 *
 *  Includes parts from the ivtv driver <http://sourceforge.net/projects/ivtv/>
 */

#include "cx23885.h"
#include "cx23885-ioctl.h"

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/firmware.h>
#include <linux/slab.h>
#include <media/v4l2-common.h>
#include <media/v4l2-ioctl.h>
#include <media/drv-intf/cx2341x.h>

#define CX23885_FIRM_IMAGE_SIZE 376836
#define CX23885_FIRM_IMAGE_NAME "v4l-cx23885-enc.fw"

static unsigned int mpegbufs = 32;
module_param(mpegbufs, int, 0644);
MODULE_PARM_DESC(mpegbufs, "number of mpeg buffers, range 2-32");
static unsigned int mpeglines = 32;
module_param(mpeglines, int, 0644);
MODULE_PARM_DESC(mpeglines, "number of lines in an MPEG buffer, range 2-32");
static unsigned int mpeglinesize = 512;
module_param(mpeglinesize, int, 0644);
MODULE_PARM_DESC(mpeglinesize,
	"number of bytes in each line of an MPEG buffer, range 512-1024");

static unsigned int v4l_debug;
module_param(v4l_debug, int, 0644);
MODULE_PARM_DESC(v4l_debug, "enable V4L debug messages");

#define dprintk(level, fmt, arg...)\
	do { if (v4l_debug >= level) \
		printk(KERN_DEBUG pr_fmt("%s: 417:" fmt), \
			__func__, ##arg); \
	} while (0)

static struct cx23885_tvnorm cx23885_tvnorms[] = {
	{
		.name      = "NTSC-M",
		.id        = V4L2_STD_NTSC_M,
	}, {
		.name      = "NTSC-JP",
		.id        = V4L2_STD_NTSC_M_JP,
	}, {
		.name      = "PAL-BG",
		.id        = V4L2_STD_PAL_BG,
	}, {
		.name      = "PAL-DK",
		.id        = V4L2_STD_PAL_DK,
	}, {
		.name      = "PAL-I",
		.id        = V4L2_STD_PAL_I,
	}, {
		.name      = "PAL-M",
		.id        = V4L2_STD_PAL_M,
	}, {
		.name      = "PAL-N",
		.id        = V4L2_STD_PAL_N,
	}, {
		.name      = "PAL-Nc",
		.id        = V4L2_STD_PAL_Nc,
	}, {
		.name      = "PAL-60",
		.id        = V4L2_STD_PAL_60,
	}, {
		.name      = "SECAM-L",
		.id        = V4L2_STD_SECAM_L,
	}, {
		.name      = "SECAM-DK",
		.id        = V4L2_STD_SECAM_DK,
	}
};

/* ------------------------------------------------------------------ */
enum cx23885_capture_type {
	CX23885_MPEG_CAPTURE,
	CX23885_RAW_CAPTURE,
	CX23885_RAW_PASSTHRU_CAPTURE
};
enum cx23885_capture_bits {
	CX23885_RAW_BITS_NONE             = 0x00,
	CX23885_RAW_BITS_YUV_CAPTURE      = 0x01,
	CX23885_RAW_BITS_PCM_CAPTURE      = 0x02,
	CX23885_RAW_BITS_VBI_CAPTURE      = 0x04,
	CX23885_RAW_BITS_PASSTHRU_CAPTURE = 0x08,
	CX23885_RAW_BITS_TO_HOST_CAPTURE  = 0x10
};
enum cx23885_capture_end {
	CX23885_END_AT_GOP, /* stop at the end of gop, generate irq */
	CX23885_END_NOW, /* stop immediately, no irq */
};
enum cx23885_framerate {
	CX23885_FRAMERATE_NTSC_30, /* NTSC: 30fps */
	CX23885_FRAMERATE_PAL_25   /* PAL: 25fps */
};
enum cx23885_stream_port {
	CX23885_OUTPUT_PORT_MEMORY,
	CX23885_OUTPUT_PORT_STREAMING,
	CX23885_OUTPUT_PORT_SERIAL
};
enum cx23885_data_xfer_status {
	CX23885_MORE_BUFFERS_FOLLOW,
	CX23885_LAST_BUFFER,
};
enum cx23885_picture_mask {
	CX23885_PICTURE_MASK_NONE,
	CX23885_PICTURE_MASK_I_FRAMES,
	CX23885_PICTURE_MASK_I_P_FRAMES = 0x3,
	CX23885_PICTURE_MASK_ALL_FRAMES = 0x7,
};
enum cx23885_vbi_mode_bits {
	CX23885_VBI_BITS_SLICED,
	CX23885_VBI_BITS_RAW,
};
enum cx23885_vbi_insertion_bits {
	CX23885_VBI_BITS_INSERT_IN_XTENSION_USR_DATA,
	CX23885_VBI_BITS_INSERT_IN_PRIVATE_PACKETS = 0x1 << 1,
	CX23885_VBI_BITS_SEPARATE_STREAM = 0x2 << 1,
	CX23885_VBI_BITS_SEPARATE_STREAM_USR_DATA = 0x4 << 1,
	CX23885_VBI_BITS_SEPARATE_STREAM_PRV_DATA = 0x5 << 1,
};
enum cx23885_dma_unit {
	CX23885_DMA_BYTES,
	CX23885_DMA_FRAMES,
};
enum cx23885_dma_transfer_status_bits {
	CX23885_DMA_TRANSFER_BITS_DONE = 0x01,
	CX23885_DMA_TRANSFER_BITS_ERROR = 0x04,
	CX23885_DMA_TRANSFER_BITS_LL_ERROR = 0x10,
};
enum cx23885_pause {
	CX23885_PAUSE_ENCODING,
	CX23885_RESUME_ENCODING,
};
enum cx23885_copyright {
	CX23885_COPYRIGHT_OFF,
	CX23885_COPYRIGHT_ON,
};
enum cx23885_notification_type {
	CX23885_NOTIFICATION_REFRESH,
};
enum cx23885_notification_status {
	CX23885_NOTIFICATION_OFF,
	CX23885_NOTIFICATION_ON,
};
enum cx23885_notification_mailbox {
	CX23885_NOTIFICATION_NO_MAILBOX = -1,
};
enum cx23885_field1_lines {
	CX23885_FIELD1_SAA7114 = 0x00EF, /* 239 */
	CX23885_FIELD1_SAA7115 = 0x00F0, /* 240 */
	CX23885_FIELD1_MICRONAS = 0x0105, /* 261 */
};
enum cx23885_field2_lines {
	CX23885_FIELD2_SAA7114 = 0x00EF, /* 239 */
	CX23885_FIELD2_SAA7115 = 0x00F0, /* 240 */
	CX23885_FIELD2_MICRONAS = 0x0106, /* 262 */
};
enum cx23885_custom_data_type {
	CX23885_CUSTOM_EXTENSION_USR_DATA,
	CX23885_CUSTOM_PRIVATE_PACKET,
};
enum cx23885_mute {
	CX23885_UNMUTE,
	CX23885_MUTE,
};
enum cx23885_mute_video_mask {
	CX23885_MUTE_VIDEO_V_MASK = 0x0000FF00,
	CX23885_MUTE_VIDEO_U_MASK = 0x00FF0000,
	CX23885_MUTE_VIDEO_Y_MASK = 0xFF000000,
};
enum cx23885_mute_video_shift {
	CX23885_MUTE_VIDEO_V_SHIFT = 8,
	CX23885_MUTE_VIDEO_U_SHIFT = 16,
	CX23885_MUTE_VIDEO_Y_SHIFT = 24,
};

/* defines below are from ivtv-driver.h */
#define IVTV_CMD_HW_BLOCKS_RST 0xFFFFFFFF

/* Firmware API commands */
#define IVTV_API_STD_TIMEOUT 500

/* Registers */
/* IVTV_REG_OFFSET */
#define IVTV_REG_ENC_SDRAM_REFRESH (0x07F8)
#define IVTV_REG_ENC_SDRAM_PRECHARGE (0x07FC)
#define IVTV_REG_SPU (0x9050)
#define IVTV_REG_HW_BLOCKS (0x9054)
#define IVTV_REG_VPU (0x9058)
#define IVTV_REG_APU (0xA064)

/**** Bit definitions for MC417_RWD and MC417_OEN registers  ***
  bits 31-16
+-----------+
| Reserved  |
+-----------+
  bit 15  bit 14  bit 13 bit 12  bit 11  bit 10  bit 9   bit 8
+-------+-------+-------+-------+-------+-------+-------+-------+
| MIWR# | MIRD# | MICS# |MIRDY# |MIADDR3|MIADDR2|MIADDR1|MIADDR0|
+-------+-------+-------+-------+-------+-------+-------+-------+
 bit 7   bit 6   bit 5   bit 4   bit 3   bit 2   bit 1   bit 0
+-------+-------+-------+-------+-------+-------+-------+-------+
|MIDATA7|MIDATA6|MIDATA5|MIDATA4|MIDATA3|MIDATA2|MIDATA1|MIDATA0|
+-------+-------+-------+-------+-------+-------+-------+-------+
***/
#define MC417_MIWR	0x8000
#define MC417_MIRD	0x4000
#define MC417_MICS	0x2000
#define MC417_MIRDY	0x1000
#define MC417_MIADDR	0x0F00
#define MC417_MIDATA	0x00FF

/* MIADDR* nibble definitions */
#define  MCI_MEMORY_DATA_BYTE0          0x000
#define  MCI_MEMORY_DATA_BYTE1          0x100
#define  MCI_MEMORY_DATA_BYTE2          0x200
#define  MCI_MEMORY_DATA_BYTE3          0x300
#define  MCI_MEMORY_ADDRESS_BYTE2       0x400
#define  MCI_MEMORY_ADDRESS_BYTE1       0x500
#define  MCI_MEMORY_ADDRESS_BYTE0       0x600
#define  MCI_REGISTER_DATA_BYTE0        0x800
#define  MCI_REGISTER_DATA_BYTE1        0x900
#define  MCI_REGISTER_DATA_BYTE2        0xA00
#define  MCI_REGISTER_DATA_BYTE3        0xB00
#define  MCI_REGISTER_ADDRESS_BYTE0     0xC00
#define  MCI_REGISTER_ADDRESS_BYTE1     0xD00
#define  MCI_REGISTER_MODE              0xE00

/* Read and write modes */
#define  MCI_MODE_REGISTER_READ         0
#define  MCI_MODE_REGISTER_WRITE        1
#define  MCI_MODE_MEMORY_READ           0
#define  MCI_MODE_MEMORY_WRITE          0x40

/*** Bit definitions for MC417_CTL register ****
 bits 31-6   bits 5-4   bit 3    bits 2-1       Bit 0
+--------+-------------+--------+--------------+------------+
|Reserved|MC417_SPD_CTL|Reserved|MC417_GPIO_SEL|UART_GPIO_EN|
+--------+-------------+--------+--------------+------------+
***/
#define MC417_SPD_CTL(x)	(((x) << 4) & 0x00000030)
#define MC417_GPIO_SEL(x)	(((x) << 1) & 0x00000006)
#define MC417_UART_GPIO_EN	0x00000001

/* Values for speed control */
#define MC417_SPD_CTL_SLOW	0x1
#define MC417_SPD_CTL_MEDIUM	0x0
#define MC417_SPD_CTL_FAST	0x3     /* b'1x, but we use b'11 */

/* Values for GPIO select */
#define MC417_GPIO_SEL_GPIO3	0x3
#define MC417_GPIO_SEL_GPIO2	0x2
#define MC417_GPIO_SEL_GPIO1	0x1
#define MC417_GPIO_SEL_GPIO0	0x0

void cx23885_mc417_init(struct cx23885_dev *dev)
{
	u32 regval;

	dprintk(2, "%s()\n", __func__);

	/* Configure MC417_CTL register to defaults. */
	regval = MC417_SPD_CTL(MC417_SPD_CTL_FAST)	|
		 MC417_GPIO_SEL(MC417_GPIO_SEL_GPIO3)	|
		 MC417_UART_GPIO_EN;
	cx_write(MC417_CTL, regval);

	/* Configure MC417_OEN to defaults. */
	regval = MC417_MIRDY;
	cx_write(MC417_OEN, regval);

	/* Configure MC417_RWD to defaults. */
	regval = MC417_MIWR | MC417_MIRD | MC417_MICS;
	cx_write(MC417_RWD, regval);
}

static int mc417_wait_ready(struct cx23885_dev *dev)
{
	u32 mi_ready;
	unsigned long timeout = jiffies + msecs_to_jiffies(1);

	for (;;) {
		mi_ready = cx_read(MC417_RWD) & MC417_MIRDY;
		if (mi_ready != 0)
			return 0;
		if (time_after(jiffies, timeout))
			return -1;
		udelay(1);
	}
}

int mc417_register_write(struct cx23885_dev *dev, u16 address, u32 value)
{
	u32 regval;

	/* Enable MC417 GPIO outputs except for MC417_MIRDY,
	 * which is an input.
	 */
	cx_write(MC417_OEN, MC417_MIRDY);

	/* Write data byte 0 */
	regval = MC417_MIRD | MC417_MIRDY | MCI_REGISTER_DATA_BYTE0 |
		(value & 0x000000FF);
	cx_write(MC417_RWD, regval);

	/* Transition CS/WR to effect write transaction across bus. */
	regval |= MC417_MICS | MC417_MIWR;
	cx_write(MC417_RWD, regval);

	/* Write data byte 1 */
	regval = MC417_MIRD | MC417_MIRDY | MCI_REGISTER_DATA_BYTE1 |
		((value >> 8) & 0x000000FF);
	cx_write(MC417_RWD, regval);
	regval |= MC417_MICS | MC417_MIWR;
	cx_write(MC417_RWD, regval);

	/* Write data byte 2 */
	regval = MC417_MIRD | MC417_MIRDY | MCI_REGISTER_DATA_BYTE2 |
		((value >> 16) & 0x000000FF);
	cx_write(MC417_RWD, regval);
	regval |= MC417_MICS | MC417_MIWR;
	cx_write(MC417_RWD, regval);

	/* Write data byte 3 */
	regval = MC417_MIRD | MC417_MIRDY | MCI_REGISTER_DATA_BYTE3 |
		((value >> 24) & 0x000000FF);
	cx_write(MC417_RWD, regval);
	regval |= MC417_MICS | MC417_MIWR;
	cx_write(MC417_RWD, regval);

	/* Write address byte 0 */
	regval = MC417_MIRD | MC417_MIRDY | MCI_REGISTER_ADDRESS_BYTE0 |
		(address & 0xFF);
	cx_write(MC417_RWD, regval);
	regval |= MC417_MICS | MC417_MIWR;
	cx_write(MC417_RWD, regval);

	/* Write address byte 1 */
	regval = MC417_MIRD | MC417_MIRDY | MCI_REGISTER_ADDRESS_BYTE1 |
		((address >> 8) & 0xFF);
	cx_write(MC417_RWD, regval);
	regval |= MC417_MICS | MC417_MIWR;
	cx_write(MC417_RWD, regval);

	/* Indicate that this is a write. */
	regval = MC417_MIRD | MC417_MIRDY | MCI_REGISTER_MODE |
		MCI_MODE_REGISTER_WRITE;
	cx_write(MC417_RWD, regval);
	regval |= MC417_MICS | MC417_MIWR;
	cx_write(MC417_RWD, regval);

	/* Wait for the trans to complete (MC417_MIRDY asserted). */
	return mc417_wait_ready(dev);
}

int mc417_register_read(struct cx23885_dev *dev, u16 address, u32 *value)
{
	int retval;
	u32 regval;
	u32 tempval;
	u32 dataval;

	/* Enable MC417 GPIO outputs except for MC417_MIRDY,
	 * which is an input.
	 */
	cx_write(MC417_OEN, MC417_MIRDY);

	/* Write address byte 0 */
	regval = MC417_MIRD | MC417_MIRDY | MCI_REGISTER_ADDRESS_BYTE0 |
		((address & 0x00FF));
	cx_write(MC417_RWD, regval);
	regval |= MC417_MICS | MC417_MIWR;
	cx_write(MC417_RWD, regval);

	/* Write address byte 1 */
	regval = MC417_MIRD | MC417_MIRDY | MCI_REGISTER_ADDRESS_BYTE1 |
		((address >> 8) & 0xFF);
	cx_write(MC417_RWD, regval);
	regval |= MC417_MICS | MC417_MIWR;
	cx_write(MC417_RWD, regval);

	/* Indicate that this is a register read. */
	regval = MC417_MIRD | MC417_MIRDY | MCI_REGISTER_MODE |
		MCI_MODE_REGISTER_READ;
	cx_write(MC417_RWD, regval);
	regval |= MC417_MICS | MC417_MIWR;
	cx_write(MC417_RWD, regval);

	/* Wait for the trans to complete (MC417_MIRDY asserted). */
	retval = mc417_wait_ready(dev);

	/* switch the DAT0-7 GPIO[10:3] to input mode */
	cx_write(MC417_OEN, MC417_MIRDY | MC417_MIDATA);

	/* Read data byte 0 */
	regval = MC417_MIRD | MC417_MIRDY | MCI_REGISTER_DATA_BYTE0;
	cx_write(MC417_RWD, regval);

	/* Transition RD to effect read transaction across bus.
	 * Transition 0x5000 -> 0x9000 correct (RD/RDY -> WR/RDY)?
	 * Should it be 0x9000 -> 0xF000 (also why is RDY being set, its
	 * input only...)
	 */
	regval = MC417_MIWR | MC417_MIRDY | MCI_REGISTER_DATA_BYTE0;
	cx_write(MC417_RWD, regval);

	/* Collect byte */
	tempval = cx_read(MC417_RWD);
	dataval = tempval & 0x000000FF;

	/* Bring CS and RD high. */
	regval = MC417_MIWR | MC417_MIRD | MC417_MICS | MC417_MIRDY;
	cx_write(MC417_RWD, regval);

	/* Read data byte 1 */
	regval = MC417_MIRD | MC417_MIRDY | MCI_REGISTER_DATA_BYTE1;
	cx_write(MC417_RWD, regval);
	regval = MC417_MIWR | MC417_MIRDY | MCI_REGISTER_DATA_BYTE1;
	cx_write(MC417_RWD, regval);
	tempval = cx_read(MC417_RWD);
	dataval |= ((tempval & 0x000000FF) << 8);
	regval = MC417_MIWR | MC417_MIRD | MC417_MICS | MC417_MIRDY;
	cx_write(MC417_RWD, regval);

	/* Read data byte 2 */
	regval = MC417_MIRD | MC417_MIRDY | MCI_REGISTER_DATA_BYTE2;
	cx_write(MC417_RWD, regval);
	regval = MC417_MIWR | MC417_MIRDY | MCI_REGISTER_DATA_BYTE2;
	cx_write(MC417_RWD, regval);
	tempval = cx_read(MC417_RWD);
	dataval |= ((tempval & 0x000000FF) << 16);
	regval = MC417_MIWR | MC417_MIRD | MC417_MICS | MC417_MIRDY;
	cx_write(MC417_RWD, regval);

	/* Read data byte 3 */
	regval = MC417_MIRD | MC417_MIRDY | MCI_REGISTER_DATA_BYTE3;
	cx_write(MC417_RWD, regval);
	regval = MC417_MIWR | MC417_MIRDY | MCI_REGISTER_DATA_BYTE3;
	cx_write(MC417_RWD, regval);
	tempval = cx_read(MC417_RWD);
	dataval |= ((tempval & 0x000000FF) << 24);
	regval = MC417_MIWR | MC417_MIRD | MC417_MICS | MC417_MIRDY;
	cx_write(MC417_RWD, regval);

	*value  = dataval;

	return retval;
}

int mc417_memory_write(struct cx23885_dev *dev, u32 address, u32 value)
{
	u32 regval;

	/* Enable MC417 GPIO outputs except for MC417_MIRDY,
	 * which is an input.
	 */
	cx_write(MC417_OEN, MC417_MIRDY);

	/* Write data byte 0 */
	regval = MC417_MIRD | MC417_MIRDY | MCI_MEMORY_DATA_BYTE0 |
		(value & 0x000000FF);
	cx_write(MC417_RWD, regval);

	/* Transition CS/WR to effect write transaction across bus. */
	regval |= MC417_MICS | MC417_MIWR;
	cx_write(MC417_RWD, regval);

	/* Write data byte 1 */
	regval = MC417_MIRD | MC417_MIRDY | MCI_MEMORY_DATA_BYTE1 |
		((value >> 8) & 0x000000FF);
	cx_write(MC417_RWD, regval);
	regval |= MC417_MICS | MC417_MIWR;
	cx_write(MC417_RWD, regval);

	/* Write data byte 2 */
	regval = MC417_MIRD | MC417_MIRDY | MCI_MEMORY_DATA_BYTE2 |
		((value >> 16) & 0x000000FF);
	cx_write(MC417_RWD, regval);
	regval |= MC417_MICS | MC417_MIWR;
	cx_write(MC417_RWD, regval);

	/* Write data byte 3 */
	regval = MC417_MIRD | MC417_MIRDY | MCI_MEMORY_DATA_BYTE3 |
		((value >> 24) & 0x000000FF);
	cx_write(MC417_RWD, regval);
	regval |= MC417_MICS | MC417_MIWR;
	cx_write(MC417_RWD, regval);

	/* Write address byte 2 */
	regval = MC417_MIRD | MC417_MIRDY | MCI_MEMORY_ADDRESS_BYTE2 |
		MCI_MODE_MEMORY_WRITE | ((address >> 16) & 0x3F);
	cx_write(MC417_RWD, regval);
	regval |= MC417_MICS | MC417_MIWR;
	cx_write(MC417_RWD, regval);

	/* Write address byte 1 */
	regval = MC417_MIRD | MC417_MIRDY | MCI_MEMORY_ADDRESS_BYTE1 |
		((address >> 8) & 0xFF);
	cx_write(MC417_RWD, regval);
	regval |= MC417_MICS | MC417_MIWR;
	cx_write(MC417_RWD, regval);

	/* Write address byte 0 */
	regval = MC417_MIRD | MC417_MIRDY | MCI_MEMORY_ADDRESS_BYTE0 |
		(address & 0xFF);
	cx_write(MC417_RWD, regval);
	regval |= MC417_MICS | MC417_MIWR;
	cx_write(MC417_RWD, regval);

	/* Wait for the trans to complete (MC417_MIRDY asserted). */
	return mc417_wait_ready(dev);
}

int mc417_memory_read(struct cx23885_dev *dev, u32 address, u32 *value)
{
	int retval;
	u32 regval;
	u32 tempval;
	u32 dataval;

	/* Enable MC417 GPIO outputs except for MC417_MIRDY,
	 * which is an input.
	 */
	cx_write(MC417_OEN, MC417_MIRDY);

	/* Write address byte 2 */
	regval = MC417_MIRD | MC417_MIRDY | MCI_MEMORY_ADDRESS_BYTE2 |
		MCI_MODE_MEMORY_READ | ((address >> 16) & 0x3F);
	cx_write(MC417_RWD, regval);
	regval |= MC417_MICS | MC417_MIWR;
	cx_write(MC417_RWD, regval);

	/* Write address byte 1 */
	regval = MC417_MIRD | MC417_MIRDY | MCI_MEMORY_ADDRESS_BYTE1 |
		((address >> 8) & 0xFF);
	cx_write(MC417_RWD, regval);
	regval |= MC417_MICS | MC417_MIWR;
	cx_write(MC417_RWD, regval);

	/* Write address byte 0 */
	regval = MC417_MIRD | MC417_MIRDY | MCI_MEMORY_ADDRESS_BYTE0 |
		(address & 0xFF);
	cx_write(MC417_RWD, regval);
	regval |= MC417_MICS | MC417_MIWR;
	cx_write(MC417_RWD, regval);

	/* Wait for the trans to complete (MC417_MIRDY asserted). */
	retval = mc417_wait_ready(dev);

	/* switch the DAT0-7 GPIO[10:3] to input mode */
	cx_write(MC417_OEN, MC417_MIRDY | MC417_MIDATA);

	/* Read data byte 3 */
	regval = MC417_MIRD | MC417_MIRDY | MCI_MEMORY_DATA_BYTE3;
	cx_write(MC417_RWD, regval);

	/* Transition RD to effect read transaction across bus. */
	regval = MC417_MIWR | MC417_MIRDY | MCI_MEMORY_DATA_BYTE3;
	cx_write(MC417_RWD, regval);

	/* Collect byte */
	tempval = cx_read(MC417_RWD);
	dataval = ((tempval & 0x000000FF) << 24);

	/* Bring CS and RD high. */
	regval = MC417_MIWR | MC417_MIRD | MC417_MICS | MC417_MIRDY;
	cx_write(MC417_RWD, regval);

	/* Read data byte 2 */
	regval = MC417_MIRD | MC417_MIRDY | MCI_MEMORY_DATA_BYTE2;
	cx_write(MC417_RWD, regval);
	regval = MC417_MIWR | MC417_MIRDY | MCI_MEMORY_DATA_BYTE2;
	cx_write(MC417_RWD, regval);
	tempval = cx_read(MC417_RWD);
	dataval |= ((tempval & 0x000000FF) << 16);
	regval = MC417_MIWR | MC417_MIRD | MC417_MICS | MC417_MIRDY;
	cx_write(MC417_RWD, regval);

	/* Read data byte 1 */
	regval = MC417_MIRD | MC417_MIRDY | MCI_MEMORY_DATA_BYTE1;
	cx_write(MC417_RWD, regval);
	regval = MC417_MIWR | MC417_MIRDY | MCI_MEMORY_DATA_BYTE1;
	cx_write(MC417_RWD, regval);
	tempval = cx_read(MC417_RWD);
	dataval |= ((tempval & 0x000000FF) << 8);
	regval = MC417_MIWR | MC417_MIRD | MC417_MICS | MC417_MIRDY;
	cx_write(MC417_RWD, regval);

	/* Read data byte 0 */
	regval = MC417_MIRD | MC417_MIRDY | MCI_MEMORY_DATA_BYTE0;
	cx_write(MC417_RWD, regval);
	regval = MC417_MIWR | MC417_MIRDY | MCI_MEMORY_DATA_BYTE0;
	cx_write(MC417_RWD, regval);
	tempval = cx_read(MC417_RWD);
	dataval |= (tempval & 0x000000FF);
	regval = MC417_MIWR | MC417_MIRD | MC417_MICS | MC417_MIRDY;
	cx_write(MC417_RWD, regval);

	*value  = dataval;

	return retval;
}

void mc417_gpio_set(struct cx23885_dev *dev, u32 mask)
{
	u32 val;

	/* Set the gpio value */
	mc417_register_read(dev, 0x900C, &val);
	val |= (mask & 0x000ffff);
	mc417_register_write(dev, 0x900C, val);
}

void mc417_gpio_clear(struct cx23885_dev *dev, u32 mask)
{
	u32 val;

	/* Clear the gpio value */
	mc417_register_read(dev, 0x900C, &val);
	val &= ~(mask & 0x0000ffff);
	mc417_register_write(dev, 0x900C, val);
}

void mc417_gpio_enable(struct cx23885_dev *dev, u32 mask, int asoutput)
{
	u32 val;

	/* Enable GPIO direction bits */
	mc417_register_read(dev, 0x9020, &val);
	if (asoutput)
		val |= (mask & 0x0000ffff);
	else
		val &= ~(mask & 0x0000ffff);

	mc417_register_write(dev, 0x9020, val);
}
/* ------------------------------------------------------------------ */

/* MPEG encoder API */
static char *cmd_to_str(int cmd)
{
	switch (cmd) {
	case CX2341X_ENC_PING_FW:
		return  "PING_FW";
	case CX2341X_ENC_START_CAPTURE:
		return  "START_CAPTURE";
	case CX2341X_ENC_STOP_CAPTURE:
		return  "STOP_CAPTURE";
	case CX2341X_ENC_SET_AUDIO_ID:
		return  "SET_AUDIO_ID";
	case CX2341X_ENC_SET_VIDEO_ID:
		return  "SET_VIDEO_ID";
	case CX2341X_ENC_SET_PCR_ID:
		return  "SET_PCR_ID";
	case CX2341X_ENC_SET_FRAME_RATE:
		return  "SET_FRAME_RATE";
	case CX2341X_ENC_SET_FRAME_SIZE:
		return  "SET_FRAME_SIZE";
	case CX2341X_ENC_SET_BIT_RATE:
		return  "SET_BIT_RATE";
	case CX2341X_ENC_SET_GOP_PROPERTIES:
		return  "SET_GOP_PROPERTIES";
	case CX2341X_ENC_SET_ASPECT_RATIO:
		return  "SET_ASPECT_RATIO";
	case CX2341X_ENC_SET_DNR_FILTER_MODE:
		return  "SET_DNR_FILTER_MODE";
	case CX2341X_ENC_SET_DNR_FILTER_PROPS:
		return  "SET_DNR_FILTER_PROPS";
	case CX2341X_ENC_SET_CORING_LEVELS:
		return  "SET_CORING_LEVELS";
	case CX2341X_ENC_SET_SPATIAL_FILTER_TYPE:
		return  "SET_SPATIAL_FILTER_TYPE";
	case CX2341X_ENC_SET_VBI_LINE:
		return  "SET_VBI_LINE";
	case CX2341X_ENC_SET_STREAM_TYPE:
		return  "SET_STREAM_TYPE";
	case CX2341X_ENC_SET_OUTPUT_PORT:
		return  "SET_OUTPUT_PORT";
	case CX2341X_ENC_SET_AUDIO_PROPERTIES:
		return  "SET_AUDIO_PROPERTIES";
	case CX2341X_ENC_HALT_FW:
		return  "HALT_FW";
	case CX2341X_ENC_GET_VERSION:
		return  "GET_VERSION";
	case CX2341X_ENC_SET_GOP_CLOSURE:
		return  "SET_GOP_CLOSURE";
	case CX2341X_ENC_GET_SEQ_END:
		return  "GET_SEQ_END";
	case CX2341X_ENC_SET_PGM_INDEX_INFO:
		return  "SET_PGM_INDEX_INFO";
	case CX2341X_ENC_SET_VBI_CONFIG:
		return  "SET_VBI_CONFIG";
	case CX2341X_ENC_SET_DMA_BLOCK_SIZE:
		return  "SET_DMA_BLOCK_SIZE";
	case CX2341X_ENC_GET_PREV_DMA_INFO_MB_10:
		return  "GET_PREV_DMA_INFO_MB_10";
	case CX2341X_ENC_GET_PREV_DMA_INFO_MB_9:
		return  "GET_PREV_DMA_INFO_MB_9";
	case CX2341X_ENC_SCHED_DMA_TO_HOST:
		return  "SCHED_DMA_TO_HOST";
	case CX2341X_ENC_INITIALIZE_INPUT:
		return  "INITIALIZE_INPUT";
	case CX2341X_ENC_SET_FRAME_DROP_RATE:
		return  "SET_FRAME_DROP_RATE";
	case CX2341X_ENC_PAUSE_ENCODER:
		return  "PAUSE_ENCODER";
	case CX2341X_ENC_REFRESH_INPUT:
		return  "REFRESH_INPUT";
	case CX2341X_ENC_SET_COPYRIGHT:
		return  "SET_COPYRIGHT";
	case CX2341X_ENC_SET_EVENT_NOTIFICATION:
		return  "SET_EVENT_NOTIFICATION";
	case CX2341X_ENC_SET_NUM_VSYNC_LINES:
		return  "SET_NUM_VSYNC_LINES";
	case CX2341X_ENC_SET_PLACEHOLDER:
		return  "SET_PLACEHOLDER";
	case CX2341X_ENC_MUTE_VIDEO:
		return  "MUTE_VIDEO";
	case CX2341X_ENC_MUTE_AUDIO:
		return  "MUTE_AUDIO";
	case CX2341X_ENC_MISC:
		return  "MISC";
	default:
		return "UNKNOWN";
	}
}

static int cx23885_mbox_func(void *priv,
			     u32 command,
			     int in,
			     int out,
			     u32 data[CX2341X_MBOX_MAX_DATA])
{
	struct cx23885_dev *dev = priv;
	unsigned long timeout;
	u32 value, flag, retval = 0;
	int i;

	dprintk(3, "%s: command(0x%X) = %s\n", __func__, command,
		cmd_to_str(command));

	/* this may not be 100% safe if we can't read any memory location
	   without side effects */
	mc417_memory_read(dev, dev->cx23417_mailbox - 4, &value);
	if (value != 0x12345678) {
		pr_err("Firmware and/or mailbox pointer not initialized or corrupted, signature = 0x%x, cmd = %s\n",
			value, cmd_to_str(command));
		return -1;
	}

	/* This read looks at 32 bits, but flag is only 8 bits.
	 * Seems we also bail if CMD or TIMEOUT bytes are set???
	 */
	mc417_memory_read(dev, dev->cx23417_mailbox, &flag);
	if (flag) {
		pr_err("ERROR: Mailbox appears to be in use (%x), cmd = %s\n",
		       flag, cmd_to_str(command));
		return -1;
	}

	flag |= 1; /* tell 'em we're working on it */
	mc417_memory_write(dev, dev->cx23417_mailbox, flag);

	/* write command + args + fill remaining with zeros */
	/* command code */
	mc417_memory_write(dev, dev->cx23417_mailbox + 1, command);
	mc417_memory_write(dev, dev->cx23417_mailbox + 3,
		IVTV_API_STD_TIMEOUT); /* timeout */
	for (i = 0; i < in; i++) {
		mc417_memory_write(dev, dev->cx23417_mailbox + 4 + i, data[i]);
		dprintk(3, "API Input %d = %d\n", i, data[i]);
	}
	for (; i < CX2341X_MBOX_MAX_DATA; i++)
		mc417_memory_write(dev, dev->cx23417_mailbox + 4 + i, 0);

	flag |= 3; /* tell 'em we're done writing */
	mc417_memory_write(dev, dev->cx23417_mailbox, flag);

	/* wait for firmware to handle the API command */
	timeout = jiffies + msecs_to_jiffies(10);
	for (;;) {
		mc417_memory_read(dev, dev->cx23417_mailbox, &flag);
		if (0 != (flag & 4))
			break;
		if (time_after(jiffies, timeout)) {
			pr_err("ERROR: API Mailbox timeout\n");
			return -1;
		}
		udelay(10);
	}

	/* read output values */
	for (i = 0; i < out; i++) {
		mc417_memory_read(dev, dev->cx23417_mailbox + 4 + i, data + i);
		dprintk(3, "API Output %d = %d\n", i, data[i]);
	}

	mc417_memory_read(dev, dev->cx23417_mailbox + 2, &retval);
	dprintk(3, "API result = %d\n", retval);

	flag = 0;
	mc417_memory_write(dev, dev->cx23417_mailbox, flag);

	return retval;
}

/* We don't need to call the API often, so using just one
 * mailbox will probably suffice
 */
static int cx23885_api_cmd(struct cx23885_dev *dev,
			   u32 command,
			   u32 inputcnt,
			   u32 outputcnt,
			   ...)
{
	u32 data[CX2341X_MBOX_MAX_DATA];
	va_list vargs;
	int i, err;

	dprintk(3, "%s() cmds = 0x%08x\n", __func__, command);

	va_start(vargs, outputcnt);
	for (i = 0; i < inputcnt; i++)
		data[i] = va_arg(vargs, int);

	err = cx23885_mbox_func(dev, command, inputcnt, outputcnt, data);
	for (i = 0; i < outputcnt; i++) {
		int *vptr = va_arg(vargs, int *);
		*vptr = data[i];
	}
	va_end(vargs);

	return err;
}

static int cx23885_api_func(void *priv, u32 cmd, int in, int out, u32 data[CX2341X_MBOX_MAX_DATA])
{
	return cx23885_mbox_func(priv, cmd, in, out, data);
}

static int cx23885_find_mailbox(struct cx23885_dev *dev)
{
	u32 signature[4] = {
		0x12345678, 0x34567812, 0x56781234, 0x78123456
	};
	int signaturecnt = 0;
	u32 value;
	int i;

	dprintk(2, "%s()\n", __func__);

	for (i = 0; i < CX23885_FIRM_IMAGE_SIZE; i++) {
		mc417_memory_read(dev, i, &value);
		if (value == signature[signaturecnt])
			signaturecnt++;
		else
			signaturecnt = 0;
		if (4 == signaturecnt) {
			dprintk(1, "Mailbox signature found at 0x%x\n", i+1);
			return i+1;
		}
	}
	pr_err("Mailbox signature values not found!\n");
	return -1;
}

static int cx23885_load_firmware(struct cx23885_dev *dev)
{
	static const unsigned char magic[8] = {
		0xa7, 0x0d, 0x00, 0x00, 0x66, 0xbb, 0x55, 0xaa
	};
	const struct firmware *firmware;
	int i, retval = 0;
	u32 value = 0;
	u32 gpio_output = 0;
	u32 gpio_value;
	u32 checksum = 0;
	u32 *dataptr;

	dprintk(2, "%s()\n", __func__);

	/* Save GPIO settings before reset of APU */
	retval |= mc417_memory_read(dev, 0x9020, &gpio_output);
	retval |= mc417_memory_read(dev, 0x900C, &gpio_value);

	retval  = mc417_register_write(dev,
		IVTV_REG_VPU, 0xFFFFFFED);
	retval |= mc417_register_write(dev,
		IVTV_REG_HW_BLOCKS, IVTV_CMD_HW_BLOCKS_RST);
	retval |= mc417_register_write(dev,
		IVTV_REG_ENC_SDRAM_REFRESH, 0x80000800);
	retval |= mc417_register_write(dev,
		IVTV_REG_ENC_SDRAM_PRECHARGE, 0x1A);
	retval |= mc417_register_write(dev,
		IVTV_REG_APU, 0);

	if (retval != 0) {
		pr_err("%s: Error with mc417_register_write\n",
			__func__);
		return -1;
	}

	retval = request_firmware(&firmware, CX23885_FIRM_IMAGE_NAME,
				  &dev->pci->dev);

	if (retval != 0) {
		pr_err("ERROR: Hotplug firmware request failed (%s).\n",
		       CX23885_FIRM_IMAGE_NAME);
		pr_err("Please fix your hotplug setup, the board will not work without firmware loaded!\n");
		return -1;
	}

	if (firmware->size != CX23885_FIRM_IMAGE_SIZE) {
		pr_err("ERROR: Firmware size mismatch (have %zu, expected %d)\n",
		       firmware->size, CX23885_FIRM_IMAGE_SIZE);
		release_firmware(firmware);
		return -1;
	}

	if (0 != memcmp(firmware->data, magic, 8)) {
		pr_err("ERROR: Firmware magic mismatch, wrong file?\n");
		release_firmware(firmware);
		return -1;
	}

	/* transfer to the chip */
	dprintk(2, "Loading firmware ...\n");
	dataptr = (u32 *)firmware->data;
	for (i = 0; i < (firmware->size >> 2); i++) {
		value = *dataptr;
		checksum += ~value;
		if (mc417_memory_write(dev, i, value) != 0) {
			pr_err("ERROR: Loading firmware failed!\n");
			release_firmware(firmware);
			return -1;
		}
		dataptr++;
	}

	/* read back to verify with the checksum */
	dprintk(1, "Verifying firmware ...\n");
	for (i--; i >= 0; i--) {
		if (mc417_memory_read(dev, i, &value) != 0) {
			pr_err("ERROR: Reading firmware failed!\n");
			release_firmware(firmware);
			return -1;
		}
		checksum -= ~value;
	}
	if (checksum) {
		pr_err("ERROR: Firmware load failed (checksum mismatch).\n");
		release_firmware(firmware);
		return -1;
	}
	release_firmware(firmware);
	dprintk(1, "Firmware upload successful.\n");

	retval |= mc417_register_write(dev, IVTV_REG_HW_BLOCKS,
		IVTV_CMD_HW_BLOCKS_RST);

	/* F/W power up disturbs the GPIOs, restore state */
	retval |= mc417_register_write(dev, 0x9020, gpio_output);
	retval |= mc417_register_write(dev, 0x900C, gpio_value);

	retval |= mc417_register_read(dev, IVTV_REG_VPU, &value);
	retval |= mc417_register_write(dev, IVTV_REG_VPU, value & 0xFFFFFFE8);

	/* Hardcoded GPIO's here */
	retval |= mc417_register_write(dev, 0x9020, 0x4000);
	retval |= mc417_register_write(dev, 0x900C, 0x4000);

	mc417_register_read(dev, 0x9020, &gpio_output);
	mc417_register_read(dev, 0x900C, &gpio_value);

	if (retval < 0)
		pr_err("%s: Error with mc417_register_write\n",
			__func__);
	return 0;
}

void cx23885_417_check_encoder(struct cx23885_dev *dev)
{
	u32 status, seq;

	status = seq = 0;
	cx23885_api_cmd(dev, CX2341X_ENC_GET_SEQ_END, 0, 2, &status, &seq);
	dprintk(1, "%s() status = %d, seq = %d\n", __func__, status, seq);
}

static void cx23885_codec_settings(struct cx23885_dev *dev)
{
	dprintk(1, "%s()\n", __func__);

	/* Dynamically change the height based on video standard */
	if (dev->encodernorm.id & V4L2_STD_525_60)
		dev->ts1.height = 480;
	else
		dev->ts1.height = 576;

	/* assign frame size */
	cx23885_api_cmd(dev, CX2341X_ENC_SET_FRAME_SIZE, 2, 0,
				dev->ts1.height, dev->ts1.width);

	dev->cxhdl.width = dev->ts1.width;
	dev->cxhdl.height = dev->ts1.height;
	dev->cxhdl.is_50hz =
		(dev->encodernorm.id & V4L2_STD_625_50) != 0;

	cx2341x_handler_setup(&dev->cxhdl);

	cx23885_api_cmd(dev, CX2341X_ENC_MISC, 2, 0, 3, 1);
	cx23885_api_cmd(dev, CX2341X_ENC_MISC, 2, 0, 4, 1);
}

static int cx23885_initialize_codec(struct cx23885_dev *dev, int startencoder)
{
	int version;
	int retval;
	u32 i, data[7];

	dprintk(1, "%s()\n", __func__);

	retval = cx23885_api_cmd(dev, CX2341X_ENC_PING_FW, 0, 0); /* ping */
	if (retval < 0) {
		dprintk(2, "%s() PING OK\n", __func__);
		retval = cx23885_load_firmware(dev);
		if (retval < 0) {
			pr_err("%s() f/w load failed\n", __func__);
			return retval;
		}
		retval = cx23885_find_mailbox(dev);
		if (retval < 0) {
			pr_err("%s() mailbox < 0, error\n",
				__func__);
			return -1;
		}
		dev->cx23417_mailbox = retval;
		retval = cx23885_api_cmd(dev, CX2341X_ENC_PING_FW, 0, 0);
		if (retval < 0) {
			pr_err("ERROR: cx23417 firmware ping failed!\n");
			return -1;
		}
		retval = cx23885_api_cmd(dev, CX2341X_ENC_GET_VERSION, 0, 1,
			&version);
		if (retval < 0) {
			pr_err("ERROR: cx23417 firmware get encoder :version failed!\n");
			return -1;
		}
		dprintk(1, "cx23417 firmware version is 0x%08x\n", version);
		msleep(200);
	}

	cx23885_codec_settings(dev);
	msleep(60);

	cx23885_api_cmd(dev, CX2341X_ENC_SET_NUM_VSYNC_LINES, 2, 0,
		CX23885_FIELD1_SAA7115, CX23885_FIELD2_SAA7115);
	cx23885_api_cmd(dev, CX2341X_ENC_SET_PLACEHOLDER, 12, 0,
		CX23885_CUSTOM_EXTENSION_USR_DATA, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0);

	/* Setup to capture VBI */
	data[0] = 0x0001BD00;
	data[1] = 1;          /* frames per interrupt */
	data[2] = 4;          /* total bufs */
	data[3] = 0x91559155; /* start codes */
	data[4] = 0x206080C0; /* stop codes */
	data[5] = 6;          /* lines */
	data[6] = 64;         /* BPL */

	cx23885_api_cmd(dev, CX2341X_ENC_SET_VBI_CONFIG, 7, 0, data[0], data[1],
		data[2], data[3], data[4], data[5], data[6]);

	for (i = 2; i <= 24; i++) {
		int valid;

		valid = ((i >= 19) && (i <= 21));
		cx23885_api_cmd(dev, CX2341X_ENC_SET_VBI_LINE, 5, 0, i,
				valid, 0 , 0, 0);
		cx23885_api_cmd(dev, CX2341X_ENC_SET_VBI_LINE, 5, 0,
				i | 0x80000000, valid, 0, 0, 0);
	}

	cx23885_api_cmd(dev, CX2341X_ENC_MUTE_AUDIO, 1, 0, CX23885_UNMUTE);
	msleep(60);

	/* initialize the video input */
	cx23885_api_cmd(dev, CX2341X_ENC_INITIALIZE_INPUT, 0, 0);
	msleep(60);

	/* Enable VIP style pixel invalidation so we work with scaled mode */
	mc417_memory_write(dev, 2120, 0x00000080);

	/* start capturing to the host interface */
	if (startencoder) {
		cx23885_api_cmd(dev, CX2341X_ENC_START_CAPTURE, 2, 0,
			CX23885_MPEG_CAPTURE, CX23885_RAW_BITS_NONE);
		msleep(10);
	}

	return 0;
}

/* ------------------------------------------------------------------ */

static int queue_setup(struct vb2_queue *q,
			   unsigned int *num_buffers, unsigned int *num_planes,
			   unsigned int sizes[], struct device *alloc_devs[])
{
	struct cx23885_dev *dev = q->drv_priv;

	dev->ts1.ts_packet_size  = mpeglinesize;
	dev->ts1.ts_packet_count = mpeglines;
	*num_planes = 1;
	sizes[0] = mpeglinesize * mpeglines;
	*num_buffers = mpegbufs;
	return 0;
}

static int buffer_prepare(struct vb2_buffer *vb)
{
	struct vb2_v4l2_buffer *vbuf = to_vb2_v4l2_buffer(vb);
	struct cx23885_dev *dev = vb->vb2_queue->drv_priv;
	struct cx23885_buffer *buf =
		container_of(vbuf, struct cx23885_buffer, vb);

	return cx23885_buf_prepare(buf, &dev->ts1);
}

static void buffer_finish(struct vb2_buffer *vb)
{
	struct vb2_v4l2_buffer *vbuf = to_vb2_v4l2_buffer(vb);
	struct cx23885_dev *dev = vb->vb2_queue->drv_priv;
	struct cx23885_buffer *buf = container_of(vbuf,
		struct cx23885_buffer, vb);

	cx23885_free_buffer(dev, buf);
}

static void buffer_queue(struct vb2_buffer *vb)
{
	struct vb2_v4l2_buffer *vbuf = to_vb2_v4l2_buffer(vb);
	struct cx23885_dev *dev = vb->vb2_queue->drv_priv;
	struct cx23885_buffer   *buf = container_of(vbuf,
		struct cx23885_buffer, vb);

	cx23885_buf_queue(&dev->ts1, buf);
}

static int cx23885_start_streaming(struct vb2_queue *q, unsigned int count)
{
	struct cx23885_dev *dev = q->drv_priv;
	struct cx23885_dmaqueue *dmaq = &dev->ts1.mpegq;
	unsigned long flags;
	int ret;

	ret = cx23885_initialize_codec(dev, 1);
	if (ret == 0) {
		struct cx23885_buffer *buf = list_entry(dmaq->active.next,
			struct cx23885_buffer, queue);

		cx23885_start_dma(&dev->ts1, dmaq, buf);
		return 0;
	}
	spin_lock_irqsave(&dev->slock, flags);
	while (!list_empty(&dmaq->active)) {
		struct cx23885_buffer *buf = list_entry(dmaq->active.next,
			struct cx23885_buffer, queue);

		list_del(&buf->queue);
		vb2_buffer_done(&buf->vb.vb2_buf, VB2_BUF_STATE_QUEUED);
	}
	spin_unlock_irqrestore(&dev->slock, flags);
	return ret;
}

static void cx23885_stop_streaming(struct vb2_queue *q)
{
	struct cx23885_dev *dev = q->drv_priv;

	/* stop mpeg capture */
	cx23885_api_cmd(dev, CX2341X_ENC_STOP_CAPTURE, 3, 0,
			CX23885_END_NOW, CX23885_MPEG_CAPTURE,
			CX23885_RAW_BITS_NONE);

	msleep(500);
	cx23885_417_check_encoder(dev);
	cx23885_cancel_buffers(&dev->ts1);
}

static const struct vb2_ops cx23885_qops = {
	.queue_setup    = queue_setup,
	.buf_prepare  = buffer_prepare,
	.buf_finish = buffer_finish,
	.buf_queue    = buffer_queue,
	.wait_prepare = vb2_ops_wait_prepare,
	.wait_finish = vb2_ops_wait_finish,
	.start_streaming = cx23885_start_streaming,
	.stop_streaming = cx23885_stop_streaming,
};

/* ------------------------------------------------------------------ */

static int vidioc_g_std(struct file *file, void *priv, v4l2_std_id *id)
{
	struct cx23885_dev *dev = video_drvdata(file);

	*id = dev->tvnorm;
	return 0;
}

static int vidioc_s_std(struct file *file, void *priv, v4l2_std_id id)
{
	struct cx23885_dev *dev = video_drvdata(file);
	unsigned int i;
	int ret;

	for (i = 0; i < ARRAY_SIZE(cx23885_tvnorms); i++)
		if (id & cx23885_tvnorms[i].id)
			break;
	if (i == ARRAY_SIZE(cx23885_tvnorms))
		return -EINVAL;

	ret = cx23885_set_tvnorm(dev, id);
	if (!ret)
		dev->encodernorm = cx23885_tvnorms[i];
	return ret;
}

static int vidioc_enum_input(struct file *file, void *priv,
	struct v4l2_input *i)
{
	struct cx23885_dev *dev = video_drvdata(file);
	dprintk(1, "%s()\n", __func__);
	return cx23885_enum_input(dev, i);
}

static int vidioc_g_input(struct file *file, void *priv, unsigned int *i)
{
	return cx23885_get_input(file, priv, i);
}

static int vidioc_s_input(struct file *file, void *priv, unsigned int i)
{
	return cx23885_set_input(file, priv, i);
}

static int vidioc_g_tuner(struct file *file, void *priv,
				struct v4l2_tuner *t)
{
	struct cx23885_dev *dev = video_drvdata(file);

	if (dev->tuner_type == TUNER_ABSENT)
		return -EINVAL;
	if (0 != t->index)
		return -EINVAL;
	strscpy(t->name, "Television", sizeof(t->name));
	call_all(dev, tuner, g_tuner, t);

	dprintk(1, "VIDIOC_G_TUNER: tuner type %d\n", t->type);

	return 0;
}

static int vidioc_s_tuner(struct file *file, void *priv,
				const struct v4l2_tuner *t)
{
	struct cx23885_dev *dev = video_drvdata(file);

	if (dev->tuner_type == TUNER_ABSENT)
		return -EINVAL;

	/* Update the A/V core */
	call_all(dev, tuner, s_tuner, t);

	return 0;
}

static int vidioc_g_frequency(struct file *file, void *priv,
				struct v4l2_frequency *f)
{
	struct cx23885_dev *dev = video_drvdata(file);

	if (dev->tuner_type == TUNER_ABSENT)
		return -EINVAL;
	f->type = V4L2_TUNER_ANALOG_TV;
	f->frequency = dev->freq;

	call_all(dev, tuner, g_frequency, f);

	return 0;
}

static int vidioc_s_frequency(struct file *file, void *priv,
	const struct v4l2_frequency *f)
{
	return cx23885_set_frequency(file, priv, f);
}

static int vidioc_querycap(struct file *file, void  *priv,
				struct v4l2_capability *cap)
{
	struct cx23885_dev *dev = video_drvdata(file);
	struct cx23885_tsport  *tsport = &dev->ts1;

	strscpy(cap->driver, dev->name, sizeof(cap->driver));
	strscpy(cap->card, cx23885_boards[tsport->dev->board].name,
		sizeof(cap->card));
	sprintf(cap->bus_info, "PCIe:%s", pci_name(dev->pci));
	cap->capabilities = V4L2_CAP_VIDEO_CAPTURE | V4L2_CAP_READWRITE |
			    V4L2_CAP_STREAMING | V4L2_CAP_VBI_CAPTURE |
			    V4L2_CAP_AUDIO | V4L2_CAP_DEVICE_CAPS;
	if (dev->tuner_type != TUNER_ABSENT)
		cap->capabilities |= V4L2_CAP_TUNER;

	return 0;
}

static int vidioc_enum_fmt_vid_cap(struct file *file, void  *priv,
					struct v4l2_fmtdesc *f)
{
	if (f->index != 0)
		return -EINVAL;

	f->pixelformat = V4L2_PIX_FMT_MPEG;

	return 0;
}

static int vidioc_g_fmt_vid_cap(struct file *file, void *priv,
				struct v4l2_format *f)
{
	struct cx23885_dev *dev = video_drvdata(file);

	f->fmt.pix.pixelformat  = V4L2_PIX_FMT_MPEG;
	f->fmt.pix.bytesperline = 0;
	f->fmt.pix.sizeimage    =
		dev->ts1.ts_packet_size * dev->ts1.ts_packet_count;
	f->fmt.pix.colorspace   = 0;
	f->fmt.pix.width        = dev->ts1.width;
	f->fmt.pix.height       = dev->ts1.height;
	f->fmt.pix.field        = V4L2_FIELD_INTERLACED;
	dprintk(1, "VIDIOC_G_FMT: w: %d, h: %d\n",
		dev->ts1.width, dev->ts1.height);
	return 0;
}

static int vidioc_try_fmt_vid_cap(struct file *file, void *priv,
				struct v4l2_format *f)
{
	struct cx23885_dev *dev = video_drvdata(file);

	f->fmt.pix.pixelformat  = V4L2_PIX_FMT_MPEG;
	f->fmt.pix.bytesperline = 0;
	f->fmt.pix.sizeimage    =
		dev->ts1.ts_packet_size * dev->ts1.ts_packet_count;
	f->fmt.pix.colorspace   = 0;
	f->fmt.pix.field        = V4L2_FIELD_INTERLACED;
	dprintk(1, "VIDIOC_TRY_FMT: w: %d, h: %d\n",
		dev->ts1.width, dev->ts1.height);
	return 0;
}

static int vidioc_s_fmt_vid_cap(struct file *file, void *priv,
				struct v4l2_format *f)
{
	struct cx23885_dev *dev = video_drvdata(file);

	f->fmt.pix.pixelformat  = V4L2_PIX_FMT_MPEG;
	f->fmt.pix.bytesperline = 0;
	f->fmt.pix.sizeimage    =
		dev->ts1.ts_packet_size * dev->ts1.ts_packet_count;
	f->fmt.pix.colorspace   = 0;
	f->fmt.pix.field        = V4L2_FIELD_INTERLACED;
	dprintk(1, "VIDIOC_S_FMT: w: %d, h: %d, f: %d\n",
		f->fmt.pix.width, f->fmt.pix.height, f->fmt.pix.field);
	return 0;
}

static int vidioc_log_status(struct file *file, void *priv)
{
	struct cx23885_dev *dev = video_drvdata(file);
	char name[32 + 2];

	snprintf(name, sizeof(name), "%s/2", dev->name);
	call_all(dev, core, log_status);
	v4l2_ctrl_handler_log_status(&dev->cxhdl.hdl, name);
	return 0;
}

static const struct v4l2_file_operations mpeg_fops = {
	.owner	       = THIS_MODULE,
	.open           = v4l2_fh_open,
	.release        = vb2_fop_release,
	.read           = vb2_fop_read,
	.poll		= vb2_fop_poll,
	.unlocked_ioctl = video_ioctl2,
	.mmap           = vb2_fop_mmap,
};

static const struct v4l2_ioctl_ops mpeg_ioctl_ops = {
	.vidioc_g_std		 = vidioc_g_std,
	.vidioc_s_std		 = vidioc_s_std,
	.vidioc_enum_input	 = vidioc_enum_input,
	.vidioc_g_input		 = vidioc_g_input,
	.vidioc_s_input		 = vidioc_s_input,
	.vidioc_g_tuner		 = vidioc_g_tuner,
	.vidioc_s_tuner		 = vidioc_s_tuner,
	.vidioc_g_frequency	 = vidioc_g_frequency,
	.vidioc_s_frequency	 = vidioc_s_frequency,
	.vidioc_querycap	 = vidioc_querycap,
	.vidioc_enum_fmt_vid_cap = vidioc_enum_fmt_vid_cap,
	.vidioc_g_fmt_vid_cap	 = vidioc_g_fmt_vid_cap,
	.vidioc_try_fmt_vid_cap	 = vidioc_try_fmt_vid_cap,
	.vidioc_s_fmt_vid_cap	 = vidioc_s_fmt_vid_cap,
	.vidioc_reqbufs       = vb2_ioctl_reqbufs,
	.vidioc_prepare_buf   = vb2_ioctl_prepare_buf,
	.vidioc_querybuf      = vb2_ioctl_querybuf,
	.vidioc_qbuf          = vb2_ioctl_qbuf,
	.vidioc_dqbuf         = vb2_ioctl_dqbuf,
	.vidioc_streamon      = vb2_ioctl_streamon,
	.vidioc_streamoff     = vb2_ioctl_streamoff,
	.vidioc_log_status	 = vidioc_log_status,
#ifdef CONFIG_VIDEO_ADV_DEBUG
	.vidioc_g_chip_info	 = cx23885_g_chip_info,
	.vidioc_g_register	 = cx23885_g_register,
	.vidioc_s_register	 = cx23885_s_register,
#endif
};

static struct video_device cx23885_mpeg_template = {
	.name          = "cx23885",
	.fops          = &mpeg_fops,
	.ioctl_ops     = &mpeg_ioctl_ops,
	.tvnorms       = CX23885_NORMS,
};

void cx23885_417_unregister(struct cx23885_dev *dev)
{
	dprintk(1, "%s()\n", __func__);

	if (dev->v4l_device) {
		if (video_is_registered(dev->v4l_device))
			video_unregister_device(dev->v4l_device);
		else
			video_device_release(dev->v4l_device);
		v4l2_ctrl_handler_free(&dev->cxhdl.hdl);
		dev->v4l_device = NULL;
	}
}

static struct video_device *cx23885_video_dev_alloc(
	struct cx23885_tsport *tsport,
	struct pci_dev *pci,
	struct video_device *template,
	char *type)
{
	struct video_device *vfd;
	struct cx23885_dev *dev = tsport->dev;

	dprintk(1, "%s()\n", __func__);

	vfd = video_device_alloc();
	if (NULL == vfd)
		return NULL;
	*vfd = *template;
	snprintf(vfd->name, sizeof(vfd->name), "%s (%s)",
		cx23885_boards[tsport->dev->board].name, type);
	vfd->v4l2_dev = &dev->v4l2_dev;
	vfd->release = video_device_release;
	return vfd;
}

int cx23885_417_register(struct cx23885_dev *dev)
{
	/* FIXME: Port1 hardcoded here */
	int err = -ENODEV;
	struct cx23885_tsport *tsport = &dev->ts1;
	struct vb2_queue *q;

	dprintk(1, "%s()\n", __func__);

	if (cx23885_boards[dev->board].portb != CX23885_MPEG_ENCODER)
		return err;

	/* Set default TV standard */
	dev->encodernorm = cx23885_tvnorms[0];

	if (dev->encodernorm.id & V4L2_STD_525_60)
		tsport->height = 480;
	else
		tsport->height = 576;

	tsport->width = 720;
	dev->cxhdl.port = CX2341X_PORT_SERIAL;
	err = cx2341x_handler_init(&dev->cxhdl, 50);
	if (err)
		return err;
	dev->cxhdl.priv = dev;
	dev->cxhdl.func = cx23885_api_func;
	cx2341x_handler_set_50hz(&dev->cxhdl, tsport->height == 576);
	v4l2_ctrl_add_handler(&dev->ctrl_handler, &dev->cxhdl.hdl, NULL, false);

	/* Allocate and initialize V4L video device */
	dev->v4l_device = cx23885_video_dev_alloc(tsport,
		dev->pci, &cx23885_mpeg_template, "mpeg");
	q = &dev->vb2_mpegq;
	q->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	q->io_modes = VB2_MMAP | VB2_USERPTR | VB2_DMABUF | VB2_READ;
	q->gfp_flags = GFP_DMA32;
	q->min_buffers_needed = 2;
	q->drv_priv = dev;
	q->buf_struct_size = sizeof(struct cx23885_buffer);
	q->ops = &cx23885_qops;
	q->mem_ops = &vb2_dma_sg_memops;
	q->timestamp_flags = V4L2_BUF_FLAG_TIMESTAMP_MONOTONIC;
	q->lock = &dev->lock;
	q->dev = &dev->pci->dev;

	err = vb2_queue_init(q);
	if (err < 0)
		return err;
	video_set_drvdata(dev->v4l_device, dev);
	dev->v4l_device->lock = &dev->lock;
	dev->v4l_device->queue = q;
	dev->v4l_device->device_caps = V4L2_CAP_VIDEO_CAPTURE |
				       V4L2_CAP_READWRITE | V4L2_CAP_STREAMING;
	if (dev->tuner_type != TUNER_ABSENT)
		dev->v4l_device->device_caps |= V4L2_CAP_TUNER;
	err = video_register_device(dev->v4l_device,
		VFL_TYPE_VIDEO, -1);
	if (err < 0) {
		pr_info("%s: can't register mpeg device\n", dev->name);
		return err;
	}

	pr_info("%s: registered device %s [mpeg]\n",
	       dev->name, video_device_node_name(dev->v4l_device));

	/* ST: Configure the encoder parameters, but don't begin
	 * encoding, this resolves an issue where the first time the
	 * encoder is started video can be choppy.
	 */
	cx23885_initialize_codec(dev, 0);

	return 0;
}

MODULE_FIRMWARE(CX23885_FIRM_IMAGE_NAME);
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              // SPDX-License-Identifier: GPL-2.0-or-later
/*
 *
 *  Support for a cx23417 mpeg encoder via cx23885 host port.
 *
 *    (c) 2004 Jelle Foks <jelle@foks.us>
 *    (c) 2004 Gerd Knorr <kraxel@bytesex.org>
 *    (c) 2008 Steven Toth <stoth@linuxtv.org>
 *      - CX23885/7/8 support
 *
 *  Includes parts from the ivtv driver <http://sourceforge.net/projects/ivtv/>
 */

#include "cx23885.h"
#include "cx23885-ioctl.h"

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/firmware.h>
#include <linux/slab.h>
#include <media/v4l2-common.h>
#include <media/v4l2-ioctl.h>
#include <media/drv-intf/cx2341x.h>

#define CX23885_FIRM_IMAGE_SIZE 376836
#define CX23885_FIRM_IMAGE_NAME "v4l-cx23885-enc.fw"

static unsigned int mpegbufs = 32;
module_param(mpegbufs, int, 0644);
MODULE_PARM_DESC(mpegbufs, "number of mpeg buffers, range 2-32");
static unsigned int mpeglines = 32;
module_param(mpeglines, int, 0644);
MODULE_PARM_DESC(mpeglines, "number of lines in an MPEG buffer, range 2-32");
static unsigned int mpeglinesize = 512;
module_param(mpeglinesize, int, 0644);
MODULE_PARM_DESC(mpeglinesize,
	"number of bytes in each line of an MPEG buffer, range 512-1024");

static unsigned int v4l_debug;
module_param(v4l_debug, int, 0644);
MODULE_PARM_DESC(v4l_debug, "enable V4L debug messages");

#define dprintk(level, fmt, arg...)\
	do { if (v4l_debug >= level) \
		printk(KERN_DEBUG pr_fmt("%s: 417:" fmt), \
			__func__, ##arg); \
	} while (0)

static struct cx23885_tvnorm cx23885_tvnorms[] = {
	{
		.name      = "NTSC-M",
		.id        = V4L2_STD_NTSC_M,
	}, {
		.name      = "NTSC-JP",
		.id        = V4L2_STD_NTSC_M_JP,
	}, {
		.name      = "PAL-BG",
		.id        = V4L2_STD_PAL_BG,
	}, {
		.name      = "PAL-DK",
		.id        = V4L2_STD_PAL_DK,
	}, {
		.name      = "PAL-I",
		.id        = V4L2_STD_PAL_I,
	}, {
		.name      = "PAL-M",
		.id        = V4L2_STD_PAL_M,
	}, {
		.name      = "PAL-N",
		.id        = V4L2_STD_PAL_N,
	}, {
		.name      = "PAL-Nc",
		.id        = V4L2_STD_PAL_Nc,
	}, {
		.name      = "PAL-60",
		.id        = V4L2_STD_PAL_60,
	}, {
		.name      = "SECAM-L",
		.id        = V4L2_STD_SECAM_L,
	}, {
		.name      = "SECAM-DK",
		.id        = V4L2_STD_SECAM_DK,
	}
};

/* ------------------------------------------------------------------ */
enum cx23885_capture_type {
	CX23885_MPEG_CAPTURE,
	CX23885_RAW_CAPTURE,
	CX23885_RAW_PASSTHRU_CAPTURE
};
enum cx23885_capture_bits {
	CX23885_RAW_BITS_NONE             = 0x00,
	CX23885_RAW_BITS_YUV_CAPTURE      = 0x01,
	CX23885_RAW_BITS_PCM_CAPTURE      = 0x02,
	CX23885_RAW_BITS_VBI_CAPTURE      = 0x04,
	CX23885_RAW_BITS_PASSTHRU_CAPTURE = 0x08,
	CX23885_RAW_BITS_TO_HOST_CAPTURE  = 0x10
};
enum cx23885_capture_end {
	CX23885_END_AT_GOP, /* stop at the end of gop, generate irq */
	CX23885_END_NOW, /* stop immediately, no irq */
};
enum cx23885_framerate {
	CX23885_FRAMERATE_NTSC_30, /* NTSC: 30fps */
	CX23885_FRAMERATE_PAL_25   /* PAL: 25fps */
};
enum cx23885_stream_port {
	CX23885_OUTPUT_PORT_MEMORY,
	CX23885_OUTPUT_PORT_STREAMING,
	CX23885_OUTPUT_PORT_SERIAL
};
enum cx23885_data_xfer_status {
	CX23885_MORE_BUFFERS_FOLLOW,
	CX23885_LAST_BUFFER,
};
enum cx23885_picture_mask {
	CX23885_PICTURE_MASK_NONE,
	CX23885_PICTURE_MASK_I_FRAMES,
	CX23885_PICTURE_MASK_I_P_FRAMES = 0x3,
	CX23885_PICTURE_MASK_ALL_FRAMES = 0x7,
};
enum cx23885_vbi_mode_bits {
	CX23885_VBI_BITS_SLICED,
	CX23885_VBI_BITS_RAW,
};
enum cx23885_vbi_insertion_bits {
	CX23885_VBI_BITS_INSERT_IN_XTENSION_USR_DATA,
	CX23885_VBI_BITS_INSERT_IN_PRIVATE_PACKETS = 0x1 << 1,
	CX23885_VBI_BITS_SEPARATE_STREAM = 0x2 << 1,
	CX23885_VBI_BITS_SEPARATE_STREAM_USR_DATA = 0x4 << 1,
	CX23885_VBI_BITS_SEPARATE_STREAM_PRV_DATA = 0x5 << 1,
};
enum cx23885_dma_unit {
	CX23885_DMA_BYTES,
	CX23885_DMA_FRAMES,
};
enum cx23885_dma_transfer_status_bits {
	CX23885_DMA_TRANSFER_BITS_DONE = 0x01,
	CX23885_DMA_TRANSFER_BITS_ERROR = 0x04,
	CX23885_DMA_TRANSFER_BITS_LL_ERROR = 0x10,
};
enum cx23885_pause {
	CX23885_PAUSE_ENCODING,
	CX23885_RESUME_ENCODING,
};
enum cx23885_copyright {
	CX23885_COPYRIGHT_OFF,
	CX23885_COPYRIGHT_ON,
};
enum cx23885_notification_type {
	CX23885_NOTIFICATION_REFRESH,
};
enum cx23885_notification_status {
	CX23885_NOTIFICATION_OFF,
	CX23885_NOTIFICATION_ON,
};
enum cx23885_notification_mailbox {
	CX23885_NOTIFICATION_NO_MAILBOX = -1,
};
enum cx23885_field1_lines {
	CX23885_FIELD1_SAA7114 = 0x00EF, /* 239 */
	CX23885_FIELD1_SAA7115 = 0x00F0, /* 240 */
	CX23885_FIELD1_MICRONAS = 0x0105, /* 261 */
};
enum cx23885_field2_lines {
	CX23885_FIELD2_SAA7114 = 0x00EF, /* 239 */
	CX23885_FIELD2_SAA7115 = 0x00F0, /* 240 */
	CX23885_FIELD2_MICRONAS = 0x0106, /* 262 */
};
enum cx23885_custom_data_type {
	CX23885_CUSTOM_EXTENSION_USR_DATA,
	CX23885_CUSTOM_PRIVATE_PACKET,
};
enum cx23885_mute {
	CX23885_UNMUTE,
	CX23885_MUTE,
};
enum cx23885_mute_video_mask {
	CX23885_MUTE_VIDEO_V_MASK = 0x0000FF00,
	CX23885_MUTE_VIDEO_U_MASK = 0x00FF0000,
	CX23885_MUTE_VIDEO_Y_MASK = 0xFF000000,
};
enum cx23885_mute_video_shift {
	CX23885_MUTE_VIDEO_V_SHIFT = 8,
	CX23885_MUTE_VIDEO_U_SHIFT = 16,
	CX23885_MUTE_VIDEO_Y_SHIFT = 24,
};

/* defines below are from ivtv-driver.h */
#define IVTV_CMD_HW_BLOCKS_RST 0xFFFFFFFF

/* Firmware API commands */
#define IVTV_API_STD_TIMEOUT 500

/* Registers */
/* IVTV_REG_OFFSET */
#define IVTV_REG_ENC_SDRAM_REFRESH (0x07F8)
#define IVTV_REG_ENC_SDRAM_PRECHARGE (0x07FC)
#define IVTV_REG_SPU (0x9050)
#define IVTV_REG_HW_BLOCKS (0x9054)
#define IVTV_REG_VPU (0x9058)
#define IVTV_REG_APU (0xA064)

/**** Bit definitions for MC417_RWD and MC417_OEN registers  ***
  bits 31-16
+-----------+
| Reserved  |
+-----------+
  bit 15  bit 14  bit 13 bit 12  bit 11  bit 10  bit 9   bit 8
+-------+-------+-------+-------+-------+-------+-------+-------+
| MIWR# | MIRD# | MICS# |MIRDY# |MIADDR3|MIADDR2|MIADDR1|MIADDR0|
+-------+-------+-------+-------+-------+-------+-------+-------+
 bit 7   bit 6   bit 5   bit 4   bit 3   bit 2   bit 1   bit 0
+-------+-------+-------+-------+-------+-------+-------+-------+
|MIDATA7|MIDATA6|MIDATA5|MIDATA4|MIDATA3|MIDATA2|MIDATA1|MIDATA0|
+-------+-------+-------+-------+-------+-------+-------+-------+
***/
#define MC417_MIWR	0x8000
#define MC417_MIRD	0x4000
#define MC417_MICS	0x2000
#define MC417_MIRDY	0x1000
#define MC417_MIADDR	0x0F00
#define MC417_MIDATA	0x00FF

/* MIADDR* nibble definitions */
#define  MCI_MEMORY_DATA_BYTE0          0x000
#define  MCI_MEMORY_DATA_BYTE1          0x100
#define  MCI_MEMORY_DATA_BYTE2          0x200
#define  MCI_MEMORY_DATA_BYTE3          0x300
#define  MCI_MEMORY_ADDRESS_BYTE2       0x400
#define  MCI_MEMORY_ADDRESS_BYTE1       0x500
#define  MCI_MEMORY_ADDRESS_BYTE0       0x600
#define  MCI_REGISTER_DATA_BYTE0        0x800
#define  MCI_REGISTER_DATA_BYTE1        0x900
#define  MCI_REGISTER_DATA_BYTE2        0xA00
#define  MCI_REGISTER_DATA_BYTE3        0xB00
#define  MCI_REGISTER_ADDRESS_BYTE0     0xC00
#define  MCI_REGISTER_ADDRESS_BYTE1     0xD00
#define  MCI_REGISTER_MODE              0xE00

/* Read and write modes */
#define  MCI_MODE_REGISTER_READ         0
#define  MCI_MODE_REGISTER_WRITE        1
#define  MCI_MODE_MEMORY_READ           0
#define  MCI_MODE_MEMORY_WRITE          0x40

/*** Bit definitions for MC417_CTL register ****
 bits 31-6   bits 5-4   bit 3    bits 2-1       Bit 0
+--------+-------------+--------+--------------+------------+
|Reserved|MC417_SPD_CTL|Reserved|MC417_GPIO_SEL|UART_GPIO_EN|
+--------+-------------+--------+--------------+------------+
***/
#define MC417_SPD_CTL(x)	(((x) << 4) & 0x00000030)
#define MC417_GPIO_SEL(x)	(((x) << 1) & 0x00000006)
#define MC417_UART_GPIO_EN	0x00000001

/* Values for speed control */
#define MC417_SPD_CTL_SLOW	0x1
#define MC417_SPD_CTL_MEDIUM	0x0
#define MC417_SPD_CTL_FAST	0x3     /* b'1x, but we use b'11 */

/* Values for GPIO select */
#define MC417_GPIO_SEL_GPIO3	0x3
#define MC417_GPIO_SEL_GPIO2	0x2
#define MC417_GPIO_SEL_GPIO1	0x1
#define MC417_GPIO_SEL_GPIO0	0x0

void cx23885_mc417_init(struct cx23885_dev *dev)
{
	u32 regval;

	dprintk(2, "%s()\n", __func__);

	/* Configure MC417_CTL register to defaults. */
	regval = MC417_SPD_CTL(MC417_SPD_CTL_FAST)	|
		 MC417_GPIO_SEL(MC417_GPIO_SEL_GPIO3)	|
		 MC417_UART_GPIO_EN;
	cx_write(MC417_CTL, regval);

	/* Configure MC417_OEN to defaults. */
	regval = MC417_MIRDY;
	cx_write(MC417_OEN, regval);

	/* Configure MC417_RWD to defaults. */
	regval = MC417_MIWR | MC417_MIRD | MC417_MICS;
	cx_write(MC417_RWD, regval);
}

static int mc417_wait_ready(struct cx23885_dev *dev)
{
	u32 mi_ready;
	unsigned long timeout = jiffies + msecs_to_jiffies(1);

	for (;;) {
		mi_ready = cx_read(MC417_RWD) & MC417_MIRDY;
		if (mi_ready != 0)
			return 0;
		if (time_after(jiffies, timeout))
			return -1;
		udelay(1);
	}
}

int mc417_register_write(struct cx23885_dev *dev, u16 address, u32 value)
{
	u32 regval;

	/* Enable MC417 GPIO outputs except for MC417_MIRDY,
	 * which is an input.
	 */
	cx_write(MC417_OEN, MC417_MIRDY);

	/* Write data byte 0 */
	regval = MC417_MIRD | MC417_MIRDY | MCI_REGISTER_DATA_BYTE0 |
		(value & 0x000000FF);
	cx_write(MC417_RWD, regval);

	/* Transition CS/WR to effect write transaction across bus. */
	regval |= MC417_MICS | MC417_MIWR;
	cx_write(MC417_RWD, regval);

	/* Write data byte 1 */
	regval = MC417_MIRD | MC417_MIRDY | MCI_REGISTER_DATA_BYTE1 |
		((value >> 8) & 0x000000FF);
	cx_write(MC417_RWD, regval);
	regval |= MC417_MICS | MC417_MIWR;
	cx_write(MC417_RWD, regval);

	/* Write data byte 2 */
	regval = MC417_MIRD | MC417_MIRDY | MCI_REGISTER_DATA_BYTE2 |
		((value >> 16) & 0x000000FF);
	cx_write(MC417_RWD, regval);
	regval |= MC417_MICS | MC417_MIWR;
	cx_write(MC417_RWD, regval);

	/* Write data byte 3 */
	regval = MC417_MIRD | MC417_MIRDY | MCI_REGISTER_DATA_BYTE3 |
		((value >> 24) & 0x000000FF);
	cx_write(MC417_RWD, regval);
	regval |= MC417_MICS | MC417_MIWR;
	cx_write(MC417_RWD, regval);

	/* Write address byte 0 */
	regval = MC417_MIRD | MC417_MIRDY | MCI_REGISTER_ADDRESS_BYTE0 |
		(address & 0xFF);
	cx_write(MC417_RWD, regval);
	regval |= MC417_MICS | MC417_MIWR;
	cx_write(MC417_RWD, regval);

	/* Write address byte 1 */
	regval = MC417_MIRD | MC417_MIRDY | MCI_REGISTER_ADDRESS_BYTE1 |
		((address >> 8) & 0xFF);
	cx_write(MC417_RWD, regval);
	regval |= MC417_MICS | MC417_MIWR;
	cx_write(MC417_RWD, regval);

	/* Indicate that this is a write. */
	regval = MC417_MIRD | MC417_MIRDY | MCI_REGISTER_MODE |
		MCI_MODE_REGISTER_WRITE;
	cx_write(MC417_RWD, regval);
	regval |= MC417_MICS | MC417_MIWR;
	cx_write(MC417_RWD, regval);

	/* Wait for the trans to complete (MC417_MIRDY asserted). */
	return mc417_wait_ready(dev);
}

int mc417_register_read(struct cx23885_dev *dev, u16 address, u32 *value)
{
	int retval;
	u32 regval;
	u32 tempval;
	u32 dataval;

	/* Enable MC417 GPIO outputs except for MC417_MIRDY,
	 * which is an input.
	 */
	cx_write(MC417_OEN, MC417_MIRDY);

	/* Write address byte 0 */
	regval = MC417_MIRD | MC417_MIRDY | MCI_REGISTER_ADDRESS_BYTE0 |
		((address & 0x00FF));
	cx_write(MC417_RWD, regval);
	regval |= MC417_MICS | MC417_MIWR;
	cx_write(MC417_RWD, regval);

	/* Write address byte 1 */
	regval = MC417_MIRD | MC417_MIRDY | MCI_REGISTER_ADDRESS_BYTE1 |
		((address >> 8) & 0xFF);
	cx_write(MC417_RWD, regval);
	regval |= MC417_MICS | MC417_MIWR;
	cx_write(MC417_RWD, regval);

	/* Indicate that this is a register read. */
	regval = MC417_MIRD | MC417_MIRDY | MCI_REGISTER_MODE |
		MCI_MODE_REGISTER_READ;
	cx_write(MC417_RWD, regval);
	regval |= MC417_MICS | MC417_MIWR;
	cx_write(MC417_RWD, regval);

	/* Wait for the trans to complete (MC417_MIRDY asserted). */
	retval = mc417_wait_ready(dev);

	/* switch the DAT0-7 GPIO[10:3] to input mode */
	cx_write(MC417_OEN, MC417_MIRDY | MC417_MIDATA);

	/* Read data byte 0 */
	regval = MC417_MIRD | MC417_MIRDY | MCI_REGISTER_DATA_BYTE0;
	cx_write(MC417_RWD, regval);

	/* Transition RD to effect read transaction across bus.
	 * Transition 0x5000 -> 0x9000 correct (RD/RDY -> WR/RDY)?
	 * Should it be 0x9000 -> 0xF000 (also why is RDY being set, its
	 * input only...)
	 */
	regval = MC417_MIWR | MC417_MIRDY | MCI_REGISTER_DATA_BYTE0;
	cx_write(MC417_RWD, regval);

	/* Collect byte */
	tempval = cx_read(MC417_RWD);
	dataval = tempval & 0x000000FF;

	/* Bring CS and RD high. */
	regval = MC417_MIWR | MC417_MIRD | MC417_MICS | MC417_MIRDY;
	cx_write(MC417_RWD, regval);

	/* Read data byte 1 */
	regval = MC417_MIRD | MC417_MIRDY | MCI_REGISTER_DATA_BYTE1;
	cx_write(MC417_RWD, regval);
	regval = MC417_MIWR | MC417_MIRDY | MCI_REGISTER_DATA_BYTE1;
	cx_write(MC417_RWD, regval);
	tempval = cx_read(MC417_RWD);
	dataval |= ((tempval & 0x000000FF) << 8);
	regval = MC417_MIWR | MC417_MIRD | MC417_MICS | MC417_MIRDY;
	cx_write(MC417_RWD, regval);

	/* Read data byte 2 */
	regval = MC417_MIRD | MC417_MIRDY | MCI_REGISTER_DATA_BYTE2;
	cx_write(MC417_RWD, regval);
	regval = MC417_MIWR | MC417_MIRDY | MCI_REGISTER_DATA_BYTE2;
	cx_write(MC417_RWD, regval);
	tempval = cx_read(MC417_RWD);
	dataval |= ((tempval & 0x000000FF) << 16);
	regval = MC417_MIWR | MC417_MIRD | MC417_MICS | MC417_MIRDY;
	cx_write(MC417_RWD, regval);

	/* Read data byte 3 */
	regval = MC417_MIRD | MC417_MIRDY | MCI_REGISTER_DATA_BYTE3;
	cx_write(MC417_RWD, regval);
	regval = MC417_MIWR | MC417_MIRDY | MCI_REGISTER_DATA_BYTE3;
	cx_write(MC417_RWD, regval);
	tempval = cx_read(MC417_RWD);
	dataval |= ((tempval & 0x000000FF) << 24);
	regval = MC417_MIWR | MC417_MIRD | MC417_MICS | MC417_MIRDY;
	cx_write(MC417_RWD, regval);

	*value  = dataval;

	return retval;
}

int mc417_memory_write(struct cx23885_dev *dev, u32 address, u32 value)
{
	u32 regval;

	/* Enable MC417 GPIO outputs except for MC417_MIRDY,
	 * which is an input.
	 */
	cx_write(MC417_OEN, MC417_MIRDY);

	/* Write data byte 0 */
	regval = MC417_MIRD | MC417_MIRDY | MCI_MEMORY_DATA_BYTE0 |
		(value & 0x000000FF);
	cx_write(MC417_RWD, regval);

	/* Transition CS/WR to effect write transaction across bus. */
	regval |= MC417_MICS | MC417_MIWR;
	cx_write(MC417_RWD, regval);

	/* Write data byte 1 */
	regval = MC417_MIRD | MC417_MIRDY | MCI_MEMORY_DATA_BYTE1 |
		((value >> 8) & 0x000000FF);
	cx_write(MC417_RWD, regval);
	regval |= MC417_MICS | MC417_MIWR;
	cx_write(MC417_RWD, regval);

	/* Write data byte 2 */
	regval = MC417_MIRD | MC417_MIRDY | MCI_MEMORY_DATA_BYTE2 |
		((value >> 16) & 0x000000FF);
	cx_write(MC417_RWD, regval);
	regval |= MC417_MICS | MC417_MIWR;
	cx_write(MC417_RWD, regval);

	/* Write data byte 3 */
	regval = MC417_MIRD | MC417_MIRDY | MCI_MEMORY_DATA_BYTE3 |
		((value >> 24) & 0x000000FF);
	cx_write(MC417_RWD, regval);
	regval |= MC417_MICS | MC417_MIWR;
	cx_write(MC417_RWD, regval);

	/* Write address byte 2 */
	regval = MC417_MIRD | MC417_MIRDY | MCI_MEMORY_ADDRESS_BYTE2 |
		MCI_MODE_MEMORY_WRITE | ((address >> 16) & 0x3F);
	cx_write(MC417_RWD, regval);
	regval |= MC417_MICS | MC417_MIWR;
	cx_write(MC417_RWD, regval);

	/* Write address byte 1 */
	regval = MC417_MIRD | MC417_MIRDY | MCI_MEMORY_ADDRESS_BYTE1 |
		((address >> 8) & 0xFF);
	cx_write(MC417_RWD, regval);
	regval |= MC417_MICS | MC417_MIWR;
	cx_write(MC417_RWD, regval);

	/* Write address byte 0 */
	regval = MC417_MIRD | MC417_MIRDY | MCI_MEMORY_ADDRESS_BYTE0 |
		(address & 0xFF);
	cx_write(MC417_RWD, regval);
	regval |= MC417_MICS | MC417_MIWR;
	cx_write(MC417_RWD, regval);

	/* Wait for the trans to complete (MC417_MIRDY asserted). */
	return mc417_wait_ready(dev);
}

int mc417_memory_read(struct cx23885_dev *dev, u32 address, u32 *value)
{
	int retval;
	u32 regval;
	u32 tempval;
	u32 dataval;

	/* Enable MC417 GPIO outputs except for MC417_MIRDY,
	 * which is an input.
	 */
	cx_write(MC417_OEN, MC417_MIRDY);

	/* Write address byte 2 */
	regval = MC417_MIRD | MC417_MIRDY | MCI_MEMORY_ADDRESS_BYTE2 |
		MCI_MODE_MEMORY_READ | ((address >> 16) & 0x3F);
	cx_write(MC417_RWD, regval);
	regval |= MC417_MICS | MC417_MIWR;
	cx_write(MC417_RWD, regval);

	/* Write address byte 1 */
	regval = MC417_MIRD | MC417_MIRDY | MCI_MEMORY_ADDRESS_BYTE1 |
		((address >> 8) & 0xFF);
	cx_write(MC417_RWD, regval);
	regval |= MC417_MICS | MC417_MIWR;
	cx_write(MC417_RWD, regval);

	/* Write address byte 0 */
	regval = MC417_MIRD | MC417_MIRDY | MCI_MEMORY_ADDRESS_BYTE0 |
		(address & 0xFF);
	cx_write(MC417_RWD, regval);
	regval |= MC417_MICS | MC417_MIWR;
	cx_write(MC417_RWD, regval);

	/* Wait for the trans to complete (MC417_MIRDY asserted). */
	retval = mc417_wait_ready(dev);

	/* switch the DAT0-7 GPIO[10:3] to input mode */
	cx_write(MC417_OEN, MC417_MIRDY | MC417_MIDATA);

	/* Read data byte 3 */
	regval = MC417_MIRD | MC417_MIRDY | MCI_MEMORY_DATA_BYTE3;
	cx_write(MC417_RWD, regval);

	/* Transition RD to effect read transaction across bus. */
	regval = MC417_MIWR | MC417_MIRDY | MCI_MEMORY_DATA_BYTE3;
	cx_write(MC417_RWD, regval);

	/* Collect byte */
	tempval = cx_read(MC417_RWD);
	dataval = ((tempval & 0x000000FF) << 24);

	/* Bring CS and RD high. */
	regval = MC417_MIWR | MC417_MIRD | MC417_MICS | MC417_MIRDY;
	cx_write(MC417_RWD, regval);

	/* Read data byte 2 */
	regval = MC417_MIRD | MC417_MIRDY | MCI_MEMORY_DATA_BYTE2;
	cx_write(MC417_RWD, regval);
	regval = MC417_MIWR | MC417_MIRDY | MCI_MEMORY_DATA_BYTE2;
	cx_write(MC417_RWD, regval);
	tempval = cx_read(MC417_RWD);
	dataval |= ((tempval & 0x000000FF) << 16);
	regval = MC417_MIWR | MC417_MIRD | MC417_MICS | MC417_MIRDY;
	cx_write(MC417_RWD, regval);

	/* Read data byte 1 */
	regval = MC417_MIRD | MC417_MIRDY | MCI_MEMORY_DATA_BYTE1;
	cx_write(MC417_RWD, regval);
	regval = MC417_MIWR | MC417_MIRDY | MCI_MEMORY_DATA_BYTE1;
	cx_write(MC417_RWD, regval);
	tempval = cx_read(MC417_RWD);
	dataval |= ((tempval & 0x000000FF) << 8);
	regval = MC417_MIWR | MC417_MIRD | MC417_MICS | MC417_MIRDY;
	cx_write(MC417_RWD, regval);

	/* Read data byte 0 */
	regval = MC417_MIRD | MC417_MIRDY | MCI_MEMORY_DATA_BYTE0;
	cx_write(MC417_RWD, regval);
	regval = MC417_MIWR | MC417_MIRDY | MCI_MEMORY_DATA_BYTE0;
	cx_write(MC417_RWD, regval);
	tempval = cx_read(MC417_RWD);
	dataval |= (tempval & 0x000000FF);
	regval = MC417_MIWR | MC417_MIRD | MC417_MICS | MC417_MIRDY;
	cx_write(MC417_RWD, regval);

	*value  = dataval;

	return retval;
}

void mc417_gpio_set(struct cx23885_dev *dev, u32 mask)
{
	u32 val;

	/* Set the gpio value */
	mc417_register_read(dev, 0x900C, &val);
	val |= (mask & 0x000ffff);
	mc417_register_write(dev, 0x900C, val);
}

void mc417_gpio_clear(struct cx23885_dev *dev, u32 mask)
{
	u32 val;

	/* Clear the gpio value */
	mc417_register_read(dev, 0x900C, &val);
	val &= ~(mask & 0x0000ffff);
	mc417_register_write(dev, 0x900C, val);
}

void mc417_gpio_enable(struct cx23885_dev *dev, u32 mask, int asoutput)
{
	u32 val;

	/* Enable GPIO direction bits */
	mc417_register_read(dev, 0x9020, &val);
	if (asoutput)
		val |= (mask & 0x0000ffff);
	else
		val &= ~(mask & 0x0000ffff);

	mc417_register_write(dev, 0x9020, val);
}
/* ------------------------------------------------------------------ */

/* MPEG encoder API */
static char *cmd_to_str(int cmd)
{
	switch (cmd) {
	case CX2341X_ENC_PING_FW:
		return  "PING_FW";
	case CX2341X_ENC_START_CAPTURE:
		return  "START_CAPTURE";
	case CX2341X_ENC_STOP_CAPTURE:
		return  "STOP_CAPTURE";
	case CX2341X_ENC_SET_AUDIO_ID:
		return  "SET_AUDIO_ID";
	case CX2341X_ENC_SET_VIDEO_ID:
		return  "SET_VIDEO_ID";
	case CX2341X_ENC_SET_PCR_ID:
		return  "SET_PCR_ID";
	case CX2341X_ENC_SET_FRAME_RATE:
		return  "SET_FRAME_RATE";
	case CX2341X_ENC_SET_FRAME_SIZE:
		return  "SET_FRAME_SIZE";
	case CX2341X_ENC_SET_BIT_RATE:
		return  "SET_BIT_RATE";
	case CX2341X_ENC_SET_GOP_PROPERTIES:
		return  "SET_GOP_PROPERTIES";
	case CX2341X_ENC_SET_ASPECT_RATIO:
		return  "SET_ASPECT_RATIO";
	case CX2341X_ENC_SET_DNR_FILTER_MODE:
		return  "SET_DNR_FILTER_MODE";
	case CX2341X_ENC_SET_DNR_FILTER_PROPS:
		return  "SET_DNR_FILTER_PROPS";
	case CX2341X_ENC_SET_CORING_LEVELS:
		return  "SET_CORING_LEVELS";
	case CX2341X_ENC_SET_SPATIAL_FILTER_TYPE:
		return  "SET_SPATIAL_FILTER_TYPE";
	case CX2341X_ENC_SET_VBI_LINE:
		return  "SET_VBI_LINE";
	case CX2341X_ENC_SET_STREAM_TYPE:
		return  "SET_STREAM_TYPE";
	case CX2341X_ENC_SET_OUTPUT_PORT:
		return  "SET_OUTPUT_PORT";
	case CX2341X_ENC_SET_AUDIO_PROPERTIES:
		return  "SET_AUDIO_PROPERTIES";
	case CX2341X_ENC_HALT_FW:
		return  "HALT_FW";
	case CX2341X_ENC_GET_VERSION:
		return  "GET_VERSION";
	case CX2341X_ENC_SET_GOP_CLOSURE:
		return  "SET_GOP_CLOSURE";
	case CX2341X_ENC_GET_SEQ_END:
		return  "GET_SEQ_END";
	case CX2341X_ENC_SET_PGM_INDEX_INFO:
		return  "SET_PGM_INDEX_INFO";
	case CX2341X_ENC_SET_VBI_CONFIG:
		return  "SET_VBI_CONFIG";
	case CX2341X_ENC_SET_DMA_BLOCK_SIZE:
		return  "SET_DMA_BLOCK_SIZE";
	case CX2341X_ENC_GET_PREV_DMA_INFO_MB_10:
		return  "GET_PREV_DMA_INFO_MB_10";
	case CX2341X_ENC_GET_PREV_DMA_INFO_MB_9:
		return  "GET_PREV_DMA_INFO_MB_9";
	case CX2341X_ENC_SCHED_DMA_TO_HOST:
		return  "SCHED_DMA_TO_HOST";
	case CX2341X_ENC_INITIALIZE_INPUT:
		return  "INITIALIZE_INPUT";
	case CX2341X_ENC_SET_FRAME_DROP_RATE:
		return  "SET_FRAME_DROP_RATE";
	case CX2341X_ENC_PAUSE_ENCODER:
		return  "PAUSE_ENCODER";
	case CX2341X_ENC_REFRESH_INPUT:
		return  "REFRESH_INPUT";
	case CX2341X_ENC_SET_COPYRIGHT:
		return  "SET_COPYRIGHT";
	case CX2341X_ENC_SET_EVENT_NOTIFICATION:
		return  "SET_EVENT_NOTIFICATION";
	case CX2341X_ENC_SET_NUM_VSYNC_LINES:
		return  "SET_NUM_VSYNC_LINES";
	case CX2341X_ENC_SET_PLACEHOLDER:
		return  "SET_PLACEHOLDER";
	case CX2341X_ENC_MUTE_VIDEO:
		return  "MUTE_VIDEO";
	case CX2341X_ENC_MUTE_AUDIO:
		return  "MUTE_AUDIO";
	case CX2341X_ENC_MISC:
		return  "MISC";
	default:
		return "UNKNOWN";
	}
}

static int cx23885_mbox_func(void *priv,
			     u32 command,
			     int in,
			     int out,
			     u32 data[CX2341X_MBOX_MAX_DATA])
{
	struct cx23885_dev *dev = priv;
	unsigned long timeout;
	u32 value, flag, retval = 0;
	int i;

	dprintk(3, "%s: command(0x%X) = %s\n", __func__, command,
		cmd_to_str(command));

	/* this may not be 100% safe if we can't read any memory location
	   without side effects */
	mc417_memory_read(dev, dev->cx23417_mailbox - 4, &value);
	if (value != 0x12345678) {
		pr_err("Firmware and/or mailbox pointer not initialized or corrupted, signature = 0x%x, cmd = %s\n",
			value, cmd_to_str(command));
		return -1;
	}

	/* This read looks at 32 bits, but flag is only 8 bits.
	 * Seems we also bail if CMD or TIMEOUT bytes are set???
	 */
	mc417_memory_read(dev, dev->cx23417_mailbox, &flag);
	if (flag) {
		pr_err("ERROR: Mailbox appears to be in use (%x), cmd = %s\n",
		       flag, cmd_to_str(command));
		return -1;
	}

	flag |= 1; /* tell 'em we're working on it */
	mc417_memory_write(dev, dev->cx23417_mailbox, flag);

	/* write command + args + fill remaining with zeros */
	/* command code */
	mc417_memory_write(dev, dev->cx23417_mailbox + 1, command);
	mc417_memory_write(dev, dev->cx23417_mailbox + 3,
		IVTV_API_STD_TIMEOUT); /* timeout */
	for (i = 0; i < in; i++) {
		mc417_memory_write(dev, dev->cx23417_mailbox + 4 + i, data[i]);
		dprintk(3, "API Input %d = %d\n", i, data[i]);
	}
	for (; i < CX2341X_MBOX_MAX_DATA; i++)
		mc417_memory_write(dev, dev->cx23417_mailbox + 4 + i, 0);

	flag |= 3; /* tell 'em we're done writing */
	mc417_memory_write(dev, dev->cx23417_mailbox, flag);

	/* wait for firmware to handle the API command */
	timeout = jiffies + msecs_to_jiffies(10);
	for (;;) {
		mc417_memory_read(dev, dev->cx23417_mailbox, &flag);
		if (0 != (flag & 4))
			break;
		if (time_after(jiffies, timeout)) {
			pr_err("ERROR: API Mailbox timeout\n");
			return -1;
		}
		udelay(10);
	}

	/* read output values */
	for (i = 0; i < out; i++) {
		mc417_memory_read(dev, dev->cx23417_mailbox + 4 + i, data + i);
		dprintk(3, "API Output %d = %d\n", i, data[i]);
	}

	mc417_memory_read(dev, dev->cx23417_mailbox + 2, &retval);
	dprintk(3, "API result = %d\n", retval);

	flag = 0;
	mc417_memory_write(dev, dev->cx23417_mailbox, flag);

	return retval;
}

/* We don't need to call the API often, so using just one
 * mailbox will probably suffice
 */
static int cx23885_api_cmd(struct cx23885_dev *dev,
			   u32 command,
			   u32 inputcnt,
			   u32 outputcnt,
			   ...)
{
	u32 data[CX2341X_MBOX_MAX_DATA];
	va_list vargs;
	int i, err;

	dprintk(3, "%s() cmds = 0x%08x\n", __func__, command);

	va_start(vargs, outputcnt);
	for (i = 0; i < inputcnt; i++)
		data[i] = va_arg(vargs, int);

	err = cx23885_mbox_func(dev, command, inputcnt, outputcnt, data);
	for (i = 0; i < outputcnt; i++) {
		int *vptr = va_arg(vargs, int *);
		*vptr = data[i];
	}
	va_end(vargs);

	return err;
}

static int cx23885_api_func(void *priv, u32 cmd, int in, int out, u32 data[CX2341X_MBOX_MAX_DATA])
{
	return cx23885_mbox_func(priv, cmd, in, out, data);
}

static int cx23885_find_mailbox(struct cx23885_dev *dev)
{
	u32 signature[4] = {
		0x12345678, 0x34567812, 0x56781234, 0x78123456
	};
	int signaturecnt = 0;
	u32 value;
	int i;

	dprintk(2, "%s()\n", __func__);

	for (i = 0; i < CX23885_FIRM_IMAGE_SIZE; i++) {
		mc417_memory_read(dev, i, &value);
		if (value == signature[signaturecnt])
			signaturecnt++;
		else
			signaturecnt = 0;
		if (4 == signaturecnt) {
			dprintk(1, "Mailbox signature found at 0x%x\n", i+1);
			return i+1;
		}
	}
	pr_err("Mailbox signature values not found!\n");
	return -1;
}

static int cx23885_load_firmware(struct cx23885_dev *dev)
{
	static const unsigned char magic[8] = {
		0xa7, 0x0d, 0x00, 0x00, 0x66, 0xbb, 0x55, 0xaa
	};
	const struct firmware *firmware;
	int i, retval = 0;
	u32 value = 0;
	u32 gpio_output = 0;
	u32 gpio_value;
	u32 checksum = 0;
	u32 *dataptr;

	dprintk(2, "%s()\n", __func__);

	/* Save GPIO settings before reset of APU */
	retval |= mc417_memory_read(dev, 0x9020, &gpio_output);
	retval |= mc417_memory_read(dev, 0x900C, &gpio_value);

	retval  = mc417_register_write(dev,
		IVTV_REG_VPU, 0xFFFFFFED);
	retval |= mc417_register_write(dev,
		IVTV_REG_HW_BLOCKS, IVTV_CMD_HW_BLOCKS_RST);
	retval |= mc417_register_write(dev,
		IVTV_REG_ENC_SDRAM_REFRESH, 0x80000800);
	retval |= mc417_register_write(dev,
		IVTV_REG_ENC_SDRAM_PRECHARGE, 0x1A);
	retval |= mc417_register_write(dev,
		IVTV_REG_APU, 0);

	if (retval != 0) {
		pr_err("%s: Error with mc417_register_write\n",
			__func__);
		return -1;
	}

	retval = request_firmware(&firmware, CX23885_FIRM_IMAGE_NAME,
				  &dev->pci->dev);

	if (retval != 0) {
		pr_err("ERROR: Hotplug firmware request failed (%s).\n",
		       CX23885_FIRM_IMAGE_NAME);
		pr_err("Please fix your hotplug setup, the board will not work without firmware loaded!\n");
		return -1;
	}

	if (firmware->size != CX23885_FIRM_IMAGE_SIZE) {
		pr_err("ERROR: Firmware size mismatch (have %zu, expected %d)\n",
		       firmware->size, CX23885_FIRM_IMAGE_SIZE);
		release_firmware(firmware);
		return -1;
	}

	if (0 != memcmp(firmware->data, magic, 8)) {
		pr_err("ERROR: Firmware magic mismatch, wrong file?\n");
		release_firmware(firmware);
		return -1;
	}

	/* transfer to the chip */
	dprintk(2, "Loading firmware ...\n");
	dataptr = (u32 *)firmware->data;
	for (i = 0; i < (firmware->size >> 2); i++) {
		value = *dataptr;
		checksum += ~value;
		if (mc417_memory_write(dev, i, value) != 0) {
			pr_err("ERROR: Loading firmware failed!\n");
			release_firmware(firmware);
			return -1;
		}
		dataptr++;
	}

	/* read back to verify with the checksum */
	dprintk(1, "Verifying firmware ...\n");
	for (i--; i >= 0; i--) {
		if (mc417_memory_read(dev, i, &value) != 0) {
			pr_err("ERROR: Reading firmware failed!\n");
			release_firmware(firmware);
			return -1;
		}
		checksum -= ~value;
	}
	if (checksum) {
		pr_err("ERROR: Firmware load failed (checksum mismatch).\n");
		release_firmware(firmware);
		return -1;
	}
	release_firmware(firmware);
	dprintk(1, "Firmware upload successful.\n");

	retval |= mc417_register_write(dev, IVTV_REG_HW_BLOCKS,
		IVTV_CMD_HW_BLOCKS_RST);

	/* F/W power up disturbs the GPIOs, restore state */
	retval |= mc417_register_write(dev, 0x9020, gpio_output);
	retval |= mc417_register_write(dev, 0x900C, gpio_value);

	retval |= mc417_register_read(dev, IVTV_REG_VPU, &value);
	retval |= mc417_register_write(dev, IVTV_REG_VPU, value & 0xFFFFFFE8);

	/* Hardcoded GPIO's here */
	retval |= mc417_register_write(dev, 0x9020, 0x4000);
	retval |= mc417_register_write(dev, 0x900C, 0x4000);

	mc417_register_read(dev, 0x9020, &gpio_output);
	mc417_register_read(dev, 0x900C, &gpio_value);

	if (retval < 0)
		pr_err("%s: Error with mc417_register_write\n",
			__func__);
	return 0;
}

void cx23885_417_check_encoder(struct cx23885_dev *dev)
{
	u32 status, seq;

	status = seq = 0;
	cx23885_api_cmd(dev, CX2341X_ENC_GET_SEQ_END, 0, 2, &status, &seq);
	dprintk(1, "%s() status = %d, seq = %d\n", __func__, status, seq);
}

static void cx23885_codec_settings(struct cx23885_dev *dev)
{
	dprintk(1, "%s()\n", __func__);

	/* Dynamically change the height based on video standard */
	if (dev->encodernorm.id & V4L2_STD_525_60)
		dev->ts1.height = 480;
	else
		dev->ts1.height = 576;

	/* assign frame size */
	cx23885_api_cmd(dev, CX2341X_ENC_SET_FRAME_SIZE, 2, 0,
				dev->ts1.height, dev->ts1.width);

	dev->cxhdl.width = dev->ts1.width;
	dev->cxhdl.height = dev->ts1.height;
	dev->cxhdl.is_50hz =
		(dev->encodernorm.id & V4L2_STD_625_50) != 0;

	cx2341x_handler_setup(&dev->cxhdl);

	cx23885_api_cmd(dev, CX2341X_ENC_MISC, 2, 0, 3, 1);
	cx23885_api_cmd(dev, CX2341X_ENC_MISC, 2, 0, 4, 1);
}

static int cx23885_initialize_codec(struct cx23885_dev *dev, int startencoder)
{
	int version;
	int retval;
	u32 i, data[7];

	dprintk(1, "%s()\n", __func__);

	retval = cx23885_api_cmd(dev, CX2341X_ENC_PING_FW, 0, 0); /* ping */
	if (retval < 0) {
		dprintk(2, "%s() PING OK\n", __func__);
		retval = cx23885_load_firmware(dev);
		if (retval < 0) {
			pr_err("%s() f/w load failed\n", __func__);
			return retval;
		}
		retval = cx23885_find_mailbox(dev);
		if (retval < 0) {
			pr_err("%s() mailbox < 0, error\n",
				__func__);
			return -1;
		}
		dev->cx23417_mailbox = retval;
		retval = cx23885_api_cmd(dev, CX2341X_ENC_PING_FW, 0, 0);
		if (retval < 0) {
			pr_err("ERROR: cx23417 firmware ping failed!\n");
			return -1;
		}
		retval = cx23885_api_cmd(dev, CX2341X_ENC_GET_VERSION, 0, 1,
			&version);
		if (retval < 0) {
			pr_err("ERROR: cx23417 firmware get encoder :version failed!\n");
			return -1;
		}
		dprintk(1, "cx23417 firmware version is 0x%08x\n", version);
		msleep(200);
	}

	cx23885_codec_settings(dev);
	msleep(60);

	cx23885_api_cmd(dev, CX2341X_ENC_SET_NUM_VSYNC_LINES, 2, 0,
		CX23885_FIELD1_SAA7115, CX23885_FIELD2_SAA7115);
	cx23885_api_cmd(dev, CX2341X_ENC_SET_PLACEHOLDER, 12, 0,
		CX23885_CUSTOM_EXTENSION_USR_DATA, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0);

	/* Setup to capture VBI */
	data[0] = 0x0001BD00;
	data[1] = 1;          /* frames per interrupt */
	data[2] = 4;          /* total bufs */
	data[3] = 0x91559155; /* start codes */
	data[4] = 0x206080C0; /* stop codes */
	data[5] = 6;          /* lines */
	data[6] = 64;         /* BPL */

	cx23885_api_cmd(dev, CX2341X_ENC_SET_VBI_CONFIG, 7, 0, data[0], data[1],
		data[2], data[3], data[4], data[5], data[6]);

	for (i = 2; i <= 24; i++) {
		int valid;

		valid = ((i >= 19) && (i <= 21));
		cx23885_api_cmd(dev, CX2341X_ENC_SET_VBI_LINE, 5, 0, i,
				valid, 0 , 0, 0);
		cx23885_api_cmd(dev, CX2341X_ENC_SET_VBI_LINE, 5, 0,
				i | 0x80000000, valid, 0, 0, 0);
	}

	cx23885_api_cmd(dev, CX2341X_ENC_MUTE_AUDIO, 1, 0, CX23885_UNMUTE);
	msleep(60);

	/* initialize the video input */
	cx23885_api_cmd(dev, CX2341X_ENC_INITIALIZE_INPUT, 0, 0);
	msleep(60);

	/* Enable VIP style pixel invalidation so we work with scaled mode */
	mc417_memory_write(dev, 2120, 0x00000080);

	/* start capturing to the host interface */
	if (startencoder) {
		cx23885_api_cmd(dev, CX2341X_ENC_START_CAPTURE, 2, 0,
			CX23885_MPEG_CAPTURE, CX23885_RAW_BITS_NONE);
		msleep(10);
	}

	return 0;
}

/* ------------------------------------------------------------------ */

static int queue_setup(struct vb2_queue *q,
			   unsigned int *num_buffers, unsigned int *num_planes,
			   unsigned int sizes[], struct device *alloc_devs[])
{
	struct cx23885_dev *dev = q->drv_priv;

	dev->ts1.ts_packet_size  = mpeglinesize;
	dev->ts1.ts_packet_count = mpeglines;
	*num_planes = 1;
	sizes[0] = mpeglinesize * mpeglines;
	*num_buffers = mpegbufs;
	return 0;
}

static int buffer_prepare(struct vb2_buffer *vb)
{
	struct vb2_v4l2_buffer *vbuf = to_vb2_v4l2_buffer(vb);
	struct cx23885_dev *dev = vb->vb2_queue->drv_priv;
	struct cx23885_buffer *buf =
		container_of(vbuf, struct cx23885_buffer, vb);

	return cx23885_buf_prepare(buf, &dev->ts1);
}

static void buffer_finish(struct vb2_buffer *vb)
{
	struct vb2_v4l2_buffer *vbuf = to_vb2_v4l2_buffer(vb);
	struct cx23885_dev *dev = vb->vb2_queue->drv_priv;
	struct cx23885_buffer *buf = container_of(vbuf,
		struct cx23885_buffer, vb);

	cx23885_free_buffer(dev, buf);
}

static void buffer_queue(struct vb2_buffer *vb)
{
	struct vb2_v4l2_buffer *vbuf = to_vb2_v4l2_buffer(vb);
	struct cx23885_dev *dev = vb->vb2_queue->drv_priv;
	struct cx23885_buffer   *buf = container_of(vbuf,
		struct cx23885_buffer, vb);

	cx23885_buf_queue(&dev->ts1, buf);
}

static int cx23885_start_streaming(struct vb2_queue *q, unsigned int count)
{
	struct cx23885_dev *dev = q->drv_priv;
	struct cx23885_dmaqueue *dmaq = &dev->ts1.mpegq;
	unsigned long flags;
	int ret;

	ret = cx23885_initialize_codec(dev, 1);
	if (ret == 0) {
		struct cx23885_buffer *buf = list_entry(dmaq->active.next,
			struct cx23885_buffer, queue);

		cx23885_start_dma(&dev->ts1, dmaq, buf);
		return 0;
	}
	spin_lock_irqsave(&dev->slock, flags);
	while (!list_empty(&dmaq->active)) {
		struct cx23885_buffer *buf = list_entry(dmaq->active.next,
			struct cx23885_buffer, queue);

		list_del(&buf->queue);
		vb2_buffer_done(&buf->vb.vb2_buf, VB2_BUF_STATE_QUEUED);
	}
	spin_unlock_irqrestore(&dev->slock, flags);
	return ret;
}

static void cx23885_stop_streaming(struct vb2_queue *q)
{
	struct cx23885_dev *dev = q->drv_priv;

	/* stop mpeg capture */
	cx23885_api_cmd(dev, CX2341X_ENC_STOP_CAPTURE, 3, 0,
			CX23885_END_NOW, CX23885_MPEG_CAPTURE,
			CX23885_RAW_BITS_NONE);

	msleep(500);
	cx23885_417_check_encoder(dev);
	cx23885_cancel_buffers(&dev->ts1);
}

static const struct vb2_ops cx23885_qops = {
	.queue_setup    = queue_setup,
	.buf_prepare  = buffer_prepare,
	.buf_finish = buffer_finish,
	.buf_queue    = buffer_queue,
	.wait_prepare = vb2_ops_wait_prepare,
	.wait_finish = vb2_ops_wait_finish,
	.start_streaming = cx23885_start_streaming,
	.stop_streaming = cx23885_stop_streaming,
};

/* ------------------------------------------------------------------ */

static int vidioc_g_std(struct file *file, void *priv, v4l2_std_id *id)
{
	struct cx23885_dev *dev = video_drvdata(file);

	*id = dev->tvnorm;
	return 0;
}

static int vidioc_s_std(struct file *file, void *priv, v4l2_std_id id)
{
	struct cx23885_dev *dev = video_drvdata(file);
	unsigned int i;
	int ret;

	for (i = 0; i < ARRAY_SIZE(cx23885_tvnorms); i++)
		if (id & cx23885_tvnorms[i].id)
			break;
	if (i == ARRAY_SIZE(cx23885_tvnorms))
		return -EINVAL;

	ret = cx23885_set_tvnorm(dev, id);
	if (!ret)
		dev->encodernorm = cx23885_tvnorms[i];
	return ret;
}

static int vidioc_enum_input(struct file *file, void *priv,
	struct v4l2_input *i)
{
	struct cx23885_dev *dev = video_drvdata(file);
	dprintk(1, "%s()\n", __func__);
	return cx23885_enum_input(dev, i);
}

static int vidioc_g_input(struct file *file, void *priv, unsigned int *i)
{
	return cx23885_get_input(file, priv, i);
}

static int vidioc_s_input(struct file *file, void *priv, unsigned int i)
{
	return cx23885_set_input(file, priv, i);
}

static int vidioc_g_tuner(struct file *file, void *priv,
				struct v4l2_tuner *t)
{
	struct cx23885_dev *dev = video_drvdata(file);

	if (dev->tuner_type == TUNER_ABSENT)
		return -EINVAL;
	if (0 != t->index)
		return -EINVAL;
	strscpy(t->name, "Television", sizeof(t->name));
	call_all(dev, tuner, g_tuner, t);

	dprintk(1, "VIDIOC_G_TUNER: tuner type %d\n", t->type);

	return 0;
}

static int vidioc_s_tuner(struct file *file, void *priv,
				const struct v4l2_tuner *t)
{
	struct cx23885_dev *dev = video_drvdata(file);

	if (dev->tuner_type == TUNER_ABSENT)
		return -EINVAL;

	/* Update the A/V core */
	call_all(dev, tuner, s_tuner, t);

	return 0;
}

static int vidioc_g_frequency(struct file *file, void *priv,
				struct v4l2_frequency *f)
{
	struct cx23885_dev *dev = video_drvdata(file);

	if (dev->tuner_type == TUNER_ABSENT)
		return -EINVAL;
	f->type = V4L2_TUNER_ANALOG_TV;
	f->frequency = dev->freq;

	call_all(dev, tuner, g_frequency, f);

	return 0;
}

static int vidioc_s_frequency(struct file *file, void *priv,
	const struct v4l2_frequency *f)
{
	return cx23885_set_frequency(file, priv, f);
}

static int vidioc_querycap(struct file *file, void  *priv,
				struct v4l2_capability *cap)
{
	struct cx23885_dev *dev = video_drvdata(file);
	struct cx23885_tsport  *tsport = &dev->ts1;

	strscpy(cap->driver, dev->name, sizeof(cap->driver));
	strscpy(cap->card, cx23885_boards[tsport->dev->board].name,
		sizeof(cap->card));
	sprintf(cap->bus_info, "PCIe:%s", pci_name(dev->pci));
	cap->capabilities = V4L2_CAP_VIDEO_CAPTURE | V4L2_CAP_READWRITE |
			    V4L2_CAP_STREAMING | V4L2_CAP_VBI_CAPTURE |
			    V4L2_CAP_AUDIO | V4L2_CAP_DEVICE_CAPS;
	if (dev->tuner_type != TUNER_ABSENT)
		cap->capabilities |= V4L2_CAP_TUNER;

	return 0;
}

static int vidioc_enum_fmt_vid_cap(struct file *file, void  *priv,
					struct v4l2_fmtdesc *f)
{
	if (f->index != 0)
		return -EINVAL;

	f->pixelformat = V4L2_PIX_FMT_MPEG;

	return 0;
}

static int vidioc_g_fmt_vid_cap(struct file *file, void *priv,
				struct v4l2_format *f)
{
	struct cx23885_dev *dev = video_drvdata(file);

	f->fmt.pix.pixelformat  = V4L2_PIX_FMT_MPEG;
	f->fmt.pix.bytesperline = 0;
	f->fmt.pix.sizeimage    =
		dev->ts1.ts_packet_size * dev->ts1.ts_packet_count;
	f->fmt.pix.colorspace   = 0;
	f->fmt.pix.width        = dev->ts1.width;
	f->fmt.pix.height       = dev->ts1.height;
	f->fmt.pix.field        = V4L2_FIELD_INTERLACED;
	dprintk(1, "VIDIOC_G_FMT: w: %d, h: %d\n",
		dev->ts1.width, dev->ts1.height);
	return 0;
}

static int vidioc_try_fmt_vid_cap(struct file *file, void *priv,
				struct v4l2_format *f)
{
	struct cx23885_dev *dev = video_drvdata(file);

	f->fmt.pix.pixelformat  = V4L2_PIX_FMT_MPEG;
	f->fmt.pix.bytesperline = 0;
	f->fmt.pix.sizeimage    =
		dev->ts1.ts_packet_size * dev->ts1.ts_packet_count;
	f->fmt.pix.colorspace   = 0;
	f->fmt.pix.field        = V4L2_FIELD_INTERLACED;
	dprintk(1, "VIDIOC_TRY_FMT: w: %d, h: %d\n",
		dev->ts1.width, dev->ts1.height);
	return 0;
}

static int vidioc_s_fmt_vid_cap(struct file *file, void *priv,
				struct v4l2_format *f)
{
	struct cx23885_dev *dev = video_drvdata(file);

	f->fmt.pix.pixelformat  = V4L2_PIX_FMT_MPEG;
	f->fmt.pix.bytesperline = 0;
	f->fmt.pix.sizeimage    =
		dev->ts1.ts_packet_size * dev->ts1.ts_packet_count;
	f->fmt.pix.colorspace   = 0;
	f->fmt.pix.field        = V4L2_FIELD_INTERLACED;
	dprintk(1, "VIDIOC_S_FMT: w: %d, h: %d, f: %d\n",
		f->fmt.pix.width, f->fmt.pix.height, f->fmt.pix.field);
	return 0;
}

static int vidioc_log_status(struct file *file, void *priv)
{
	struct cx23885_dev *dev = video_drvdata(file);
	char name[32 + 2];

	snprintf(name, sizeof(name), "%s/2", dev->name);
	call_all(dev, core, log_status);
	v4l2_ctrl_handler_log_status(&dev->cxhdl.hdl, name);
	return 0;
}

static const struct v4l2_file_operations mpeg_fops = {
	.owner	       = THIS_MODULE,
	.open           = v4l2_fh_open,
	.release        = vb2_fop_release,
	.read           = vb2_fop_read,
	.poll		= vb2_fop_poll,
	.unlocked_ioctl = video_ioctl2,
	.mmap           = vb2_fop_mmap,
};

static const struct v4l2_ioctl_ops mpeg_ioctl_ops = {
	.vidioc_g_std		 = vidioc_g_std,
	.vidioc_s_std		 = vidioc_s_std,
	.vidioc_enum_input	 = vidioc_enum_input,
	.vidioc_g_input		 = vidioc_g_input,
	.vidioc_s_input		 = vidioc_s_input,
	.vidioc_g_tuner		 = vidioc_g_tuner,
	.vidioc_s_tuner		 = vidioc_s_tuner,
	.vidioc_g_frequency	 = vidioc_g_frequency,
	.vidioc_s_frequency	 = vidioc_s_frequency,
	.vidioc_querycap	 = vidioc_querycap,
	.vidioc_enum_fmt_vid_cap = vidioc_enum_fmt_vid_cap,
	.vidioc_g_fmt_vid_cap	 = vidioc_g_fmt_vid_cap,
	.vidioc_try_fmt_vid_cap	 = vidioc_try_fmt_vid_cap,
	.vidioc_s_fmt_vid_cap	 = vidioc_s_fmt_vid_cap,
	.vidioc_reqbufs       = vb2_ioctl_reqbufs,
	.vidioc_prepare_buf   = vb2_ioctl_prepare_buf,
	.vidioc_querybuf      = vb2_ioctl_querybuf,
	.vidioc_qbuf          = vb2_ioctl_qbuf,
	.vidioc_dqbuf         = vb2_ioctl_dqbuf,
	.vidioc_streamon      = vb2_ioctl_streamon,
	.vidioc_streamoff     = vb2_ioctl_streamoff,
	.vidioc_log_status	 = vidioc_log_status,
#ifdef CONFIG_VIDEO_ADV_DEBUG
	.vidioc_g_chip_info	 = cx23885_g_chip_info,
	.vidioc_g_register	 = cx23885_g_register,
	.vidioc_s_register	 = cx23885_s_register,
#endif
};

static struct video_device cx23885_mpeg_template = {
	.name          = "cx23885",
	.fops          = &mpeg_fops,
	.ioctl_ops     = &mpeg_ioctl_ops,
	.tvnorms       = CX23885_NORMS,
};

void cx23885_417_unregister(struct cx23885_dev *dev)
{
	dprintk(1, "%s()\n", __func__);

	if (dev->v4l_device) {
		if (video_is_registered(dev->v4l_device))
			video_unregister_device(dev->v4l_device);
		else
			video_device_release(dev->v4l_device);
		v4l2_ctrl_handler_free(&dev->cxhdl.hdl);
		dev->v4l_device = NULL;
	}
}

static struct video_device *cx23885_video_dev_alloc(
	struct cx23885_tsport *tsport,
	struct pci_dev *pci,
	struct video_device *template,
	char *type)
{
	struct video_device *vfd;
	struct cx23885_dev *dev = tsport->dev;

	dprintk(1, "%s()\n", __func__);

	vfd = video_device_alloc();
	if (NULL == vfd)
		return NULL;
	*vfd = *template;
	snprintf(vfd->name, sizeof(vfd->name), "%s (%s)",
		cx23885_boards[tsport->dev->board].name, type);
	vfd->v4l2_dev = &dev->v4l2_dev;
	vfd->release = video_device_release;
	return vfd;
}

int cx23885_417_register(struct cx23885_dev *dev)
{
	/* FIXME: Port1 hardcoded here */
	int err = -ENODEV;
	struct cx23885_tsport *tsport = &dev->ts1;
	struct vb2_queue *q;

	dprintk(1, "%s()\n", __func__);

	if (cx23885_boards[dev->board].portb != CX23885_MPEG_ENCODER)
		return err;

	/* Set default TV standard */
	dev->encodernorm = cx23885_tvnorms[0];

	if (dev->encodernorm.id & V4L2_STD_525_60)
		tsport->height = 480;
	else
		tsport->height = 576;

	tsport->width = 720;
	dev->cxhdl.port = CX2341X_PORT_SERIAL;
	err = cx2341x_handler_init(&dev->cxhdl, 50);
	if (err)
		return err;
	dev->cxhdl.priv = dev;
	dev->cxhdl.func = cx23885_api_func;
	cx2341x_handler_set_50hz(&dev->cxhdl, tsport->height == 576);
	v4l2_ctrl_add_handler(&dev->ctrl_handler, &dev->cxhdl.hdl, NULL, false);

	/* Allocate and initialize V4L video device */
	dev->v4l_device = cx23885_video_dev_alloc(tsport,
		dev->pci, &cx23885_mpeg_template, "mpeg");
	q = &dev->vb2_mpegq;
	q->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	q->io_modes = VB2_MMAP | VB2_USERPTR | VB2_DMABUF | VB2_READ;
	q->gfp_flags = GFP_DMA32;
	q->min_buffers_needed = 2;
	q->drv_priv = dev;
	q->buf_struct_size = sizeof(struct cx23885_buffer);
	q->ops = &cx23885_qops;
	q->mem_ops = &vb2_dma_sg_memops;
	q->timestamp_flags = V4L2_BUF_FLAG_TIMESTAMP_MONOTONIC;
	q->lock = &dev->lock;
	q->dev = &dev->pci->dev;

	err = vb2_queue_init(q);
	if (err < 0)
		return err;
	video_set_drvdata(dev->v4l_device, dev);
	dev->v4l_device->lock = &dev->lock;
	dev->v4l_device->queue = q;
	dev->v4l_device->device_caps = V4L2_CAP_VIDEO_CAPTURE |
				       V4L2_CAP_READWRITE | V4L2_CAP_STREAMING;
	if (dev->tuner_type != TUNER_ABSENT)
		dev->v4l_device->device_caps |= V4L2_CAP_TUNER;
	err = video_register_device(dev->v4l_device,
		VFL_TYPE_VIDEO, -1);
	if (err < 0) {
		pr_info("%s: can't register mpeg device\n", dev->name);
		return err;
	}

	pr_info("%s: registered device %s [mpeg]\n",
	       dev->name, video_device_node_name(dev->v4l_device));

	/* ST: Configure the encoder parameters, but don't begin
	 * encoding, this resolves an issue where the first time the
	 * encoder is started video can be choppy.
	 */
	cx23885_initialize_codec(dev, 0);

	return 0;
}

MODULE_FIRMWARE(CX23885_FIRM_IMAGE_NAME);
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              // SPDX-License-Identifier: GPL-2.0-or-later
/*
 *
 *  Support for CX23885 analog audio capture
 *
 *    (c) 2008 Mijhail Moreyra <mijhail.moreyra@gmail.com>
 *    Adapted from cx88-alsa.c
 *    (c) 2009 Steven Toth <stoth@kernellabs.com>
 */

#include "cx23885.h"
#include "cx23885-reg.h"

#include <linux/module.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/interrupt.h>
#include <linux/vmalloc.h>
#include <linux/dma-mapping.h>
#include <linux/pci.h>

#include <asm/delay.h>

#include <sound/core.h>
#include <sound/pcm