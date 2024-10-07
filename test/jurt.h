/*
  MIT License

  Copyright (c) 2023 JulianDroid

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/

#ifndef _JURT_H_
#define _JURT_H_

#include "stdint.h"
#include "stdarg.h"
#include "stdlib.h"
#include "string.h"
#include "sys/time.h"
#include "stdio.h"

#ifdef _WIN32
	#define JLOS_WINDOWS
#else
	#define JLOS_LINUX
#endif

#ifdef JLOS_WINDOWS
	#include "windows.h"
#else
	#include "pthread.h"
	#include "errno.h"
	#include "signal.h"
#endif

/* ================ Precompile Helper ================ */

#ifdef JLOS_WINDOWS
	#define JL_IMPORTF_PREV(chunkName,suff) binary_##chunkName##suff
#else
	#define JL_IMPORTF_PREV(chunkName,suff) _binary_##chunkName##suff
#endif

#define JL_IMPORTF_PREV_START(n) JL_IMPORTF_PREV(n,_start)
#define JL_IMPORTF_PREV_END(n) JL_IMPORTF_PREV(n,_end)
#define JL_IMPORTF_PREV_SIZE(n) (&JL_IMPORTF_PREV_END(n)-&JL_IMPORTF_PREV_START(n))


#define JL_IMPORTF_AUTOV(name) \
	extern char JL_IMPORTF_PREV_START(name);\
	extern char JL_IMPORTF_PREV_END(name);

/* [================ Precompile Helper ================] */

/*
	nanosleep port
	Usage:
		jl_nanosleep(nanoseconds)
*/
void jl_nanosleep(int64_t);

/*
	get nanosecond (timestamp of a day)
	Usage:
		jl_getnanotimestamp()
*/
int64_t jl_getnanotimestamp(void);

/*
	sleep in milliseconds
	Usage:
		jl_microsleep(milliseconds)
*/
void jl_millisleep(int64_t);

/*
	get millisecond of a day
	Usage:
		jl_getmillitimestamp()
*/
int64_t jl_getmillitimestamp(void);

/* ================ Universal Utils ================ */




/* ================ JuRt Logger Format ================ */

/*
	Static strings
*/
#define INF_STARTSTR "\x1b[33;40m"
#define ERR_STARTSTR "\x1b[30;43m"
#define INFERR_ENDSTR "\x1b[0;0m\n"

/*
	Show log
	Usage:
		jl_inf(msg, ...)
*/
void jl_inf(const char*, ...);

/*
	Show an error
	Usage:
		jl_err(msg, ...)
*/
void jl_err(const char*, ...);

/* [================ JuRt Logger Format ================] */


/* ================ Assert ================ */
/* to check if returned value is valid */

/*
	callback function type to deal with error
	Args:
		vl: vl@jl_assert?
*/
typedef void (*JL_ERRFUNC)(int64_t);

/*
	check if bigger than 0
	Usage:
		assertp(vl, errmsg, callbackFunc, terminate)
	Returns:
		vl@assertp
*/
int64_t jl_assertp(int64_t, char*, JL_ERRFUNC, int);

/*
	check if equals 0
	Usage:
		assert0(vl, errmsg, callbackFunc, terminate)
	Returns:
		vl@assert0
*/
int64_t jl_assert0(int64_t, char*, JL_ERRFUNC, int);

/*
	check if bigger or equals 0
	Usage:
		assert0p(vl, errmsg, callbackFunc, terminate)
	Returns:
		vl@assert0
*/
int64_t jl_assert0p(int64_t, char*, JL_ERRFUNC, int);

/* [================ Assert ================] */



/* ================ Dynamic Array Helper Library (Stack Only) ================ */

/* chunk size of array, also minimum size of a array  */
#define JLDA_CHUNK_SIZE 16

/* dynamic array structure, used to manage an array */
typedef struct {
	void* array;
	size_t len;
	size_t item_size;
	size_t chunk_size;
	size_t alloc_size;
} jlda_t;

typedef jlda_t* jlda_ptr;

/*
	check if a dynamic array is invalid (creation failure)
	Usage:
		jlda_is_invalid(da)
	Returns:
		1 if invalid, otherwise 0
*/
int jlda_is_invalid(jlda_ptr);

/*
	create a dynamic array
	Usage:
		jlda_create(size_per_item)
	Returns:
		jlda_ptr, jlda_ptr->array=NULL if error
*/
jlda_ptr jlda_create(int);

