#include "whittedintegrator.h"
#include "../core/utils.h"
#include "../core/scene.h"
#include "../materials/material.h"
#include "../lightsources/pointlightsource.h"

// Tiny offset to avoid self-intersection (“shadow acne”)
static inline Vector3D safeOffsetPoint(const Vector3D& p, const Vector3D& n) {
    return p + 1e-4 * n;
}
// Put near your other helpers in WhittedIntegrator.cpp
static inline Vector3D safeOffsetAlong(const Vector3D& p, const Vector3D& dir) {
    return p + dir.normalized() * 1e-4;
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
    const Material& mat = its.shape->getMaterial();

    // View direction (pointing from surface to camera)
    const Vector3D wo = (-r.d).normalized();





    Vector3D Lo(0.0);

    // ----- Perfect specular transmission (glass-like) -----
    // ----- Perfect specular transmission (refraction) -----
    // ----- Perfect specular transmission (refraction) -----
    if (mat.hasTransmission() && !mat.hasDiffuseOrGlossy() && !mat.hasSpecular()) {
        if (r.depth >= maxDepth) return Vector3D(0.0);   // recursion guard

        // Outgoing/view dir per pixel (PDF uses ωo)
        const Vector3D wo = (-r.d).normalized();         // from hit point towards camera

        // Front or back face?  (for front face, n·wo > 0)
        const bool entering = ( dot(n, wo) > 0.0);
        const Vector3D n_face = entering ? n : -n;

        // Indices: air=1.0, material=η (from the Transmissive material)
        const double eta_mat = mat.getIndexOfRefraction();  // η2 in the PDF
        const double eta1 = entering ? 1.0 : eta_mat;   // outside → inside OR inside → outside
        const double eta2 = entering ? eta_mat : 1.0;
        const double mu_t = eta1 / eta2;                    // μt = η1 / η2 (PDF)

        // Eq. (8) requires nx·ωo and the radicand
        const double ndotwo = dot(n_face, wo);
        const double oneMinus = 1.0 - ndotwo * ndotwo;
        const double radicand = 1.0 - (mu_t * mu_t) * oneMinus;

        // Total Internal Reflection → perfect reflection
        if (radicand < 0.0) {
            // reflect the *incident* direction (which is -ωo) about n_face
            const Vector3D wi = -wo;
            Vector3D wr = wi - n_face * (2.0 * dot(wi, n_face));
            wr = wr.normalized();

            Ray reflRay(p + wr * 1e-4, wr);               // offset along ray dir
            reflRay.minT = 1e-4;
            reflRay.maxT = std::numeric_limits<double>::infinity();
            reflRay.depth = r.depth + 1;
            return computeColor(reflRay, objList, lsList);
        }

        // Eq. (8): ωt = −μt ωo + nx ( μt(nx·ωo) − sqrt(1 − μt^2 (1 − (nx·ωo)^2)) )
        const double root = std::sqrt(radicand);
        Vector3D wt = (-mu_t) * wo + n_face * (mu_t * ndotwo - root);
        wt = wt.normalized();

        // Spawn refracted ray; offset *along wt* to avoid re-hitting the same surface
        Ray refrRay(p + wt * 1e-4, wt);
        refrRay.minT = 1e-4;
        refrRay.maxT = std::numeric_limits<double>::infinity();
        refrRay.depth = r.depth + 1;

        return computeColor(refrRay, objList, lsList);
    }




    if (mat.hasSpecular() && !mat.hasDiffuseOrGlossy() && !mat.hasTransmission()) {
        if (r.depth >= maxDepth) return Vector3D(0.0);   // recursion guard

        // Incident direction is the incoming ray direction at the hit
        Vector3D wi = r.d.normalized();

        // Standard reflection: wr = wi - 2 * dot(wi, n) * n
        Vector3D wr = wi - n * (2.0 * dot(wi, n));
        wr = wr.normalized();

        // Offset the origin to avoid self-intersections
        Ray reflRay(safeOffsetPoint(p, n), wr);
        reflRay.minT = 1e-4;
        reflRay.maxT = std::numeric_limits<double>::infinity();
        reflRay.depth = r.depth + 1;

        // Pure mirror: only the reflected color (no diffuse/ambient mixed)
        return computeColor(reflRay, objList, lsList);
    }




    // Ambient (Eq. 7)
    if (ambient > 0.0 && mat.hasDiffuseOrGlossy()) {
        Lo += mat.getDiffuseReflectance() * ambient;                  // at * ρd
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
        const Vector3D fr = mat.getReflectance(n, wo, wi);            // Phong BRDF (Eq. 4)

        // Cosine term max(0, wi·n)
        const double cosNI = std::max(0.0, dot(wi, n));

        // Accumulate Li * f_r * (wi·n)
        Lo += Li * fr * cosNI;
    }

    return Lo;
}