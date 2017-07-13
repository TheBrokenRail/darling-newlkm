/*
 * Copyright (c) 2000-2008 Apple Inc. All rights reserved.
 *
 * @APPLE_OSREFERENCE_LICENSE_HEADER_START@
 * 
 * This file contains Original Code and/or Modifications of Original Code
 * as defined in and that are subject to the Apple Public Source License
 * Version 2.0 (the 'License'). You may not use this file except in
 * compliance with the License. The rights granted to you under the License
 * may not be used to create, or enable the creation or redistribution of,
 * unlawful or unlicensed copies of an Apple operating system, or to
 * circumvent, violate, or enable the circumvention or violation of, any
 * terms of an Apple operating system software license agreement.
 * 
 * Please obtain a copy of the License at
 * http://www.opensource.apple.com/apsl/ and read it before using this file.
 * 
 * The Original Code and all software distributed under the License are
 * distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
 * Please see the License for the specific language governing rights and
 * limitations under the License.
 * 
 * @APPLE_OSREFERENCE_LICENSE_HEADER_END@
 */
/*
 * @OSF_COPYRIGHT@
 */

#ifndef	_KERN_MACHINE_H_
#define	_KERN_MACHINE_H_

#include <mach/kern_return.h>
#include <mach/processor_info.h>
#include <kern/kern_types.h>

/*
 * Machine support declarations.
 */

extern void		processor_up(
					processor_t		processor);

extern void		processor_offline(
					processor_t		processor);

extern void		processor_start_thread(void *machine_param);

/*
 * Must be implemented in machine dependent code.
 */

/* Initialize machine dependent ast code */
extern void init_ast_check(
					processor_t         processor);

/* Cause check for ast */
extern void cause_ast_check(
					processor_t         processor);

extern kern_return_t cpu_control(
					int                 slot_num,
					processor_info_t    info,
					unsigned int        count);

extern void	cpu_sleep(void);

extern kern_return_t cpu_start(
					int                 slot_num);

extern void cpu_exit_wait(
					int                 slot_num);

#ifndef __DARLING__
extern kern_return_t cpu_info(
					processor_flavor_t  flavor,
					int                 slot_num,
					processor_info_t    info,
					unsigned int        *count);
#endif

extern kern_return_t cpu_info_count(
					processor_flavor_t  flavor,
					unsigned int        *count);

extern thread_t		machine_processor_shutdown(
						thread_t            thread,
						void                (*doshutdown)(processor_t),
						processor_t         processor);

extern void machine_idle(void);

extern void machine_track_platform_idle(boolean_t);

/* Signals a processor to bring it out of idle */
extern void machine_signal_idle(
					processor_t         processor);

/* Signals a processor to bring it out of idle unless canceled */
extern void machine_signal_idle_deferred(
					processor_t         processor);

/* Cancels an outstanding machine_signal_idle_deferred, if this is supported */
extern void machine_signal_idle_cancel(
					processor_t         processor);

extern void halt_cpu(void);

extern void halt_all_cpus(
					boolean_t           reboot);

extern char *machine_boot_info(
					char                *buf,
					vm_size_t           buf_len);

/*
 * Machine-dependent routine to fill in an array with up to callstack_max
 * levels of return pc information.
 */
extern void machine_callstack(
					uintptr_t           *buf,
					vm_size_t           callstack_max);

extern void consider_machine_collect(void);

/*
 * Machine-dependent routine to inform platform layer and external
 * CPU power management about context switches
 */

extern void	machine_thread_going_on_core(thread_t	new_thread,
					int		urgency,
					uint64_t	sched_latency,
					uint64_t	dispatch_time);

extern void machine_thread_going_off_core(thread_t old_thread, boolean_t thread_terminating, uint64_t last_dispatch);

extern void machine_max_runnable_latency(uint64_t bg_max_latency,
										 uint64_t default_max_latency,
										 uint64_t realtime_max_latency);

extern void machine_work_interval_notify(thread_t thread,
										 uint64_t work_id,
										 uint64_t start_abstime,
										 uint64_t finish_abstime,
										 uint64_t deadline_abstime,
										 uint64_t next_start_abstime,
										 uint16_t urgency,
										 uint32_t flags);

#endif	/* _KERN_MACHINE_H_ */
