#ifndef GPGPU_HF_VOLUMEMESH_H
#define GPGPU_HF_VOLUMEMESH_H


#include <GL/gl.h>
#include <CL/cl_platform.h>

#include "CLBuffer.hpp"
#include "CLKernel.hpp"
#include "ObjLoader.hpp"
#include "AbstractObject.hpp"

class VolumeMesh : public AbstractObject {
    ObjLoader obj;

    int maxDegree = 64;
    int maxCornered = 16;

    float initVolume;

    CLBuffer<cl_float4> positionBuffer;
    CLBuffer<cl_float4> velocityBuffer;
    CLBuffer<cl_float> inverseMassBuffer;
    CLBuffer<cl_float4> forceBuffer;

    CLBuffer<cl_int> degreeBuffer;

    CLBuffer<cl_int> pairBuffer;
    CLBuffer<cl_float2> pairParamBuffer;

    CLBuffer<cl_int4> faceBuffer;
    CLBuffer<cl_int> corneredBuffer;
    CLBuffer<cl_int2> otherCornerBuffer;
    CLBuffer<cl_float> volumeBuffer;


    CLKernel<int, cl_mem, cl_mem, cl_mem, cl_mem, cl_mem> calcForcesKernel;
    CLKernel<cl_mem, cl_mem, cl_mem> calcVolumesKernel;
    CLKernel<float, int, cl_mem, cl_mem, cl_mem, cl_mem> applyPressureKernel;
    CLKernel<float, cl_mem, cl_mem, cl_mem, cl_mem> integrate1EulerKernel;
    CLKernel<float, cl_mem, cl_mem, cl_mem> integrate2EulerKernel;

public:
    VolumeMesh(const std::string &filename);

    float getVolume();

    void step(float dt);

    void render();
};


#endif //GPGPU_HF_VOLUMEMESH_H
