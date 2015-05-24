#ifndef GPGPU_HF_SPRINGYOBJECT_H
#define GPGPU_HF_SPRINGYOBJECT_H

#include <GL/gl.h>
#include <CL/cl_platform.h>

#include "CLBuffer.hpp"
#include "CLKernel.hpp"
#include "ObjLoader.hpp"

class SpringyObject {
    ObjLoader obj;

    int maxDegree = 64;

    CLBuffer<cl_float4> positionBuffer;
    CLBuffer<cl_float4> velocityBuffer;
    CLBuffer<cl_float> inverseMassBuffer;
    CLBuffer<cl_float4> forceBuffer;

    CLBuffer<cl_int> degreeBuffer;

    CLBuffer<cl_int> pairBuffer;
    CLBuffer<cl_float2> pairParamBuffer;

    CLKernel<int, cl_mem, cl_mem, cl_mem, cl_mem, cl_mem> calcForcesKernel;
    CLKernel<float, cl_mem, cl_mem, cl_mem, cl_mem> integrate1EulerKernel;
    CLKernel<float, cl_mem, cl_mem, cl_mem> integrate2EulerKernel;


public:
    SpringyObject(const std::string &filename);

    void step(float dt);

    void render();
};


#endif //GPGPU_HF_SPRINGYOBJECT_H
