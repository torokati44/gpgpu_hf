#ifndef GPGPU_HF_CLBUFFER_H
#define GPGPU_HF_CLBUFFER_H

#include <CL/cl.h>
#include "clwrapper.hpp"

#include <cstdio>
#include <cstring>

template<typename T>
class CLBuffer {

    cl_mem mem;
    T *mapped = 0;
    size_t length;
    int mapcount = 0;

public:

    CLBuffer(size_t length) : length{length} {
        mem = clCreateBuffer(CLWrapper::instance->context(), CL_MEM_READ_WRITE,
                             length * sizeof(T), NULL, NULL);

        map();

        memset(mapped, 0, length * sizeof(T));

        unmap();
    }

    T *map() {
        ++mapcount;
        //std::cout << "mapping, count = " << mapcount << std::endl;

        if (!mapped) {
            cl_event ev;
            mapped = (T *) clEnqueueMapBuffer(CLWrapper::instance->cqueue(),
                                              mem, CL_TRUE, CL_MAP_READ | CL_MAP_WRITE, 0,
                                              length * sizeof(T),
                                              0, NULL, &ev, NULL);
            clWaitForEvents(1, &ev);
        } else {
            std::cerr << "MAPPING AGAIN!" << std::endl;
        }

        return mapped;
    }

    void unmap() {
        --mapcount;
        //std::cout << "unmapping, count = " << mapcount << std::endl;
        cl_event ev;
        if (mapped) {
            clEnqueueUnmapMemObject(CLWrapper::instance->cqueue(), mem, mapped, 0, NULL, &ev);
            clWaitForEvents(1, &ev);
        }
        mapped = 0;
    }

    operator cl_mem () {
        return mem;
    }

    ~CLBuffer() {
        //std::cout << "mapcount = " << mapcount << std::endl;
        if (mapped) {
            unmap();
        }

        clReleaseMemObject(mem);
    }
};


#endif //GPGPU_HF_CLBUFFER_H
