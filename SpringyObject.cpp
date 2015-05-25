#include <CL/cl_platform.h>
#include "SpringyObject.hpp"

#include <cmath>

SpringyObject::SpringyObject(const std::string &filename):
        obj{filename},
        positionBuffer{obj.points.size()},
        velocityBuffer{obj.points.size()},
        inverseMassBuffer{obj.points.size()},
        forceBuffer{obj.points.size()},
        degreeBuffer{obj.points.size()},
        pairBuffer{obj.points.size() * maxDegree},
        pairParamBuffer{obj.points.size() * maxDegree},
        calcForcesKernel{"calcForces"},
        integrate1EulerKernel{"integrate1Euler"},
        integrate2EulerKernel{"integrate2Euler"}
{
    auto positions = positionBuffer.map();
    auto velocities = velocityBuffer.map();
    auto inverseMasses = inverseMassBuffer.map();
    auto degrees = degreeBuffer.map();
    auto pairs = pairBuffer.map();
    auto pairParams = pairParamBuffer.map();

    for (size_t i = 0; i < obj.points.size(); ++i) {
        positions[i] = obj.points[i];

        velocities[i].s[0] = 0;
        velocities[i].s[1] = 0;
        velocities[i].s[2] = 0;
        velocities[i].s[3] = 0;

        inverseMasses[i] = 5;
        degrees[i] = 0;
    }

    for (size_t i = 0; i < obj.edges.size(); ++i) {
        int a = obj.edges[i].s[0];
        int b = obj.edges[i].s[1];

        if ((degrees[a] >= maxDegree) || (degrees[b] >= maxDegree)) {
            std::cerr << "TOO MANY EDGES!\n";
            continue;
        }

        float dx = positions[a].s[0] - positions[b].s[0];
        float dy = positions[a].s[1] - positions[b].s[1];
        float dz = positions[a].s[2] - positions[b].s[2];

        float dist = sqrtf(dx*dx + dy*dy + dz*dz);
        float strength = 2000;

        pairs[maxDegree * a + degrees[a]] = b;
        pairs[maxDegree * b + degrees[b]] = a;

        pairParams[maxDegree * a + degrees[a]].s[0] = dist;
        pairParams[maxDegree * a + degrees[a]].s[1] = strength;

        pairParams[maxDegree * b + degrees[b]].s[0] = dist;
        pairParams[maxDegree * b + degrees[b]].s[1] = strength;

        ++degrees[a];
        ++degrees[b];

    }

    positionBuffer.unmap();
    velocityBuffer.unmap();
    inverseMassBuffer.unmap();
    degreeBuffer.unmap();
    pairBuffer.unmap();
    pairParamBuffer.unmap();
}

void SpringyObject::step(float dt) {
    calcForcesKernel.execute(obj.points.size(), maxDegree, positionBuffer, inverseMassBuffer, degreeBuffer, pairBuffer, pairParamBuffer, forceBuffer);

    integrate1EulerKernel.execute(obj.points.size(), dt, inverseMassBuffer, velocityBuffer, forceBuffer, velocityBuffer);

    integrate2EulerKernel.execute(obj.points.size(), dt, positionBuffer, velocityBuffer, positionBuffer);

}

void SpringyObject::render() {
    auto positions = positionBuffer.map();

    glPointSize(3);


    glColor4f(1,1,1,1);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 16, positions);

    glDrawArrays(GL_POINTS, 0, obj.points.size());

    glDisableClientState(GL_VERTEX_ARRAY);


/*
    glBegin(GL_POINTS);
    glColor4f(1,1,1,1);
    for (int i = 0; i < obj.points.size(); ++i) {
        glVertex3fv(positions[i].s);
    }
    glEnd();
*/



    glBegin(GL_LINES);
    glColor3f(0.8, 0.4, 0.2);
    for (auto &e : obj.edges) {
        glVertex3fv(positions[e.s[0]].s);
        glVertex3fv(positions[e.s[1]].s);
    }
    glEnd();

    positionBuffer.unmap();
}