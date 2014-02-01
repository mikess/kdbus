/*
 * Copyright (C) 2014 Michal Kulling <mike@mikes.pl>
 *
 * kdbus is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the
 * Free Software Foundation; either version 2.1 of the License, or (at
 * your option) any later version.
 */

#define pr_fmt(fmt)    KBUILD_MODNAME ": " fmt
#include <linux/uaccess.h>
#include <linux/proc_fs.h>
#include <linux/mutex.h>
#include <linux/seq_file.h>
#include <linux/hashtable.h>
#include <linux/idr.h>


#include "monitor.h"



#include <linux/device.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/sizes.h>
#include <linux/slab.h>

#include "defaults.h"
#include "util.h"
#include "bus.h"
#include "handle.h"
#include "namespace.h"


DEFINE_MUTEX(kdbus_procfs_mutex);

struct proc_dir_entry *kdbus_info_procfs_entry;


/**
 * kdbus_info_procfs_open() - show information about kdbus module
 * @fd:		file descriptor
 * @buf:	buffer
 * @len:	length
 * @off:	offset
 *
 */
int kdbus_monitor_dump(struct seq_file *seq_h, void *v)
{
	seq_printf(seq_h, "%d", dada);
	return 0;

}

/**
 * kdbus_info_procfs_open() - action for open /proc/kdbusinfo
 *
 * @inode:	inode
 * @file:	file descriptor
 *
 */
int kdbus_info_procfs_open(struct inode *inode, struct  file *file)
{
	return single_open(file, kdbus_monitor_dump, NULL);
}


const struct file_operations kdbus_info_procfs_fops = {
	.open		= kdbus_info_procfs_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= seq_release,
};


/**
 * kdbus_info_procfs_create() - create a read-only /proc file
 *
 * @kdbus_procfs_fops :		file operations
 */
int kdbus_info_procfs_create()
{
	mutex_init(&kdbus_procfs_mutex);
	mutex_lock(&kdbus_procfs_mutex);

	kdbus_info_procfs_entry = proc_create("kdbusinfo", 0400,
					NULL, &kdbus_info_procfs_fops);

	if(kdbus_info_procfs_entry == NULL){
		mutex_unlock(&kdbus_procfs_mutex);
		return -EFAULT;
	}

	return 0;
}

/**
 * kdbus_info_procfs_remove()	- remove procfs entry
 *
 * @kdbus_fops_s :		file operations
 *
 * Return -1 whe mutex is locked, return 0 when procfs was removed successfully
 */
int kdbus_info_procfs_remove()
{
	if(mutex_is_locked(&kdbus_procfs_mutex)){
		return -1;
	}

	mutex_lock(&kdbus_procfs_mutex);	

	proc_remove(kdbus_info_procfs_entry);

	mutex_unlock(&kdbus_procfs_mutex);

	return 0;
}


