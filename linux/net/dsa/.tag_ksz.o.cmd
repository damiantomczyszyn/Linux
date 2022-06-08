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
       ÖVÈ~èf=4kì›“®,ôœ ìC;lUNKÜNÆq:BqÞÑ´Ñ«ï
õ?j{ÔÊ?»¿À9ÆÒmŒDâ¨6Nœwº™hôž	p©÷¶XÚ§ Ë8îd+MÒšÙ!-µÊLªU!2981øéëèE`§JŽÏ+²Óê_^Òîè’ÀR_[
qÖÕÚö²E„+¸‘»ÔpÞŒ¦oÄÿ>h„ÎþÔ>õ}ð)ÌÞ/p”À‰ø	ˆ›Lá|î4ôõs¦Cø±Y¹åå4X¶‰e:FºMï<ý¹¥šÆ]P´Èò8&@f³Bp!gDÃ.ñ"ì¶t½0™z"y²ñíI¡rÒN“íO ‰Åz“—|›y‰sâP%ùœÝÂK¸öÙ…tnÙlÞ•`àâ#KN"ID†RÔ³0º#O ”á-£aùW‡âƒ	tº¶õàc¦âäö Ùª$!oÒMÝ.\ö£¤˜t©‘¢  a9ÌÚ.ÝXÄgiš2i¼ýùãëÖÈÉwÀÑÇ4‰÷gÆ&wÝ¶[QÂP†O sm7T©oâŠq=Y'•¦\áÒ8¸Ämëòbw",å>R¢f›òœA
#ŠÇÛíH/»¬UHf/Nuñg „8†6±öœ¦u'}‘+hcý¾„M¥Ôfká;•GqÀyÁ³G22)+²FøWHr[ƒ/.S)`à±³#tQ	LºÁ[>ä£×@/µ#AH7F>TyÙ—"d‹£”Þ!%ì¾<j=J.üW˜áþS]¬7¿·ðÎhìÍÄpb)=Øˆö’45…æAÕÁÑ_Ç?¥VvA¡Ñ$'îsrP^˜-&¤z˜®É:@ÖqºÛu¤Jx¦@Â]³ÿk3NùÄIp”'Æ9È†ºº•¸ÛT 7ÿ:¢FŽçŸîÂ‘JÁAí\pC¨‘Å¨Ã
™ps'C^~Ã‚ÃÉöÌµM‡2B`ãœ~{qK#üT •¤àgÔ‡}b1ÔÉHnÐŸ;2ŽýJmçîm‚|&Y¬-:ñ…f®_ÒîÌ‰xO—÷Í¿<kv—w2½
ŠÍFkŒÿñ¸Ž“ûžRZFd˜Œ8zÎˆþ‘ïY>ÚÜ læÔ°·Þ\ˆÛ39‡î‘ÄLþ¹ÄÊÀÛ¾mHFÁ•ßÿÛ-µë ®Z;šXXÕ¿P$~K^š7äÅÈœúüð×ôX
ïÊ“ÒŒÛºqïŸ{½çðc±oÞO,cùo‚pñ ·Cè§J7²ìP¾ÿ_a†Û¡Ü£òFÏˆ.Çôn5<a‹Þ›9,¾0‘´kÔ £&-nofÿ\½Û¨s é§õ|â<Ÿ@nÏ¦WÕ/Ÿ0:ž¬cçxÎ;VžP¿í­lc¸0Ù+#Ûæëë’§r;ÐÀG2äH»#IÙ#¾Üd¡’ZöSÇéÑÉÕ«fáò²û£‚I¢×õ1ŒLIû?‡z	Ñ)þ˜PiiÂ:Wª¹yÙæ;–Ì<½b³“eç“ÃêÑ¡	¡ïRÁ­xß¾JJË]8Ö
j<I}m‘«SQ½Ãþ®|}Àˆ¯‘ŒQžãXÓ\uYëß@‹q0}‘ätF^•Æ¬lJóXß±·nzµeÜx†x~M>»êBàØ¶dÓ¿QœÆÉ^¢RE¢E¢^TæšÒ³ôˆÉm@R¨à…6yÂð	ª §‰©g„/WÞí‚ÈëÚôHwPÿsøˆ
–ü Šú°µj‹ä< ÀhÓ±Ò+žéš#e÷ö«‡sÆ’bƒVñ|Øžq¯¶$XbÈ\ó©n½*2zr.»â«8‡@€o -ªA=dÕ‚Aî¥i5ˆpRdÜn¹À•#<c¸¿Ïçš"PZÚ7ª±0]Uø;¼áç£'5“yœto,¥JÁ.z‘ d Ù!¦‰a 16èŒ[ÈË±n–5ÈH­C%¬(ß­§„ÄXzûwÂfwçŽ^ÄD29•¹¡À¬î¢š30îU7õZ(q‹u
Ý¡?çõó™A»¬
cígYe§(´®ú½—|HJ3·äõåðõs?…Þ¡en6émâ‹uä<ßk>ÞR0Kµ[ÒØ^ÝöGÊ/é)‡’y:6WãƒÆ›»eØtþO)€çÛ¾½¯pÏ«ø·ªEýôh'$uß½@Y	ÌGŸÛ±½0ð•ž3ŸÔ pÕYX\¥—ð±¤µ;ÈåBwîé£)ì·æÙ—„½Ã6+3¦iîwÙ8¢jTý„…Ð™DƒÕÏÐ‡'kÕ[§xï§¼~ KžóNÑÝ?›öÕ’ë„‹Ì­izø¬sþb¶Œ>!xþ ÃóŸÕ\ß©tKÊ¾^…‰8.åõLÑÜáG$ÐHÚ³ÜÐVþ’ïV3	ë Á6öDõž#-FO0 ‡%á§»¾–5.Ð’¡
Nhn©2`û†ïÙ,qàÉ¶*”üäß‚GÊ8rª°¾¸Ý.`£'I²O?ÖSeZØ;SˆŸEÛ
(ªÁÜ–É‰ûö×aíÅ†7‡ÝØžòCºÐ±^`¬8›“ögóf¬‹jý©‘×õàùÅËˆx0"1»<°N€L~™uÕ_ŽèóLoq—ÎÌ¦ë(Þ=>eí—¿”›ê¯b€Ó%ùf ËaàJ|V¼9”KÁš|b”óÓ·ÇkiYÅÿq¾ÿP–4ùSšóëhN­¹î±áÌ_è²ßÄÄdƒþò•W_eÄÅ+fddëæ¦®Ãg!¤ø¤ÒØl8'M¿€-º£CRÀÀJÏªžX=Jº•½¾4¡pÈö }Àgþì:´|Ùh’¤ÍvÒ›Y	„cy –èªÅuªÚò“@»ÕvžŸq"j¤W¤
5´"4àÀ~y0R ø7',IËx$QžoE¢ååI¡G¿m—ÆL "œ7J) õÙ	w±%=NT•0–^gO0çÓÍs,-˜ê¨ríy’ºæË9³ø!	{±ÄÚ;Ô5„æP_Fÿ0­?eŽÊÌÚ%Å®— ì¢lL}%b¥üõÒçK´"»øäjù>P»ªV+=ŽM~4nò†Ê·‡ }†’ÈS$r ŒøšÑ±Œi"ê+sá£[µˆBgðEŠÐAi=ŽtÔ<É«Ô‚iº_ö7žoÓEM—ÄE—‘XÒ[ßˆná¡À_w5åVßåÇ±pT×îF¶õ©ŠÎ[£lRWÉ8²–ÎÊ	/ôO'ˆD@ÈH”p×o~ëòxª1÷×ºýP©»â‚w_ˆ›*ØÂ„?Å¢‡ÐøÔ¶mVÁ/«kI”MÌÄ3vzmÖe^-Ü–:Å•žrá¼€Ï4®ŽßwÌo6þœ•‚!K ”HÛ’Ð26Ÿ½:Ga_ýø¥Ä†ú2Œø|égSÓy›á·:—ÌÙ¿ÎbÀX÷{(­²vµ:pv3‡?ÂÛUŸKï)C´L£áÇmÆmtAîu&› äoÚ †ÿVdšó1Iå x5¬EPìu¼@¤¤‘Â-'Ýt_©X¡žt.úc½N
™‹ò~kr;<¾cËð_ô7Éi:ºƒ+vÚÀ[ˆº"h¾E¾ô1‘ÇiÔ¿`¿9œ\v…Pp¸zÓí,NŒWKû¬ë´[òˆBÕE8¿GíeË¸„œ
‘z6 õÙÿgŠÈk,}f1ðWU
yç«Nn¼7|¢lþ‹‚™ñØ<¼'&Ó…qx&1º”„~¬(¶ks¨EÓS8Ûä61Èû8>€qCJEÙq™)gÙ ãÊÛG÷ÆyH$Ž ×—iBRx“»ßÄNåáÈx+å„|¡^†*í”š«Äþ°'Š³ý×[Ðøz-Ð"Ãe+ŠÂÏù/óÁ­›à‚ð­s½óEª oÒ#Ž"nÇÞÚÊß$—»íâ Þv¸: KÇWël€ù|&Ýn5í˜¬N–ÈD‰=R1Õ÷É+	ÕÕÇ*+¦¬[jú/&+ƒ”€bÏ±öù†
„Ûßˆ+4þÝùó•'!¤MÒ'ª(?k	%¤Ld@hÒõ²g›. ñÌ+ðªèi¿ ÈN£dx1o/RíÈ„ÿ¾\h•²ÞBÓ§æÞÞ¢›ô‘y‘@ºË­ö·Ë€pt«9ï§à¾ò¯öKHuÜ£&´{¶çcD’©	 ø9´¾`~€…­Ö³x,géngÛ¹Ú·,‚××÷Ž /­?û…BÁê*KëóüC/Fq7Ü [n]48bÓñµ/ÔO%y½áÈòbúè®ù¦”e$¥{þ°þ38zDWÊ:Þ
Lf¼í/öjÃüÀZàn½óyü×J%}y
Æ³dù‰Vê;´fåîÛNŠÒ.Ð«ØXöÔÄÝ"¦Òƒ¯0×Vó”üNàÌ“û?ŒË,óÞ•î#ö;ŽW	"þ…dR3I…ÏˆT‹âg_!Có¨×)¤(*,PhÏ+Ú)aþ$FƒÌàõWmÃxN‹ÏÊ¥×ÜQønñÂË|:£©‚h`/H‡ÈÈ”(ŸŸ”Ý±ý´[®“2ÒbÑuRç±Ýzƒxk92¬[7,áÐŸC§7vð›á#§H²ˆš¸•ËzLñŽœz€ýxo<eÑGjx£%Ô¢!ï¨Ì'YpÔ“dìX§Ñ7.ÊI™&ä®ŸÐ»çc(.t!Ü@¦ê\TI\õ›#EWÙSƒCéÞÊˆ˜•NÐœšæ¦cnK¥¹W<,f€VÎW¿¿	ä‹y€BEÚåÔ|-m‰`w7;ýš
$EJÿˆbbëø…}§¸Xpt=˜ó<ýÌÆ K@Å¦OK¸ÚÎˆÂŽû"*’4}k’Xýºp4UÚlØÆF uØÏªâG«ÈÝÃÄ`Ã©‹TÀ±ÎëºLùLMÄ,6R‘p_a}ò²´ûû$Éû/a}yÜðóq å ƒb½	Õk1ˆ öØG^æ½&¤s‰¬é/Fâ'>ÜóÃð\¦æ‹â¡ÍùS»lo÷A>O)Q‚Vî„©ƒíG §ákaÕsÑLM}µ(ÃÇbOD!7TÏUZub€•¨i)]i´0LžŽ“lÊ‡5…^§G’°÷*÷ç¦Àª9êaŠM£È~âÑ-"¿Ýš”ã´3ÿôBÆ-­ý’•¶¼ž=Ïmqü¢ü‚q¦ÏËhÔò–¨Ë¼@…¦%’QVž4x!éš_o
g8œ“ sB‹Ö»á&+7òôI	$ìíÕ‹z¸‡ÐZóðW:&O¾˜ßüIýdUH«yÔ¢éq)Øe®1>ÏµUÞ´Â!&t	×/íö< ºîoß0 Kã]w"Éî½‰e°•Y°OdWÆwµ%7œÄnzoÏê.‡)’
Ï—…IE¡‚Ÿ~à¾=)¬Ú+ÃÛ[tÁ¡nØÕbðÀFŸØ†%qôw¤w­Èo¹˜Bp¨Ù{sÿ3ˆˆtW¨öw¶Ã¿´éÑ„³ÍÞòc¹'Gç.	£8’PT*ÊPƒÏ!xXßFD“dŒÝ%^yëÒ†4$ÿµK›ŸÅbÙ5ˆº¶°¥tá¨è½}{î3œ4ùaZ'êY—‹ð Ý‚¦¤¡¸š4Ÿ?@;L>OÏ7€˜ÞæDßC™\‘1WLgÒÊÍŸ÷|D‘´~6fUðïÃ —Ú °³Õ#µ:”ÇŠ·š¦Ûæ0v3Û¬&üàÍnÈ×Z‡5Y…hÛ4¿:D†¬\Á:Ö‰Û²ÄšC}{O>ÒíešÚ÷jÿDéãxÃ•5Bó¨£lì#âó£þÛZ£h#y¹ør5††[àÈyXÞ|%Õn²P&hñÅêß:yV+×üc­¾Û¶ùŽì$–µRþ±h¢ÁÍrÒPÜýE©ÏLwKÁÌ'CRÈ/é™4r­DÚC…d½Rsl¯wË~êžñ:ß2HÿsxóóË§>‡6,qdyZVÐŽB÷ô\’ƒ‹Ó#QÕr}éÚ»!™#±*ILQr´ýàlXü:{¬géŒ/øW½˜‘Ø’äýwL%ÞM@CO±feÇË£p‡Em–½º_ºM¨&	r£5ðœ­ÑIYÄ 8 M,Å7îþ¯G
©êÇ{S.O<3žºÂ°w8¬5êHsÄ_H¥,ÌóØC@*]\Gˆž¶kláhí©=p¿O{ÛÍÌ„Â]¸6ø.Ôo{|ÌÛÝ Òð±V0•›c´ÞåýñˆS™ˆHÕnQÅq±¸k¢liÚÞf®I0ï
¼­øÉ÷ýnÔ6KEmÓÿèÎâÀvò¤¡$— ÂöKmz<0êØp®ä;ì¥ˆx,”éÆ"‹IÖÁxªqlJÖ–8ƒqÈ“ð-•Cí$CÆ“‰Ø—ÐlNhàU0v®?D©|WXõË)¼ì$M
ÎPte?g’Lû–0ËB}*ï˜p6ÄçrÙ
¤«Ô8hÄöVú™àÑ5Rèüh€‹”èå$,®Aß7 Œ×èáÜÒINTVïåŽWóŠnÓŽþ¶~èé™^×aÊõ^’¨Žù’WœºÁ¬ gê4Næ2¸eÈ°û£Àß5ª"D–&UUw“ŠñŽ;€ü~òˆ\%-š@§7çÚðå6 >OüõQ¶#XkØ¦è¦vewÎ°ü¤ØÈ€~aFxŠÇQ³Z›ëˆØšæžÕÒ¶¹6,W§Ã˜zdb'YŠÕAûñ‘´›Ä†<(²ÙÈL—iÂ*ò06ÔÊ0á™K«¸Ì”ð "!ŽÖ–l«XËÈÑ•˜×°¨”ÿúÇÕ-è¬ýY8¯Ç¢c¬Ž&üŠfG«pÝX·£;-Ñ‰U“WdÎ²ðçp!…¤%jU÷ô9
rðð³àÕƒçS§rú:÷1q\Mñ÷Ùv –‡¶ü:¶0wó¬õŸ%øVaQßåíŒ?WÔ%W8+‘m@Ô¯újÁ0O¡ãROçÑ¢¨Ù`†
'V{ÈqqY±_"ÕÈ¿ùªA¯E­º¥¡9ä%×LŸÜ8en0Ëñÿ†»Æ*³WfÈ26ÊT+„aÜ0)Ãj¬Ú‰×Ð›¼z[æIlø%“ð`"”X\Éfo[¼:‚àõåãí´-])·’iT‰ŒØú;íÀðBˆ‹(¥/vÝÈsÀûn1¯*º»mÉšæÑ"û…Ï¬«˜añ¯?¡› ^æëa*¶ývr‹SŽÎÏ”Vì®¹cš]¢¥#H~™>2ºißpñPkg,2Ja¢£Èy«í5¾¹
äE$pd>GáNÞ›W×å[w—êÎ¼‰;4ãj|þÐk‘­Øøþ¸jø•M,v…à•Â°|²wRÛîGH‘a–IÀlåc¦lÛ7±)½÷þÓ–e}ûìúÙƒtU¿í‹¤p_X1½v_/lM¶…éj ¯ÿiÒm™uHì—o-‚Ð±Ý­+r€/óMfªõBQÀŠ:
å8ARøþPAV@6´Q…6ÝJ¸|³H	k$[ç1–çMk0¦ÑÍ(¢åø½ï‚P±Uá—lÿD8jæez¿·t] \TòÈ;Áõj˜ {vÃÄG8;`š³SDýˆg'¨Ì;OÙŽ3ünâ'©³òçåW+tnß¢{[=W5ª¡¶áð…±ÎTàu¦¾¨à±eg§ôzÁQÿ@Mî1?)	¢½F`¼é˜'g|ÀûmtÌ~&Ðæ-B®p´ñ%QEØƒ–&ÔE¹:ÙÑ¤¢ï
¯ÊjfÆz*›8¶I¹g¸à™‹†Ë±«u2º’¥·6AbÀHh¿Hð“çH|»c|{œ‡4„·þ2!`ýä­‰ÍùNIêÊIè¶¡`º0aJýÒTTš1ÀÙýo¨ÇwÁ«•¸}èÛpÔI–ùgf@˜€^zŽ6Ö´u4cì”åŒ‘ô,û½6ÿ»´ÚøÃ#¤x<nTî(úáî5Ûº›ª[pÖÈŽ
ôðÏCcÇ†èeÔ7ªiñZúPjÝo‡ aˆ%q,5¸ÀœÃ1Õ«T&<gÎ,æ÷Ž‰`¿<q+6Ö=‘ˆT»÷Ôßñq*<D·ë³Šù-0Îè6s g~&S2ÆûJ%——ÜG‹Êa/!»|†ï”€Ï5ö]Dú‰;­=–­ßãw$àdšì•bK&> 'õV—­ºhÏ<ƒ³(xËMb¡‡wþDsUxþå`¨Ì¬0Ò€UŒ†öñä£à?hÊÙa†¼aÿ“°ÌÄCM·G¼©¡­a.úÒ»¾è•Oò¹áËá,Ð•]GcÌ2ÞŒšŸ· Ðà2GsZ®7È„²d¾D|ñ¦‚yà7î¥àc¾ºkkîÿf3`ãr3¾®K¡„TmJë’ßúêØò<êoª±L	æwÇþ`ÑŸÂÿkÞ7%^XYýÊÝ\×´¬¤‰^v{”Ö†ùéCªÄñèáUÝ“o¾ìëõëPôÞ"LH ªž)D…NsIFÄçeÁÒˆ”q×Þ/½ÇÃøî9)1e¦©²Èô5Œrø’þ_š}i{¢W4ŸÈ²ý×]_Òjd«3šXòUITÓ—ür$Ž—¼2­šþO§ç®ž½uïÃ™â&°¨B]9»/ðBÝÊ!Ì”– bw<J±zÕöp	«Ü<ï@Ñ]¦¡p;ŒgœzGÊ…â@Ó©$Ç…—¯þ5q £2›?fÓØVþ2ÔÔb¶ìË”Ú×º¬Cu³¾6„iÁ‹O°®çQí3›Åœè©‹?¦˜E°wƒ/Ý¨ë×xª%Up!3-®&wwR‚Úat9˜?Ì¼vâ4««HiÍF¬<¡Óè~o¢ $qÁ¡Ñª(®$pÉf…È&¾úãxxà,±H`o_ÍîÙ@ôø	.•Õ¾“_@í¬hVµþÀ'Þ!l"·ùèÕäÈ0ð@¼Ãy)6)ºŠ‹ájY¡üµS5––Á4–°ä:%Z¤T0é·˜?œÔVÝ¡?304,ÙH5E¿ø:ù
OõïŒAFN“Väð^u*«¸YÆU ÙQò¼å#qm¤'ÜŠöß¯vÀïpGÜÖdm¿–‘«TŽmMi? CÏ€¸ûªg¥š,
£zÓ.“$.Ö„Fš¸¯)ÞÝá.°Y2²Ñ&ÿ²–åqžÊBLajµää4‚oÜSZ¥ñüý½b#ÎÝ¼Dž‹}ÑKw‘®ïÀ”¼Æt’\RøAŸÔŠ&¾ëŽç!X3îuT€VýCoì¤!Ö¥”Ù:vWº^gÉªÒê”ö þ³¶˜ÆBf"ÃPy«ãdY¥÷j £ÎÎ“âqS9óxŸbN!€½ãkË¯EŽ²Êùª‚æÅ|¡îrêµñ(\¹åÔÙö9Ñ¨aÊëZiJÝ5*1Ñ8ŠFœùÔ6òá7	ÁÙÔbòyaKÄª)ëŒóâºQFt‰E1óþË2+Wf—Ì%ãÏÀÕVù¤°Š¸…æã•^
¯:SÌcáCLµ|,Îô#°É ®.Ñ"£{ü ÞÛú‹xÇóÞf‡ä”ÿvF›ƒD‚Ü±uð‚Êôo"-mùè¹Þ’ºíD"èìÂëükûÜP wjŠîè³]Ó{ZÈäµYó÷ÓiN÷z€æ"çÿk½ø°ü¤;˜8™l¤+ÛFdŒ\«1-P¼/Û„Cëg:*µ-è¶«êÝ÷`÷pcÕE-ÖÕ;Ð¥ð•"º1â"FsMýi®Y0×7µuŸÏZ$êzZ ô_Ÿí/?ÁÞOü"uxÈó\oßUÝwã}æeü1vˆb¬Ýï'œ„üäXWM/ÆÕP§étÙ!Ù}SˆÌ-‚w‰$c‰ë&Ç|æF1½éw’}uQ¦AŸW×û¸Ã®UW(³=ÝLQ’Tï„äÞÏ…ëôþ-aój¿—”ÕwT<w€WAQ=ãV/Ç‹ˆJÔÏÐV¨Qk1v÷_¡´•Ç«!Ý»øã½sÅ§Ñ|C“Î³{"½N´ô*[œ)qü&ÓÑ+„¯!jsu:%N,ºHö¢¤½ûÜIÃîãƒ)Rcð÷Û·¼pÐÚ«÷~ÕèÎ¿¾ÿ “v©^¦"þSÊ[ÖªÕ	Y½”†`+WJºÚŸ9¡âHZÖ_0õø­¦Ê	}ˆÛ8{½GoÊûè(¼|oZ¯"Tx4xõR†ëb–ÿ@AöDHoêýŽ	æ©KR¶ÍàŠõ‘’‡ÅK‰s“Ê¥[5Ÿ•G™hgÆÈzœ÷DHüÎaM»©™Îš ñ}$;î&†_öy¡ÝGËEÌCyÌ6Ä¬e)N}ŽwÉûÕýÇa»Tîö~5kºpï&ù$ùÍÉ¤BºøAO´pé‘çkYY‹”™§Óó\ûëÜÀûö|4BNþäX¤»œÂˆI–ÙÙã§Ã÷¶]3Wn8æ ŠýY±-zùá{*uìçiçðÑºãÿq&Ù
Ñ´7 É‰—8÷&Åíä+¹êžì«–ÖÆ3$Úœg€:ïÓoÆ€©,‹®?ÅdG×•é¹¬kœß£CïI"ò?LÇ×ÔÂŒê·¥ ¬146Öá02ÖqXö¶Êx6kÞ„ÿlDgz·œnÚ
qxV»é ¼s\C¸kK 5w£ð'¤ G`t:/PÊ×Z‡úò \+YCFiœ¤ú=Åýï=÷ÿÉ v¥@:X²7ÍqøHã’H¼À~¯F(‹a’µUöZŒuyíx< c*/%©BË¹`_§ùEA$%ôqžG}sò—”ïu9ÿÆõï¾Z.fäÇ‹,‚ôpYÇX.
=C× ´Æ€¼†ÒDõN9c6okÅ¢—gÑ`Ó¦N©5µ\X]Rªò¿×ã^={‚³Û3¿ò‚êÚ=Äîi¹à;X³•æÁ"ãûá±Ân¨Çn|fŸÔ¼ûd·8»°áê*¼ž¨P­ÛPUV{­ó’åÔ?ÿÑ_u Ó0~ZhyÝ@ŸŒ<7sþRa.Ô%êûñžGjŠEØ»ƒˆ?H‘@ª¥²\‡DúvÀ‡>AÅ
ãÛSs2îB•M|'4Ëb,UºZÎOî[!±j8÷³lð‹÷”Ì'µ}›ñ¦;]{@/Š[Ðp‰¥X‚!µs½ÿ=Õ`èÈè“ø›¤5z+ H{+i.ï;Þ’à´D¨œ9©ÝN€ÄýÒäÊ\¤ê³t¦¢ÍÈ¥S{ôGMøŠ#øöíŸo´ñÁ!D‡8Ÿ6ªWZN“3í¯ñÜÛ¶¾Ø0¦N=O3íò_ý4N»b ¡oìYƒ±%€C­älƒj1Ï~)ˆ²Œz„L(<}…2ZäsÔJqƒ“wÙZŠeÛy6ÓFq­Ð„W%H}ù›1oœ,w§…í‰ŸÜæ÷@¸Ý"€|ýZ¬éŠŸ;lZVy66ô‰C•Iÿk™8Ñé¢qj;g_Ô~”¿(£¾
ÚçBkT©—Q	_º‚®åÍŸêz6ò÷%è·k3 ›ÁåÏ96³MqÃ´ ]5ç<Ó\YäõðeUe%ûSêðB`	UÐ$:Ú·"dÅGÿM1ÉïÖÞM’BË»sæÚ‡R‹¾•àÏÜL9þ§6š ”à|ïÙè‘G.ŒTË<gn+—°"zúc77¸uõ·'}Lû÷Á	fM6ÅšêTx”©›ø¢c¾8oLÿÿ9Ü“éB	öÇIEÙY-ˆ]ðŠv‹ÑÉ8R ÿ	!r)é?¬t¡¾4àþ’%¾sMŒrÓ™³GÞiC)8t¬xföå@Y7àŠà”\„}Jž¼!»‘õá}°Y™ œjj<º Y5b€Päæëh²¦¤ã_wÔœ\¶=*òM‘F½»O¡'É>üÎÓGÅ©nS+lZm7ÿs¨8£ÖóDCÎ”‹ÄeñÌ\·s¹€3ììA£µ«ÉÙCxü,â·#ZZ¤Ûsœr„µ}ð	*§HZ–‰Ã/cÆ¦ä‘>t¿Åë¸=›&WÐÝ¥r/ß96~/IËÀû	€vl)¨ûzÿ¢lRJÄ/Òkk‰xE¢d¥ú9ebD	[°f6Ô_e÷„ÝEŒt†C@£NÑ²¤ú>—¡tñ!õÐ¦‚÷£ÂåD´Úiß¼ôö‚ÙTª¤º/²›…©öÑÈüŒrÃ¿VH¥~8+t6Šõ[ç¯œ£í|Žï[ah R°_’¥ÖÌüÛµÓ¹¾¸¢šX`ð%"†ô‚ì“½‘#Îÿ³ˆÜÍaço;”÷î¹xïPhàÂï«Ó}åN:ž­§‹
D‘¡	'ésˆFè CÞ¾{^½×õHÜ_ÄŠ³ŸµfÉk†¬'»!]e¾-®$@Ò¨×/÷ZÂÿéQi±qNÚyzÊ{¤ø=Ëœ…Î×nßÅ6¤UŠZ4ú‚¹âÊË‹~ü²áú,ûoºÀ‘Ö¹IÓ˜uý~iíÇBm”Y_ÖÉ–ý|"½-ƒç‘Yt¼[M—°õ`±xB©™ˆ?å™–Ž*ÒpBâÕ“Æ%n)±- BÉmµÙÖöÞMŠ¬€‰c‡w#û¯ Z`aÖ:Ó²™¦S $>:åÍT09Aö0X«¡ýŸûî|¶iwß•æ(n´G°´<Õ’_%ˆY¿ëñZÿ-ñq`ûÊ&ãÂnñ~’)7?¨=j´Ìkº}ï=ž5ÇüV}/z_tþltYËý]xö,[eNB-”³­ur]8Y!#Eª“ˆý`òoç€{9ÒºS|Wªeåõ–öÀViµ A»ó3û‰›GlS>•I2$fuÂS1TõDÚñ+.¯ÝØ…ð-¯#‚Õ7†® ì~µ{'—Ûü¸‡ðK·Cx#Än}î$ÄtÎ§ˆZé^’ÞZ”£u•wà‹Ü‹„å(
vÕ²‹&pb:ÈY*÷Ä?hÛüuIe¸ÔÖ ±%%ËÞ\X2ó–žóÙ– hÏí2F§ÝÐ­N’ŠjTËq3¶³{ÞIÂ,×B=ÒÁó;Œ»Kc¯zÖß´é²øWMŠ6Š•Œ`â0—ŽcÜÞXº]ÃyB/‚‘¿òÍÐZ•“êv†ö1GiúwQf1tWù‡ôò*‚2ä­€Ö"ÝŠ¥)C÷ƒþíÉ±mWà^ã•2QTSIÐWô¡
}‡Qpº1Ù#²¤¦_¨Pâ™(Ü•Š?*7"öÇŠ5™”1dJ}·¨É$ÿ€N™H9Z¨5Ð\
Üv>^@KÀe6¿‘9·oÆùKð‘Â2í´5•À>Ö”|D)Ö"7h´A[H8þ„}zÃ½z‡\æ¼ª¨ÎjçÅ¾æ‰Ií7„T²öt›'÷ }õìÁ˜=Þ&åÅ­I=w¥BÎ†!µ&5.2SÝ`½ÚèÁi&×)2—º9N±ríjV'	Ä¤¤ŽÀ¬´äÆW™É8jS<¯’‘½ŠÞŸÉvµ~éÔ›Vm&òËÐ–Æ¼lƒ:ú£Y—·ÔlzÁ
È¸ªöz2 æ§/úüÎKÞøû¸	Â×Ì¨Ofˆ~Iw¾÷Ô–$¡(H0~FÿÎ´ÔãA<Y†	sDÏ:œ×e-)Ó¸ÀÃ8ZXPÅËõsÙÇÀÊ
Ãèo2&Çre©œtÜüeÞsQB "³æôq~	iIECïìÆmÇmýLñú}#+N“Ô¸%)fP»xI¦ÙOsb­÷µàNOš[Ü¬Í§8„è˜¹R+o¿˜‹AVcâIîÖà6=TÓ{ÒÄ(.$œ[8û|¥žÙ¤êç.ËOÓ}Ô¡î_^€_50VÅ-´´Zí|(¸b— «H¬z¡ýi<üj.a`bºŒŽ‘oŸ-$+‹ÿ]NPÁ¶êc¡Š|½ýó&'šÂ)á°Þl¨Ê@ÌŠX+&½žÌ¨•_;0zt°ª{&‘í`CäÏD níîýöK…'ðv>¦¨$/0g6”ƒF¡è},aBpSG–µ›¢“=:
ª6õZVâêbk"~pùÐ}t½rÛÒ§vîsâ:ÞÞî*¸ß0*[†Ýª¹J
 G‡¯'¼‹Nà¬$‚ ÆGcBãKB/2T’9ÿiý××‘³ÔmÄÜ™rq±Dâ†œ$«S:.îÜÕ¸YÏFú[Å«W[}¦Ü%6»P %ìX}´3+œö”w¬o5«‘¸ToÆ³ÁÒŠ¹D%°ãÚÈZß}oWJ%Ù2ªú÷¾ž«Éî#öqM¤3KÞÐ7›`Í¯%{ŠÞ"F6ÞÛWÂl}aØ)8ô¨½£ùÄº8ŠÊþk¿x3OL³	Õ¹?!ÝlZÅgâØu™{Ã×¿4-sï9¶ Ù¯ƒÀ,Óx^â¯ýn¯È…½†Lðþ?6íw40ñêâ|]‘ûºp¶æÊ­D…è>L3ÚƒÒÁ3M¿F[2#ð†OöSœï«åonT1`à¦ÿb-ÉqnRàB3.(ÄÐ˜6 Ä„xu<Óí-£@õ¬T„t|ï¥7x‰ZÜ<¸.ãêÝìeÊ•A-3íþ2ËŒ–]þù?qlZ¡ÙACLOlˆù ñŒËU?Aiî;Kb¾ý¨ðmàZ­(ºcÛi×Aêò^žhÃY('ÛæZq£&ûË~¨p>Óñ‡Î]FÝ7½Kjzd†G=0n°7«^› 3rûP÷ÊSÉÎÓW46Á°øîBˆ/­E_O¥GA9?o[Ž­|W$Ø^Pá‡Ë.ÿDLºKw×Ê-DrQKSXe¶,fZÅ±cç	š.Ã°t×úïWîV«;9óÈÿ¸Kº‘´‚øÆ6·E…)sÓSÖ46Â_2³–	eè1êU@ñÄ=8kÑi	1zéÌ”æNÎÌäÂøÁ&zLJ ¦ã¡ùm‹p¬F$†Ewô~~'+­	|gÎ>{ì‚øS8«E©êNÆÅú(f(;â2‚¿2Hß
¯Þ¯ÇÞ¢	z÷ˆqöK„”žR‚á}²î›ú†9\Bv!ýOà¯šÆX®6s!½‚¡n¡hš+mé›yóS^4E;ÄŽV_¶þBKÚÃÖa¬òhsÑžÇ[x±Îºªªî +VÚßïŽ‡5V~ÄÅ°)é©á•rŽ¦
 "3·B¡¾ÄH±ÈAŒ’ÄG‰Ã¬íoVô‰;¾ÀÆBS7FSaÚñ°±ü|ÏGþQõNPH[H&vWBŸmò{£#uh15íx%lÇ’]çCÄªá,´e"{B¨¸œÚb# .„ì£D°3ì°GgÈ«¹>2?œÏ#Ž‚„ê½ôXêj!¸âüýqóXú÷_ïÓà†çª®'É0iËwyØDNÞDú¶Íixœ¢.2õ•C[_—£˜ô/YÍ(ˆ•"‰w4Àdá[Ë@dµ»¶¿R/²ùôrÖ4D.— ÷s¦½}sloï–ºwM°ÓG#¯äFLÖy"<tùC·OczÌKñ¸iÈl
k³„ôºófì„Gâ&‚{Ó‡{Ï‚óÀŠ?Š½€ØöWç‚2}Ñ\óç•U~ä¸t õ¬gue°2ò²ýáb]âyœb”¼g¼Ž|Ë4`k£cAtŒêˆÌefŒI][|6òÃëÛå?¡3ŠxéH4}æ,ª®ú.HQ%ç0'Q­¥=½ÿ…dÞÏ5ƒTo÷´Å£
ÆŠè³x¸GK#àW'Ðõ¹d:äp:"[7K|.A¸#»„íŒoÒEÓ›I„§þò&Ô¨Ýw%k¥MÃ¸].•
Ô<X(çÞÎ²v¿L–a¡Å¶[#ÜNý£'ãBzP‡„Í„ÌÈ)~€5J¨Kó§^ýc~¯Žs·ÔõéŠÎ…{ÔA/Ü!mõ!ªPªÑŽÂº(—Ý5Ä‡ñD1*7“¡´_ÂÖ Ãˆ	À‘ÒGl6ow÷]b›qZ˜å‚âiÀ(‰0•>y¶§c«gxõ|Ðo²±IW\¦%*r8¬SXÀ‹Âõ8j'Ilêlû©÷¤TFK±aCòüèaeqP¼ÀÃ•¾u«giòSÓrÁîýyuæ.yá)¹ñAòäO(Äm.[Ílfì‰Ñ gé.ªû>eaƒkÇ9½ðC]ó–»ä`³º6•"|Ø¡´ÚTÐè.Æ±]8Ýt¡žBÔ1ŒÚÝøSJ*]®Ý¿ï á”¾ij.ö˜ƒ¤wÏ*‡ƒ“*L5ƒÚøƒû*Çž[îOõS¬*
ETšfÜßïÂðXc6ƒLCGBÃ¾ÅÃ˜1O_Š½iýfâ§8v5Æ«ñÌ,…AQ}tÛŒÈÏu¦½âè|æ³î*§¬ôoí„YSç®õq1ºNGR’­Ýp”ÖÓŒº±,ÜOcž³ó5iõûp®ã4sÙcn¦Û-+™‡šq¼§pãî|¸+Ð²UœZ9lT…öí%cðPæþ><6t®>Âb»‹j¤®î¸mí/éüà@´eëõ–ˆ[)`À+I­RojïÄ8F«XUãK#yC=G&V©câŸuô-ÈÊPÞäË+Ø´æþ"F02Y*ýO~ÒˆMUžÝÆ8Æ˜Û¿»YžzŒJÚƒ3>Ê×M1Fx¨(ª‰³`x?}ªã=™¢÷1líÅJ)Ç‰û›ˆ¬§„ÒaìÊh“ñj	¬0þfû×E9 }á¾À¿žÓîŒ&d—ïÖ¨JFGxÐÜÇâÇågí¢gúiÚfqmN¦Í²èœÞËòÑ\÷Âñ~¬¸pw×^lÐQÓDp´#¨µIÞ–÷cS3Þ
Éa­6øÏ¬pymçcê«³(Î›„O=mWž¢‰¢tK¡À}¢@‹(Ó ®.8ÁÿÉ°ZÑ_šâOÎ€´“ÍJ¾ “¯MåC[Ç…ÌÂµÐúlŸ‹üß"lï"xï`¤T Œe)Î¸Uë»¨*óŠÜ&	¢küƒ.è
Oþº©ú$yÙ1ýŠHùßæ³Ç§±j]æoÃP¶lð†áqQOÚ¨a¾ö¾¹>ì.††‚g?È³ƒÿËßQ3SŸ)£uð£µ—…Ø
R7ðýã%ÜUCä8ÆEÌÈ7°beO–@
áohú=ÆßZ¸¨"f6¹Uÿò¶œ<;š"ä.T^Q¨ì«óüúÉZ~vXdK×Æ+¿lšGý3KÞÀ kì3l[xôŸBCñFÙÅ‹ŠºùHŽBvºÛ³ó”L*	f•‘àõ'AÃÙ„•ï˜º¢³œL@j¬õì0®òl¾Ü\**@%ËÈ'Æï–D¸UA¸`í0ö„áß‘|‘™é§juMhn˜G;¬$É9§¯óÚëmú_<à1}t¢íq”Ñöï¢Öp#–é,<z%[àv:bó5Î(¦¿þùf(¼²™jNµë;ï¼•&òØµš&Óùý™æA{aŠÄ™ ¨±P¶‰w¬(+‚¬7CqBeBÜì˜Ó^ÿ³yvCO–¢Ð×^øtªšVØ®òò˜uÕ BéíY,
#Í¨iÜïÊøæ]´ø›ž<nÅ,H›D>ßèÂš^¯¨ÍžÁ<[yØþhcløÖ6CuÚ÷ùØçZÚ2?D…qÎÊµpÉª4`
¸ìÔ²¾Ï¾„/—v±¸[Ï¦ë6qˆÊmü·*çOB4>Ña‹?u­4$ò{«µUƒiðZÁ)ókYˆÍ­vP„šÁœ«ïÉ)^;Î–¤jå5M¹a ‚ádÊ›Å¤°p=®øQÅÿ3­b:,#Šñ½ÌòšznV_×}(Óo7‚=ç¶«º7eò#K¼~yÎT”,ÃàË{ºx6 o­²z˜ÙC¨©ÓTP`ð{s×òÙ%µšYºãŽ¨¤Âuò3÷uî¿ÿú:éRÐŒéqNµ+l³Š1ÆÎVxéÈÛì‘}ìaM6µfZQ®¦äfóIÃDû!HÃ_‹Å<8¨µ:c²mÓñß“¾“+È›êyö+s¢RMoÃÖ6A!±ÿ×7¼¨d“ÏÂ¶ƒâad 7˜a]wÞ<âOE;»Xg–ò=fÆF)msNû†T”rª!Ä}ºØzÐô’²4Öø×±~ì2ÃËö"6Ï­³>~¾A›ÿb¨Ñ£áQâˆ2ô¹T½“§éÀ¶õtiÔÖhÝ—!R
Šy“Jä^N7™Täj)ð‚»-}’ˆêf²]b†D€uÔ§ß³• Bp_§¦…’"g1ýhšI†ö%mî˜®W ¢±,v•¹Y‚Fßk/
†q±½Ÿ“`´ §‹üó3»Gä	4µâ/¯W$ÇVÛ(~{…hb“ŒéÓ`’¯.±!±ð–;$Ø6ù$»>§³å’wã.8kóÕŽþe’œà~mRá_?.’qB $TÒ9XÿpN	€†}ÉÑ|ŒK¾Ë‹…ít”H`¢¸³ê°Ii£èŸõWªDBff*_â§Ò+
‘ÐRóm?Å3½óo€Òh¤GKe$²tÝ8è–QY}œø“Ùä==úP*:g%0~)U#}ÑF¹£á~×Ï«òT×šçwSbÓÑvµ*¶ÅSÏG6š5q­µ¿]hê#©«*Î
Ay,)Sýú±_Jïè{“ué[ØÕ¡§èÑb~9åÇÏ4¿ÿq»)Wmy¡µpe,ò2ð†sòìÍ“Sxt»-q’ë…¨5WÚÎ©v£'t“Â$`‹Dâ$¦á[ÐÜk(YXE3ä5÷èî0Ò.R8±³ÚŽVR,àDÄ…F(ÎD”$(	uNN£d'÷œÍâÞw0:;S]ˆŽj¡)ì£så\…û¼Sô¾ 9·Ù8wÿIIMkà;ÇkMµœ?ð[*ÿ=*ÁvŠP`”^T·°%º¡—VÜC,dL_‰–&¯µTC9y_Dè;©Óë³`*¾mkÉÝ8Ü[Cº;ÏO­IÒéð¤Òf¯· ;T¶²c@pîLÑîzL=ï2sÞªïó\}³Z
.ÿ˜¹Ñ¥¾>ì¦õ|æ
cIƒï€ÛðLÜ´‡Zåuw¢:ï!3;g8‹€ð‚R²;ðÏÎÉkPòÐ/ÏZY°mÎÊè{òA}'uK§Ípê pÝW»9SÇz…åýG-ù2A½ –´t½ÛIì[%K›«Ë219Yö»Æ B„ßÇlØ±"Ç7WAäyZ•@Ë‹9qÇ¢Ã‰7ëHßnË<
âA1›¶*”Ë¸¶…6|‹¶#Ný§üŸ’¯žŒ!l!çÕƒÒB5lwO¦€ÎT¶&¢AŒ¼+˜ø”[VðÍVÀñÀCZLÑÿ/“ðËLcè9å³þU%ž‚ð4æxe¦¸Gçi©zL¯âÄi'+­¥âÕôô¿€™í8h+ð ÝiòõÑ…Rœ™ø%@ÜVºpc×\ÛÜ	ÙûäùËþ¨]sª½Ò)  %ÿµ Óžè(i&u#„“Þ.C"„nÕSÚÁ‘5Úuc_80–‡;Ái,FÉ0Y(ëà;‚xV.Xgœ…N¶âßWªOœ:;îz‹÷òøÿùÏ±Ês¨QjºÇ²Ñ6_‡¦è‘÷nk×TTË¤×€ÙtBq%Mh`-š˜gåùM‹ž€+¡B;›±
3&]aëÝ†r?-,š¬ÈS1aP†+ŽÐZªæÖáw!vöÂÈþkÏG±LáºQfÞºM&+jÌ¾Š1,ðÜÛí,7H|kåV¯žû½ôZåëÑÅ@në5…¡­Å”v‰¤€;o©Ž-!Jx¡fÚŸ¯pÕÜtPsÝ»û¹@h»i{zC›úôr'&¾yuvœj½†£ú'þPp4tD õ÷n»thÇ§…ÎG½¦ó‘–!ŒŒ"Î~]gM€ÐS€¿ëf=¸=Úèì…`ô3}q-“[ëæíukLåw.ZzuRž_·p&¼íN…,ü®þt7·|®G}C×HÙ0þê”d¼fÒÀ‡í‚ŒµÎP¹æøße½cÀ©.æµ­¢`…ý«xÝôy^S†P…jªw§D£FZÌI´ [ðæ?VBUne'MhèŽ`B«t’6¿Q)‡Ã­ÿÚUÊ8ªFOågD•ÌÎª\dŠÎ·)q¸:|•	òr53‡)MóëqÕ|ÂÆ«¤•‰4½xÇ8%ƒYÔM;ÿï
žôQÀ¸•Ê ü®†ðÁâŒ—–q Ò|
qQ(9ú–œ½¬Â©ElDcÓEkïËJÉßf Ö&G-É½’Ð„Ø¬¦çqnòÁåÿv[yãx_Áo&ý* Ââ…efÌ`Vlƒ‹Ž<S›ø_“ -Œ[t~Ç#w]†èP7‹¼Ã¬&î—z…fŠFÞð‰³´ât–zãÅgL3¨.Š,ŽéÇã‹z³Þ é'ÛÍ£ƒ<íŒU`Žú`ñI9ê/z‹RPÇ›©µöŠPÚûðKWy•ŸiÆ/igÂo¹W/–{Ï³#´žq$E\‘^šàÊúá«hßzB”¥›Øë@½Àú¢-êXã™ùÓwÐ“º–æÃ ÿßõŸ½ÝI1Av,¸ 55†dÑ ÚYn‰V P¨WA[¿êtbc9|ô l¶²d‡!¢ –0¨î xqc…×«œ»Dt‚»@•CuÊ¥¡º%Àÿ s1”fájr…û:"¢ïQî?¹.+¶3)˜õœ»›!XB]PàÕü<Ûs:É$Q~½`”4^úÅJú±¦’Ò[«ù›±½-«¶{;Ô™5èccÚjÅ_(+·*îé4%ùÓÑ×:ŸM„¹¬àúWj›a3¯…½>o o]¤Ø„ø[o‘ÁL4·õPLB”Å¬Ÿ’åvW$f	çtLÐ†¢ÀK“ªO¼å[x:vöÁOJæ¨íúÜ½C’˜ØQÔÕÊ ãp¥Ÿª/`«u÷*©'ˆ¾Ó.NåS(dß~
èM–+d¢ÿGÿYBÛX’eèVý~Õ(xªóà*–àÆ–·—A5˜Q¾
‚â"5¢úêT­“ë„[òqž^¡~zAQº­9Ånáu¾b‡ŠÂ”·Záo"û¬ù¡7þÄÛp‡OwÛÓíuhÌú§§Xš\
§üKF.”TÂÉŽÒàév’ÞŒ¾„†Ì'Jz‡ò)<X
\=s‹M¼Gb/Ÿ53øÉaÅº‹«@6ºÂòo¨ãö¥‚e‹Ý·kÑàè\“.-œ7”jÝ†ÛA6S²-hUˆåÃ9	.ˆv(>«ÚVÊvIå˜œñþ{A¬{™™X‹M$; %×èëVÓ„Dôý­«qZÔ0HÅxéÑtÂ;„>£„mõòf#ÃD8‹µj'D2m2Æi=åeiÅ†&ŽAØ¡i«Ž·ç˜±ãm^E¬•c™i¯-è`‰¶,ÅÒM+»ŠˆÛ_$Œ{0o¡¯4`oÐÏãõ
^x=Oï7¼~ÏVÔ~"ÕÖ¹úY*‚?2¡#Öz>‘_¯¿wKüÐË5Q6!‚õ«Î•0Ugþê˜anyû+3õt–ŒÕÑM~±'ŒkÉ#Ÿ“2êëÐ›¼ÏÆyrZV Øª‹“­èV$õx¦.¤lG*6h?Ñf­Ç	!cüBè.¯Io\4îÿD®ˆÏ†'iî?.—-ë–üh¡…ª~Ñ
Ê¡hSZ]šE—ýÝšo­à§ÜDíÉÆ†1ãªÈ¼^<"HSŽN:3»ÚýpjE4Wkåª¿×{ÛEŒ›L'¤ä2©í„g
y÷Œúä˜W`X%,È¯é
 •4˜À¿ì€ÓtÜÖfdm¨¹džEî62ïdÕYØ4é?GÛü­©KFöÃµ}Ã¢˜Å±;šœû!DûfLXq«–°™•š¾‚¬ÒX£þ¬¥¶1ù$Æ\®ÊKIxx`Ÿ,ê³’3¥ua  ÝL&£wÑŒ|9P®¬oA±4ªD‰ë[µ52bz¾¾þª:»h›B±ÕÔ{L.è¯î4SÊbáYà„$2ÂGþyótý–á¹v¤úËÇó_WÉXF,cª[js
“E(c/p¬yL¶Þ*S€ƒ?C)[teç»9<tF^
öVRåÉ §“ØëÇü}ÔEò:=U±Ö¨êÄùÞŠ
”]lÑ®®¾À$"ƒÛ2`®¬8‰ÏX§É&«ò%°ÕHø5If¿ƒ? Šì#_xàÝµx"¦äÝ¡Y6ÏØŠ®îü5UµÄÜNauÝnü+Ë¸F:`WÑÀpŠÚI=Ð£g±v®[â¯/W}§¸¿Â÷-Ø[Â/‚_èðÿ·´2ú@3ëó¡­Ò*#T­îÄÌ°cZðM»„Mî¬‹¢ÀŸKYˆP‘€J‡Ÿz½’B¤e¨&žÔà¹p0¶	‡?•²¨|ÎÃ±×HÍgO¸Ï<19fœðNÊ)w6ù­ìÉV×#ª¶Ušˆ'	?=­Sô2Wº_âM§nI7Øu$Ê¼^óF£U|¸¦ x}?Ir6ýÍßºåd{´aÎ—~µˆŠ±ó	ùü•y„¬ÈXäDL÷€šý¤‰#x1Š|jsAô8î…€Ü±Ï4ùúôÂ_­kNÜ“µyò‚‡\·“„¹,2	îÛ¾”rúv+zÑÒ2ëXˆRòð
8¾RÂWþ…§ÐÉ_8q¾`Šîì"mŸ‘ûÐÄ’í¤=wuüú†y:„ÙÁ™«K-ßM
j²Ùïëa›~mèyqáRônÂClÓª (Á³¯<|0È §ûDí ) Lè‹Á=$Ì¥uS/ £ïP¶$, ëÚ²þÃ'©/ÐäÃ #ë\êN%ˆÚÎ(R¼fº¬žud6µ‚Cáý?s¦oµÒjäRêr«¨;”zYžÉüi°S‰WÁÜÚâU,^°Àþ+ééÃef¦Ëãê_HÎtÈƒt&€ gÝk¶F.Ð·>CþÿÏ1NÚu#CÛÆ *Çþ6¹«Â“î¢š|Ø©1{µñ®òìÄ¸ûkË+ŒS›ÒMÉv½w_ÀC ïƒ“3q{&Ëhý1èª/ÔÞ¥áÜ„þÊßi¨13®ä‹–L­ÆSÙßµooz¼Ê_%o=Ä ,`Q Î·ƒ¬Ãù™»±¸Ä SˆHsçÕà+ÿ¼P£ù¸d© ”Ë¼‰U¥hÇ7')ï¦‚ÌœùEƒ¹“!»ñ30åÛ†×bÑ0üê {ee¯’qàÂ¾‚¬^÷²døb_óÞ0“SêP	)‡­…5®·µò€†“NrŠrõ FŠ_"¿¤ÀØ$&õ^îøÆ¶Cµ;Ï	˜†½M40ºÛäáÕaA
BÍ¹G$w*Ø=ÝUô. ¢‘™’z€ÍÁ°cŽS¬ªiAôÔ+8·÷É¬&{N›ñ¡VÓzRú)9Á·6Ö&&íèÆš’Yÿ×ùfy¸ÍÚtÆÒö”ÛáBòÜXÅ Šž=-°ÎSÀ?à	^ð —ÝBƒ,•&º“>€xl2kj¯=vJJì]Ûm<#”Øº\<›"|‰¼võ«‘€(Ü*¯Ç^eLt`"ù,[íÉ…2÷ì÷tX@92NäßÁÖd½c]d®Q4§)Kµ›ýg¹F‡{«üãqÇÅ^ÔP`hÄå³ †Å§%ó¿‘j§¾"GVáDö¶9ï i”/86êI#ê	|L˜#¨<£œ©0)ÌG’& F¥ÕŸ"Ú@"’Žzºz/bªœžùº:HÁ’B¹Ëàáô¶©Ç—ª\T¸åq`ºfÜI°g#£›@0¡R&`@«Ž—oz†iè‚)²c½lª‘Î,ÖI†Éæ Ãùn &s*#Lµ÷Oê7@eF,”1Àh@U+¹üÍè¢ c<Ó,øžÜ”ºÑä¦'-UŽÅ².~tC²…´~”€ÕCweŸ]rÎ¼àÃ©¬;W©´ÉžqÃ©uêþ‰ñÝh{âªô…‹Ýpwª`–:¿@XKa~à¹ÜÁfÑ\uìæÈ ;Äü„	DzÁä?Óù™ÄÙ'o\À>În7]¦	H5!•+@,ï0B‹rºÏã]â€…ž¿E&½dod£õMÒil“ã4s×æ/·ætÁ=zK«‰xz ö¡Ên¨Pp—Ar:¯š<BÇÉÞžiEDÀzBÃTÛž]å/"xù9ä›KWV}Ûí¤<¯Ó"…MŒm$OnS€”)±ÛT	¥»Ú'5ÞG¼°QÄVg~éíäéõ¨Xu” )ÎñˆG^~\Ò¸ùaúÚ"ž	Xy2üòJ1ª…¬NVçÀô÷EÏz8Ä÷œŽ'Kÿã¼)wUkSUÖÆ:OÙßÑ†8èùÃpF¤-²Àô>‡iþÍ¿OÅ¢“wÏ…FWQiX×³7Ì®e%Nf§>ßžOšlÖ•ËW.ÌEkÇ­;‚Šü‰ßË_ª¤£3?ZUAhYˆ‘…ÄmûB¦¤¥	û4Ìôšïó~èpžú,‚æ}1ƒWq*ËQg<)ø”ÿ›Ç^€“¦Q!ßžYJ¼ÝNq8èiéw#Ûg.VðTCc&)ÂgêÔ&ò&3”^¨£(åØq •ŸÏjú~‹žw^Ã­ÌgLÂ ƒ¸û¦4Cn©üŠÃ_ƒb„€eâ^ÛÉIê¤Ÿv„;„Izì›Èsw|‡‚3Â(%* r=ç­E˜ÈDgsždEsSQ†wŠJ8Ÿª»PØ1Ödb3Æ}ÿ{÷	#%dÀvQ<™!n y­üË·ô¼æf­ø­÷ãlÁÆŠGú3 ÌW#¼Üºž,ÿDnLé€`ü“°/ÃÀj´·Ã¸¯zÆ?djxŠÁ‘.¾GÜÔºZëH1ŠÒ@Z‚ÂåòéôM=Š„`é»E¼Tj5¿ºÁ7)ÚÅåV@{7]òý8%g ¸ÇØšç¯_6{"V©p×L!G˜&màã”¾]îáhQ„wLÓä· wŒ„”û(çªµÛ@âá'¶&7é'm$R¶ÔÆÁÈê+ØüÄï<ÇßÕ^Ë8­ñ{›(àPKá2GlíòðMü¢ze¹¥:g–QÔŒvº	]7‰í°T¢¿1†Ð[x'"°%m~…I_¦ìxeÍ.6Ö½[’„Ä:±ÿM(ˆï*½·V•­ÈÕU9Õ]ƒ=6D)X}l]&Ù
jw(Ø7à ý«y1Žã(ž@¨ÄiwÎÝ³øl‹(½áP²74×CæoDÏô÷VK\³tÇ©iz×)Ø
Ãw#ÜÂ¼CâIàÉ÷ºbBq°—5T\Í¥L¹ß‹ýKÖI+ÑÂëì.ÔQiD:Ò«žÂ)^ê›òÇ¢n4V#úGW#ôšNó=-ÆLîèúÂ³;ü^â¶-‰ÞzÍ@_ÖSÈjã¤ÏVº4»O¥ºò c–Ã(J‘çú©ÛF³-+–>?ƒ\ÑŒâ(Ýú•ØTŸ _ƒ,íºçÜTÛväº5Ëb¼ª£ùjü¹Õ+W'¦~®£2VDCk™î
èq˜1„Ø³¼8U¡gš±&ñ´ì-NQ·§fÝÆJäj®z&—çYì’?ðÞ‹p™³¹£žº-G0jñ£ÿá‰yÕõE ú¦Œ²1@W)ªÙEÝGår×P~\=	„©¢¹SC´kä§!Wëý<1IS3
}·N+¼¥kµÐŠŽzv{@û‚—1Gxª}ë9ûËË˜ü¶n»†ÂÚže½è¶ÎÆæ‘˜L\JÑ#Ù6Ô|cbŸsîÉ!×¿¨½Gl‰Iy[" Opè. …Ö½Éžž@5['¿€Ú#ÿ‚\Ýv¼Þç‰G¬G<s7[‡3,ó`Ñ‡ôq¬=ºRxT¨Þ
A2_mô?ð@pp|…~O€7Ù
³Ð!\Þ>ÇQ%°€@ê¯éÌíxªðc%Å¬Æ3Y)#ógq žk‘ŸªX.ó·ŒÀ‰Ž
.6ŠM¶­Ïå¹»ð†!6–œž@¹7íâ§§ GþÖZdŠÎŒgÌÿí2êºæ’}ÊH³‚WX ÿnVŽƒÕ¸û¹r"Y„K9D7”g¸èlfê‹íŸcF5fƒ‹í5èÃ¯·$>4õn×â–·ž1˜óºÆ«íõÚó›Ðrg4š²w[àQµŠ5ÈÖÃô„S€_Q	 „h±úLmõ¼ Ò¤]-H .{9§¸S»“‹T&‹ø~Õc;·]YóŠ·š>t3%gH¶·ÈÔïTŽ1ÈR×/–LïÎñÓq‚JûqÑA#”kìYyÓŒª2#Þ7ŠÏ
‚`Ðâ
/œ›„yi{0„€tãW*×ð¨Íø74;Ú9J±Ñ¨Ÿss	ìçaÙìì÷+¶"¬òÙò÷„i§{Q2+qúû©ù2Á´bå=ÞiÆPÛ™GÑ)a3ÌâGÎz)ø¶³ýUÁ.éoH„I·¤#ÿ¯'‘’ç„
ðgÍ4£j?+jN™ß¸YU‘¿­O?º$mÈv9Ô!-Q‹ª{W¾OÅR€»õ5|œKéÜ€ï]9[÷É®$á’±‘¨gõYIÛ;b¶þ?ïÛóè*`’þÜÅÅ:‹eyÐ¯øà˜ö\åøõ˜®[¦ˆ f™ï¹üÉ8!;½)”9mð¡R%§†më,
ElSjøJ=…éòRn²KËkÿr¢·³80†Ð#1÷=9 T¾o*¶ÞòÉ±] ã¼ bn¼ðúÍ±#ÖŠ52èqðŠ´ûéò›ãX¯^É^T‹‹IÓž>¾WQ^y¬”q,ÿÌ¥à(]µÙÍrdüø“[èéoïsð‹O´ÿ¡¸éêZXÏó°ò•V(àÑ_ÿXyŒ¤Bäîáøu?A¹”[¼Ž‹… I–IAšˆþ®°sú=ÀHàE?’ô†s#”QÁüó@7¬ùÎ[9µã[D3Õ[Â Ø'›P& Sò¤Çø~„éÅ˜üÝÿWš#à>–—"f!qÀ€)Xža«ð"Ò‰W>ŸLÅ|ôX5we˜4‹¶è““#<,µ2Šf÷jÊDÚþy².¨`/*{‘ä¯¾\GtáX¦Å)#ÙÐ?Å/#|“ð:kƒôR<E›âe=…Æ‡oè×—‚ÍlóÙþ4ek‰¼Vœøìâ‰!<=Z¨ðôC–Øˆ Âû§IÖë´‹µ®á°¸bA2`÷ªþÒ†/NŠ[~ÊBÆ8i[‡Dpç5ÏíöŠ/ŸG1zk¹¨Jšu±óßÏÍ¸ÐS«í‡X"î¿€ßfFr=&2tºÇžãœÃJ¼Í¤Óó]°}€à^ÍœóÜz«òàçÄô	-ü Ž 7ZÆrò…íï®Z YñsªcÉ’ô"çþþÎ‚švDÀFÞ¤OÊÉ^›®ýç1i}•L;‰=†° ¬{°I	IOò#öV’ƒ‰*‰‹Gsrñà^JT7i“2~ê‹ºáøÖð“8ŽMM+ìy,ýL:˜$–²Ëòï\CÇm§ôâ¥[TµÎü4M`¦	)¤PJÒj2ï›¨ÌPpÖÇÙG×N}@¾¤yÉjFy¸Ç£#v M>PÍª$Ó@â{òÓ³G7“pgO;Ú¹ë“ŒßÓD ,ÏlÕ5Ë¦R6Éÿ®RT[¹_DGECÛvSUã.Ô…ÂÊú°ê>Ç:oït¶ ¦VY:åþŠýõ‚N¨3´]}ƒ–³¤<×[«Z+`6d—‰±)÷¡ÃŠÜ˜rÞui:Ñ•â>´äqÒLÆîmž|ð“ý¯™ô" Àˆ
8«e+À8Cj°·¢zq]3 e!«¸þ¥‚êñ”ãkðšÖxRëÞJuÂø­Ô¸4±ú°Á+±kí‚õ	7)YÝ°,Ï¦QñÎDm¶È)ÙÐr•œXépì„Y+‘HaØ˜â&î»¾Îà!ýd¦ÍúnAE{†k’Z}.|•
dÛH¤ãï
á÷Ž½"¥¿ü‡æ~JO<¶ë‰JL[¹@
gZÿoÊ¬st÷« Qmúîxô\»òž¸(yî©ÌN?5ÇnËƒz‡'ã¸Ž-dk%×ž1È*®ªi·—Õè‹ô\o6˜~ê°#jõ-à‘Û…Ñ~±¼ÿ
Ò$>+úÐ^í?¬ÒºvÄ ×GøiÇ±¯a¥²òŒ>æ¤¥C©P÷Ø—ßË&ˆ èƒã¿š¥Ðº™ô]³‘+Xú_ñ’ØorüûƒXnkA$ŸCÛb³À¯L•ÛçŠÄö"•PEÑªé»9ÂÒÂ7jüW>Î€©”q#x:pÿ4ÈÊ((…òïw<f­¶î©Œ&ôSÚµ›ZKƒ(¤ÿ¬#Ÿ¨p«ëìj§®(üPâÁ–5¡Áà–	“ŠÄ@Dh	!à—zÖS ¾®X…Îùº÷;§ë•¦UY§T	ðoÁZý¬ò	À›šHÁÌƒ%Þ:½?÷ZÝý¬ÜñÔoEáå:…cý+à®Éígwc}ØL1“ð%ûÝ™\çXøõD fMÙ#P4"ŠìÔ§âÌÂ”„I Eµ,àY¹ÙÑ<EQˆ2!àæÞ&ñnp-Öô4h<ˆ9Øï9é§¼Ô®À½#”¿Šµy5±|D2—&<¾ˆ–ˆPjŽ„±~\†Ü«
v•2~ðZiP?+O®X¡“|‡ò[{.	ZŽé®½Gƒjísï¿Ž.:ºX¢[ã°À;Òu^aìÓï™löV»€ÆX	‹ºpY¿ù@4`:ðŽÿž¦9#Hñös´Zq¥ü¼o0…ŸŒš·R:C(ë…kîìÅtEËì ¨­úEC	yå@p«§ãÓ´ÊôQîþ¡¸“¾2áÍYà©QmÄLÔÍ\bwQá¦ÉJÑaÝ%ã7É®1²Ä om Vï:ôý(Z–B«mh2=#Ì|1Ê4†÷žÇÂØ‰w¥ö—HJöš-å.+àÕEúÒCêÇ¸$µo9Õ°ËÁš„‘<ª Îw(„ü´ýÈÇ"ÂelÓ«ŸnQý¥A|`ê²°óÿ¦Aa&eªöÜ##¿Þ;þ‡Tp]'5Ã©¤¨+ ¢’ÃAëÒ‰L8)(ižëýÅû¦,l˜	¢Æ	±¯ RGñBÏ† /ÙÕÇ«`ß”_Q1Œ'«óè“ùi÷3éÇi~.ë	–;˜üûJOG²—ý'*Ôí|GO9³˜gu-NoN¦lžTOôe>R)È«w//Ða×\öFÌíƒ-,Fì}JÞîÔ‰?'GP1ò£|­ÞCOf'Ãs<cÐ"¢p=vAvÉîöm Ü‹gÖÆÞ‡ºo8ñbâæápõNu€}ÓŠ‘òºÔX¼ïÀC}ÝFç§íœ*õ¹z3‘3S“s;ÚØ›»iRü£Á¨VÌÅÞ…ÇŽPÉ÷Y^«_q~
Ã4ãÄ­çÍ¼‰©¶^ÏÂôÆ1ÿy8€g.AOCbÊž­7rï%[:j*‰q½:yëÇÉLþ·¥*O`õ`?†ûáZØKžÝ9Él¼É°¨—Ë¦ÚÿêZÜò±s†r€c ×|ÿxéš]nƒ¯ƒ&ïÊ6,;ÄòÚî*%þšÙ±TS£…Fsð“WŒ£—¸™óŽøcÙ{K,EÛ9È?¨£Yë®ëHU&*_ƒ œqjÖ-,•õ€W›æ±¥’CršHÙ7qómyÉø§Dê0RòÕ!ëo¦®$©Ó|•{Åc;<¯·P~ûêàFG!9HmšJb-nœYËû4)MiÀ w'ÍšóPá¨ó&¸e@3¼Khöõa>ú0­qIù7ý°µÑ'Aé<]ô7]1¡‘SŸAh’,N‘c1„\LH±Ò·õÿ0Ÿ/i©—Ýë›/I·P`xÀÁNçGGA@”LòçB1’Vë@jíòÈ„ÀŽS;²ÈË‡ßÍ¾tÜ¸höW¬nÏ´qþwU/)65V·|èGÀQ«ú‰oBË^íqØ¤!êmF 9eqö,hfzK,ß%™ò<ŽMÒ¶C!Íª{%ÿ³4™3ðAmþü#'ZˆLÈØr8°,‘”ëgõ§CQ¢æ-î@ò¹ÕYÐX *>¤ÚƒÝº&dŒ‰Mæ5I¸ËÚHH¬/C¥ÖGõ-vREöwÇß~;Ú€+£’Z‚¾ŒW^!
}s‘&×r÷×x"KÀÜöóÕÕXŽ.öin•æîK§Ó.ŠmHÑ±k÷õ·ü¨ƒaAtƒ ²ò%ùö+D½^²º|‹rl`åc«˜'Ÿ)Sç6®£j†Å§uóX!Î“JX6¢‹Úû4ŒŠ:Ë«ïkÐI©‹OfDŸxò(·änå·¦å9E#Ð3N¯v’Ü^AqÚ'lS¡Þ>?ùÜ^èn¸cNÌÔIƒú±Øˆ¢6á®Üi§®bâ*¥Äê˜˜šC.¡[ ÃÒ`Ÿé‚STÄ2‚&äƒsßp¾#KFá­žÀx¾L/"'¡hIí!ÓrüªûK&ÁâIZ?CQÃwî#B5Io|¨÷XŒ,À
 À#~4œG“úhíË‚žGÚý ¨v’Æ¯×q¢¬Ë•’aº”LéçÈÆ?ÿÅõ%†qùÂ#Ž.	¤‘Ó„îY®™Ž6BÄnUÀý£mw’…Í;Cwb®öÐè·gT~2ú¸bâÙeÄòUWWñô< {§qìÎ‰³¤C”ÅI^çË#¾€¨1‡xé¾â«	TýòÁŒË¸Z?-žx»¦+µ0—\ùŽÝ{­º é¦Ë0TbÇ„î¯ 3¤£ù·`EÛÔIvm ÂÇBÏvƒóîð÷atÜ™'¥ë
j‹c#3.ŠµÁßwlúÁ¦g«½—:£4ýWh(îD¤; «çqÖ‹=/gmR÷ð¸QÑWä5%Â
4ˆè(ï~ê¦LD,ûcÓºQCÁ©Ðm@ÕEî,%;[Ø¨”µ§K[ÕŠSLË>'–ÊÅAò(k<.çÄ1¨¯kÚ²
!"ÇÇ^‰µÄu¤ÕEDrO»~8»›	|‘ÂáçÆEËGš]Ùú§ödìÉ-áqBÓ’h¶!8a§WpX±ÂE5AÁr¨±y\ÍƒÕG6af]ØNë‰*ehô“ñË8ŸÔ œ"ŸsÔk:­á®h¤2-åíý(¼˜5¿¯ Vî¢ŽßZóMD0ØÄ7™ƒ¬˜èFÉÏ4¤ÃkîÈ¸`°lV;©¦žqP´5ê@º,·Ï³8©6|j?M¶q‚÷»]ÿÆM
_¤¹ç²fÆõ$u<Rä:-ëE$Ýv[ô/¢Õ±MP3{U—s_KØ»Ò¾êºâ
o&¨dZüáüCi×â9JLUŠÎ‘ ©cÂbö²òéMRü®å*c
Ð¥uWUŠ½Ú÷›„|Ì’vKvíOipýƒ"yê•èÐFgíçã€—  49˜ƒpo>±|€ay¦q_Øµ]¬ÑX8Ï!]ÄæÏE«’±tô){Ü"çHÊó~gÄ~(#Ä+MÕ{®Ýîâ—@÷¸=¢‹@2ûÑÖ,Køá[Ó“ELAX®£‹×#ãCí˜™öýüöó/=d4fç“4„Šú	6h2à¶#[%wäÝQ­2ÓFœZ¯á¾Ýé‰0–^¸g6¢H®«è<ÓJIÓî™úìý©Z´B.ÇAÔ0‘wˆ™Åë÷ª„=«uRTœâÎúž]çM_«¶ÇB*:•+‰|vF#œ,ÍT$€Q–åÝØ§²Ü¸¸ìiêüt>ý€ ¶8«\†èb¯|Ì™LS	e3øpå\ÝòÀüIü”2«#‰HeVëj—‰¤XZ²I­ÃH¾+LwßÚÇ•L¼w÷ì@¨™p0"+¸p˜¾KïçzTŒg‚'¬K=ÁBÆƒ¦­Ä-VÄÆ¶Bw)0*ÙT÷“ÏMùâgx0yË$‚#šìï,,q“ˆ«FVÖ‹ÄƒÌ¯~õÕG#|<)f:F´Îyôñù3q³ÅRaÊ„íÄ—Þ8ËNœòžžWƒ=«„ÉƒõšXç³,F›¼6ó‹T€(«“XG›Â]["Wûö«‹ÉkI¹üÜž‹pÔ|mýÀáƒJš¨>/Í“®u	‚÷v‘Ó.…a8äoÒŽJÖâÛTDÉWÆDJ~vºŽiõýÕô ôÞ$ë!Œ hµ(î 0
zøî³’ ¿¯Já Hc,Ai”$Ø“þýp³-{A>vv«Ÿë
:gCHþÓŸŠá°@Á¬¥ð!ÿhÅZ_š‡­!éÆÌ7Úi0m·§™–Bó®…Jõô*Áq>äwçìxulÑêŸ·¯¡|Ž Ýßù¹¥/ZÁ°àŸÜ÷Y4Š“Tù€hË‡A+ÅÇ6Ž±8¦íŽgs?T„:öyƒ2ò ÝIæÚY\V4¡¯¨éz
¼þ†›Xjû£K0×}yØ>Ì¾˜Ð«ð/o]C™S<l–F÷yÄogôýtbŽÌ¥†Gs<%ùÎr–‹t÷ÍÏ÷2×4Ü-¬â\ÐŒª'K"%ò\œ4é´34ž|`ëÊf·œüàó«¡isþZï£©8	u›4ÙãÙm‚ÁTeH}Æ4mú` ë²ã‚Ø§÷*’ãª•0×“›˜ù³ºìILcµŠåW¢U’Ñ$U(ƒ»žÅs/ë¨&(ÿÖ¼æÞ‘ïà±J=”´Ö¹¤¨/¯V‰ë£¦`±,âeÀFËŠòŒ´0t‘ÁmÏiÐ!þÊl“ÉžUr“`ÞìHD¸n >q«†žW
§Z§Nþ‰²·r^B˜Q!A;fsîN7QÕwTªŽÉ¢‡å64Mmù4wõÈ!“È¸9xš¼¡ô|h+…Ì2²Ñ šÀ«Tô·N3%vænõë·¥åoÌ3¢xã¨‚ —Gr$5)JÉÍ›5ê.n@!ý†¢
%J~›ãÌ|2Í†^y'°(ó¦¹È0ZÃÃ©©hx½9÷ØÌ_"ˆÑÄ‡…+ò òWXÝ…I/„w3S”Å¡®-=~ƒ+ÖàFÃ¶tÞÕéj—ã‹L¹ì„Rjl=9p.{\×Òû“(¡ýA>ã“	Yg¯ô!ßVðšÿPŠÊRûŠo4®Ñò·¶¡IÜÆtsM“p“§â#è
?îÙ“ïsëlÅ•‡&Ú<Øº½q~âœX·¦€V"&¾§¼¿Y¤uµ-D:²ÎÊ—NÄó‘òPí+ó×Ñ`rUÔkü"™ûC»å[À«Ž‡)S«8s‚’‚i¹òT‹—8¨ö¥cªÍÌ9ŠÓn¥'®F:UFÏÌ•Öüâ9<ÑVCQƒ˜êÈ#$Ñ#(6eÄøThær¸»euA#sG´RÌF_æ3*¿>îêÁÙ§g³^jwÐQ¥xŒõC!kö½â²é›úµÂÃ Ï¥ô¯uF8ûÃK¹	‹üLx×ÂÝü;Å=3¦1€µSëðodZôˆƒj|ƒÿuQr:¸ª|‡W®þ“1¸§C&žD˜óºöÁ£=¬WC< Ìˆ½Â¨#å&gãíiÑŠîKœ ôU”•¡|÷7Ò•F&€áŒœyE>C¨¼¬“˜*¶ª7pVùvš¸	K•/èµ_o.0òùÍöé>ÎÁ•ÙÄÑ–Ê÷Òfú\°¹ô º¥Šb„V…2ùd…­¶œ>R{#‚Ûú¾+š—è·½kÚ[©BE‘èjªÈCô¤Y7ìfó±Z²NaìXMð'ç­²)ß$/nÂxžØÇ{ûGÁäNÍó5VFWmúªƒ²–Ù¨ § Öè–ûÝMGˆþ¹+ åY…Êt•-å®U¤,´T;›*\4»¼æp3QÕ
³êÙkø(ì¼¿ÔðcˆwurX•Ù^ÐµãÕwÕç×UD<ì–˜ŠÓ“HŒ¥6óoæ^˜ï“,å´A]	ƒÀsîàŽ’~‚üÕl»}I‹ˆ’çÊé›ë),ÂÐNÌþÒP‘àÏPº}uN9yžóòèÙSÆïg©æBÙ¤@ò2EÎõa€øYÜè°4–¥ùžƒý§]ªùÁE-òîŽÆÐJÑKŸ"V×?¶rpö\@9v§¼ û«LÈ/ÅÃÛŽ]Jl\9cM™7±pSn@¹/²b‘Ê"Ñõ2æÅã|•z#•·ã—%ì9áÌBMQûÜQ@§¤¦Ú,mjµ,Ïõ1#¬T/·f¸Ë”‡¬ƒpñ+Ð°L™>Ó	hïíésÄ,°™´¥¡f;ÏøÜÎGÐS&ÊpO¨êã}`XQÈ4ná_ÒÔBˆVZyÊèþÓ‡½+NB¶óÖY†\©²3ì^ž"ÿ|ØANšW	v­8;ã®`þd}W·ærñ 	Ç|B¼‰ÆNÿ§ÁªÈTdï@cVÚÁ•4‡	Ù8÷¾×óh¥KœÛ-fËÕ
kW_’Å–2N±šqd€êFlðÙóÍ=Fz\y’U(ü(m¥¦®Ú‰îïÂpð¥È#Ç§0Éˆ¨Øb¨mÌšI¦„<»!“áxwßŽã%}ëràüp2I£™SôKbÄþò•Pjç¯­""<FJbf£czÑ¹/Ô’WdL†ÿ
¿sÁäùª¹NìócŽ<B›¤y‘>ƒðJ¼–p–Sñ©ÙpñUwè»iÑŠ\BTÓ—„3,èåÂç¦Ï×v¯èiEd  Þ0F!£g’gsw:æ˜Pì˜…À\Ï!1ÜY¤æ%Ï{Æ²„ÔÑ}Ž­6õœ«&ÿî
hK/t@«k…Öà!9áÇ»@4Œ‰Í<TÒ¿éÂ$ŸƒÙB^ü)&²"ˆÝß	Çº×ÍICéoÉä‰u@j‡
`vêA¦ô¨<”¨3L§zA1˜M<jð]©Ž€¬ÙðŠoÅs»J9\¿½0(‹N¯ t»Þ¯T5üw"	å^|”Ì7_‰È`…i)ý-wŸ‹nÊmO½½sèTü¸¥c©aÙQ®³Ã¡:¤Ž¿¶Š#¾å—Ÿ‹ûÒIqFúXé1Ð>ÝÛ$=¿¯cIr½D%bMúÇæˆ›¥ËY~wÖ«à>¶ÁlS•`‚6¾éÑªq^ñžáU6„äß+PWƒÓ$öÙý‰êô¬nhQaìÊë£[${×Ôeo¶|¶2ÇrPžî9s‡QX|]$PÜæÐúË¢–&åë\ê“•'‡ÇÛYéº7 Ñ&E×ÅüéHèß·,ÛûÅ~ë¸4U"ZzÜÑÐà™Î‹Äfðâ41µ€B?ü‚œ¼3†2àT+‹&sqUÆ–§†cHq¹Â=Ø÷1ŒW¯Kw[ß»µ–ó²ëºa!;æwþÇÌ…4”0iƒ–ÆÅƒí¬Ø
á¦™Êî§4•(ÐDÞ6xˆøCÞ$­¢£-XVc®åºè…#q¿xóæj~„¢‚lùƒâµn.ÚËÑv#¼aAv”NhÝÏu™¤u¢,Ûbˆ2ØQ	Ôò;ÝüñÀ0qSiá˜²V?ÄiXËÑ‡¶¨ƒ„‹bÎ«n¬Òê!Fyéå™Äb9M˜ž4Õz£…Y]pˆga©h^bµb™‹wÕ½øÛà0õá<$“: ±4RaR€q€£/òúÜI4ët°Ñ cÉIb×ª”¯±("ÍºhèT3éµKå,`¸{\d#ZÐÊ0F0¬‰-+>5Ý¿\œ£
@…¶‹fŒwû›NI:×E\ëò<À€Ÿ™B6iÝöŠ@‹BtTâ2Ÿ?ªbòN}:úHó9èÆ@'cšº1|tö3)ð1²2ùÔÑòÈZ~Á  zl8ñ·Ñ {t"ÓÃ:ˆé¢ìQ¾|Ø«ämÔîaù\WH ]àu·[S®o®Z'&¯ƒ™Ñ|-R2<ñß.9Ûn„ Æ¹€S,†L%€†Œ¶¹{/ÑsM©¯¾P Ûç>ÆM-‰Ü]ç7FÈ;®ÞÎNlñ$ Ó˜™ìm§Í]I/Üê/Ë•c“05æžb&:¬cE,þ”hOSâ©ª@Àž	RñÍ»Š…ê•–Öê—"OE$Èa.vKfK&¸|¡þð±¿žY£)pgÖZÖ;.½iÜÔ<æ oAÕÔÑÈ;¤ªŽífCñ³÷\“ÿ,sÆh@fÍ-uaí/¶˜£Êq¨ƒÒ÷>=mÃ»nYïÜÈÑòÌùP$úmw¡\¿Õ¹˜B#B dë}jÿdºIp(z…çŠS²Z†©Š·þ{]›q<¹—/3Wç‹>[©C7õ§1po¼¶ˆ˜÷É`æôÒÕªßÇç¡fQÎAØ ­øtÔsåµ.FfGn8¯“Þˆ>9½¸Â@µÈ¿/ç½Zý€¥ª¤Êº±vÅ|8òEÂ	Ñƒ:‚vŸMc ã~òàðÕÑs#4ØhFD'Q€viFÚûÅÞ–¶â¡«$_8ÁJÅÍ£%£¸Ê¬þ{…}…øwµ¶]dywBÜ xŽ‡ž:½°êÒÈ’GhåáÉ´OŒ€Œ­ßùÚ1à<Õˆ±Gî˜üz<¾
[§Óñ™.P ØedŸû~6D<ÍÄ©?ëÇ95=v[Ÿ¥JaÃÙHM±¤à#½]+N£M±ÚuØd?L™l¼«õ&ëU³}Ï#qº_N\y\eIXEMÿÍ¤[Om‡Ñ´X˜×=/nøÚ=™Ÿ\JH ¯uj7¾„V u•Œ…$§÷ÍOpCh0-¾dT…R¦p«0šfù¯úvr¾ÁÕ×|náõNÁ(û^}mŒRº©ùMrÊaÙ÷Á»Þf3ƒû"£öYNp}öSÑük±\Ž\Ø$‚î‰ êñ¸þÞ’mCç¼Â'	7P×gŠ•lëû>’'¬ô“C–¼	–‘D"@@Ë¾ßž>ŸŒG~±µ(Ð'Ö:È9p¿i¨BÑcÙÔz&Ï%ž8 „dŠ’h[í«~«…²ÆgÀÒ„Ï/©,µ–·©ŠãÕÕ¢•Þ§a£eú^‰—e·_„’XÕ©N 'ßÚæ§îZüØ6=Yç®À6kØ!“åßCÕ®[a/ÉŒ¾¢G»TL¤}n	ÎoPýC¡Îà¥#ü…R¥Žüÿ¡…ÐÍp˜¨óÄ±;g¬™D+ìüáoË_»s¾µÛØÅ[ÐGüÂ«Hª(hû¸Ã_ÀÈ|Lèòc‰]»J»×D<H„øÊ4a3ç,MrŠùö­,q»ð¥‹â)Aa½€GÐb7ÂƒbÄs,Áº{‡:s±'¨úÙ‡\È¬‹„ñ™6Kf// SPDX-License-Identifier: GPL-2.0-or-later
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