//
// Created by attila on 2015.05.24..
//

#ifndef GPGPU_HF_ABSTRACTOBJECT_H
#define GPGPU_HF_ABSTRACTOBJECT_H


class AbstractObject {
public:
    virtual void step(float dt) = 0;
    virtual void render() = 0;

    //these only make sense for volume objects
    virtual void inflate(float dt) {};
    virtual void deflate(float dt) {};

    virtual ~AbstractObject() {};
};


#endif //GPGPU_HF_ABSTRACTOBJECT_H
