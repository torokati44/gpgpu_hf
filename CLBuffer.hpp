#ifndef GPGPU_HF_CLBUFFER_H
#define GPGPU_HF_CLBUFFER_H

#include <CL/cl.h>
#include "clwrapper.hpp"

template<typename T>
class CLBuffer {

    cl_mem mem;
    T *mapped = 0;
    size_t length;

public:

    CLBuffer(size_t length) : length{length} {
        mem = clCreateBuffer(CLWrapper::instance->context(), CL_MEM_READ_WRITE,
                             length * sizeof(T), NULL, NULL);
    }

    T *map() {
        return mapped ?
                mapped :
                mapped = (T *) clEnqueueMapBuffer(CLWrapper::instance->cqueue(),
                                                  mem, CL_TRUE, CL_MAP_READ | CL_MAP_WRITE, 0,
                                                  length * sizeof(T),
                                                  0, NULL, NULL, NULL);
    }

    void unmap() {
        clEnqueueUnmapMemObject(CLWrapper::instance->cqueue(), mem, mapped, 0, NULL, NULL);
        mapped = 0;
    }

    operator cl_mem () {
        return mem;
    }

    ~CLBuffer() {
        if (mapped) {
            unmap();
        }
    }
};


#endif //GPGPU_HF_CLBUFFER_H
