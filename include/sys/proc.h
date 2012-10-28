/*	$OpenBSD: proc.h,v 1.149 2012/01/07 05:38:12 guenther Exp $	*/
/*	$NetBSD: proc.h,v 1.44 1996/04/22 01:23:21 christos Exp $	*/

/*-
 * Copyright (c) 1986, 1989, 1991, 1993
 *	The Regents of the University of California.  All rights reserved.
 * (c) UNIX System Laboratories, Inc.
 * All or some portions of this file are derived from material licensed
 * to the University of California by American Telephone and Telegraph
 * Co. or Unix System Laboratories, Inc. and are reproduced herein with
 * the permission of UNIX System Laboratories, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)proc.h	8.8 (Berkeley) 1/21/94
 */

#ifndef _SYS_PROC_H_
#define	_SYS_PROC_H_

#include <machine/proc.h>		/* Machine-dependent proc substruct. */
#include <sys/selinfo.h>		/* For struct selinfo */
#include <sys/queue.h>
#include <sys/timeout.h>		/* For struct timeout */
#include <sys/event.h>			/* For struct klist */
#include <sys/mutex.h>			/* For struct mutex */
#include <machine/cpu.h>

#ifdef _KERNEL
#define __need_process
#endif

/*
 * One structure allocated per session.
 */
struct process;
struct	session {
	int	s_count;		/* Ref cnt; pgrps in session. 引用pgrps会话计数*/
	struct	process *s_leader;	/* Session leader. 会话主机*/
	struct	vnode *s_ttyvp;		/* Vnode of controlling terminal.对控制终端的vnode */
	struct	tty *s_ttyp;		/* Controlling terminal. 控制终端*/
	char	s_login[MAXLOGNAME];	/* Setlogin() name. */
};

/*
 * One structure allocated per process group.每个进程组分配一个结构。
 */
struct	pgrp {
	LIST_ENTRY(pgrp) pg_hash;	/* Hash chain. 哈希链*/
	LIST_HEAD(, process) pg_members;/* Pointer to pgrp members./ pgrp成员指针。 */
	struct	session *pg_session;	/* Pointer to session. 指针到会话*/
	pid_t	pg_id;			/* Pgrp id. */
	int	pg_jobc;	/* # procs qualifying pgrp for job control 通过触发条件完成作业控制*/
};

/*
 * One structure allocated per emulation.
 */
struct exec_package;
struct proc;
struct ps_strings;
struct uvm_object;
union sigval;

struct	emul {
	char	e_name[8];		/* Symbolic name */
	int	*e_errno;		/* Errno array */
					/* Signal sending function */
	void	(*e_sendsig)(sig_t, int, int, u_long, int, union sigval);
	int	e_nosys;		/* Offset of the nosys() syscall */
	int	e_nsysent;		/* Number of system call entries */
	struct sysent *e_sysent;	/* System call array */
	char	**e_syscallnames;	/* System call name array */
	int	e_arglen;		/* Extra argument size in words */
					/* Copy arguments on the stack */
	void	*(*e_copyargs)(struct exec_package *, struct ps_strings *,
				    void *, void *);
					/* Set registers before execution */
	void	(*e_setregs)(struct proc *, struct exec_package *,
				  u_long, register_t *);
	int	(*e_fixup)(struct proc *, struct exec_package *);
	int	(*e_coredump)(struct proc *, void *cookie);
	char	*e_sigcode;		/* Start of sigcode */
	char	*e_esigcode;		/* End of sigcode */
	int	e_flags;		/* Flags, see below */
	struct uvm_object *e_sigobject;	/* shared sigcode object */
					/* Per-process hooks */
	void	(*e_proc_exec)(struct proc *, struct exec_package *);
	void	(*e_proc_fork)(struct proc *p, struct proc *parent);
	void	(*e_proc_exit)(struct proc *);
};
/* Flags for e_flags */
#define	EMUL_ENABLED	0x0001		/* Allow exec to continue */
#define	EMUL_NATIVE	0x0002		/* Always enabled */

extern struct emul *emulsw[];		/* All emuls in system */
extern int nemuls;			/* Number of emuls */

/*
 * Description of a process.
 *
 * These structures contain the information needed to manage a thread of
 * control, known in UN*X as a process; it has references to substructures
 * containing descriptions of things that the process uses, but may share
 * with related processes.
 *
 * struct process is the higher level process containing information
 * shared by all threads in a process, while struct proc contains the
 * run-time information needed by threads.
 */
