#ifndef NORMALINTEGRATOR_H
#define NORMALINTEGRATOR_H

#include "shader.h"

class NormalIntegrator : public Shader
{
public:
    NormalIntegrator();
    NormalIntegrator(Vector3D bgColor_);

    virtual Vector3D computeColor(const Ray& r,
        const std::vector<Shape*>& objList,
        const std::vector<LightSource*>& lsList) const;
};

#endif // NORMALINTEGRATOR_H
