#ifndef HEMISPHERICALINTEGRATOR_H
#define HEMISPHERICALINTEGRATOR_H

#include "shader.h"

class HemisphericalIntegrator : public Shader {
public:
    HemisphericalIntegrator();
    explicit HemisphericalIntegrator(const Vector3D bgColor_);

    virtual Vector3D computeColor(const Ray& r,
                          const std::vector<Shape*>& objList,
                          const std::vector<LightSource*>& lsList) const override;

private:
    // bgColor ya está en Shader, no es necesario aquí
};

#endif