#ifdef __need_process
struct process {
	/*
	 * ps_mainproc is the main thread in the process.
	 * Ultimately, we shouldn't need that, threads should be able to exit
	 * at will. Unfortunately until the pid is moved into struct process
	 * we'll have to remember the main threads and abuse its pid as the
	 * the pid of the process. This is gross, but considering the horrible
	 * pid semantics we have right now, it's unavoidable.
	 */
	struct	proc *ps_mainproc;
	struct	pcred *ps_cred;		/* Process owner's identity. */

	TAILQ_HEAD(,proc) ps_threads;	/* Threads in this process. */

	LIST_ENTRY(process) ps_pglist;	/* List of processes in pgrp. */
	struct	process *ps_pptr; 	/* Pointer to parent process. */
	LIST_ENTRY(process) ps_sibling;	/* List of sibling processes. */
	LIST_HEAD(, process) ps_children;/* Pointer to list of children. */

/* The following fields are all zeroed upon creation in process_new. */
#define	ps_startzero	ps_klist
	struct	klist ps_klist;		/* knotes attached to this process */
	int	ps_flags;		/* PS_* flags. */

	struct	proc *ps_single;	/* Single threading to this thread. */
	int	ps_singlecount;		/* Not yet suspended threads. */

	int	ps_traceflag;		/* Kernel trace points. */
	struct	vnode *ps_tracevp;	/* Trace to vnode. */
	struct	ucred *ps_tracecred;	/* Creds for writing trace */

/* End area that is zeroed on creation. */
#define	ps_endzero	ps_startcopy

/* The following fields are all copied upon creation in process_new. */
#define	ps_startcopy	ps_limit

	struct	plimit *ps_limit;	/* Process limits. */
	struct	pgrp *ps_pgrp;		/* Pointer to process group. */
	u_int	ps_rtableid;		/* Process routing table/domain. */
	char	ps_nice;		/* Process "nice" value. */

/* End area that is copied on creation. */
#define ps_endcopy	ps_refcnt

	int	ps_refcnt;		/* Number of references. */
};

#define	ps_pid		ps_mainproc->p_pid
#define	ps_session	ps_pgrp->pg_session
#define	ps_pgid		ps_pgrp->pg_id

#endif /* __need_process */

/*
 * These flags are kept in ps_flags, but they used to be in proc's p_flag
 * and were exported to userspace via the KERN_PROC sysctl.  We'll retain
 * compat by using non-overlapping bits for PS_* and P_* flags and just
 * OR them together for export.
 */
#define	PS_CONTROLT	_P_CONTROLT
#define	PS_PPWAIT	_P_PPWAIT
#define	PS_PROFIL	_P_PROFIL
#define	PS_SUGID	_P_SUGID
#define	PS_SYSTEM	_P_SYSTEM
#define	PS_TRACED	_P_TRACED
#define	PS_WAITED	_P_WAITED
#define	PS_EXEC		_P_EXEC
#define	PS_ISPWAIT	_P_ISPWAIT
#define	PS_SUGIDEXEC	_P_SUGIDEXEC
#define	PS_NOZOMBIE	_P_NOZOMBIE
#define	PS_INEXEC	_P_INEXEC
#define	PS_SYSTRACE	_P_SYSTRACE
#define	PS_CONTINUED	_P_CONTINUED
#define	PS_STOPPED	_P_STOPPED
#define	PS_SINGLEEXIT	_P_SINGLEEXIT
#define	PS_SINGLEUNWIND	_P_SINGLEUNWIND

struct proc {
	TAILQ_ENTRY(proc) p_runq;
	LIST_ENTRY(proc) p_list;	/* List of all processes.所有进程的列表。 */

	struct	process *p_p;		/* The process of this thread.该线程的进程。 */
	TAILQ_ENTRY(proc) p_thr_link;/* Threads in a process linkage.在一个过程联动的线程。 */

	/* substructures子结构: */
	struct	filedesc *p_fd;		/* Ptr to open files structure.指向打开文件的结构。 */
	struct	pstats *p_stats;	/* Accounting/statistics 会计/统计*/
	struct	vmspace *p_vmspace;	/* Address space. 地址空间。*/
	struct	sigacts *p_sigacts;	/* Signal actions, state 信号动作，状态*/
#define	p_cred		p_p->ps_cred
#define	p_ucred		p_cred->pc_ucred
#define	p_rlimit	p_p->ps_limit->pl_rlimit

