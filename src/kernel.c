#include <assert.h>

#include <string.h>
#include <stdint.h>
#include <limits.h>

#include "service.h"

#include "kernel.h"
#include "kernel_exec.h"
#include "kernel_signal.h"

#include "kernel_service.h"

#define USER_EVENTS_POOL               (50)
#define MAX_EVENTS                     (USER_EVENTS_POOL * 2)

#define MAX_QUEUE_EVENTS			   (10)
#define MAX_EVT_SUBSCRIBERS            (10)

enum {

	KERNEL_IDLE,
	KERNEL_SVC,
	KERNEL_EVT,
	KERNEL_PROC,
	KERNEL_PROC_EVT
};

struct evt_s {

	uint8_t evtId;
	evt_data_t evtData;
};

struct evt_sub_token_s {

	uint8_t evt_id;

	union {
		uint8_t a8;
		struct {
			uint8_t proc_sub : 1;
			uint8_t krnl_sub : 1;
		} flags;
	} attrib;

	evt_subscriber_t evt_sub;
};

struct execution_context_s {

	int ret_code;

	int argc;
	char **argv;

	sigset_t sigs;
	sigset_t sigMask;

	int (*callee)(const int argc, const char **argv);
};

typedef unsigned char __sigsem_t;
typedef __sigsem_t sigsem_t;

typedef struct execution_context_s process_t;

typedef struct evt_s evt_t;
typedef struct evt_sub_token_s evt_subscription_token_t;

static uint8_t evt_head                                 = 0;
static uint8_t evt_tail                                 = 0;

static uint8_t free_events                              = MAX_QUEUE_EVENTS;
static uint8_t free_evtSubs                             = MAX_EVT_SUBSCRIBERS;

static process_t proc;
static process_t *rt_proc                               = NULL;

static evt_t events[MAX_QUEUE_EVENTS];
static evt_subscription_token_t evt_tokens[MAX_EVT_SUBSCRIBERS];

static uint8_t kernel_pipeline                         = 0;

static sigset_t kernel_sigs                            = 0;
static sigset_t kernel_sigMask                         = 0;

static sigsem_t kernel_sigbussem					   = 0;
static sigsem_t kernel_sigintsem                       = 0;

static sigset_t* context_signals(void);
static sigset_t* context_sigMask(void);

static void sigblock(const sigset_t set);
static void sigunblock(const sigset_t set);

static void sigsem_inc(const sigset_t set);
static void sigsem_dec(const sigset_t set);

static inline sigsem_t sem_inc(sigsem_t sem);
static inline sigsem_t sem_dec(sigsem_t sem);

static void subscribe_evt(uint8_t evtId, evt_subscriber_t sub);

static void run_proc(process_t *p);
static void exit_proc(process_t *p);

static void exec_svcs(void);
static void exec_evts(void);

static void enque_evt(uint8_t evtId, evt_data_t* evtData);

extern service_t __svc_table[];
extern service_t __end_svc_table[];

void _kernel_pubEvt(uint8_t id, evt_data_t* data) {

	if ((rt_proc != NULL) &&
			(id < USER_EVENTS_POOL)) { 
				return; // process tries publish kernel events
	}

	enque_evt(id, data);
}

void _kernel_subEvt(uint8_t id, evt_subscriber_t subscriber) {

	if ((rt_proc != NULL) && (id < USER_EVENTS_POOL)) {
		return; // subscribing kernel events by the process is not allowed
	}

	subscribe_evt(id, subscriber);
}

void _kernel_pipeline() {

	exec_svcs(); // execute services (can publish events)
	exec_evts(); // execute events (subscribers)
}

// ------------------------------------------ kenel svc -----------------------------------------------------------

void const* _kernel_service(void const* svc, char const** svc_name) {
	
	if (svc_name == NULL) {
		return NULL;
	}

	service_t *prev_svc = (service_t *) svc;
	
	service_t *svc_table = (service_t *) &__svc_table;
	service_t *end_svc_table = (service_t *) &__end_svc_table;

	service_t *next_svc = 
		(prev_svc == NULL) ? svc_table : (++prev_svc);

	if ((next_svc == end_svc_table) || 
			((next_svc < svc_table) || (next_svc > end_svc_table))) {
		return NULL;
	}

	*svc_name = next_svc -> s_name;

	return next_svc;
}

// ------------------------------------------ kexec -------------------------------------------------------------

void _kernel_exec_f(
		int (*exec_callee_addr)(const int argc, const char **argv),
		const int argc, const char **argv) {

	if (rt_proc != NULL) {
		return; // Running more than one user process isn`t allowed;
	}

	proc.argc = (int) argc;
	proc.argv = (char **) argv;

	proc.callee = exec_callee_addr;

	rt_proc = &proc;

	run_proc(rt_proc);  /* execute calle program as process. would block here */
	exit_proc(rt_proc); /* calle returned. exit process execution context */

	rt_proc = NULL;     /* user process done */
}

