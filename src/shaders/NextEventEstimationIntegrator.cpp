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
    int N = 256;
    if (r.depth > 0) {
        N = 1; 
    }   

    Intersection hit;
    if (!Utils::getClosestIntersection(r, objList, hit)) {
        return bgColor;
    }
    const Material& material = hit.shape->getMaterial();
    const Vector3D n = hit.normal.normalized();
    const Vector3D x = hit.itsPoint;
    const Vector3D wo = (-r.d).normalized();

    Vector3D Lo(0.0);
    for (int s = 0; s < N; ++s) {
        for (const LightSource* light : lsList) {
            Lo += ReflectedRadiance(hit, -r.d, r.depth, objList, const_cast<LightSource*>(light));
        }
    }
	Vector3D Le = Vector3D(0.0);
    if (material.isEmissive()) {
		Le = material.getEmissiveRadiance();

    }

     if (material.hasSpecular()) {
        const Vector3D wr = (r.d - 2.0 * dot(r.d, n) * n).normalized();
        Ray reflRay(x + n * Epsilon, wr, r.depth);
        return Lo + computeColor(reflRay, objList, lsList);
    }

    if (material.hasTransmission()) {
        double eta = material.getIndexOfRefraction();
        Vector3D n_local = n;
        double cosNoWo = dot(n, wo);
        bool entering = cosNoWo > 0.0;
        if (!entering) { n_local = -n_local; eta = 1.0 / eta; cosNoWo = -cosNoWo; }

        const double k = 1.0 - eta * eta * (1.0 - cosNoWo * cosNoWo);
        if (k >= 0.0) {
            const Vector3D wt = (-eta * wo + (eta * cosNoWo - std::sqrt(k)) * n_local).normalized();
            Ray refrRay(x - n_local * Epsilon, wt, r.depth);
            return Lo + computeColor(refrRay, objList, lsList);
        }
        else {
            // Reflexión total interna
            const Vector3D wr = (r.d - 2.0 * dot(r.d, n) * n).normalized();
            Ray reflRay(x + n * Epsilon, wr, r.depth);
            return Lo + computeColor(reflRay, objList, lsList);
        }
    }
    return Le + (Lo / double(N));
}

Vector3D NextEventEstimationIntegrator::ReflectedRadiance(Intersection x, Vector3D wo, int depth, const std::vector<Shape*>& objList, LightSource* light) const {
    Vector3D L_dir = DirectRadiance(x, wo, light, objList);
    Vector3D L_ind = IndirectRadiance(x, wo, depth, objList, light);
    return L_dir + L_ind;
}

Vector3D NextEventEstimationIntegrator::DirectRadiance(Intersection x, Vector3D wo, LightSource* light, const std::vector<Shape*>& objList) const {

    const Vector3D y  = light->sampleLightPosition();
    const Vector3D Le = light->getIntensity();

    Vector3D wi = (y - x.itsPoint);
    const double dist2 = wi.lengthSq();
    if (dist2 <= Epsilon) return Vector3D(0.0);
    wi = wi.normalized();

    const Vector3D n  = x.normal.normalized();
    const Vector3D nl = light->getNormal().normalized();

    const double cosX = std::max(0.0, dot(n,  wi));
    const double cosY = std::max(0.0, dot(nl, -wi));

    const double A   = light->getArea();
    const double pdf = (1 / A);
    const double G   = (cosX * cosY) / dist2;


    Vector3D Vxy;


    Vector3D L = y - x.itsPoint; //We get the vector from the intersection point to the light source
    const double dist = L.length();
    const Vector3D shadowOrigin = x.itsPoint; // Small offset to avoid self-intersection
    Ray shadowRay(shadowOrigin, wi, 0, Epsilon, dist - Epsilon);
    if (Utils::hasIntersection(shadowRay, objList)) {
        Vxy = 0;

    }
    else {
        Vxy = 1;
    }



    const Vector3D f_r = x.shape->getMaterial().getReflectance(n, wo, wi)*Vxy;

    // NEE: L_dir = Li * f_r * G / pdf
    return Le * f_r * (G / pdf);
}

Vector3D NextEventEstimationIntegrator::IndirectRadiance(Intersection x, Vector3D wo, int depth,
    const std::vector<Shape*>& objList, LightSource* light) const
{
    Vector3D n = x.normal;
    HemisphericalSampler sampler;
    const Vector3D wi = sampler.getSample(n).normalized();
    Ray newR(x.itsPoint + n*Epsilon, wi, depth + 1);


	Vector3D Li(0.0);
    if (depth < MaxDepth) {

        Intersection y;
        if (!Utils::getClosestIntersection(newR, objList, y)) {
            return Vector3D(0.0);
        }

        const Material& material = x.shape->getMaterial();
        Li = ReflectedRadiance(y, -wi, depth + 1, objList, light);
        Li = Li * material.getReflectance(n, wo, wi) * dot(x.normal, wi) * (2 * 3.14159265358979323846);


    }
    return Li;
}