/*
	get the length of a dynamic array
	Usage:
		jlda_length(da)
	Returns:
		the length of the da
*/
size_t jlda_length(jlda_ptr);

/*
	get the raw data of a dynamic array
	Usage:
		jlda_rawdata(da)
	Returns:
		the pointer to the start of the array
*/
void* jlda_rawdata(jlda_ptr);

/*
	get an item from a dynamic array
	Usage:
		jlda_get(da, index)
	Returns:
		address of item, NULL if an error occured, no need to free()
*/
void* jlda_get(jlda_ptr, int);

/*
	get an int from DynamicArray
	TODO: returns another value instead of -1
	Usage:
		jlda_get_int(da, index)
	Returns:
		value of item and -1 if an error occured
*/
int jlda_getint(jlda_ptr, int);

/*
	push a duplicated value at the end of the array
	Usage:
		jlda_push(da, data)
*/
void jlda_push(jlda_ptr, void*);

/*
	++++++++ need to be freed ++++++++
	pop from an array
	Usage:
		jlda_pop(da)
	Returns:
		value of the last item, or NULL if none left
*/
void* jlda_pop(jlda_ptr);

/*
	pop an integer from an array
	TODO: returns another value instead of -1
	Usage:
		jlda_pop_int(da)
	Returns:
		value of the last item and -1 if an error occured
*/
int jlda_popint(jlda_ptr);

/*
	free the entire array
	Usage:
		DA_free(da)
*/
void jlda_free(jlda_ptr);

/* [================ Dynamic Array Helper Library ================] */



/* ================ PThread Helper Library ================ */

#ifdef JURT_ENABLE_THREAD
/* callback function as init */
typedef void* (*JL_THREAD_INNER_FUNC)(void*);

/* thread max count */
#define JL_THREAD_MAX_COUNT 256

/* thread lock type */
#ifdef JLOS_WINDOWS
# define JL_MUTEX_T HANDLE
#else
# define JL_MUTEX_T pthread_mutex_t
#endif

/*
	get pthread_t by id:index
	Usage:
		jl_getthreadbyid(id)
	Returns:
		typed *; otherwise 0(NULL)
*/
#ifdef JLOS_WINDOWS
	HANDLE* jl_getthreadbyid(int);
#else
	pthread_t* jl_getthreadbyid(int);
#endif

/*
	create a thread with pthread
	Usage:
		jl_createthread(thread_function, args, do_not_detach)
	Returns:
		id, bigger than 0, or other values as errors
*/
int jl_createthread(JL_THREAD_INNER_FUNC, void*, int);

/*
	wait for an undetached thread to complete
	Usage:
		jl_waitthread(id)
	Returns:
		value that thread returns, or NULL when no return or error
*/
void* jl_waitthread(int);

/* no macros for now.. */
/*
	create a mutex
	Usage:
		jl_mutex_create()
	Returns:
		mutex object
*/
JL_MUTEX_T jl_mutex_create();

/*
	lock
	Usage:
		jl_mutex_lock(mutex)
*/
void jl_mutex_lock(JL_MUTEX_T*);

/*
	unlock
	Usage:
		jl_mutex_unlock(mutex)
*/
void jl_mutex_unlock(JL_MUTEX_T*);

/*
	destroy a mutex
	Usage:
		jl_mutex_destroy(mutex)
*/
void jl_mutex_destroy(JL_MUTEX_T*);

/*
	destroy a thread
	Usage:
		jl_killthread(id)
	Returns:
		0 as success, other as error
*/
int jl_killthread(int);
#endif // JURT_ENABLE_THREAD

/* [================ PThread Helper Library ================] */



#ifdef JURT_IMPLEMENTATION

/* ================ Universal Utils ================ */
void jl_nanosleep(int64_t nanoseconds){
	if(nanoseconds<0) return;
	#ifdef JLOS_WINDOWS
		/* Declarations */
		HANDLE timer;	/* Timer handle */
		LARGE_INTEGER li;	/* Time defintion */
		/* Create timer */
		if(!(timer = CreateWaitableTimer(NULL, TRUE, NULL)))
			return;
		/* Set timer properties */
		li.QuadPart = -ns;
		if(!SetWaitableTimer(timer, &li, 0, NULL, NULL, FALSE)){
			CloseHandle(timer);
			return;
		}
		/* Start & wait for timer */
		WaitForSingleObject(timer, INFINITE);
		/* Clean resources */
		CloseHandle(timer);
		/* Slept without problems */
		// return TRUE;
	#else
		// tv_sec, tv_nsec
		struct timespec ns = {
			.tv_sec = nanoseconds/(long)1e9,
			.tv_nsec = nanoseconds%(long)1e9
		};
		nanosleep(&ns, NULL);
	#endif
}

