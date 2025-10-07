#include "whittedintegrator.h"
#include "../core/utils.h"
#include "../core/scene.h"
#include "../materials/material.h"
#include "../lightsources/pointlightsource.h"

// Tiny offset to avoid self-intersection (“shadow acne”)
static inline Vector3D safeOffsetPoint(const Vector3D& p, const Vector3D& n) {
    return p + 1e-4 * n;
}

Vector3D WhittedIntegrator::computeColor(
    const Ray& r,
    const std::vector<Shape*>& objList,
    const std::vector<LightSource*>& lsList) const
{
    // Closest hit
    Intersection its;
    if (!Utils::getClosestIntersection(r, objList, its)) {
        return bgColor;                                                // miss → background
    }

    // Shading data
    const Vector3D p = its.itsPoint;
    const Vector3D n = its.normal.normalized();
    const Material* mat = &its.shape->getMaterial();                  // use accessor (material is private)

    // View direction (pointing from surface to camera)
    const Vector3D wo = (-r.d).normalized();

    Vector3D Lo(0.0);



    if (mat->hasSpecular() && !mat->hasDiffuseOrGlossy() && !mat->hasTransmission()) { // solo espejo perfecto
        if (r.depth >= maxDepth) return Vector3D(0.0); // límite de profundidad

        // Vector incidente hacia la superficie    if (mat->hasSpecular() && !mat->hasDiffuseOrGlossy() && !mat->hasTransmission()) { // solo espejo perfecto
        if (r.depth >= maxDepth) return Vector3D(0.0); // límite de profundidad

        // Vector incidente hacia la superficie
        const Vector3D wi_in = -wo;
        const double ndotI = dot(n, wi_in);
        Vector3D wr = (n * (2.0 * ndotI)) - wi_in; // reflect direction
        wr.normalized();

        Ray reflRay(safeOffsetPoint(p, n), wr);
        reflRay.minT = 1e-4;
        reflRay.maxT = std::numeric_limits<double>::infinity();
        reflRay.depth = r.depth + 1;

        // Pure mirror: return only reflected color
        return computeColor(reflRay, objList, lsList);
    }



    // Ambient (Eq. 7)
    if (ambient > 0.0 && mat->hasDiffuseOrGlossy()) {
        Lo += mat->getDiffuseReflectance() * ambient;                  // at * ρd
    }

    // Direct illumination from point lights (Eq. 6)
    for (const LightSource* L : lsList) {
        const PointLightSource* pls = dynamic_cast<const PointLightSource*>(L);
        if (!pls) continue;

        const Vector3D lp = pls->sampleLightPosition();
        const Vector3D Li = pls->getIntensity();                       // (watts/sr)
        const Vector3D wi = (lp - p).normalized();
        const double   dist = (lp - p).length();

        // Visibility term Vs with a shadow ray limited to the light
        Ray shadowRay(safeOffsetPoint(p, n), wi);
        shadowRay.minT = 1e-4;                                         // Ray has minT/maxT per spec
        shadowRay.maxT = dist - 1e-4;

        if (Utils::hasIntersection(shadowRay, objList)) continue;      // occluded → Vs = 0

        // BRDF f_r(n, wi, wo)
        const Vector3D fr = mat->getReflectance(n, wo, wi);            // Phong BRDF (Eq. 4)

        // Cosine term max(0, wi·n)
        const double cosNI = std::max(0.0, dot(wi, n));

        // Accumulate Li * f_r * (wi·n)
        Lo += Li * fr * cosNI;
    }

    return Lo;
}