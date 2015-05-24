//
// Created by attila on 2015.05.24..
//

#include "Camera.hpp"

#include <GL/gl.h>
#include <GL/glu.h>

#include <cmath>

void Camera::look() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45, 16.0/9.0, 1, 1000);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    float ex = x + r * cosf(theta) * cosf(phi);
    float ey = y + r * sinf(theta) * cosf(phi);
    float ez = z + r * sinf(phi);

    gluLookAt(ex, ey, ez, x, y, z, 0, 0, 1);
}

void Camera::left(float dt) {
    theta += 10 * dt;
}

void Camera::right(float dt) {
    theta -= 10 * dt;
}

void Camera::up(float dt) {
    phi -= 10 * dt;
}

void Camera::down(float dt) {
    phi += 10 * dt;
}


void Camera::move_left(float dt) {
    x += 10 * sinf(theta) * dt;
    y -= 10 * cosf(theta) * dt;
}

void Camera::move_right(float dt) {
    x -= 10 * sinf(theta) * dt;
    y += 10 * cosf(theta) * dt;
}

void Camera::move_forward(float dt) {
    x -= 10 * cosf(theta) * dt;
    y -= 10 * sinf(theta) * dt;
}

void Camera::move_backward(float dt) {
    x += 10 * cosf(theta) * dt;
    y += 10 * sinf(theta) * dt;
}

void Camera::move_up(float dt) {
    z += 10 * dt;
}

void Camera::move_down(float dt) {
    z -= 10 * dt;
}