int64_t jl_getnanotimestamp(void){
	struct timeval tv;
	if(gettimeofday(&tv, NULL) < 0) return 0;
	return (int64_t)tv.tv_sec * (int64_t)1000000000L + (int64_t)tv.tv_usec * (int64_t)1000L;
}

void jl_millisleep(int64_t milliseconds){
	if(milliseconds<0) return;
	jl_nanosleep(milliseconds*(int64_t)1000000);
}

int64_t jl_getmillitimestamp(void){
	struct timeval tv;
	if(gettimeofday(&tv, NULL) < 0) return 0;
	return tv.tv_sec*(int64_t)1000 + tv.tv_usec/1000000;
}

/* ================ JuRt Logger Format ================ */
void jl_inf(const char* format, ...){
	return;
	va_list va;
	int len = strlen(format);
	char* newStr = (char*) malloc((len+sizeof(INF_STARTSTR)+sizeof(INFERR_ENDSTR))*sizeof(char));
	sprintf(newStr, INF_STARTSTR "%s" INFERR_ENDSTR, format);
	va_start(va, format);
	vprintf(newStr, va);
	va_end(va);
	free(newStr);
}

void jl_err(const char* format, ...){
	va_list va;
	int len = strlen(format);
	char* newStr = (char*) malloc((len+sizeof(ERR_STARTSTR)+sizeof(INFERR_ENDSTR))*sizeof(char));
	sprintf(newStr, ERR_STARTSTR "%s" INFERR_ENDSTR, format);
	va_start(va, format);
	vprintf(newStr, va);
	va_end(va);
	free(newStr);
}



/* ================ Assert ================ */

int64_t jl_assertp(int64_t fd, char* msg, JL_ERRFUNC cb, int terminate){
	if(fd > 0) return fd;
	jl_inf("assertp: %s", msg);
	if(cb) cb(fd);
	if(terminate){
		jl_err("program terminated");
		exit(fd|1);
	}
	return fd;
}

int64_t jl_assert0(int64_t fd, char* msg, JL_ERRFUNC cb, int terminate){
	if(fd == 0) return fd;
	jl_inf("assert0: %s", msg);
	if(cb) cb(fd);
	if(terminate){
		jl_err("program terminated");
		exit(fd|1);
	}
	return fd;
}

int64_t jl_assert0p(int64_t fd, char* msg, JL_ERRFUNC cb, int terminate){
	if(fd >= 0) return fd;
	jl_inf("assert0: %s", msg);
	if(cb) cb(fd);
	if(terminate){
		jl_err("program terminated");
		exit(fd|1);
	}
	return fd;
}



/* ================ Dynamic Array Helper Library ================ */

int jlda_is_invalid(jlda_ptr da){
	return da->array == NULL;
}

jlda_ptr jlda_create(int size_per_item){
	int act_chunksiz = size_per_item * JLDA_CHUNK_SIZE;
	void* array = malloc(act_chunksiz);
	if(!array){
		jl_err("cannot alloc mem for array.");
		return NULL;
	}
	jlda_t da = {
		array,
		0,
		size_per_item,
		act_chunksiz,
		act_chunksiz
	};
	jlda_ptr daptr = malloc(sizeof(jlda_t));
	memcpy(daptr, &da, sizeof(jlda_t));
	return daptr;
}

size_t jlda_length(jlda_ptr da){
	return da->len;
}

void* jlda_rawdata(jlda_ptr da){
	return da->array;
}

void* jlda_get(jlda_ptr da, int index){
	if(!da->array){
		jl_err("DA_get: array is null.");
		return NULL;
	}
	if(index >= da->len){
		jl_err("index %d out of range.", index);
		return NULL;
	}
	if(index < 0){
		jl_err("index < 0.");
		return NULL;
	}
	return da->array+index*da->item_size;
}

int jlda_getint(jlda_ptr da, int index){
	void* data = jlda_get(da, index);
	if(!data){
		jl_err("DA_get got an error.");
		return -1;
	}
	int integer = *(int*)data;
	// free(data);
	return integer;
}

