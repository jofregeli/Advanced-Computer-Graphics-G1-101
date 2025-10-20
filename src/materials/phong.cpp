#include "phong.h"
#include <algorithm>
#include <cmath>
#include <iostream>

Phong::Phong()
{ }

Phong::Phong(Vector3D Kd_, Vector3D Ks_, float alpha_):
kd(Kd_), Ks(Ks_), alpha(alpha_){}


Vector3D Phong::getReflectance(const Vector3D& n,
    const Vector3D& wo,
    const Vector3D& wi) const
{
	Vector3D n_normalized = n.normalized(); //We normalize the normal vector
	Vector3D wo_normalized = wo.normalized(); //We normalize the wo vector
	Vector3D wi_normalized = wi.normalized(); //We normalize the wi vector

    // Compute:  wr = 2(n·wi)n - wi
    Vector3D wr = (n_normalized * (2.0 * dot(n_normalized, wi_normalized))) - wi_normalized; //We compute the wr with the formula given in theory

    // We compute  ρd / pi
	Vector3D equation_1 = kd * (1.0 / 3.14159265); //We compute the first part of the equation

    // The final part of the equation will be =  Ks * (wo·wr)^α
    double dot_product = dot(wr, wo_normalized); //We compute the (wo·wr)
	Vector3D equation_2 = Ks *((alpha+2)/(2 * 3.14159265))  * pow(dot_product, (double)alpha); //We use the dot_product result to compute the second part of the equation ( Ks * (wo·wr)^α)

    return equation_1 +equation_2; //We return the sum of both parts of the equation. This will be the BRDF of the Phong material
}


// ---------------------------------------------------------  LAB 1 PHONG ---------------------------------------------------------

//Vector3D Phong::getReflectance(const Vector3D& n,
//    const Vector3D& wo,
//    const Vector3D& wi) const
//{
//    Vector3D n_normalized = n.normalized(); //We normalize the normal vector
//    Vector3D wo_normalized = wo.normalized(); //We normalize the wo vector
//    Vector3D wi_normalized = wi.normalized(); //We normalize the wi vector
//
//    // Compute:  wr = 2(n·wi)n - wi
//    Vector3D wr = (n_normalized * (2.0 * dot(n_normalized, wi_normalized))) - wi_normalized; //We compute the wr with the formula given in theory
//
//    // We compute  ρd / pi
//    Vector3D equation_1 = pd * (1.0 / 3.14159265); //We compute the first part of the equation
//
//    // The final part of the equation will be =  Ks * (wo·wr)^α
//    double dot_product = dot(wo_normalized, wr); //We compute the (wo·wr)
//    Vector3D equation_2 = Ks * pow(dot_product, (double)alpha); //We use the dot_product result to compute the second part of the equation ( Ks * (wo·wr)^α)
//
//    return equation_1 + equation_2; //We return the sum of both parts of the equation. This will be the BRDF of the Phong material
//}
// ---------------------------------------------------------  LAB 1 PHONG ---------------------------------------------------------



double Phong::getIndexOfRefraction() const
{
    std::cout << "Warning! Calling \"Material::getIndexOfRefraction()\" for a non-transmissive material"
              << std::endl;

    return -1;
}


Vector3D Phong::getEmissiveRadiance() const
{
    return Vector3D(0.0);
}



// --- LAB 1 ---
//Vector3D Phong::getDiffuseReflectance() const
//{
//    return pd;
//}

Vector3D Phong::getDiffuseReflectance() const
{
    return kd;
}