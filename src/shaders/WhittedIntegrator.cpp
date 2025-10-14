#include "WhittedIntegrator.h"

#include "../core/utils.h"
#include "../core/intersection.h"
#include "../shapes/shape.h"
#include "../materials/material.h"
#include "../lightsources/lightsource.h"

#include <algorithm>
#include <cmath>

WhittedIntegrator::WhittedIntegrator()
    : Shader(Vector3D(0.0)), ambient(0.1), maxDepth(5) {}

Vector3D WhittedIntegrator::computeColor( const Ray& r, const std::vector<Shape*>& objList, const std::vector<LightSource*>& lsList) const
{
    Intersection hit;
    if (!Utils::getClosestIntersection(r, objList, hit)) {
        return bgColor;
    }

	const Material& material = hit.shape->getMaterial(); //We want to get the material of the intersected object
	const Vector3D hit_normal = hit.normal.normalized(); //We want to get the normal of the intersected object
    const Vector3D wo = (-r.d).normalized(); //We want to get the direction to the camera

	Vector3D final_color(0.0); //We initialize the final color

	// Exercise 4.5.2 (Part 2)
    if (material.hasDiffuseOrGlossy() && ambient > 0.0) {
        final_color += ambient * material.getDiffuseReflectance(); //We do this to avoid having a too dark scene
        // It is added at the start of the equation. 
    }

    //Exercise 4.5.2     
    if (material.hasDiffuseOrGlossy()) {
        for (const LightSource* light : lsList) {   //We iterate for each light in the scene
            const Vector3D lp = light->sampleLightPosition(); //We get the position of the light
			const Vector3D Li = light->getIntensity(); //We get the intensity of the light. 
			// The Li will be the first term that we will add to the equation. 

			Vector3D L = lp - hit.itsPoint; //We get the vector from the intersection point to the light source
			const double dist = L.length(); //We get the distance from the intersection point to the light source
			if (dist <= 0.0) continue; // avoid division by zero

			const Vector3D wi = L / dist; //We get the direction from the intersection point to the light source


			const Vector3D shadowOrigin = hit.itsPoint + hit_normal * Epsilon; // Small offset to avoid self-intersection
            Ray shadowRay(shadowOrigin, wi, 0, Epsilon, dist - Epsilon);
            if (Utils::hasIntersection(shadowRay, objList)) {
				continue; // We have to make sure that the light is visible from the intersection point
            }

            const Vector3D fr = material.getReflectance(hit_normal, wo, wi); //We use the function created in the phong material to get the BRDF. 
            //The fr will be necesary for the function

			const double final_part_equation = dot(hit_normal, wi); //We get the last term of the equation.

			final_color += Li * fr * final_part_equation; //We finaly get the final color by adding all the terms of the equation.
        }
    }

    // Mirror material
    if (material.hasSpecular()) { //This is done for the mirror material
		const Vector3D wr = (r.d - 2.0 * dot(r.d, hit_normal) * hit_normal).normalized(); //We get the reflection direction
		Ray reflRay(hit.itsPoint + hit_normal * Epsilon, wr, r.depth + 1); //We create the reflection ray with a small offset to avoid problems
		final_color += computeColor(reflRay, objList, lsList); //We call the function recursively to get the color of the reflection ray
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
            final_color += computeColor(refrRay, objList, lsList); //We send the ray again as a recursive function
        }
        else {
            // If we are inside the else, means that we have total internal reflection

            // If we have total internal reflection, we will trat it as a mirror reflection, this means that we can directly copy the code used for the mirror.

            //------------- MIRROR CODE --------------
            const Vector3D wr = (r.d - 2.0 * dot(r.d, hit_normal) * hit_normal).normalized();
            Ray reflRay(hit.itsPoint + hit_normal * Epsilon, wr, r.depth + 1);
            final_color += computeColor(reflRay, objList, lsList);
            //------------- END MIRROR CODE --------------
        }
    }

    return final_color;  //We return the final color. 
}