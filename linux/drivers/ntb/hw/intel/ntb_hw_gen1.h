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
	return altera_ci_op_cam(en50221, slot, NETUP_CI_FLG_