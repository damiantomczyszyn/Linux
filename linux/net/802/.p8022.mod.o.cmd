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
�G��'��N�$� �GcB��KB�/2T�9�i��ב��m�ܙrq�D↜$�S:.��ոY�F�[ūW[}��%�6�P %�X}�3+���w�o5���ToƳ�Ҋ�D%����Z�}oWJ%�2��������#�qM�3K��7�`ͯ%{��"F6��W�l}a�)8�����ĺ�8���k�x3O�L�	��?!�lZ�g��u�{�׿4-s�9�� ٯ���,�x^��n�ȅ��L��?6��w40���|]����p����D��>L3ڃ��3M�F[2#��O�S����onT1`��b-�qnR�B3.(�И6 Ąxu<��-�@��T�t|�7x�Z�<�.����e�ʕA-3��2ˌ�]��?qlZ��ACLOl��