#ifndef AREADIRECTILLUMINATIONINTEGRATOR_H
#define AREADIRECTILLUMINATIONINTEGRATOR_H

#include "shader.h"

class AreaDirectIlluminationIntegrator : public Shader {
public:
    AreaDirectIlluminationIntegrator();
    explicit AreaDirectIlluminationIntegrator(const Vector3D bgColor_);

    virtual Vector3D computeColor(const Ray& r,
                          const std::vector<Shape*>& objList,
                          const std::vector<LightSource*>& lsList) const override;

private:
    // bgColor ya está en Shader, no es necesario aquí
};

#endif