	int	p_exitsig;		/* Signal to send to parent on exit. 信号发送至父退出时。*/
	int	p_flag;			/* P_* flags. */
	u_char	p_os;			/* OS tag */
	char	p_stat;			/* S* process status. 进程的状态。*/
	char	p_pad1[1];
	u_char	p_descfd;		/* if not 255, fdesc permits this fd 如果不是255，fdesc允许这个FD*/

	pid_t	p_pid;			/* Process identifier. 进程标识符*/
	LIST_ENTRY(proc) p_hash;	/* Hash chain. */

/* The following fields are all zeroed upon creation in fork. 以下字段都在建立叉后归零。*/
#define	p_startzero	p_oppid

	pid_t	p_oppid;	 /* Save parent pid during ptrace. XXX ptrace的保存期间父进程的PID。XXX*/
	int	p_dupfd;	 /* Sideways return value from filedescopen. XXX 返回值来自filedescopen。XXX*/

	long 	p_thrslpid;	/* for thrsleep syscall / sleep系统调用*/
	int	p_sigwait;	/* signal handled by sigwait() 信号处理经过sigwait()*/

#ifndef	__HAVE_MD_TCB
	void	*p_tcb;		/* user-space thread-control-block address 用户空间线程控制块地址*/
# define TCB_SET(p, addr)	((p)->p_tcb = (addr))
# define TCB_GET(p)		((p)->p_tcb)
#endif

	/* scheduling 调度*/
	u_int	p_estcpu;	 /* Time averaged value of p_cpticks. /	p_cpticks平均时间值。*/
	int	p_cpticks;	 /* Ticks of cpu time. CPU的时间刻度。*/
	fixpt_t	p_pctcpu;	 /* %cpu for this process during p_swtime %CPU属于p_swtime这个过程中*/
	const volatile void *p_wchan;/* Sleep address. 休眠的地址。*/
	struct	timeout p_sleep_to;/* timeout for tsleep() 调用tsleep()超时*/
	const char *p_wmesg;	 /* Reason for sleep. 休眠的原因*/
	u_int	p_swtime;	 /* Time swapped in or out. 时间交换或退出*/
	u_int	p_slptime;	 /* Time since last blocked.  自最后阻塞时间 */
	struct	cpu_info * __volatile p_cpu; /* CPU we're running on./ CPU运行信息。*/

	struct	itimerval p_realtimer;	/* Alarm timer. 警报定时器。*/
	struct	timeout p_realit_to;	/* Alarm timeout. 警报超时。*/
	struct	timeval p_rtime;	/* Real time.真正的时间。 */
	u_quad_t p_uticks;		/* Statclock hits in user mode.在用户模式下Statclock采样数。 */
	u_quad_t p_sticks;		/* Statclock hits in system mode. */
	u_quad_t p_iticks;		/* Statclock hits processing intr. */

	void	*p_systrace;		/* Back pointer to systrace 返回指向到systrace*/

	int	p_ptmask;		/* Ptrace event mask / ptrace的事件掩码*/
	struct	ptrace_state *p_ptstat;	/* Ptrace state */

	int	p_siglist;		/* Signals arrived but not delivered. 到达的信号，但未传递。*/

	struct	vnode *p_textvp;	/* Vnode of executable. / vnode的可执行。*/

	void	*p_emuldata;		/* Per-process emulation data, or 每个进程的仿真数据,或*/
					/* NULL. Malloc type M_EMULDATA / NULL. Malloc类型M_EMULDATA*/

	sigset_t p_sigdivert;		/* Signals to be diverted to thread.信号被转移到线程。 */

/* End area that is zeroed on creation. 端区，创造零。*/
#define	p_endzero	p_startcopy

/* The following fields are all copied upon creation in fork. 以下字段都抄在FORK后的创建。*/
#define	p_startcopy	p_sigmask

	sigset_t p_sigmask;	/* Current signal mask. 当前的信号掩码。*/

	u_char	p_priority;	/* Process priority. 进程的优先级。*/
	u_char	p_usrpri;	/* User-priority based on p_cpu and ps_nice. / p_cpu和ps_nice基于用户优先上。*/
	char	p_comm[MAXCOMLEN+1];

