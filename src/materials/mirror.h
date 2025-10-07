#ifndef MIRROR_H
#define MIRROR_H

#include "material.h"

class Mirror : public Material
{
public:
    Mirror();


    Mirror(Vector3D Kd_, Vector3D Ks_, float    alpha_);

    Vector3D getReflectance(const Vector3D& n, const Vector3D& wo,
        const Vector3D& wi)const;

    bool hasSpecular() const { return true; }
    bool hasTransmission() const { return false; }
    bool hasDiffuseOrGlossy() const { return false; }
    bool isEmissive() const { return false; }



    double getIndexOfRefraction() const;
    Vector3D getEmissiveRadiance() const;
    Vector3D getDiffuseReflectance() const;


private:
    Vector3D rho_d;
    Vector3D Ks;
    float    alpha;
};

#endif // MIRROR_H