// TODO check bug
void jlda_push(jlda_ptr da, void* item){
	// jl_inf("push0, len=%d, chunk_size=%d, itemsiz=%d, alloc=%d", da->len, da->chunk_size, da->item_size, da->alloc_size);
	if((da->len+1)*da->item_size > da->alloc_size){
		// jl_inf("DA_push: %d, %d", da->alloc_size, da->chunk_size);
		void* new_da = realloc(da->array, da->alloc_size += da->chunk_size);
		if(!new_da){
			jl_err("cannot alloc memory for pushing");
			return;
		}
		da->array = new_da;
	}
	memcpy(da->array+da->len*da->item_size, item, da->item_size);
	++da->len;
}

void* jlda_pop(jlda_ptr da){
	if(da->len == 0) return NULL;
	void* data = malloc(da->item_size);
	memcpy(data, da->array+(da->len-1)*da->item_size, da->item_size);
	--da->len;
	if(da->len%JLDA_CHUNK_SIZE == JLDA_CHUNK_SIZE-1){
		// free
		void* new_arr = realloc(da->array, da->alloc_size -= da->chunk_size);
		if(!new_arr){
			jl_err("cannot realloc for DA_pop");
		}else da->array = new_arr;
	}
	return data;
}

int jlda_popint(jlda_ptr da){
	void* data = jlda_pop(da);
	if(!data){
		jl_err("DA_pop got an error.");
		return -1;
	}

	int integer = *(int*)(data);
	free(data);
	return integer;
}

void jlda_free(jlda_ptr da){
	if(!da) return;
	if(da->array){
		free(da->array);
		da->array = NULL;
	}
	free(da);
}


/* ================ PThread Helper Library ================ */

#ifdef JURT_ENABLE_THREAD
/*
	++++++++ NON_EXPORTED ++++++++
	to store thread ids
	[0] stores nothing
*/
#ifdef JLOS_WINDOWS
	HANDLE jl_i_thread_ids[JL_THREAD_MAX_COUNT];
#else
	pthread_t jl_i_thread_ids[JL_THREAD_MAX_COUNT];
#endif

/*
	++++++++ NON_EXPORTED ++++++++
	to identify using threads
	[0] stores the count of using threads
*/
int jl_i_thread_ids_using[JL_THREAD_MAX_COUNT];

/*
	++++++++ NON_EXPORTED ++++++++
	clean up a thread
	Usage:
		thread_cleanup(id)
	Returns:
		0 as success, other as error
*/
int jl_thread_cleanup(int);

/*
	++++++++ NON_EXPORTED ++++++++
	thread wrapper arg
*/
typedef struct {
	int id;
	JL_THREAD_INNER_FUNC func;
	void* args;
} jl_thread_wrapper_arg;

/*
	++++++++ NON_EXPORTED ++++++++
	thread wrapper
	Usage:
		_JL_THREAD_WRAPPER(inner_func, args);
*/
#ifdef JLOS_WINDOWS
	DWORD WINAPI _JL_THREAD_WRAPPER(LPVOID);
#else
	void* _JL_THREAD_WRAPPER(void*);
#endif