// ------------------------------------------ signals -------------------------------------------------------------

int	_kernel_raise(int sgl) {

	int flag = sgl;

	flag--;
	if ((flag < 0) || 
			((size_t) flag > (sizeof(sigset_t) * CHAR_BIT))) {
		return -1;
	}

	sigset_t rising_set = (((sigset_t) 1) << flag);
	sigsem_inc(rising_set);

	return 0;
}

int _kernel_sigwait(const sigset_t *set, int *sgl) {

	static_assert(((sigset_t) - 1) > 0, "sigset_t must be unsigned");

	if (set == NULL) {
		return -1;
	}

	sigset_t sgls = 0;
	sigset_t wait_set = *set;

	sigset_t *ctx_flags = context_signals();
	sigset_t *ctx_flagMask = context_sigMask();

	if (!(wait_set & (*ctx_flagMask))) {
		return -1;
	}

	do {

		if (kernel_pipeline != KERNEL_SVC) {
			_kernel_pipeline();
		}

		sgls = ((*ctx_flags) & wait_set);
	} while(sgls == 0);

	int s;
	sigset_t i;

	for (i = 0, s = (sgls & 1); 
			(s == 0) && (i < (sizeof(sigset_t) * CHAR_BIT)); 
					i++, sgls >>= 1, s = (sgls & 1)) { }

	int awsgl = (i + 1);
	
	/* to unblock SIGINT and SIGBUS requires additional assertion 
	 * i.e. the source is unknown here and its nearly impossible 
	 * define a common iface for such assert thats easy to maintain 
	 * also its under awaiter responsibility to ack these signals */

	*sgl = awsgl;

	if ((awsgl == SIGINT) ||
			(awsgl == SIGBUS)) {				
		return 0;
	}

	sigunblock((((sigset_t) 1) << i));
	return 0;
}

int _kernel_sigpending(sigset_t *set) {

	static_assert(((sigset_t) - 1) > 0, "sigset_t must be unsigned");

	sigset_t pending_set = 0;
	sigset_t *ctx_flags = context_signals();

	if (kernel_pipeline != KERNEL_SVC) {
		_kernel_pipeline();
	}

	pending_set = *ctx_flags;

	*set = pending_set;
	return 0;
}

int _kernel_sigprocmask(int what, const sigset_t *set, sigset_t *oldset) {
	
	static_assert(((sigset_t) - 1) > 0, "sigset_t must be unsigned");

	sigset_t *ctx_flagMask = context_sigMask();

	if (oldset != NULL) {
		*oldset = *ctx_flagMask;
	}

	if (set == NULL) {
		return 0;
	}

	sigset_t sigset = *set;

	switch(what) {

		case SIG_UNBLOCK: {
			sigsem_dec(sigset);
		} break;

		case SIG_BLOCK: { sigblock(sigset); } break;
		case SIG_SETMASK: { *ctx_flagMask = sigset; } break;

		default:
			return -1;
	}

	return 0;
}

int _kernel_sigemptyset(sigset_t *set) {
	*set = 0;
	return 0;
}

int _kernel_sigaddset(sigset_t *set, const int sgl) {

	int flag = sgl;

	flag--;
	if ((flag < 0) ||
			((size_t) flag > sizeof(sigset_t) * CHAR_BIT)) {
		return -1;
	}

	*set |= (((sigset_t) 1) << flag);

	return 0;
}

bool _kernel_sigismember(sigset_t *set, const int sgl) {

	int flag = sgl;

	flag--;
	if ((set == NULL) || (flag < 0) || 
			((size_t) flag > (sizeof(sigset_t) * CHAR_BIT))) {
		return false;
	}

	return ((*set) & ((((sigset_t) 1) << flag)));
}

// -------------------------------------------- internal private --------------------------------------------------

static inline sigsem_t sem_inc(sigsem_t sem) {

	sigsem_t next_sem = sem;
	if ((++next_sem) != 0) {
		return next_sem;
	}

	return sem;
}

static inline sigsem_t sem_dec(sigsem_t sem) {

	sigsem_t next_sem = sem;
	if ((--next_sem) != 0xFF) {
		return next_sem;
	}

	return sem;
}

static sigset_t* context_signals(void) {
	return (rt_proc != NULL) ?
			&(rt_proc -> sigs) : &kernel_sigs;
}

static sigset_t* context_sigMask(void) {
	return (rt_proc != NULL) ?
		&(rt_proc -> sigMask) : &kernel_sigMask;
}

static void sigblock(const sigset_t set) {

	sigset_t *ctx_flagMask = context_sigMask();
	*ctx_flagMask |= set;
}

