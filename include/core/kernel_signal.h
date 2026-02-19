#pragma once

#include <stdbool.h>

/* -------------------------------------------------------------------------- */
/* Signal Mask Operations                                                     */
/* -------------------------------------------------------------------------- */

/**
 * @ingroup kernel_core
 * @brief Replace current signal mask.
 *
 * Used with _kernel_sigprocmask() to set the calling context's
 * signal mask to the provided set.
 */
#define SIG_SETMASK  0

/**
 * @ingroup kernel_core
 * @brief Add signals to current mask (block).
 *
 * Used with _kernel_sigprocmask() to block specified signals.
 */
#define SIG_BLOCK    1

/**
 * @ingroup kernel_core
 * @brief Remove signals from current mask (unblock).
 *
 * Used with _kernel_sigprocmask() to unblock specified signals.
 */
#define SIG_UNBLOCK  2

/* -------------------------------------------------------------------------- */
/* Signal Definitions                                                         */
/* -------------------------------------------------------------------------- */

/**
 * @ingroup kernel_core
 * @brief Signal numeric identifiers.
 *
 * @details
 * Defines supported kernel signal identifiers. Numeric
 * values are implementation-defined but fixed for ABI compatibility.
 */

#define SIGHUP    1   /**< Hangup. */
#define SIGINT    2   /**< Interrupt. */
#define SIGQUIT   3   /**< Quit. */
#define SIGILL    4   /**< Illegal instruction. */
#define SIGTRAP   5   /**< Trace trap. */
#define SIGIOT    6   /**< IOT instruction. */
#define SIGABRT   6   /**< Abort signal (alias for SIGIOT). */
#define SIGEMT    7   /**< EMT instruction. */
#define SIGFPE    8   /**< Floating point exception. */
#define SIGKILL   9   /**< Non-catchable kill signal. */
#define SIGBUS    10  /**< Bus error. */
#define SIGSEGV   11  /**< Segmentation violation. */
#define SIGSYS    12  /**< Bad system call argument. */
#define SIGPIPE   13  /**< Write to pipe with no reader. */
#define SIGALRM   14  /**< Alarm signal. */
#define SIGTERM   15  /**< Termination request. */
#define SIGURG    16  /**< Urgent I/O condition. */
#define SIGSTOP   17  /**< Non-catchable stop signal. */
#define SIGTSTP   18  /**< Terminal stop signal. */
#define SIGCONT   19  /**< Continue execution. */
#define SIGCHLD   20  /**< Child process status change. */
#define SIGCLD    20  /**< System V alias for SIGCHLD. */
#define SIGTTIN   21  /**< Background read from TTY. */
#define SIGTTOU   22  /**< Background write to TTY. */
#define SIGIO     23  /**< I/O possible. */
#define SIGPOLL   SIGIO /**< Alias for SIGIO. */
#define SIGXCPU   24  /**< CPU time limit exceeded. */
#define SIGXFSZ   25  /**< File size limit exceeded. */
#define SIGVTALRM 26  /**< Virtual timer expired. */
#define SIGPROF   27  /**< Profiling timer expired. */
#define SIGWINCH  28  /**< Window size change. */
#define SIGLOST   29  /**< Resource lost. */
#define SIGUSR1   30  /**< User-defined signal 1. */
#define SIGUSR2   31  /**< User-defined signal 2. */
#define _NSIG     32  /**< Total number of supported signals. */

/* -------------------------------------------------------------------------- */
/* Signal Set Types                                                           */
/* -------------------------------------------------------------------------- */

typedef unsigned long __sigset_t;

/**
 * @ingroup kernel_core
 * @brief Signal set type.
 *
 * @details
 * Represents a bitmask of signals.
 * Each bit corresponds to one signal number.
 */
typedef __sigset_t sigset_t;

/* -------------------------------------------------------------------------- */
/* Signal API                                                                 */
/* -------------------------------------------------------------------------- */

/**
 * @ingroup kernel_core
 * @brief Sends a signal to the current execution context.
 *
 * @param[in] sgl Signal number.
 *
 * @return
 * 0 on success, negative value on error.
 *
 * @pre
 * - sgl must be a valid signal number (< _NSIG).
 */
int _kernel_raise(int sgl);

/**
 * @ingroup kernel_core
 * @brief Retrieves pending signals.
 *
 * @param[out] set Signal set receiving pending signals.
 *
 * @return
 * 0 on success, negative value on error.
 *
 * @pre
 * - set must not be NULL.
 */
int _kernel_sigpending(sigset_t *set);

/**
 * @ingroup kernel_core
 * @brief Waits synchronously for a signal.
 *
 * @param[in]  set Signal set to wait for.
 * @param[out] sgl Pointer receiving delivered signal number.
 *
 * @return
 * 0 on success, negative value on error.
 *
 * @pre
 * - set and sgl must not be NULL.
 *
 * @warning
 * Might block calling context 
 * if no signals from the set are raised yet 
 * at the moment of calling this function
 */
int _kernel_sigwait(const sigset_t *set, int *sgl);

/**
 * @ingroup kernel_core
 * @brief Examines or modifies signal mask.
 *
 * @param[in]  what   Operation type (SIG_SETMASK, SIG_BLOCK, SIG_UNBLOCK).
 * @param[in]  set    Signal set to apply.
 * @param[out] oldset Previous signal mask (may be NULL to retrieve old mask).
 *
 * @return
 * 0 on success, negative value on error.
 */
int _kernel_sigprocmask(int what,
                        const sigset_t *set,
                        sigset_t *oldset);

/**
 * @ingroup kernel_core
 * @brief Initializes a signal set to empty.
 *
 * @param[out] set Signal set to initialize.
 *
 * @return
 * 0 on success, negative value on error.
 */
int _kernel_sigemptyset(sigset_t *set);

/**
 * @ingroup kernel_core
 * @brief Adds a signal to a set.
 *
 * @param[in,out] set Signal set.
 * @param[in]     sgl Signal number to add.
 *
 * @return
 * 0 on success, negative value on error.
 */
int _kernel_sigaddset(sigset_t *set, const int sgl);

/**
 * @ingroup kernel_core
 * @brief Tests whether a signal is a member of a set.
 *
 * @param[in] set Signal set.
 * @param[in] sgl Signal number.
 *
 * @retval true  Signal is a member.
 * @retval false Signal is not a member.
 */
bool _kernel_sigismember(sigset_t *set, const int sgl);
