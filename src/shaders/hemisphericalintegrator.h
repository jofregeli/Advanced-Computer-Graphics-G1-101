#ifndef HEMISPHERICALINTEGRATOR_H
#define HEMISPHERICALINTEGRATOR_H

#include <vector>
#include "shader.h"

#include "../core/ray.h"
#include "../lightsources/pointlightsource.h"
#include "../lightsources/arealightsource.h"
#include "../shapes/shape.h"

class HemisphericalIntegrator : public Shader
{
public:
    HemisphericalIntegrator();
    HemisphericalIntegrator(Vector3D bgColor_);

    virtual Vector3D computeColor(const Ray& r, const std::vector<Shape*>& objList, const std::vector<LightSource*>& lsList) const;


private:
    
};

#endif // SHADER_H
