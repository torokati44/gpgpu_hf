
__constant float4 gravity = (float4)(0, 0, -10, 0);


__kernel void calcForces(int maxDegree,
        __global float4 *positionBuffer,
        __global float *inverseMassBuffer,
        __global int *degreeBuffer,
        __global int *pairBuffer,
        __global float2 *pairParamBuffer,
        __global float4 *forceBuffer)
{
        int point = get_global_id(0);
        int first = point * maxDegree;

        float invMass = inverseMassBuffer[point];
        if (invMass > 1e-5f) {
            forceBuffer[point] = gravity / invMass;
        }
        for (int i = 0; i < degreeBuffer[point]; ++i) {
                int other = pairBuffer[first + i];

                float dist = distance(positionBuffer[other], positionBuffer[point]);

                if (dist < 1e-5f) continue;

                float4 to_other = (positionBuffer[other] - positionBuffer[point]) / dist;

                forceBuffer[point] += to_other * pairParamBuffer[first + i].y * (dist - pairParamBuffer[first + i].x);
        }
}

__kernel void integrate1Euler(
        float dt,
        __global float *invmass_in,
        __global float4 *velocity_in,
        __global float4 *force_in,
        __global float4 *velocity_out) {
    int id = get_global_id(0);
    velocity_out[id] = velocity_in[id] + dt * force_in[id] * invmass_in[id];
}

__kernel void integrate2Euler(
        float dt,
        __global float4 *position_in,
        __global float4 *velocity_in,
        __global float4 *position_out) {
    int id = get_global_id(0);

    position_out[id] = position_in[id] + dt * velocity_in[id];

    if ((position_out[id].z < position_out[id].y * 0.3f)) {
        position_out[id].z = position_out[id].y * 0.3f;
        velocity_in[id].z = 0.0f;
        velocity_in[id] *= 0.5f;
    }
    velocity_in[id] *= 0.999f;
}


__kernel void calcVolumes(
        __global float4 *positionBuffer,
        __global int4 *faceBuffer,
        __global float *volumeBuffer) {
    int face = get_global_id(0);

    volumeBuffer[face] = dot(
            positionBuffer[faceBuffer[face].x],
            cross(
                    positionBuffer[faceBuffer[face].y],
                    positionBuffer[faceBuffer[face].z])) / 6.0f;
}


__kernel void applyPressure(float pressureDiff, int maxCornered,
        __global float4 *positionBuffer,
        __global int *corneredBuffer,
        __global int2 *otherCornerBuffer,
        __global float4 *forceBuffer
) {
    int point = get_global_id(0);

    float4 normal = (float4)(0);
    for (int i = 0; i < corneredBuffer[point]; ++i) {
        int other1 = otherCornerBuffer[maxCornered * point + i].x;
        int other2 = otherCornerBuffer[maxCornered * point + i].y;

        float4 a = positionBuffer[point];
        float4 b = positionBuffer[other1];
        float4 c = positionBuffer[other2];

        float4 cp = cross(b-a, c-a);
        forceBuffer[point] += cp * pressureDiff * 20000;
        normal += normalize(cp);
    }

    //forceBuffer[point] += normal * 1000;

}