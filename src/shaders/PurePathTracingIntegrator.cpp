#include "PurePathTracingIntegrator.h"

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

PurePathTracingIntegrator::PurePathTracingIntegrator()
    : Shader(Vector3D(0.0)) { }

PurePathTracingIntegrator::PurePathTracingIntegrator(const Vector3D bgColor_)
    : Shader(bgColor_) { }

Vector3D PurePathTracingIntegrator::computeColor(
    const Ray& r,
    const std::vector<Shape*>& objList,
    const std::vector<LightSource*>& lsList) const
{
    constexpr int MaxDepth = 4;
    constexpr double Pi = 3.14159265358979323846;


    // Puedes mantener más de una muestra por vértice si quieres menos ruido
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
    const Vector3D x = hit.itsPoint; // no normalizar puntos
    const Vector3D wo = (-r.d).normalized();

    // Emisión del punto (si es una luz)
    Vector3D Lo(0.0);




    // Criterio de parada: no seguir trazando más allá de MaxDepth
 

    // Difuso/glossy: estimador MC con muestreo uniforme del hemisferio
    HemisphericalSampler sampler;
    Vector3D Ls(0.0);

    if (material.hasDiffuseOrGlossy()) {
        for (int s = 0; s < N; ++s) {
            const Vector3D wi = sampler.getSample(n).normalized();
            const double cosTheta = std::max(0.0, dot(n, wi));
            
            if (material.isEmissive()) {
                 Lo += material.getEmissiveRadiance();
            }

            if (r.depth >= MaxDepth) {
                return Lo;
            }
 

            Ray newR(x + n * Epsilon, wi, r.depth + 1);
            const Vector3D Li = computeColor(newR, objList, lsList);
            const Vector3D f = material.getReflectance(n, wo, wi);

            // L_o += Li * f * (n·wi) / pdf
            Ls += (Li * f) * (cosTheta * (2.0 * Pi));
        }
    }
    // Manejo de materiales delta (espejo/transmisión) respetando MaxDepth
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



    Lo += Ls / static_cast<double>(N);
    return Lo;
}