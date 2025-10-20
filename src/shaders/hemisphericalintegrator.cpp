#include "hemisphericalintegrator.h"

#include "../core/utils.h"
#include "../core/intersection.h"
#include "../shapes/shape.h"
#include "../materials/material.h"
#include "../lightsources/lightsource.h"
#include "../core/hemisphericalsampler.h"


#include <algorithm>
#include <cmath>
#include <limits>

HemisphericalIntegrator::HemisphericalIntegrator()
    : Shader(Vector3D(0.0)) { }

HemisphericalIntegrator::HemisphericalIntegrator(const Vector3D bgColor_)
    : Shader(bgColor_) { }


Vector3D HemisphericalIntegrator::computeColor(const Ray& r,
    const std::vector<Shape*>& objList,
    const std::vector<LightSource*>& lsList) const
{
    int N = 256;
    Intersection hit;
    HemisphericalSampler sampler;

    if (!Utils::getClosestIntersection(r, objList, hit)) {
        return bgColor;
    }

    const Material& material = hit.shape->getMaterial();
    const Vector3D hit_normal = hit.normal.normalized();
    const Vector3D hit_point = hit.itsPoint; // ¡no normalizar puntos!

    Vector3D L_e = 0;
    if (material.isEmissive()) {
        L_e = material.getEmissiveRadiance();
    }

    Vector3D wo = (-r.d).normalized();

    Vector3D sum_Lo_dir = 0;
    for (int j = 0; j < N; ++j) {
        Intersection hit_dir;

        Vector3D wi = sampler.getSample(hit_normal).normalized();

        Ray new_ray;
        new_ray.o = hit_point + hit_normal * Epsilon; // desplazar a lo largo de la normal
        new_ray.d = wi;
        new_ray.minT = Epsilon;
        new_ray.maxT = std::numeric_limits<double>::infinity();

        // Visibilidad: si no toca nada, contribución 0 (no abortar el píxel)
        if (!Utils::getClosestIntersection(new_ray, objList, hit_dir)) {
            continue;
        }

        Vector3D Li = 0;
        const Material& material_y = hit_dir.shape->getMaterial();
        if (material_y.isEmissive()) {
            Li = material_y.getEmissiveRadiance();
        }

        // BRDF del punto actual (no del punto y) y con orden correcto (n, wo, wi)
        Vector3D f_r = material.getReflectance(hit_normal, wo, wi);

        double cosTheta = std::max(0.0, dot(wi, hit_normal));

        // Estimador MC para muestreo uniforme del hemisferio: pdf = 1/(2π)
        constexpr double TwoPi = 6.283185307179586;
        Vector3D contrib = (Li * f_r) * (cosTheta * TwoPi);

        sum_Lo_dir += contrib;
    }

    Vector3D L_o_dir = sum_Lo_dir / static_cast<double>(N);
    Vector3D L_o_indir = 0; // aún no implementado

    Vector3D L_r = L_o_dir + L_o_indir;
    Vector3D L_tot = L_e + L_r;
    return L_tot;
}


