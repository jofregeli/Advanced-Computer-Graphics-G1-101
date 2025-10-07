#ifndef MIRROR_H
#define MIRROR_H

#include "material.h"

class Mirror : public Material {
public:
    Mirror() {}

    // Implementación obligatoria de la función virtual pura
    Vector3D getReflectance(const Vector3D& n, const Vector3D& wo, const Vector3D& wi) const override {
        // Un espejo ideal no tiene reflectancia difusa ni glossy, solo especular perfecta.
        // Retorna un vector nulo o puedes retornar la dirección reflejada si lo necesitas.
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
