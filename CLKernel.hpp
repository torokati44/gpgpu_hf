//
// Created by attila on 2015.05.24..
//

#ifndef GPGPU_HF_CLKERNEL_H
#define GPGPU_HF_CLKERNEL_H

#include "clwrapper.hpp"

template<typename... paramTypes>
class CLKernel {

    cl_kernel kernel;

    template <typename First>
    void setParam(int i, First first) {
        clSetKernelArg(kernel, i, sizeof(First), &first);
    }

    template <typename First, typename... Tail>
    void setParam(int i, First first, Tail... tail) {
        clSetKernelArg(kernel, i, sizeof(First), &first);
        setParam(i + 1, tail...);
    }


public:

    CLKernel(const char *name) {
        kernel = CLWrapper::instance->createKernel(CLWrapper::instance->program(), name);
    }


    void execute(size_t size, paramTypes... params) {
        setParam(0, params...);
        clEnqueueNDRangeKernel(CLWrapper::instance->cqueue(),kernel, 1, NULL, &size, NULL, 0, NULL, NULL);
    }
};


#endif //GPGPU_HF_CLKERNEL_H
