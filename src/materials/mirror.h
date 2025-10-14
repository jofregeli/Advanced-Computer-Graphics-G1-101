#ifndef MIRROR_H
#define MIRROR_H

#include "material.h"

class Mirror : public Material {
public:
    Mirror() {}

    Vector3D getReflectance(const Vector3D& n, const Vector3D& wo, const Vector3D& wi) const override {
       
        return Vector3D(0.0);
    }

    bool hasSpecular()        const override { return true; }
    bool hasTransmission()    const override { return false; }
    bool hasDiffuseOrGlossy() const override { return false; }
    bool isEmissive()         const override { return false; }

    // Irrelevante para espejo, valores neutros
    double   getIndexOfRefraction() const override { return -1; }
    Vector3D getEmissiveRadiance()  const override { return Vector3D(0.0); }
    Vector3D getDiffuseReflectance()const override { return Vector3D(0.0); }
};

#endif // MIRROR_H
