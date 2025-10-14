#ifndef WHITTEDINTEGRATOR_H
#define WHITTEDINTEGRATOR_H

#include "shader.h"

class WhittedIntegrator : public Shader {
public:
    WhittedIntegrator();
    WhittedIntegrator(Vector3D bgColor_, double ambient_, int maxDepth_)
        : Shader(bgColor_), ambient(ambient_), maxDepth(maxDepth_) {
    }
    virtual Vector3D computeColor(const Ray& r,
        const std::vector<Shape*>& objList,
        const std::vector<LightSource*>& lsList) const;

private:
    double ambient; // at in the PDF
    int    maxDepth;
};

#endif
