#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdatomic.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <time.h>

#include "cl_runtime/cl_runtime.h"
#include "cl_runtime/cl_service_seg_256.h"

static uint64_t now_ns(void){ struct timespec ts; clock_gettime(CLOCK_MONOTONIC,&ts); return (uint64_t)ts.tv_sec*1000000000ull+(uint64_t)ts.tv_nsec; }

static int map_core_rw(void **out_base, size_t *out_sz){
    int fd=shm_open(CL_CORE_SHM_NAME,O_RDWR,0); if(fd<0) return -1;
    struct stat st; if(fstat(fd,&st)!=0){close(fd);return -2;} if(st.st_size<=0){close(fd);return -3;}
    void *p=mmap(NULL,(size_t)st.st_size,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0); close(fd);
    if(p==MAP_FAILED) return -4; *out_base=p; *out_sz=(size_t)st.st_size; return 0;
}

static cl_service_seg_256_t* find_service_seg(void *core_base){
    cl_root_t *r=(cl_root_t*)core_base;
    cl_toc_t *toc=(cl_toc_t*)((uint8_t*)core_base+(size_t)r->toc_offset);
    for(uint32_t i=0;i<toc->header.entry_count && i<CL_TOC_MAX_ENTRIES;i++){
        cl_toc_entry_t *e=&toc->entries[i];
        if((uint16_t)e->type==(uint16_t)CL_SERVICE_SEG_256) return (cl_service_seg_256_t*)((uint8_t*)core_base+(size_t)e->offset_bytes);
    }
    return NULL;
}

static cl_service_slot_32_t* slot_for(cl_service_seg_256_t *svc, cl_service_id_t id){
    switch(id){
        case CL_SVC_CORE0: return &svc->g1.s0;
        case CL_SVC_HAL0: return &svc->g1.s1;
        case CL_SVC_LINK0: return &svc->g2.s2;
        case CL_SVC_FLOW0: return &svc->g2.s3;
        case CL_SVC_ORACLE0: return &svc->g3.s4;
        case CL_SVC_MONITOR: return &svc->g3.s5;
        default: return NULL;
    }
}

int main(void){
    void *core=NULL; size_t core_sz=0;
    int rc=map_core_rw(&core,&core_sz);
    if(rc!=0){ printf("oracle0: cannot map core shm: rc=%d errno=%d (%s)\n",rc,errno,strerror(errno)); return 2; }
    cl_service_seg_256_t *svc=find_service_seg(core);
    if(!svc){ printf("oracle0: service segment not found\n"); return 3; }
    cl_service_slot_32_t *sl=slot_for(svc,CL_SVC_ORACLE0);
    if(!sl) return 4;

    for(;;){
        atomic_store(&sl->last_heartbeat_ns, now_ns());
        struct timespec ts={0}; ts.tv_nsec=250000000L; nanosleep(&ts,NULL);
    }
}

