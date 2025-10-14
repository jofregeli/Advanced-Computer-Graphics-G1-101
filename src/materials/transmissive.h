#ifndef TRANSMISSIVE_H
#define TRANSMISSIVE_H

#include "material.h"

// Perfect specular transmission (glass-like). Handled in the integrator.
class Transmissive : public Material {
public:
    explicit Transmissive(double eta_ = 1.5) : eta(eta_) {}

    // Not used for delta materials; integrator spawns the refracted ray.
    Vector3D getReflectance(const Vector3D&, const Vector3D&, const Vector3D&) const override {
        return Vector3D(0.0);
    }

    bool hasSpecular()        const override { return false; }
    bool hasTransmission()    const override { return true; }
    bool hasDiffuseOrGlossy() const override { return false; }
    bool isEmissive()         const override { return false; }

    Vector3D getDiffuseReflectance() const override { return Vector3D(0.0); }
    Vector3D getEmissiveRadiance()   const override { return Vector3D(0.0); }
    double   getIndexOfRefraction()  const override { return eta; }

private:
    double eta; 
};

#endif // TRANSMISSIVE_H
