#ifndef CF_THREADPOOL_H
#define CF_THREADPOOL_H

#ifndef CF_THREADPOOL_GLOBAL_COUNT
#define CF_THREADPOOL_GLOBAL_COUNT  10
#endif

#ifdef __cplusplus
extern "C"{
#endif

int cf_threadpool_run(void (*run)(void*),void* d);

#ifdef __cplusplus
}
#endif
#endif//CF_THREADPOOL_H