static void sigunblock(const sigset_t set) {

	sigset_t *ctx_flags = context_signals();
	sigset_t *ctx_flagMask = context_sigMask();

	sigset_t clrmask = ~(set);

	*ctx_flags &= clrmask;
	*ctx_flagMask &= clrmask;
}

static void sigsem_inc(const sigset_t set) {

	sigset_t sigint_mask = (((sigset_t) 1) << (SIGINT - 1));
	sigset_t sigbus_mask = (((sigset_t) 1) << (SIGBUS - 1));

	if (set & sigint_mask) {
		kernel_sigintsem = sem_inc(kernel_sigintsem);
	}

	if (set & sigbus_mask) {
		kernel_sigbussem = sem_inc(kernel_sigbussem);
	}

	sigset_t *ctx_flags = context_signals();
	*ctx_flags |= (set);
}

static void sigsem_dec(const sigset_t set) {

	sigset_t unblock_set = set;

	sigset_t sigint_mask = (((sigset_t) 1) << (SIGINT - 1));
	sigset_t sigbus_mask = (((sigset_t) 1) << (SIGBUS - 1));

	if (set & sigint_mask) {
		kernel_sigintsem = sem_dec(kernel_sigintsem);
	}

	if (set & sigbus_mask) {
		kernel_sigbussem = sem_dec(kernel_sigbussem);
	}
	
	if (kernel_sigintsem > 0) {
		unblock_set &= (~(sigint_mask));
	}

	if (kernel_sigbussem > 0) {
		unblock_set &= (~(sigbus_mask));
	}

	sigunblock(unblock_set);
}

static __attribute__((noinline)) void exec_svcs(void) {

	kernel_pipeline = KERNEL_SVC;

	for (service_t *service = (service_t *) &__svc_table; 
			service != (service_t *) &__end_svc_table; service++) {
		service -> svc_main();
	}
}

static __attribute__((noinline)) void exec_evts(void) {

	kernel_pipeline = 
		(rt_proc != NULL) ? KERNEL_PROC_EVT: KERNEL_EVT;

	if (free_events == MAX_QUEUE_EVENTS) {
		return;
	}

	do {

		// pull next event

		evt_t *recv_evt =
				&events[evt_head % MAX_QUEUE_EVENTS];

		evt_head++;
		free_events++;

		// call subscribers

		size_t i;
		evt_subscription_token_t *token;

		uint8_t recv_evtId = recv_evt -> evtId;

		for (i = 0, token = evt_tokens; 
				i < MAX_EVT_SUBSCRIBERS; i++, token++) {

			if ((token -> evt_sub != NULL) && 
					(token -> evt_id == recv_evtId)) {
						(token -> evt_sub)(&(recv_evt -> evtData));				
			}
		}

	} while(free_events < MAX_QUEUE_EVENTS);
}

static __attribute__((noinline)) void subscribe_evt(uint8_t evtId, evt_subscriber_t sub) {

	if (free_evtSubs == 0) {
		return;
	}

	size_t i;
	evt_subscription_token_t *token;

	for (i = (evtId % MAX_EVT_SUBSCRIBERS), 
			token = &evt_tokens[i]; token -> evt_sub != NULL; 
				i++, i %= MAX_EVT_SUBSCRIBERS, token = &evt_tokens[i]) { }	

	bool proc_sub = (rt_proc != NULL);

	token -> evt_id = evtId;
	token -> evt_sub = sub;

	token -> attrib.flags.proc_sub = proc_sub;
	token -> attrib.flags.krnl_sub = !proc_sub;

	free_evtSubs--;
}

static __attribute__((noinline)) void enque_evt(uint8_t evtId, evt_data_t* evtData) {

	if (free_events == 0) { 
		return;
	}

	evt_t *evt =
			&events[evt_tail % MAX_QUEUE_EVENTS];

	evt -> evtId = evtId;

	if (evtData != NULL) {
		memcpy(&evt -> evtData, evtData, sizeof(evt_data_t));
	}

	evt_tail++;
	free_events--;
}

static __attribute__((noinline)) void exit_proc(process_t *p) {

	if (p == NULL) {
		return;
	}

	// nullify process subscriptions

	size_t i = 0;
	evt_subscription_token_t *token;
	
	for (i = 0, token = evt_tokens; 
			i < MAX_EVT_SUBSCRIBERS; i++, token++) {

		if (token -> attrib.flags.proc_sub) {

			token -> attrib.a8 = 0;
			token -> evt_sub = NULL;
		}
	}
}

static __attribute__((noinline)) void run_proc(process_t *p) {

	int argc = p -> argc;
	char **argv = p -> argv;

	int (*callee_addr)(const int argc,
			const char **argv) = p -> callee;

	kernel_pipeline = KERNEL_PROC;
	p -> ret_code = callee_addr(argc, (const char **) argv);
}