	struct	emul *p_emul;		/* Emulation information 仿真的信息*/
	struct	sigaltstack p_sigstk;	/* sp & on stack state variable / SP：堆栈状态变量*/
	vaddr_t	p_sigcode;		/* user pointer to the signal code.用户的信号代码的指向 */

/* End area that is copied on creation. 端区，创建复制。*/
#define	p_endcopy	p_addr

	sigset_t p_oldmask;	/* Saved mask from before sigpause 保存掩码前sigpause*/
	union sigval p_sigval;	/* For core dump/debugger XXX 核心转储/调试XXX*/
	int	p_sisig;	/* For core dump/debugger XXX 对于核心转储/调试器XXX*/
	int	p_sicode;	/* For core dump/debugger XXX 对于核心转储/调试器XXX*/
	long	p_sitrapno;	/* For core dump/debugger XXX 对于核心转储/调试器XXX*/

	struct	user *p_addr;	/* Kernel virtual addr of u-area 用户区的内核虚拟地址*/
	struct	mdproc p_md;	/* Any machine-dependent fields. 任何机器依赖的字段。*/

	u_short	p_xstat;	/* Exit status for wait; also stop signal. 退出等待状态;也停止信号。*/
	u_short	p_acflag;	/* Accounting flags. 会计标志。*/
	struct	rusage *p_ru;	/* Exit information. XXX 退出信息。XXX*/
};

/* Status values. */
#define	SIDL	1		/* Process being created by fork. */
#define	SRUN	2		/* Currently runnable. */
#define	SSLEEP	3		/* Sleeping on an address. */
#define	SSTOP	4		/* Process debugging or suspension. */
#define	SZOMB	5		/* Awaiting collection by parent. */
#define SDEAD	6		/* Process is almost a zombie. */
#define	SONPROC	7		/* Process is currently on a CPU. */

#define P_ZOMBIE(p)	((p)->p_stat == SZOMB || (p)->p_stat == SDEAD)

/*
 * These flags are kept in p_flag, except those with a leading underbar,
 * which are in process's ps_flags
 */
#define	P_INKTR		0x000001	/* In a ktrace op, don't recurse */
#define	_P_CONTROLT	0x000002	/* Has a controlling terminal. */
#define	P_INMEM		0x000004	/* Loaded into memory. UNUSED */
#define	P_SIGSUSPEND	0x000008	/* Need to restore before-suspend mask*/
#define	_P_PPWAIT	0x000010	/* Parent waits for exec/exit. */
#define	P_PROFIL	0x000020	/* Has started profiling. */
#define	P_SELECT	0x000040	/* Selecting; wakeup/waiting danger. */
#define	P_SINTR		0x000080	/* Sleep is interruptible. */
#define	_P_SUGID	0x000100	/* Had set id privs since last exec. */
#define	P_SYSTEM	0x000200	/* No sigs, stats or swapping. */
#define	P_TIMEOUT	0x000400	/* Timing out during sleep. */
#define	P_TRACED	0x000800	/* Debugged process being traced. */
#define	P_WAITED	0x001000	/* Debugging proc has waited for child. */
/* XXX - Should be merged with INEXEC */
#define	P_WEXIT		0x002000	/* Working on exiting. */
#define	_P_EXEC		0x004000	/* Process called exec. */

/* Should be moved to machine-dependent areas. */
#define	P_OWEUPC	0x008000	/* Owe proc an addupc() at next ast. */
#define	_P_ISPWAIT	0x010000	/* Is parent of PPWAIT child. */

/* XXX Not sure what to do with these, yet. */
#define	P_SSTEP		0x020000	/* proc needs single-step fixup ??? */
#define	_P_SUGIDEXEC	0x040000	/* last execve() was set[ug]id */