#ifdef JLOS_WINDOWS
	HANDLE* jl_getthreadbyid(int id){
#else
	pthread_t* jl_getthreadbyid(int id){
#endif
	if(jl_i_thread_ids_using[id]) return &jl_i_thread_ids[id];
	return NULL;
}

int jl_thread_cleanup(int id){
	#ifdef JLOS_WINDOWS
		jl_err("impl");
	#else
		pthread_t* thp = NULL;
		if(!(thp=(pthread_t*)jl_assertp(
			(long)jl_getthreadbyid(id),
			"cleanup: cannot find thread",
			NULL, 0
		))) return 1;

		pthread_t thread = *thp;

		int ret = pthread_kill(thread, 0);
		if(ret && ret != ESRCH){
			jl_err("thread %d is still alive, cannot cleanup", id);
			return 2;
		}
	#endif

	jl_i_thread_ids_using[id] = 0;
	--jl_i_thread_ids_using[0];

	jl_inf("thread %d cleaned up", id);
	return 0;
}

#ifdef JLOS_WINDOWS
DWORD WINAPI _JL_THREAD_WRAPPER(LPVOID args){
#else
void* _JL_THREAD_WRAPPER(void* args){
#endif
	jl_thread_wrapper_arg arg = *(jl_thread_wrapper_arg*)args;
	free(args);
	jl_inf("thread %d start", arg.id);
	void* ret = arg.func(arg.args);
	jl_inf("thread %d end", arg.id);
	// TODO on thread end ?
	jl_thread_cleanup(arg.id);
	return ret?ret:"normal exit";
}


int jl_createthread(JL_THREAD_INNER_FUNC func, void* args, int nodetach){
	if(jl_i_thread_ids_using[0]>=JL_THREAD_MAX_COUNT-1){
		jl_err("thread pool is full");
		return 0;
	}

	#ifdef JLOS_WINDOWS
		HANDLE thread;
	#else
		pthread_t thread;
	#endif

	// find a free pool
	int id = 0;
	for(int i=1; i<JL_THREAD_MAX_COUNT; ++i){
		if(!jl_i_thread_ids_using[i]){
			id = i;
			break;
		}
	}
	if(!id){
		jl_err("cannot find a free thread pool");
		return 0;
	}

	// prepare args
	jl_thread_wrapper_arg* arg = (jl_thread_wrapper_arg*) malloc(sizeof(jl_thread_wrapper_arg));
	arg->args = args;
	arg->func = func;
	arg->id = id;


	#ifdef JLOS_WINDOWS
		thread = CreateThread(NULL, 0, _JL_THREAD_WRAPPER, arg, 0, 0);
	#else
		pthread_attr_t attr;
		pthread_attr_init(&attr);
		if(!nodetach) pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
		pthread_create(&thread, &attr, _JL_THREAD_WRAPPER, arg);
		pthread_attr_destroy(&attr);
	#endif


	// put
	jl_i_thread_ids[id] = thread;
	jl_i_thread_ids_using[id] = 1;
	++jl_i_thread_ids_using[0];

	jl_inf("thread %d created", id);

	return id;
}

void* jl_waitthread(int id){
	void* res = NULL;
	#ifdef JLOS_WINDOWS
		HANDLE* threadp = jl_getthreadbyid(id);
		HANDLE thread;
	#else
		pthread_t* threadp = jl_getthreadbyid(id);
		pthread_t thread;
	#endif

	if(!threadp){
		jl_err("jl_waitthread: cannot find thread %d", id);
		return NULL;
	}
	thread = *threadp;

	#ifdef JLOS_WINDOWS
		jl_inf("waitThread: Win32 does not support getting return value from threads");
		DWORD stat = WaitForSingleObject(thread, INFINITE);
		if(stat != WAIT_OBJECT_0){
			jl_err("cannot wait for thread %d, error code=%d", id, stat);
		}
	#else
		int stat = pthread_join(thread, &res);
		if(stat){
			jl_err("cannot wait for thread %d, error code=%d", id, stat);
		}
	#endif
	return res;
}

JL_MUTEX_T jl_mutex_create(){
	JL_MUTEX_T mutex;
	#ifdef JLOS_WINDOWS
		mutex = CreateMutex(NULL, 0, NULL);
	#else
		pthread_mutex_init(&mutex, NULL);
	#endif
	return mutex;
}

void jl_mutex_lock(JL_MUTEX_T* mutex){
	#ifdef JLOS_WINDOWS
		WaitForSingleObject(*mutex, INFINITE);
	#else
		pthread_mutex_lock(mutex);
	#endif
}

void jl_mutex_unlock(JL_MUTEX_T* mutex){
	#ifdef JLOS_WINDOWS
		ReleaseMutex(*mutex);
	#else
		pthread_mutex_unlock(mutex);
	#endif
}

void jl_mutex_destroy(JL_MUTEX_T* mutex){
	#ifdef JLOS_WINDOWS
		CloseHandle(*mutex);
	#else
		pthread_mutex_destroy(mutex);
	#endif
}

int jl_killthread(int id){
	#ifdef JLOS_WINDOWS
		jl_err("impl");
		return -1;
	#else
		pthread_t* threadp = jl_getthreadbyid(id);
		if(!threadp){
			jl_err("killThread: cannot find thread %d", id);
			return 1;
		}

		pthread_t thread = *threadp;

		if(jl_assert0(
			(long)pthread_cancel(thread),
			"cannot kill thread",
			NULL, 0
		)){
			jl_inf("thread %d may be killed", id);
			return 0;
		}
	#endif
	jl_thread_cleanup(id);
	return 2;
}
#endif // JURT_ENABLE_THREAD

#endif // JURT_IMPLEMENTATION

#endif // _JURT_H_
