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
    const Material* mat =
        its.shape->getMaterial();                                      // ← use accessor (material is private) :contentReference[oaicite:0]{index=0}

    // View direction (pointing from surface to camera)
    const Vector3D wo = (-r.d).normalized();

    Vector3D Lo(0.0);

    // Ambient (Eq. 7)
    if (ambient > 0.0 && mat->hasDiffuseOrGlossy()) {                  // :contentReference[oaicite:1]{index=1}
        Lo += mat->getDiffuseReflectance() * ambient;                  // at * ρd
    }

    // Direct illumination from point lights (Eq. 6)
    for (const LightSource* L : lsList) {
        const PointLightSource* pls = dynamic_cast<const PointLightSource*>(L);
        if (!pls) continue;

        const Vector3D lp = pls->sampleLightPosition();                // use API getters :contentReference[oaicite:2]{index=2}
        const Vector3D Li = pls->getIntensity();                       // (watts/sr)     :contentReference[oaicite:3]{index=3}
        const Vector3D wi = (lp - p).normalized();
        const double   dist = (lp - p).length();

        // Visibility term Vs with a shadow ray limited to the light
        Ray shadowRay(safeOffsetPoint(p, n), wi);
        shadowRay.Tmin = 1e-4;                                         // Ray has Tmin/Tmax per spec :contentReference[oaicite:4]{index=4}
        shadowRay.Tmax = dist - 1e-4;                                  // (capitalize T)

        if (Utils::hasIntersection(shadowRay, objList)) continue;      // occluded → Vs = 0

        // BRDF f_r(n, wi, wo)
        const Vector3D fr = mat->getReflectance(n, wo, wi);            // Phong BRDF (Eq. 4) :contentReference[oaicite:5]{index=5}

        // Cosine term max(0, wi·n)  (use Vector3D::dot from the math class)
        const double cosNI = std::max(0.0, Vector3D::dot(wi, n));

        // Accumulate Li * f_r * (wi·n)
        Lo += Li * fr * cosNI;
    }

    return Lo;
}
