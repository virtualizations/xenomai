/**
 * @file
 * Real-Time Driver Model for Xenomai
 *
 * @note Copyright (C) 2005, 2006 Jan Kiszka <jan.kiszka@web.de>
 * @note Copyright (C) 2005 Joerg Langenberg <joerg.langenberg@gmx.net>
 *
 * Xenomai is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Xenomai is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Xenomai; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/*!
 * @ingroup rtdm
 * @defgroup profiles Device Profiles
 *
 * Device profiles define which operation handlers a driver of a certain class
 * has to implement, which name or protocol it has to register, which IOCTLs
 * it has to provide, and further details. Sub-classes can be defined in order
 * to extend a device profile with more hardware-specific functions.
 */

#include <nucleus/pod.h>
#ifdef __KERNEL__
#include <rtdm/syscall.h>
#endif /* __KERNEL__ */

#include "rtdm/internal.h"

MODULE_DESCRIPTION("Real-Time Driver Model");
MODULE_AUTHOR("jan.kiszka@web.de");
MODULE_LICENSE("GPL");

static void __exit rtdm_skin_shutdown(int xtype)
{
	rtdm_dev_cleanup();
	rtdm_proc_cleanup();
	rtdm_syscall_cleanup();
	xnpod_shutdown(xtype);
}

static int __init SKIN_INIT(rtdm)
{
	int err;

	err = xnpod_init();
	if (err)
		goto fail;

	err = rtdm_dev_init();
	if (err)
		goto cleanup_pod;

	err = rtdm_proc_init();
	if (err)
		goto cleanup_dev;

	err = rtdm_syscall_init();
	if (err)
		goto cleanup_proc;

	rtdm_initialised = 1;

	xnprintf("starting RTDM services.\n");

	return 0;

cleanup_proc:
	rtdm_proc_cleanup();

cleanup_dev:
	rtdm_dev_cleanup();

cleanup_pod:
	xnpod_shutdown(err);

fail:
	xnlogerr("RTDM skin init failed, code %d.\n", err);
	return err;
}

static void __exit SKIN_EXIT(rtdm)
{
	xnprintf("stopping RTDM services.\n");
	rtdm_skin_shutdown(XNPOD_NORMAL_EXIT);
}

module_init(__rtdm_skin_init);
module_exit(__rtdm_skin_exit);
