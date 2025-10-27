#ifndef NEXTEVENTESTIMATIONINTEGRATOR_H
#define NEXTEVENTESTIMATIONINTEGRATOR_H

#include "shader.h"

class NextEventEstimationIntegrator : public Shader {
public:
    NextEventEstimationIntegrator();
    explicit NextEventEstimationIntegrator(const Vector3D bgColor_);

    virtual Vector3D computeColor(const Ray& r,
                          const std::vector<Shape*>& objList,
                          const std::vector<LightSource*>& lsList) const override;

    Vector3D  ReflectedRadiance(Intersection x, Vector3D wo, int depth, const std::vector<Shape*>& objList, LightSource* light) const;
    Vector3D DirectRadiance(Intersection x, Vector3D wo, LightSource* light) const;
    Vector3D IndirectRadiance(Intersection x, Vector3D wo, int depth, const std::vector<Shape*>& objList, LightSource* light) const;

private:

};

#endif
