#include "AreaDirectIlluminationIntegrator.h"

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

AreaDirectIlluminationIntegrator::AreaDirectIlluminationIntegrator()
    : Shader(Vector3D(0.0)) { }

AreaDirectIlluminationIntegrator::AreaDirectIlluminationIntegrator(const Vector3D bgColor_)
    : Shader(bgColor_) { }


Vector3D AreaDirectIlluminationIntegrator::computeColor(const Ray& r,
    const std::vector<Shape*>& objList,
    const std::vector<LightSource*>& lsList) const
{
    int N = 256;
    Intersection hit;
    HemisphericalSampler sampler;
    Vector3D L_tot = (0,0,0);

    if (!Utils::getClosestIntersection(r, objList, hit)) {
        return bgColor;
    }


    

    const Material& material = hit.shape->getMaterial();
    const Vector3D hit_normal = hit.normal.normalized();
    const Vector3D hit_point = hit.itsPoint; // ¡no normalizar puntos!

    Vector3D L_e = 0;
    Vector3D wo = (-r.d).normalized();

    Vector3D sum_Lo_dir = 0;

    if (material.hasDiffuseOrGlossy()) {
        if (material.isEmissive()) {
            L_e = material.getEmissiveRadiance();
        }

        for (const LightSource* light : lsList) {   //We iterate for each light in the scene
            for (int j = 0; j < N; ++j) {
           

    
                Vector3D light_position = light->sampleLightPosition();

                Vector3D wi = (light_position - hit.itsPoint).normalized();

                Vector3D Li = 0;
                Li = light->getIntensity();
                

                // BRDF del punto actual (no del punto y) y con orden correcto (n, wo, wi)
                Vector3D f_r = material.getReflectance(hit_normal, wo, wi);

				float equation_part = (hit.itsPoint - light_position).length();
                Vector3D Gxy = (dot(hit_normal, wi.normalized()) * dot(-wi.normalized(), light->getNormal())) / pow(equation_part, 2);
                Vector3D Vxy;


                Vector3D L = light_position - hit.itsPoint; //We get the vector from the intersection point to the light source
                const double dist = L.length();
                const Vector3D shadowOrigin = hit.itsPoint; // Small offset to avoid self-intersection
                Ray shadowRay(shadowOrigin, wi, 0, Epsilon, dist - Epsilon);
                if (Utils::hasIntersection(shadowRay, objList)) {
                    Vxy = 0;

                }
                else {
                    Vxy = 1;
                }

                int A = light->getArea();
                Vector3D contrib = (Li * f_r) * (Gxy * Vxy) * (A);
               


                sum_Lo_dir += contrib;
            }
        }
    }

    if (material.hasSpecular()) { //This is done for the mirror material
        const Vector3D wr = (r.d - 2.0 * dot(r.d, hit_normal) * hit_normal).normalized(); //We get the reflection direction
        Ray reflRay(hit.itsPoint + hit_normal * Epsilon, wr, r.depth + 1); //We create the reflection ray with a small offset to avoid problems
        L_tot += computeColor(reflRay, objList, lsList); //We call the function recursively to get the color of the reflection ray
    }

    // Transmissive material
    if (material.hasTransmission()) {
        double index_of_refraction = material.getIndexOfRefraction();
        Vector3D hit_normal_local = hit_normal;  //We create a local normal to be able to invert it if we are exiting the material
        double dot_n_wo = dot(hit_normal, wo); //We compute in advance the dot preduct betwen the normal and wo. (Will be used later for the equation)

        if (dot_n_wo < 0.0) { //We do this to know if we are entering or exiting the material
            hit_normal_local = -hit_normal_local; //We invert the normal if we are exiting the material
            index_of_refraction = 1.0 / index_of_refraction; //We invert the index of refraction if we are exiting the material
            dot_n_wo = -dot_n_wo; //We also invert the dot product
        }

        const double inside_square_root = 1.0 - index_of_refraction * index_of_refraction * (1.0 - dot_n_wo * dot_n_wo); //We compute the part that is inside the square root
        if (inside_square_root >= 0.0) { // We check if we have total internal reflection
            // If we are inside the if, means that we don't have total internal reflection

            const Vector3D wt = (-index_of_refraction * wo + (index_of_refraction * dot_n_wo - std::sqrt(inside_square_root)) * hit_normal_local).normalized(); //We compute wt using the formula given in theory
            // Pequeño desplazamiento hacia el interior para evitar auto-intersección
            Ray refrRay(hit.itsPoint - hit_normal_local * Epsilon, wt, r.depth + 1); //As shown in the Pseudo-code, we will create a new ray for the refraction with the computed wt
            L_tot += computeColor(refrRay, objList, lsList); //We send the ray again as a recursive function
        }
        else {
            // If we are inside the else, means that we have total internal reflection

            // If we have total internal reflection, we will trat it as a mirror reflection, this means that we can directly copy the code used for the mirror.

            //------------- MIRROR CODE --------------
            const Vector3D wr = (r.d - 2.0 * dot(r.d, hit_normal) * hit_normal).normalized();
            Ray reflRay(hit.itsPoint + hit_normal * Epsilon, wr, r.depth + 1);
            L_tot += computeColor(reflRay, objList, lsList);
            //------------- END MIRROR CODE --------------
        }
    }

    Vector3D L_o_dir = sum_Lo_dir / static_cast<double>(N);
    Vector3D L_o_indir = 0; // aún no implementado

    Vector3D L_r = L_o_dir + L_o_indir;
    L_tot += L_e + L_r;
    return L_tot;
}


