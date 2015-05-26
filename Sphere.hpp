//
// Created by attila on 2015.05.26..
//

#ifndef GPGPU_HF_SPHERE_H
#define GPGPU_HF_SPHERE_H

#include <CL/cl.h>

#include <GL/glu.h>

class Sphere {
    cl_float4 position;
    cl_float4 velocity;
    cl_float4 force;
    cl_float radius;
    cl_float inverseMass;

    GLUquadricObj *quadratic;

public:
    Sphere();

    void render();

    ~Sphere();
};


#endif //GPGPU_HF_SPHERE_H