#define	P_SUSPSINGLE	0x080000	/* Need to stop for single threading. */
#define	P_NOZOMBIE	0x100000	/* Pid 1 waits for me instead of dad */
#define P_INEXEC	0x200000	/* Process is doing an exec right now */
#define P_SYSTRACE	0x400000	/* Process system call tracing active*/
#define P_CONTINUED	0x800000	/* Proc has continued from a stopped state. */
#define	_P_SINGLEEXIT	0x1000000	/* Other threads must die. */
#define	_P_SINGLEUNWIND	0x2000000	/* Other threads must unwind. */
#define	P_THREAD	0x4000000	/* Only a thread, not a real process */
#define	P_SUSPSIG	0x8000000	/* Stopped from signal. */
#define	P_SOFTDEP	0x10000000	/* Stuck processing softdep worklist */
#define P_STOPPED	0x20000000	/* Just stopped, need sig to parent. */
#define P_CPUPEG	0x40000000	/* Do not move to another cpu. */

#ifndef _KERNEL
#define	P_CONTROLT	_P_CONTROLT
#define	P_PPWAIT	_P_PPWAIT
#define	P_SUGID		_P_SUGID
#define	P_EXEC		_P_EXEC
#define	P_SUGIDEXEC	_P_SUGIDEXEC
#endif

#define	P_BITS \
    ("\20\02CONTROLT\03INMEM\04SIGPAUSE\05PPWAIT\06PROFIL\07SELECT" \
     "\010SINTR\011SUGID\012SYSTEM\013TIMEOUT\014TRACED\015WAITED\016WEXIT" \
     "\017EXEC\020PWEUPC\021ISPWAIT\022SSTEP\023SUGIDEXEC\024SUSPSINGLE" \
     "\025NOZOMBIE\026INEXEC\027SYSTRACE\030CONTINUED" \
     "\031SINGLEEXIT\032SINGLEUNWIND" \
     "\033THREAD\034SUSPSIG\035SOFTDEP\036STOPPED\037CPUPEG")

/* Macro to compute the exit signal to be delivered. */
#define P_EXITSIG(p) \
    (((p)->p_flag & P_TRACED) ? SIGCHLD : (p)->p_exitsig)

#define	THREAD_PID_OFFSET	1000000

/*
 * MOVE TO ucred.h?
 *
 * Shareable process credentials (always resident).  This includes a reference
 * to the current user credentials as well as real and saved ids that may be
 * used to change ids.
 */
struct	pcred {
	struct	ucred *pc_ucred;	/* Current credentials. */
	uid_t	p_ruid;			/* Real user id. */
	uid_t	p_svuid;		/* Saved effective user id. */
	gid_t	p_rgid;			/* Real group id. */
	gid_t	p_svgid;		/* Saved effective group id. */
};

#ifdef _KERNEL

struct uidinfo {
	LIST_ENTRY(uidinfo) ui_hash;
	uid_t   ui_uid;
	long    ui_proccnt;	/* proc structs */
	long	ui_lockcnt;	/* lockf structs */
};

struct uidinfo *uid_find(uid_t);

/*
 * We use process IDs <= PID_MAX; PID_MAX + 1 must also fit in a pid_t,
 * as it is used to represent "no process group".
 * We set PID_MAX to (SHRT_MAX - 1) so we don't break sys/compat.
 */
#define	PID_MAX		32766
#define	NO_PID		(PID_MAX+1)

#define SESS_LEADER(pr)	((pr)->ps_session->s_leader == (pr))
#define	SESSHOLD(s)	((s)->s_count++)
#define	SESSRELE(s) do {						\
	if (--(s)->s_count == 0)					\
		pool_put(&session_pool, (s));				\
} while (/* CONSTCOND */ 0)

/*
 * Flags to fork1().
 */
#define FORK_FORK	0x00000001
#define FORK_VFORK	0x00000002
#define FORK_RFORK	0x00000004
#define FORK_PPWAIT	0x00000008
#define FORK_SHAREFILES	0x00000010
#define FORK_CLEANFILES	0x00000020
#define FORK_NOZOMBIE	0x00000040
#define FORK_SHAREVM	0x00000080
#define FORK_TFORK	0x00000100
#define FORK_SIGHAND	0x00000200
#define FORK_PTRACE	0x00000400
#define FORK_THREAD	0x00000800

#define EXIT_NORMAL	0x00000001
#define EXIT_THREAD	0x00000002

#define	PIDHASH(pid)	(&pidhashtbl[(pid) & pidhash])
extern LIST_HEAD(pidhashhead, proc) *pidhashtbl;
extern u_long pidhash;

#define	PGRPHASH(pgid)	(&pgrphashtbl[(pgid) & pgrphash])
extern LIST_HEAD(pgrphashhead, pgrp) *pgrphashtbl;
extern u_long pgrphash;

