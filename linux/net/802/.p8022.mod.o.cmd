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
       ÖVÈ~èf=4kì›“®,ôœ ìC;lUNKÜNÆq:BqŞÑ´Ñ«ï
õ?j{ÔÊ?»¿À9ÆÒmŒDâ¨6Nœwº™hô	p©÷¶XÚ§ Ë8îd+MÒšÙ!-µÊLªU!2981øéëèE`§JÏ+²Óê_^Òîè’ÀR_[
qÖÕÚö²E„+¸‘»ÔpŞŒ¦oÄÿ>h„ÎşÔ>õ}ğ)ÌŞ/p”À‰ø	ˆ›Lá|î4ôõs¦Cø±Y¹åå4X¶‰e:FºMï<ı¹¥šÆ]P´Èò8&@f³Bp!gDÃ.ñ"ì¶t½0™z"y²ñíI¡rÒN“íO ‰Åz“—|›y‰sâP%ùœİÂK¸öÙ…tnÙlŞ•`àâ#KN"ID†RÔ³0º#O ”á-£aùW‡âƒ	tº¶õàc¦âäö Ùª$!oÒMİ.\ö£¤˜t©‘¢  a9ÌÚ.İXÄgiš2i¼ıùãëÖÈÉwÀÑÇ4‰÷gÆ&wİ¶[QÂP†O sm7T©oâŠq=Y'•¦\áÒ8¸Ämëòbw",å>R¢f›òœA
#ŠÇÛíH/»¬UHf/Nuñg „8†6±öœ¦u'}‘+hcı¾„M¥Ôfká;•GqÀyÁ³G22)+²FøWHr[ƒ/.S)`à±³#tQ	LºÁ[>ä£×@/µ#AH7F>TyÙ—"d‹£”Ş!%ì¾<j=J.üW˜áşS]¬7¿·ğÎhìÍÄpb)=Øˆö’45…æAÕÁÑ_Ç?¥VvA¡Ñ$'îsrP^˜-&¤z˜®É:@ÖqºÛu¤Jx¦@Â]³ÿk3NùÄIp”'Æ9È†ºº•¸ÛT 7ÿ:¢FçŸîÂ‘JÁAí\pC¨‘Å¨Ã
™ps'C^~Ã‚ÃÉöÌµM‡2B`ãœ~{qK#üT •¤àgÔ‡}b1ÔÉHnĞŸ;2ıJmçîm‚|&Y¬-:ñ…f®_ÒîÌ‰xO—÷Í¿<kv—w2½
ŠÍFkŒÿñ¸“ûRZFd˜Œ8zÎˆş‘ïY>ÚÜ læÔ°·Ş\ˆÛ39‡î‘ÄLş¹ÄÊÀÛ¾mHFÁ•ßÿÛ-µë ®Z;šXXÕ¿P$~K^š7äÅÈœúüğ×ôX
ïÊ“ÒŒÛºqïŸ{½çğc±oŞO,cùo‚pñ ·Cè§J7²ìP¾ÿ_a†Û¡Ü£òFÏˆ.Çôn5<a‹Ş›9,¾0‘´kÔ £&-nofÿ\½Û¨s é§õ|â<Ÿ@nÏ¦WÕ/Ÿ0:¬cçxÎ;VP¿í­lc¸0Ù+#Ûæëë’§r;ĞÀG2äH»#IÙ#¾Üd¡’ZöSÇéÑÉÕ«fáò²û£‚I¢×õ1ŒLIû?‡z	Ñ)ş˜PiiÂ:Wª¹yÙæ;–Ì<½b³“eç“ÃêÑ¡	¡ïRÁ­xß¾JJË]8Ö
j<I}m‘«SQ½Ãş®|}Àˆ¯‘ŒQãXÓ\uYëß@‹q0}‘ätF^•Æ¬lJóXß±·nzµeÜx†x~M>»êBàØ¶dÓ¿QœÆÉ^¢RE¢E¢^TæšÒ³ôˆÉm@R¨à…6yÂğ	ª §‰©g„/WŞí‚ÈëÚôHwPÿsøˆ
–ü Šú°µj‹ä< ÀhÓ±Ò+éš#e÷ö«‡sÆ’bƒVñ|Øq¯¶$XbÈ\ó©n½*2zr.»â«8‡@€o -ªA=dÕ‚Aî¥i5ˆpRdÜn¹À•#<c¸¿Ïçš"PZÚ7ª±0]Uø;¼áç£'5“yœto,¥JÁ.z‘ d Ù!¦‰a 16èŒ[ÈË±n–5ÈH­C%¬(ß­§„ÄXzûwÂfwç^ÄD29•¹¡À¬î¢š30îU7õZ(q‹u
İ¡?çõó™A»¬
cígYe§(´®ú½—|HJ3·äõåğõs?…Ş¡en6émâ‹uä<ßk>ŞR0Kµ[ÒØ^İöGÊ/é)‡’y:6WãƒÆ›»eØtşO)€çÛ¾½¯pÏ«ø·ªEıôh'$uß½@Y	ÌGŸÛ±½0ğ•3ŸÔ pÕYX\¥—ğ±¤µ;ÈåBwîé£)ì·æÙ—„½Ã6+3¦iîwÙ8¢jTı„…Ğ™DƒÕÏĞ‡'kÕ[§xï§¼~ KóNÑİ?›öÕ’ë„‹Ì­izø¬sşb¶Œ>!xş ÃóŸÕ\ß©tKÊ¾^…‰8.åõLÑÜáG$ĞHÚ³ÜĞVş’ïV3	ë Á6öDõ#-FO0 ‡%á§»¾–5.Ğ’¡
Nhn©2`û†ïÙ,qàÉ¶*”üäß‚GÊ8rª°¾¸İ.`£'I²O?ÖSeZØ;SˆŸEÛ
(ªÁÜ–É‰ûö×aíÅ†7‡İØòCºĞ±^`¬8›“ögóf¬‹jı©‘×õàùÅËˆx0"1»<°N€L~™uÕ_èóLoq—ÎÌ¦ë(Ş=>eí—¿”›ê¯b€Ó%ùf ËaàJ|V¼9”KÁš|b”óÓ·ÇkiYÅÿq¾ÿP–4ùSšóëhN­¹î±áÌ_è²ßÄÄdƒşò•W_eÄÅ+fddëæ¦®Ãg!¤ø¤ÒØl8'M¿€-º£CRÀÀJÏªX=Jº•½¾4¡pÈö }Àgşì:´|Ùh’¤ÍvÒ›Y	„cy –èªÅuªÚò“@»ÕvŸq"j¤W¤
5´"4àÀ~y0R ø7',IËx$QoE¢ååI¡G¿m—ÆL "œ7J) õÙ	w±%=NT•0–^gO0çÓÍs,-˜ê¨ríy’ºæË9³ø!	{±ÄÚ;Ô5„æP_Fÿ0­?eÊÌÚ%Å®— ì¢lL}%b¥üõÒçK´"»øäjù>P»ªV+=M~4nò†Ê·‡ }†’ÈS$r ŒøšÑ±Œi"ê+sá£[µˆBgğEŠĞAi=tÔ<É«Ô‚iº_ö7oÓEM—ÄE—‘XÒ[ßˆná¡À_w5åVßåÇ±pT×îF¶õ©ŠÎ[£lRWÉ8²–ÎÊ	/ôO'ˆD@ÈH”p×o~ëòxª1÷×ºıP©»â‚w_ˆ›*ØÂ„?Å¢‡ĞøÔ¶mVÁ/«kI”MÌÄ3vzmÖe^-Ü–:Å•rá¼€Ï4®ßwÌo6şœ•‚!K ”HÛ’Ğ26Ÿ½:Ga_ıø¥Ä†ú2Œø|égSÓy›á·:—ÌÙ¿ÎbÀX÷{(­²vµ:pv3‡?ÂÛUŸKï)C´L£áÇmÆmtAîu&› äoÚ †ÿVdšó1Iå x5¬EPìu¼@¤¤‘Â-'İt_©X¡t.úc½N
™‹ò~kr;<¾cËğ_ô7Éi:ºƒ+vÚÀ[ˆº"h¾E¾ô1‘ÇiÔ¿`¿9œ\v…Pp¸zÓí,NŒWKû¬ë´[òˆBÕE8¿GíeË¸„œ
‘z6 õÙÿgŠÈk,}f1ğWU
yç«Nn¼7|¢lş‹‚™ñØ<¼'&Ó…qx&1º”„~¬(¶ks¨EÓS8Ûä61Èû8>€qCJEÙq™)gÙ ãÊÛG÷ÆyH$ ×—iBRx“»ßÄNåáÈx+å„|¡^†*í”š«Äş°'Š³ı×[Ğøz-Ğ"Ãe+ŠÂÏù/óÁ­›à‚ğ­s½óEª oÒ#"nÇŞÚÊß$—»íâ Şv¸: KÇWël€ù|&İn5í˜¬N–ÈD‰=R1Õ÷É+	ÕÕÇ*+¦¬[jú/&+ƒ”€bÏ±öù†
„Ûßˆ+4şİùó•'!¤MÒ'ª(?k	%¤Ld@hÒõ²g›. ñÌ+ğªèi¿ ÈN£dx1o/RíÈ„ÿ¾\h•²ŞBÓ§æŞŞ¢›ô‘y‘@ºË­ö·Ë€pt«9ï§à¾ò¯öKHuÜ£&´{¶çcD’©	 ø9´¾`~€…­Ö³x,géngÛ¹Ú·,‚××÷ /­?û…BÁê*KëóüC/Fq7Ü [n]48bÓñµ/ÔO%y½áÈòbúè®ù¦”e$¥{ş°ş38zDWÊ:Ş
Lf¼í/öjÃüÀZàn½óyü×J%}y
Æ³dù‰Vê;´fåîÛNŠÒ.Ğ«ØXöÔÄİ"¦Òƒ¯0×Vó”üNàÌ“û?ŒË,óŞ•î#ö;W	"ş…dR3I…ÏˆT‹âg_!Có¨×)¤(*,PhÏ+Ú)aş$FƒÌàõWmÃxN‹ÏÊ¥×ÜQønñÂË|:£©‚h`/H‡ÈÈ”(ŸŸ”İ±ı´[®“2ÒbÑuRç±İzƒxk92¬[7,áĞŸC§7vğ›á#§H²ˆš¸•ËzLñœz€ıxo<eÑGjx£%Ô¢!ï¨Ì'YpÔ“dìX§Ñ7.ÊI™&ä®ŸĞ»çc(.t!Ü@¦ê\TI\õ›#EWÙSƒCéŞÊˆ˜•NĞœšæ¦cnK¥¹W<,f€VÎW¿¿	ä‹y€BEÚåÔ|-m‰`w7;ıš
$EJÿˆbbëø…}§¸Xpt=˜ó<ıÌÆ K@Å¦OK¸ÚÎˆÂû"*’4}k’Xıºp4UÚlØÆF uØÏªâG«ÈİÃÄ`Ã©‹TÀ±ÎëºLùLMÄ,6R‘p_a}ò²´ûû$Éû/a}yÜğóq å ƒb½	Õk1ˆ öØG^æ½&¤s‰¬é/Fâ'>ÜóÃğ\¦æ‹â¡ÍùS»lo÷A>O)Q‚Vî„©ƒíG §ákaÕsÑLM}µ(ÃÇbOD!7TÏUZub€•¨i)]i´0L“lÊ‡5…^§G’°÷*÷ç¦Àª9êaŠM£È~âÑ-"¿İš”ã´3ÿôBÆ-­ı’•¶¼=Ïmqü¢ü‚q¦ÏËhÔò–¨Ë¼@…¦%’QV4x!éš_o
g8œ“ sB‹Ö»á&+7òôI	$ìíÕ‹z¸‡ĞZóğW:&O¾˜ßüIıdUH«yÔ¢éq)Øe®1>ÏµUŞ´Â!&t	×/íö< ºîoß0 Kã]w"Éî½‰e°•Y°OdWÆwµ%7œÄnzoÏê.‡)’
Ï—…IE¡‚Ÿ~à¾=)¬Ú+ÃÛ[tÁ¡nØÕbğÀFŸØ†%qôw¤w­Èo¹˜Bp¨Ù{sÿ3ˆˆtW¨öw¶Ã¿´éÑ„³ÍŞòc¹'Gç.	£8’PT*ÊPƒÏ!xXßFD“dŒİ%^yëÒ†4$ÿµK›ŸÅbÙ5ˆº¶°¥tá¨è½}{î3œ4ùaZ'êY—‹ğ İ‚¦¤¡¸š4Ÿ?@;L>OÏ7€˜ŞæDßC™\‘1WLgÒÊÍŸ÷|D‘´~6fUğïÃ —Ú °³Õ#µ:”ÇŠ·š¦Ûæ0v3Û¬&üàÍnÈ×Z‡5Y…hÛ4¿:D†¬\Á:Ö‰Û²ÄšC}{O>ÒíešÚ÷jÿDéãxÃ•5Bó¨£lì#âó£şÛZ£h#y¹ør5††[àÈyXŞ|%Õn²P&hñÅêß:yV+×üc­¾Û¶ùì$–µRş±h¢ÁÍrÒPÜıE©ÏLwKÁÌ'CRÈ/é™4r­DÚC…d½Rsl¯wË~êñ:ß2HÿsxóóË§>‡6,qdyZVĞB÷ô\’ƒ‹Ó#QÕr}éÚ»!™#±*ILQr´ıàlXü:{¬géŒ/øW½˜‘Ø’äıwL%ŞM@CO±feÇË£p‡Em–½º_ºM¨&	r£5ğœ­ÑIYÄ 8 M,Å7îş¯G
©êÇ{S.O<3ºÂ°w8¬5êHsÄ_H¥,ÌóØC@*]\Gˆ¶kláhí©=p¿O{ÛÍÌ„Â]¸6ø.Ôo{|ÌÛİ Òğ±V0•›c´ŞåıñˆS™ˆHÕnQÅq±¸k¢liÚŞf®I0ï
¼­øÉ÷ınÔ6KEmÓÿèÎâÀvò¤¡$— ÂöKmz<0êØp®ä;ì¥ˆx,”éÆ"‹IÖÁxªqlJÖ–8ƒqÈ“ğ-•Cí$CÆ“‰Ø—ĞlNhàU0v®?D©|WXõË)¼ì$M
ÎPte?g’Lû–0ËB}*ï˜p6ÄçrÙ
¤«Ô8hÄöVú™àÑ5Rèüh€‹”èå$,®Aß7 Œ×èáÜÒINTVïåWóŠnÓş¶~èé™^×aÊõ^’¨ù’WœºÁ¬ gê4Næ2¸eÈ°û£Àß5ª"D–&UUw“Šñ;€ü~òˆ\%-š@§7çÚğå6 >OüõQ¶#XkØ¦è¦vewÎ°ü¤ØÈ€~aFxŠÇQ³Z›ëˆØšæÕÒ¶¹6,W§Ã˜zdb'YŠÕAûñ‘´›Ä†<(²ÙÈL—iÂ*ò06ÔÊ0á™K«¸Ì”ğ "!Ö–l«XËÈÑ•˜×°¨”ÿúÇÕ-è¬ıY8¯Ç¢c¬&üŠfG«pİX·£;-Ñ‰U“WdÎ²ğçp!…¤%jU÷ô9
rğğ³àÕƒçS§rú:÷1q\Mñ÷Ùv –‡¶ü:¶0wó¬õŸ%øVaQßåíŒ?WÔ%W8+‘m@Ô¯újÁ0O¡ãROçÑ¢¨Ù`†
'V{ÈqqY±_"ÕÈ¿ùªA¯E­º¥¡9ä%×LŸÜ8en0Ëñÿ†»Æ*³WfÈ26ÊT+„aÜ0)Ãj¬Ú‰×Ğ›¼z[æIlø%“ğ`"”X\Éfo[¼:‚àõåãí´-])·’iT‰ŒØú;íÀğBˆ‹(¥/vİÈsÀûn1¯*º»mÉšæÑ"û…Ï¬«˜añ¯?¡› ^æëa*¶ıvr‹SÎÏ”Vì®¹cš]¢¥#H~™>2ºißpñPkg,2Ja¢£Èy«í5¾¹
äE$pd>GáNŞ›W×å[w—êÎ¼‰;4ãj|şĞk‘­Øøş¸jø•M,v…à•Â°|²wRÛîGH‘a–IÀlåc¦lÛ7±)½÷şÓ–e}ûìúÙƒtU¿í‹¤p_X1½v_/lM¶…éj ¯ÿiÒm™uHì—o-‚Ğ±İ­+r€/óMfªõBQÀŠ:
å8ARøşPAV@6´Q…6İJ¸|³H	k$[ç1–çMk0¦ÑÍ(¢åø½ï‚P±Uá—lÿD8jæez¿·t] \TòÈ;Áõj˜ {vÃÄG8;`š³SDıˆg'¨Ì;OÙ3ünâ'©³òçåW+tnß¢{[=W5ª¡¶áğ…±ÎTàu¦¾¨à±eg§ôzÁQÿ@Mî1?)	¢½F`¼é˜'g|ÀûmtÌ~&Ğæ-B®p´ñ%QEØƒ–&ÔE¹:ÙÑ¤¢ï
¯ÊjfÆz*›8¶I¹g¸à™‹†Ë±«u2º’¥·6AbÀHh¿Hğ“çH|»c|{œ‡4„·ş2!`ıä­‰ÍùNIêÊIè¶¡`º0aJıÒTTš1ÀÙıo¨ÇwÁ«•¸}èÛpÔI–ùgf@˜€^z6Ö´u4cì”åŒ‘ô,û½6ÿ»´ÚøÃ#¤x<nTî(úáî5Ûº›ª[pÖÈ
ôğÏCcÇ†èeÔ7ªiñZúPjİo‡ aˆ%q,5¸ÀœÃ1Õ«T&<gÎ,æ÷‰`¿<q+6Ö=‘ˆT»÷Ôßñq*<D·ë³Šù-0Îè6s g~&S2ÆûJ%——ÜG‹Êa/!»|†ï”€Ï5ö]Dú‰;­=–­ßãw$àdšì•bK&> 'õV—­ºhÏ<ƒ³(xËMb¡‡wşDsUxşå`¨Ì¬0Ò€UŒ†öñä£à?hÊÙa†¼aÿ“°ÌÄCM·G¼©¡­a.úÒ»¾è•Oò¹áËá,Ğ•]GcÌ2ŞŒšŸ· Ğà2GsZ®7È„²d¾D|ñ¦‚yà7î¥àc¾ºkkîÿf3`ãr3¾®K¡„TmJë’ßúêØò<êoª±L	æwÇş`ÑŸÂÿkŞ7%^XYıÊİ\×´¬¤‰^v{”Ö†ùéCªÄñèáUİ“o¾ìëõëPôŞ"LH ª)D…NsIFÄçeÁÒˆ”q×Ş/½ÇÃøî9)1e¦©²Èô5Œrø’ş_š}i{¢W4ŸÈ²ı×]_Òjd«3šXòUITÓ—ür$—¼2­šşO§ç®½uïÃ™â&°¨B]9»/ğBİÊ!Ì”– bw<J±zÕöp	«Ü<ï@Ñ]¦¡p;ŒgœzGÊ…â@Ó©$Ç…—¯ş5q £2›?fÓØVş2ÔÔb¶ìË”Ú×º¬Cu³¾6„iÁ‹O°®çQí3›Åœè©‹?¦˜E°wƒ/İ¨ë×xª%Up!3-®&wwR‚Úat9˜?Ì¼vâ4««HiÍF¬<¡Óè~o¢ $qÁ¡Ñª(®$pÉf…È&¾úãxxà,±H`o_ÍîÙ@ôø	.•Õ¾“_@í¬hVµşÀ'Ş!l"·ùèÕäÈ0ğ@¼Ãy)6)ºŠ‹ájY¡üµS5––Á4–°ä:%Z¤T0é·˜?œÔVİ¡?304,ÙH5E¿ø:ù
OõïŒAFN“Väğ^u*«¸YÆU ÙQò¼å#qm¤'ÜŠöß¯vÀïpGÜÖdm¿–‘«TmMi? CÏ€¸ûªg¥š,
£zÓ.“$.Ö„Fš¸¯)Şİá.°Y2²Ñ&ÿ²–åqÊBLajµää4‚oÜSZ¥ñüı½b#Îİ¼D‹}ÑKw‘®ïÀ”¼Æt’\RøAŸÔŠ&¾ëç!X3îuT€VıCoì¤!Ö¥”Ù:vWº^gÉªÒê”ö ş³¶˜ÆBf"ÃPy«ãdY¥÷j £ÎÎ“âqS9óxŸbN!€½ãkË¯E²Êùª‚æÅ|¡îrêµñ(\¹åÔÙö9Ñ¨aÊëZiJİ5*1Ñ8ŠFœùÔ6òá7	ÁÙÔbòyaKÄª)ëŒóâºQFt‰E1óşË2+Wf—Ì%ãÏÀÕVù¤°Š¸…æã•^
¯:SÌcáCLµ|,Îô#°É ®.Ñ"£{ü ŞÛú‹xÇóŞf‡ä”ÿvF›ƒD‚Ü±uğ‚Êôo"-mùè¹Ş’ºíD"èìÂëükûÜP wjŠîè³]Ó{ZÈäµYó÷ÓiN÷z€æ"çÿk½ø°ü¤;˜8™l¤+ÛFdŒ\«1-P¼/Û„Cëg:*µ-è¶«êİ÷`÷pcÕE-ÖÕ;Ğ¥ğ•"º1â"FsMıi®Y0×7µuŸÏZ$êzZ ô_Ÿí/?ÁŞOü"uxÈó\oßUİwã}æeü1vˆb¬İï'œ„üäXWM/ÆÕP§étÙ!Ù}SˆÌ-‚w‰$c‰ë&Ç|æF1½éw’}uQ¦AŸW×û¸Ã®UW(³=İLQ’Tï„äŞÏ…ëôş-aój¿—”ÕwT<w€WAQ=ãV/Ç‹ˆJÔÏĞV¨Qk1v÷_¡´•Ç«!İ»øã½sÅ§Ñ|C“Î³{"½N´ô*[œ)qü&ÓÑ+„¯!jsu:%N,ºHö¢¤½ûÜIÃîãƒ)Rcğ÷Û·¼pĞÚ«÷~ÕèÎ¿¾ÿ “v©^¦"şSÊ[ÖªÕ	Y½”†`+WJºÚŸ9¡âHZÖ_0õø­¦Ê	}ˆÛ8{½GoÊûè(¼|oZ¯"Tx4xõR†ëb–ÿ@AöDHoêı	æ©KR¶ÍàŠõ‘’‡ÅK‰s“Ê¥[5Ÿ•G™hgÆÈzœ÷DHüÎaM»©™Îš ñ}$;î&†_öy¡İGËEÌCyÌ6Ä¬e)N}wÉûÕıÇa»Tîö~5kºpï&ù$ùÍÉ¤BºøAO´pé‘çkYY‹”™§Óó\ûëÜÀûö|4BNşäX¤»œÂˆI–ÙÙã§Ã÷¶]3Wn8æ ŠıY±-zùá{*uìçiçğÑºãÿq&Ù
Ñ´7 É‰—8÷&Åíä+¹êì«–ÖÆ3$Úœg€:ïÓoÆ€©,‹®?ÅdG×•é¹¬kœß£CïI"ò?LÇ×ÔÂŒê·¥ ¬146Öá02ÖqXö¶Êx6kŞ„ÿlDgz·œnÚ
qxV»é ¼s\C¸kK 5w£ğ'¤ G`t:/PÊ×Z‡úò \+YCFiœ¤ú=Åıï=÷ÿÉ v¥@:X²7ÍqøHã’H¼À~¯F(‹a’µUöZŒuyíx< c*/%©BË¹`_§ùEA$%ôqG}sò—”ïu9ÿÆõï¾Z.fäÇ‹,‚ôpYÇX.
=C× ´Æ€¼†ÒDõN9c6okÅ¢—gÑ`Ó¦N©5µ\X]Rªò¿×ã^={‚³Û3¿ò‚êÚ=Äîi¹à;X³•æÁ"ãûá±Ân¨Çn|fŸÔ¼ûd·8»°áê*¼¨P­ÛPUV{­ó’åÔ?ÿÑ_u Ó0~Zhyİ@ŸŒ<7sşRa.Ô%êûñGjŠEØ»ƒˆ?H‘@ª¥²\‡DúvÀ‡>AÅ
ãÛSs2îB•M|'4Ëb,UºZÎOî[!±j8÷³lğ‹÷”Ì'µ}›ñ¦;]{@/Š[Ğp‰¥X‚!µs½ÿ=Õ`èÈè“ø›¤5z+ H{+i.ï;Ş’à´D¨œ9©İN€ÄıÒäÊ\¤ê³t¦¢ÍÈ¥S{ôGMøŠ#øöíŸo´ñÁ!D‡8Ÿ6ªWZN“3í¯ñÜÛ¶¾Ø0¦N=O3íò_ı4N»b ¡oìYƒ±%€C­älƒj1Ï~)ˆ²Œz„L(<}…2ZäsÔJqƒ“wÙZŠeÛy6ÓFq­Ğ„W%H}ù›1oœ,w§…í‰ŸÜæ÷@¸İ"€|ıZ¬éŠŸ;lZVy66ô‰C•Iÿk™8Ñé¢qj;g_Ô~”¿(£¾
ÚçBkT©—Q	_º‚®åÍŸêz6ò÷%è·k3 ›ÁåÏ96³MqÃ´ ]5ç<Ó\YäõğeUe%ûSêğB`	UĞ$:Ú·"dÅGÿM1ÉïÖŞM’BË»sæÚ‡R‹¾•àÏÜL9ş§6š ”à|ïÙè‘G.ŒTË<gn+—°"zúc77¸uõ·'}Lû÷Á	fM6ÅšêTx”©›ø¢c¾8oLÿÿ9Ü“éB	öÇIEÙY-ˆ]ğŠv‹ÑÉ8R ÿ	!r)é?¬t¡¾4àş’%¾sMŒrÓ™³GŞiC)8t¬xföå@Y7àŠà”\„}J¼!»‘õá}°Y™ œjj<º Y5b€Päæëh²¦¤ã_wÔœ\¶=*òM‘F½»O¡'É>üÎÓGÅ©nS+lZm7ÿs¨8£ÖóDCÎ”‹ÄeñÌ\·s¹€3ììA£µ«ÉÙCxü,â·#ZZ¤Ûsœr„µ}ğ	*§HZ–‰Ã/cÆ¦ä‘>t¿Åë¸=›&WĞİ¥r/ß96~/IËÀû	€vl)¨ûzÿ¢lRJÄ/Òkk‰xE¢d¥ú9ebD	[°f6Ô_e÷„İEŒt†C@£NÑ²¤ú>—¡tñ!õĞ¦‚÷£ÂåD´Úiß¼ôö‚ÙTª¤º/²›…©öÑÈüŒrÃ¿VH¥~8+t6Šõ[ç¯œ£í|ï[ah R°_’¥ÖÌüÛµÓ¹¾¸¢šX`ğ%"†ô‚ì“½‘#Îÿ³ˆÜÍaço;”÷î¹xïPhàÂï«Ó}åN:­§‹
D‘¡	'ésˆFè CŞ¾{^½×õHÜ_ÄŠ³ŸµfÉk†¬'»!]e¾-®$@Ò¨×/÷ZÂÿéQi±qNÚyzÊ{¤ø=Ëœ…Î×nßÅ6¤UŠZ4ú‚¹âÊË‹~ü²áú,ûoºÀ‘Ö¹IÓ˜uı~iíÇBm”Y_ÖÉ–ı|"½-ƒç‘Yt¼[M—°õ`±xB©™ˆ?å™–*ÒpBâÕ“Æ%n)±- BÉmµÙÖöŞMŠ¬€‰c‡w#û¯ Z`aÖ:Ó²™¦S $>:åÍT09Aö0X«¡ıŸûî|¶iwß•æ(n´G°´<Õ’_%ˆY¿ëñZÿ-ñq`ûÊ&ãÂnñ~’)7?¨=j´Ìkº}ï=5ÇüV}/z_tşltYËı]xö,[eNB-”³­ur]8Y!#Eª“ˆı`òoç€{9ÒºS|Wªeåõ–öÀViµ A»ó3û‰›GlS>•I2$fuÂS1TõDÚñ+.¯İØ…ğ-¯#‚Õ7†® ì~µ{'—Ûü¸‡ğK·Cx#Än}î$ÄtÎ§ˆZé^’ŞZ”£u•wà‹Ü‹„å(
vÕ²‹&pb:ÈY*÷Ä?hÛüuIe¸ÔÖ ±%%ËŞ\X2ó–óÙ– hÏí2F§İĞ­N’ŠjTËq3¶³{ŞIÂ,×B=ÒÁó;Œ»Kc¯zÖß´é²øWMŠ6Š•Œ`â0—cÜŞXº]ÃyB/‚‘¿òÍĞZ•“êv†ö1GiúwQf1tWù‡ôò*‚2ä­€Ö"İŠ¥)C÷ƒşíÉ±mWà^ã•2QTSIĞWô¡
}‡Qpº1Ù#²¤¦_¨Pâ™(Ü•Š?*7"öÇŠ5™”1dJ}·¨É$ÿ€N™H9Z¨5Ğ\
Üv>^@KÀe6¿‘9·oÆùKğ‘Â2í´5•À>Ö”|D)Ö"7h´A[H8ş„}zÃ½z‡\æ¼ª¨ÎjçÅ¾æ‰Ií7„T²öt›'÷ }õìÁ˜=Ş&åÅ­I=w¥BÎ†!µ&5.2Sİ`½ÚèÁi&×)2—º9N±ríjV'	Ä¤¤À¬´äÆW™É8jS<¯’‘½ŠŞŸÉvµ~éÔ›Vm&òËĞ–Æ¼lƒ:ú£Y—·ÔlzÁ
È¸ªöz2 æ§/úüÎKŞøû¸	Â×Ì¨Ofˆ~Iw¾÷Ô–$¡(H0~FÿÎ´ÔãA<Y†	sDÏ:œ×e-)Ó¸ÀÃ8ZXPÅËõsÙÇÀÊ
Ãèo2&Çre©œtÜüeŞsQB "³æôq~	iIECïìÆmÇmıLñú}#+N“Ô¸%)fP»xI¦ÙOsb­÷µàNOš[Ü¬Í§8„è˜¹R+o¿˜‹AVcâIîÖà6=TÓ{ÒÄ(.$œ[8û|¥Ù¤êç.ËOÓ}Ô¡î_^€_50VÅ-´´Zí|(¸b— «H¬z¡ıi<üj.a`bºŒ‘oŸ-$+‹ÿ]NPÁ¶êc¡Š|½ıó&'šÂ)á°Şl¨Ê@ÌŠX+&½Ì¨•_;0zt°ª{&‘í`CäÏD níîıöK…'ğv>¦¨$/0g6”ƒF¡è},aBpSG–µ›¢“=:
ª6õZVâêbk"~pùĞ}t½rÛÒ§vîsâ:ŞŞî*¸ß0*[†İª¹J
 G‡¯'¼‹Nà¬$‚ ÆGcBãKB/2T’9ÿiı××‘³ÔmÄÜ™rq±Dâ†œ$«S:.îÜÕ¸YÏFú[Å«W[}¦Ü%6»P %ìX}´3+œö”w¬o5«‘¸ToÆ³ÁÒŠ¹D%°ãÚÈZß}oWJ%Ù2ªú÷¾«Éî#öqM¤3KŞĞ7›`Í¯%{ŠŞ"F6ŞÛWÂl}aØ)8ô¨½£ùÄº8ŠÊşk¿x3OL³	Õ¹?!İlZÅgâØu™{Ã×¿4-sï9¶ Ù¯ƒÀ,Óx^â¯ın¯È…½†Lğş?6íw40ñêâ|]‘ûºp¶æÊ­D…è>L3ÚƒÒÁ3M¿F[2#ğ†OöSœï«åonT1`à¦ÿb-ÉqnRàB3.(ÄĞ˜6 Ä„xu<Óí-£@õ¬T„t|ï¥7x‰ZÜ<¸.ãêİìeÊ•A-3íş2ËŒ–]şù?qlZ¡ÙACLOlˆù