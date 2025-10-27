#include "NextEventEstimationIntegrator.h"

#include "../core/utils.h"
#include "../core/intersection.h"
#include "../shapes/shape.h"
#include "../materials/material.h"
#include "../lightsources/lightsource.h"
#include "../core/hemisphericalsampler.h"
#include "../lightsources/arealightsource.h"


#include <algorithm>
#include <cmath>
#include <limits>

#define MaxDepth 4

NextEventEstimationIntegrator::NextEventEstimationIntegrator()
    : Shader(Vector3D(0.0)) { }

NextEventEstimationIntegrator::NextEventEstimationIntegrator(const Vector3D bgColor_)
    : Shader(bgColor_) { }

Vector3D NextEventEstimationIntegrator::computeColor(
    const Ray& r,
    const std::vector<Shape*>& objList,
    const std::vector<LightSource*>& lsList) const
{
    int N = (r.depth > 0) ? 1 : 50;

    Intersection hit;
    if (!Utils::getClosestIntersection(r, objList, hit)) {
        return bgColor;
    }

    Vector3D Lo(0.0);
    for (int s = 0; s < N; ++s) {
        for (const LightSource* light : lsList) {
            Lo += ReflectedRadiance(hit, -r.d, r.depth, objList, const_cast<LightSource*>(light));
        }
    }
    return Lo / double(N);
}

Vector3D NextEventEstimationIntegrator::ReflectedRadiance(Intersection x, Vector3D wo, int depth, const std::vector<Shape*>& objList, LightSource* light) const {
    Vector3D L_dir = DirectRadiance(x, wo, light);
    Vector3D L_ind = IndirectRadiance(x, wo, depth, objList, light);
    return L_dir + L_ind;
}

Vector3D NextEventEstimationIntegrator::DirectRadiance(Intersection x, Vector3D wo, LightSource* light) const {
    Vector3D y = light->sampleLightPosition();
    Vector3D Le = light->getIntensity();
    Vector3D wi = (y - x.itsPoint).normalized();
    Vector3D n = x.normal;
    Vector3D L_dir = Le * x.shape->getMaterial().getReflectance(n, wo, wi) * dot(n, wi) * (2 * 3.14159265358979323846);
    return L_dir;
}

Vector3D NextEventEstimationIntegrator::IndirectRadiance(Intersection x, Vector3D wo, int depth,
    const std::vector<Shape*>& objList, LightSource* light) const
{
    Vector3D n = x.normal;
    HemisphericalSampler sampler;
    const Vector3D wi = sampler.getSample(n).normalized();
    Ray newR(x.itsPoint + n*Epsilon, wi, depth + 1);

    if (depth >= MaxDepth) return Vector3D(0.0);

    Intersection y;
    if (!Utils::getClosestIntersection(newR, objList, y)) {
        return Vector3D(0.0);
    }

    const Material& material = x.shape->getMaterial();
    const Vector3D Li = ReflectedRadiance(y, -wi, depth + 1, objList, light);
    return Li * material.getReflectance(n, wo, wi) * dot(x.normal, wi) * (2 * 3.14159265358979323846);
}