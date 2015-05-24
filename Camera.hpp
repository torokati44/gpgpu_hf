//
// Created by attila on 2015.05.24..
//

#ifndef GPGPU_HF_CAMERA_H
#define GPGPU_HF_CAMERA_H

class Camera {

    float x = 0, y = 0, z = 0;
    float r = 10, theta = 0, phi = 0;

public:
    void look();

    void left(float dt);
    void right(float dt);
    void up(float dt);
    void down(float dt);

    void move_left(float dt);
    void move_right(float dt);
    void move_up(float dt);
    void move_down(float dt);
    void move_forward(float dt);
    void move_backward(float dt);

};


#endif //GPGPU_HF_CAMERA_H
