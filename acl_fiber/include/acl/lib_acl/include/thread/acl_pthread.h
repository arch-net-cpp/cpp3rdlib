#ifndef	__ACL_PTHREAD_INCLUDE_H__
#define	__ACL_PTHREAD_INCLUDE_H__

#ifdef	__cplusplus
extern "C" {
#endif

#include "../stdlib/acl_define.h"

#define ACL_MUTEX_MAXWAIT	(~(unsigned int)0)
#ifdef	ACL_HAS_PTHREAD

#ifdef ACL_UNIX
# ifndef  _GNU_SOURCE
#  define _GNU_SOURCE
# endif
#endif
# include <pthread.h>

# if defined(ACL_FREEBSD) || defined(ACL_SUNOS5) || defined(ACL_MACOSX) || defined(ALPINE) || defined(MINGW)
#  define ACL_PTHREAD_MUTEX_RECURSIVE	PTHREAD_MUTEX_RECURSIVE
# else
#  define ACL_PTHREAD_MUTEX_RECURSIVE	PTHREAD_MUTEX_RECURSIVE_NP
# endif

typedef pthread_t acl_pthread_t;
typedef pthread_attr_t acl_pthread_attr_t;
typedef pthread_mutex_t acl_pthread_mutex_t;
typedef pthread_cond_t acl_pthread_cond_t;
typedef pthread_mutexattr_t acl_pthread_mutexattr_t;
typedef pthread_condattr_t acl_pthread_condattr_t;
typedef pthread_key_t acl_pthread_key_t;
typedef pthread_once_t acl_pthread_once_t;

#define acl_pthread_attr_init           pthread_attr_init
#define acl_pthread_attr_setstacksize   pthread_attr_setstacksize
#define acl_pthread_attr_setdetachstate pthread_attr_setdetachstate
#define acl_pthread_attr_destroy        pthread_attr_destroy
#define acl_pthread_self                pthread_self
#define acl_pthread_create              pthread_create
#define acl_pthread_detach              pthread_detach
#define acl_pthread_once                pthread_once
#define acl_pthread_join                pthread_join
#define	acl_pthread_mutexattr_init	pthread_mutexattr_init
#define	acl_pthread_mutexattr_destroy	pthread_mutexattr_destroy
#define	acl_pthread_mutexattr_settype	pthread_mutexattr_settype
#define acl_pthread_mutex_init          pthread_mutex_init
#define acl_pthread_mutex_destroy       pthread_mutex_destroy
#define acl_pthread_mutex_lock          pthread_mutex_lock
#define acl_pthread_mutex_unlock        pthread_mutex_unlock
#define acl_pthread_mutex_trylock       pthread_mutex_trylock
#define acl_pthread_cond_init           pthread_cond_init
#define acl_pthread_cond_destroy        pthread_cond_destroy
#define acl_pthread_cond_signal         pthread_cond_signal
#define acl_pthread_cond_broadcast      pthread_cond_broadcast
#define acl_pthread_cond_timedwait      pthread_cond_timedwait
#define acl_pthread_cond_wait           pthread_cond_wait
#define	acl_pthread_key_create          pthread_key_create
#define	acl_pthread_getspecific         pthread_getspecific
#define	acl_pthread_setspecific         pthread_setspecific

#define	ACL_PTHREAD_CREATE_DETACHED     PTHREAD_CREATE_DETACHED
#define	ACL_PTHREAD_CREATE_JOINABLE     PTHREAD_CREATE_JOINABLE
#define	ACL_TLS_OUT_OF_INDEXES          0xffffffff
#define	ACL_PTHREAD_KEYS_MAX            PTHREAD_KEYS_MAX
#ifdef	ACL_SUNOS5
# define ACL_PTHREAD_ONCE_INIT          { PTHREAD_ONCE_INIT }
#else
# define ACL_PTHREAD_ONCE_INIT           PTHREAD_ONCE_INIT
#endif

#else

#include <time.h>

#include "acl_sem.h"

#define	ACL_PTHREAD_CREATE_DETACHED     1
#define	ACL_PTHREAD_CREATE_JOINABLE     0
#define	ACL_TLS_OUT_OF_INDEXES          0xffffffff
#define	ACL_PTHREAD_KEYS_MAX            1024
#define ACL_PTHREAD_MUTEX_RECURSIVE		1

/*
 * see https://docs.microsoft.com/en-us/windows/win32/api/synchapi/nf-synchapi-initonceexecuteonce
 * for the supported windows OS's version, if not please comment out it.
 */
#ifdef _WIN32_WINNT
# if defined(_WIN32_WINNT_WS08)
#  if _WIN32_WINNT >= _WIN32_WINNT_WS08
#   define HAS_ONCE
#  endif
# elif defined(_WIN32_WINNT_VISTA)
#  if _WIN32_WINNT >= _WIN32_WINNT_VISTA
#   define HAS_ONCE
#  endif
# endif
#endif

#ifdef HAS_ONCE
#define ACL_PTHREAD_ONCE_INIT           INIT_ONCE_STATIC_INIT
#else
#define ACL_PTHREAD_ONCE_INIT           0
#endif

typedef struct acl_pthread_t acl_pthread_t;
typedef struct acl_pthread_attr_t acl_pthread_attr_t;
typedef struct acl_pthread_mutex_t acl_pthread_mutex_t;
typedef struct acl_pthread_cond_t acl_pthread_cond_t;
typedef struct acl_pthread_mutexattr_t acl_pthread_mutexattr_t;
typedef struct acl_pthread_condattr_t acl_pthread_condattr_t;
typedef int acl_pthread_key_t;

#ifdef HAS_ONCE
typedef INIT_ONCE acl_pthread_once_t;
#else
typedef int acl_pthread_once_t;
#endif

struct acl_pthread_t {
	unsigned long id;
	HANDLE handle;
	void *(*start_routine)(void *);
	void *routine_arg;
	char  detached;
};

struct acl_pthread_attr_t {
	SECURITY_ATTRIBUTES attr;
	size_t stacksize;
	char   detached;
};

struct acl_pthread_mutex_t {
	HANDLE id;
	char  dynamic;
};

struct acl_pthread_mutexattr_t {
	SECURITY_ATTRIBUTES attr;
};

struct acl_pthread_cond_t {
	acl_pthread_mutex_t *lock;
	int waiting;
	int nsignal;
	ACL_SEM *wait_sem;
	ACL_SEM *wait_done;
	char  dynamic;	
};

struct acl_pthread_condattr_t {
	char  unused_name[1];
};

# if _MSC_VER < 1900
struct timespec {
	time_t tv_sec;		/* Seconds. */
	long int tv_nsec;	/* Nanoseconds. */
};
#endif

/* in acl_pthread.c */
ACL_API void acl_pthread_end(void);
ACL_API int acl_pthread_once(acl_pthread_once_t *once_control,
		void (*init_routine)(void));
ACL_API int acl_pthread_key_create(acl_pthread_key_t *key_ptr,
		void (*destructor)(void*));
ACL_API void *acl_pthread_getspecific(acl_pthread_key_t key);
ACL_API int acl_pthread_setspecific(acl_pthread_key_t key, void *value);
ACL_API int acl_pthread_attr_init(acl_pthread_attr_t *attr);
ACL_API int acl_pthread_attr_setstacksize(acl_pthread_attr_t *attr,
		size_t stacksize);
ACL_API int acl_pthread_attr_setdetachstate(acl_pthread_attr_t *attr,
		int detached);
ACL_API int acl_pthread_attr_destroy(acl_pthread_attr_t *thr_attr);
ACL_API unsigned long acl_pthread_self(void);
ACL_API int  acl_pthread_create(acl_pthread_t *thread,
		acl_pthread_attr_t *attr, void * (*start_routine)(void *),
		void *arg);
ACL_API int acl_pthread_detach(acl_pthread_t thread);
ACL_API int acl_pthread_join(acl_pthread_t thread, void **thread_return);

/* in acl_pthread_mutex.c */
ACL_API int acl_pthread_mutexattr_init(acl_pthread_mutexattr_t *attr);
ACL_API int acl_pthread_mutexattr_destroy(acl_pthread_mutexattr_t *attr);
ACL_API int acl_pthread_mutexattr_settype(acl_pthread_mutexattr_t *attr, int type);
ACL_API int acl_pthread_mutex_init(acl_pthread_mutex_t *mutex,
		const acl_pthread_mutexattr_t *mattr);
ACL_API int acl_pthread_mutex_destroy(acl_pthread_mutex_t *mutex);
ACL_API int acl_pthread_mutex_lock(acl_pthread_mutex_t *mutex);
ACL_API int acl_pthread_mutex_unlock(acl_pthread_mutex_t *mutex);

#define	acl_pthread_mutex_trylock        acl_pthread_mutex_lock

/* in acl_pthread_cond.c */
ACL_API int acl_pthread_cond_init(acl_pthread_cond_t *cond,
		acl_pthread_condattr_t *cond_attr);
ACL_API acl_pthread_cond_t *acl_thread_cond_create(void);
#ifndef acl_pthread_cond_create
#define acl_pthread_cond_create acl_thread_cond_create
#endif

ACL_API int acl_pthread_cond_destroy(acl_pthread_cond_t *cond);
ACL_API int acl_pthread_cond_signal(acl_pthread_cond_t *cond);
ACL_API int acl_pthread_cond_broadcast(acl_pthread_cond_t *cond);
ACL_API int acl_pthread_cond_timedwait(acl_pthread_cond_t *cond,
		acl_pthread_mutex_t *mutex, const struct timespec *timeout);
ACL_API int acl_pthread_cond_wait(acl_pthread_cond_t *cond,
		acl_pthread_mutex_t *mutex);

#endif  /* !ACL_HAS_PTHREAD */

/* general functions */

/* in acl_pthread_mutex.c */
ACL_API int acl_thread_mutex_lock(acl_pthread_mutex_t *mutex);
ACL_API int acl_thread_mutex_unlock(acl_pthread_mutex_t *mutex);
ACL_API int acl_thread_mutex_nested(acl_pthread_mutex_t *mutex);

/* in acl_pthread.c */
ACL_API int acl_pthread_atexit_add(void *arg, void (*free_callback)(void*));
ACL_API int acl_pthread_atexit_remove(void *arg, void (*free_callback)(void*));

ACL_API int acl_pthread_tls_set_max(int max);
ACL_API int acl_pthread_tls_get_max(void);
ACL_API void *acl_pthread_tls_get(acl_pthread_key_t *key_ptr);
ACL_API int acl_pthread_tls_set(acl_pthread_key_t key, void *ptr,
		void (*free_fn)(void *));
ACL_API int acl_pthread_tls_del(acl_pthread_key_t key);
ACL_API void acl_pthread_tls_once_get(acl_pthread_once_t *control_once);
ACL_API void acl_pthread_tls_once_set(acl_pthread_once_t control_once);
ACL_API acl_pthread_key_t acl_pthread_tls_key_get(void);
ACL_API void acl_pthread_tls_key_set(acl_pthread_key_t key);

#ifdef	__cplusplus
}
#endif

#endif
