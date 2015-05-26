//
// Created by attila on 2015.05.26..
//

#include "Sphere.hpp"

Sphere::Sphere() {
    position.s[0] = 0;
    position.s[1] = 0;
    position.s[2] = 0;
    position.s[3] = 0;

    velocity.s[0] = 0;
    velocity.s[1] = 0;
    velocity.s[2] = 0;
    velocity.s[3] = 0;

    force.s[0] = 0;
    force.s[1] = 0;
    force.s[2] = 0;
    force.s[3] = 0;

    radius = 0.1;
    inverseMass = 1;

    quadratic = gluNewQuadric();
}

void Sphere::render() {
    glEnable(GL_LIGHTING);
    glPushMatrix();
    glTranslatef(position.s[0], position.s[1], position.s[2]);
    glColor3f(0.75, 0.75, 0.75);
    gluSphere(quadratic,radius,32,32);
    glPopMatrix();
    glDisable(GL_LIGHTING);
}

Sphere::~Sphere() {
    gluDeleteQuadric(quadratic);
}