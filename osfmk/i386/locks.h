/*
 * Copyright (c) 2004-2012 Apple Inc. All rights reserved.
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

#ifndef	_I386_LOCKS_H_
#define	_I386_LOCKS_H_

#include <sys/appleapiopts.h>
#include <kern/kern_types.h>

#ifdef	MACH_KERNEL_PRIVATE

#include <i386/hw_lock_types.h>

extern	unsigned int	LcksOpts;

#define enaLkDeb		0x00000001	/* Request debug in default attribute */
#define enaLkStat		0x00000002	/* Request statistic in default attribute */
#define disLkRWPrio		0x00000004	/* Disable RW lock priority promotion */

#endif /* MACH_KERNEL_PRIVATE */

#if	defined(MACH_KERNEL_PRIVATE)
#ifdef __DARLING__
#else
typedef struct {
	volatile uintptr_t	interlock;
#if	MACH_LDEBUG
	unsigned long   lck_spin_pad[9];	/* XXX - usimple_lock_data_t */
#endif
} lck_spin_t;
#endif

#define	LCK_SPIN_TAG_DESTROYED		0x00002007	/* lock marked as Destroyed */

#else /* MACH_KERNEL_PRIVATE */
#ifdef	KERNEL_PRIVATE
#ifdef __DARLING__
#else
typedef struct {
	unsigned long    opaque[10];
} lck_spin_t;
#endif
#else /* KERNEL_PRIVATE */
typedef	struct __lck_spin_t__	lck_spin_t;
#endif
#endif

#ifdef	MACH_KERNEL_PRIVATE
#if defined (__DARLING__) && ! defined (__cplusplus)
#else
/* The definition of this structure, including the layout of the
 * state bitfield, is tailored to the asm implementation in i386_lock.s
 */
typedef struct _lck_mtx_ {
	union {
		struct {
			volatile uintptr_t		lck_mtx_owner;
			union {
				struct {
					volatile uint32_t
						lck_mtx_waiters:16,
						lck_mtx_pri:8,
						lck_mtx_ilocked:1,
						lck_mtx_mlocked:1,
						lck_mtx_promoted:1,
						lck_mtx_spin:1,
						lck_mtx_is_ext:1,
						lck_mtx_pad3:3;
				};
					uint32_t	lck_mtx_state;
			};
			/* Pad field used as a canary, initialized to ~0 */
			uint32_t			lck_mtx_pad32;
		};
		struct {
			struct _lck_mtx_ext_		*lck_mtx_ptr;
			uint32_t			lck_mtx_tag;
			uint32_t			lck_mtx_pad32_2;
		};
	};
} lck_mtx_t;
#endif

/* This pattern must subsume the interlocked, mlocked and spin bits */
#define	LCK_MTX_TAG_INDIRECT			0x07ff1007	/* lock marked as Indirect  */
#define	LCK_MTX_TAG_DESTROYED			0x07fe2007	/* lock marked as Destroyed */

/* Adaptive spin before blocking */
extern uint64_t 	MutexSpin;
extern int		lck_mtx_lock_spinwait_x86(lck_mtx_t *mutex);
extern void		lck_mtx_lock_wait_x86(lck_mtx_t *mutex);
extern void		lck_mtx_lock_acquire_x86(lck_mtx_t *mutex);
extern void		lck_mtx_unlock_wakeup_x86(lck_mtx_t *mutex, int prior_lock_state);

extern void		lck_mtx_lock_mark_destroyed(lck_mtx_t *mutex);
extern int		lck_mtx_lock_grab_mutex(lck_mtx_t *mutex);

extern void		hw_lock_byte_init(volatile uint8_t *lock_byte);
extern void		hw_lock_byte_lock(volatile uint8_t *lock_byte);
extern void		hw_lock_byte_unlock(volatile uint8_t *lock_byte);

typedef struct {
	unsigned int		type;
	unsigned int		pad4;
	vm_offset_t		pc;
	vm_offset_t		thread;
} lck_mtx_deb_t;

#define MUTEX_TAG       0x4d4d

typedef struct {
	unsigned int		lck_mtx_stat_data;
} lck_mtx_stat_t;

typedef struct _lck_mtx_ext_ {
	lck_mtx_t		lck_mtx;
	struct _lck_grp_	*lck_mtx_grp;
	unsigned int		lck_mtx_attr;
	unsigned int		lck_mtx_pad1;
	lck_mtx_deb_t		lck_mtx_deb;
	uint64_t		lck_mtx_stat;
	unsigned int		lck_mtx_pad2[2];
} lck_mtx_ext_t;