extern struct proc proc0;		/* Process slot for swapper. */
extern int nprocs, maxproc;		/* Current and max number of procs. */
extern int randompid;			/* fork() should create random pid's */

LIST_HEAD(proclist, proc);
extern struct proclist allproc;		/* List of all processes. */
extern struct proclist zombproc;	/* List of zombie processes. */

extern struct proc *initproc;		/* Process slots for init, pager. */
extern struct proc *syncerproc;		/* filesystem syncer daemon */

extern struct pool process_pool;	/* memory pool for processes */
extern struct pool proc_pool;		/* memory pool for procs */
extern struct pool rusage_pool;		/* memory pool for zombies */
extern struct pool ucred_pool;		/* memory pool for ucreds */
extern struct pool session_pool;	/* memory pool for sessions */
extern struct pool pgrp_pool;		/* memory pool for pgrps */
extern struct pool pcred_pool;		/* memory pool for pcreds */

struct simplelock;

struct process *prfind(pid_t);	/* Find process by id. */
struct proc *pfind(pid_t);	/* Find thread by id. */
struct pgrp *pgfind(pid_t);	/* Find process group by id. */
void	proc_printit(struct proc *p, const char *modif,
    int (*pr)(const char *, ...));

int	chgproccnt(uid_t uid, int diff);
int	enterpgrp(struct process *, pid_t, struct pgrp *, struct session *);
void	fixjobc(struct process *, struct pgrp *, int);
int	inferior(struct process *, struct process *);
void	leavepgrp(struct process *);
void	preempt(struct proc *);
void	pgdelete(struct pgrp *);
void	procinit(void);
void	resetpriority(struct proc *);
void	setrunnable(struct proc *);
void	endtsleep(void *);
void	unsleep(struct proc *);
void	reaper(void);
void	exit1(struct proc *, int, int);
void	exit2(struct proc *);
void	cpu_exit(struct proc *);
int	fork1(struct proc *, int, int, void *, pid_t *, void (*)(void *),
	    void *, register_t *, struct proc **);
int	groupmember(gid_t, struct ucred *);

enum single_thread_mode {
	SINGLE_SUSPEND,		/* other threads to stop wherever they are 其他线程停止,无论他们在哪里*/
	SINGLE_UNWIND,		/* other threads to unwind and stop 其他线程展开和停止*/
	SINGLE_EXIT		/* other threads to unwind and then exit 其他线程去展开,然后退出*/
};
int	single_thread_set(struct proc *, enum single_thread_mode, int);
void	single_thread_clear(struct proc *);
int	single_thread_check(struct proc *, int);

void	child_return(void *);

int	proc_cansugid(struct proc *);
void	proc_finish_wait(struct proc *, struct proc *);
void	proc_zap(struct proc *);

struct sleep_state {
	int sls_s;
	int sls_catch;
	int sls_do_sleep;
	int sls_sig;
};

#if defined(MULTIPROCESSOR)
void	proc_trampoline_mp(void);	/* XXX */
#endif

/*
 * functions to handle sets of cpus.CPU的集合处理函数。
 *
 * For now we keep the cpus in ints so that we can use the generic
 * atomic ops.
 * 现在我们保持在整型的CPU，所以我们可以使用的通用OPS原型。
 */
#define CPUSET_ASIZE(x) (((x) - 1)/32 + 1)
#define CPUSET_SSIZE CPUSET_ASIZE(MAXCPUS)
struct cpuset {
	int cs_set[CPUSET_SSIZE];
};

void cpuset_init_cpu(struct cpu_info *);

void cpuset_clear(struct cpuset *);
void cpuset_add(struct cpuset *, struct cpu_info *);
void cpuset_del(struct cpuset *, struct cpu_info *);
int cpuset_isset(struct cpuset *, struct cpu_info *);
void cpuset_add_all(struct cpuset *);
void cpuset_copy(struct cpuset *, struct cpuset *);
void cpuset_union(struct cpuset *, struct cpuset *, struct cpuset *);
void cpuset_intersection(struct cpuset *t, struct cpuset *, struct cpuset *);
void cpuset_complement(struct cpuset *, struct cpuset *, struct cpuset *);
struct cpu_info *cpuset_first(struct cpuset *);

#endif	/* _KERNEL */
#endif	/* !_SYS_PROC_H_ */

