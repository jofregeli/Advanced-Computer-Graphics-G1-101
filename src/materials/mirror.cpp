#include "mirror.h"
#include <algorithm>
#include <cmath>
#include <iostream>

Mirror::Mirror()
{
}

Mirror::Mirror(Vector3D Kd_, Vector3D Ks_, float alpha_) :
    rho_d(Kd_), Ks(Ks_), alpha(alpha_) {
}


Vector3D Mirror::getReflectance(const Vector3D& n,
    const Vector3D& wo,
    const Vector3D& wi) const
{
    Vector3D nn = n.normalized();
    Vector3D woi = wo.normalized();
    Vector3D wii = wi.normalized();

    // Ideal reflection direction wr = 2(n·wi)n - wi (Eq. 5)
    Vector3D wr = (nn * (2.0 * dot(nn, wii))) - wii;

    // Diffuse term ρd / π
    Vector3D diffuse = rho_d * (1.0 / 3.14159265);

    // Specular term Ks * (max(0, wo·wr))^α
    double cos_rw = std::max(0.0, dot(woi, wr));
    Vector3D specular = Ks * std::pow(cos_rw, (double)alpha);

    return diffuse + specular;
}

double Mirror::getIndexOfRefraction() const
{
    std::cout << "Warning! Calling \"Material::getIndexOfRefraction()\" for a non-transmissive material"
        << std::endl;

    return -1;
}


Vector3D Mirror::getEmissiveRadiance() const
{
    return Vector3D(0.0);
}


Vector3D Mirror::getDiffuseReflectance() const
{
    return rho_d;
}