#define	LCK_MTX_ATTR_DEBUG	0x1
#define	LCK_MTX_ATTR_DEBUGb	0
#define	LCK_MTX_ATTR_STAT	0x2
#define	LCK_MTX_ATTR_STATb	1

#define LCK_MTX_EVENT(lck) ((event_t)(((unsigned int*)lck)+(sizeof(lck_mtx_t)-1)/sizeof(unsigned int)))

#else /* MACH_KERNEL_PRIVATE */
#ifdef	XNU_KERNEL_PRIVATE
#ifndef __DARLING__
typedef struct {
	unsigned long		opaque[2];
} lck_mtx_t;
#endif

typedef struct {
	unsigned long		opaque[10];
} lck_mtx_ext_t;
#else
#ifdef	KERNEL_PRIVATE
typedef struct {
	unsigned long		opaque[2];
} lck_mtx_t;

typedef struct {
	unsigned long		opaque[10];
} lck_mtx_ext_t;

#else
typedef struct __lck_mtx_t__		lck_mtx_t;
typedef struct __lck_mtx_ext_t__	lck_mtx_ext_t;
#endif
#endif
#endif

#ifdef	MACH_KERNEL_PRIVATE
#pragma pack(1)		/* Make sure the structure stays as we defined it */
typedef struct _lck_rw_t_internal_ {
	volatile uint16_t	lck_rw_shared_count;	/* No. of accepted readers */
	volatile uint8_t	lck_rw_interlock; 	/* Interlock byte */
	volatile uint8_t
				lck_rw_priv_excl:1,	/* Writers prioritized if set */
				lck_rw_want_upgrade:1,	/* Read-to-write upgrade waiting */
				lck_rw_want_write:1,	/* Writer waiting or locked for write */
				lck_r_waiting:1,	/* Reader is sleeping on lock */
				lck_w_waiting:1,	/* Writer is sleeping on lock */
				lck_rw_can_sleep:1,	/* Can attempts to lock go to sleep? */
				lck_rw_padb6:2; 		/* padding */

	uint32_t		lck_rw_tag; /* This can be obsoleted when stats
					     * are in
					     */
	uint32_t		lck_rw_pad8;
	uint32_t		lck_rw_pad12;
} lck_rw_t;
#pragma pack()

#define	LCK_RW_ATTR_DEBUG	0x1
#define	LCK_RW_ATTR_DEBUGb	0
#define	LCK_RW_ATTR_STAT	0x2
#define	LCK_RW_ATTR_STATb	1
#define LCK_RW_ATTR_READ_PRI	0x3
#define LCK_RW_ATTR_READ_PRIb	2
#define	LCK_RW_ATTR_DIS_THREAD	0x40000000
#define	LCK_RW_ATTR_DIS_THREADb	30
#define	LCK_RW_ATTR_DIS_MYLOCK	0x10000000
#define	LCK_RW_ATTR_DIS_MYLOCKb	28

#define	LCK_RW_TAG_DESTROYED		0x00002007	/* lock marked as Destroyed */

#if LOCK_PRIVATE

#define disable_preemption_for_thread(t) ((cpu_data_t GS_RELATIVE *)0UL)->cpu_preemption_level++

#define LCK_MTX_THREAD_TO_STATE(t)	((uintptr_t)t)
#define PLATFORM_LCK_ILOCK		0

#define LOCK_SNOOP_SPINS	1000
#define LOCK_PRETEST		1

/* Spinlock panic deadline, in mach_absolute_time units (ns on i386) */
#define LOCK_PANIC_TIMEOUT	0xf00000  /* 250 ms (huge) */

#endif	// LOCK_PRIVATE

#else
#ifdef	KERNEL_PRIVATE
#pragma pack(1)
typedef struct {
	uint32_t		opaque[3];
	uint32_t		opaque4;
} lck_rw_t;
#pragma pack()
#else
typedef struct __lck_rw_t__	lck_rw_t;
#endif
#endif

#ifdef MACH_KERNEL_PRIVATE

extern void		kernel_preempt_check (void);

#endif /* MACH_KERNEL_PRIVATE */

#endif	/* _I386_LOCKS_H_ */
