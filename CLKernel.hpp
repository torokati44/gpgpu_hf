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

    std::string name;
public:

    CLKernel(const char *name) : name{name} {
        kernel = CLWrapper::instance->createKernel(CLWrapper::instance->program(), name);
    }


    void execute(size_t size, paramTypes... params) {
        //std::cout << "executing " << name << std::endl;
        cl_event ev;
        setParam(0, params...);
        int result = clEnqueueNDRangeKernel(CLWrapper::instance->cqueue(), kernel, 1, NULL, &size, NULL, 0, NULL, &ev);

        if(result != CL_SUCCESS)
            std::cerr << CLWrapper::getErrorString(result) << std::endl;
        else {
            clWaitForEvents(1, &ev);
        }
    }

    ~CLKernel() {
        clReleaseKernel(kernel);
    }
};


#endif //GPGPU_HF_CLKERNEL_H
