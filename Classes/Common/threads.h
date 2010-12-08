#ifndef CROSSPLATFORM_THREADS
#define CROSSPLATFORM_THREADS

#ifdef WIN32

#include <process.h>
#define thread_type uintptr_t
#define thread HANDLE
#define MUTEX CRITICAL_SECTION
#define thread_create(desc,func,arg,flag,id) desc = _beginthreadex(NULL,0,func,arg,flag,id)
#define thread_mutex_init(m) InitializeCriticalSection(&m)
#define thread_mutex_destroy(m) DeleteCriticalSection(&m)
#define thread_mutex_lock(m) EnterCriticalSection(&m)
#define thread_mutex_unlock(m) LeaveCriticalSection(&m)

#define thread_join(t) WaitForSingleObject (t, INFINITE)
#define thread_exit() _endthread()
#define thread_cancel(t) TerminateThread(t,0)
#define thread_testcancel()
#define thread_cleanup_push(f,p)
#define thread_cleanup_pop(e)
#define thread_self() GetCurrentThreadId()
#define THREAD
#define YIELD Sleep(0)

#else

#include <pthread.h>
#define thread_type pthread_t
#define thread_mutex_init(m) pthread_mutex_init(m, NULL)
#define MUTEX pthread_mutex_t
#define thread_mutex_lock(m) pthread_mutex_lock(m)
#define thread_mutex_trylock(m) pthread_mutex_trylock(m)
#define thread_mutex_unlock(m) pthread_mutex_unlock(m)
#define thread_create(desc,func,arg,flag,id) \
pthread_attr_t attr; \
pthread_attr_init (&attr); \
pthread_attr_setdetachstate (&attr, PTHREAD_CREATE_DETACHED); \
pthread_create (desc, &attr, func, arg)
#define thread pthread_tx
#define thread_join(t) pthread_join(t, NULL)
#define THREAD pthread_setcanceltype (PTHREAD_CANCEL_DEFERRED, NULL);\
pthread_setcancelstate (PTHREAD_CANCEL_DISABLE, NULL)
#define thread_exit() pthread_exit (NULL)
#define thread_mutex_destroy(m) pthread_mutex_destroy(m)
#define thread_cancel(t) pthread_cancel(t)
#define thread_testcancel() \
pthread_setcancelstate (PTHREAD_CANCEL_ENABLE, NULL); \
pthread_testcancel (); \
pthread_setcancelstate (PTHREAD_CANCEL_DISABLE, NULL)
#define thread_cleanup_push(f,p) pthread_cleanup_push(f,p)
#define thread_cleanup_pop(e) pthread_cleanup_pop(e)
#define thread_self() pthread_self()

//#ifdef WIN32
//#define YIELD Sleep (0);
//#else
//#define YIELD pthread_yield()
//#endif

#endif

#endif