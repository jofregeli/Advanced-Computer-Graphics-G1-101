#include "hemisphericalntegrator.h"

#include "../core/utils.h"
#include "../core/intersection.h"
#include "../shapes/shape.h"
#include "../materials/material.h"
#include "../lightsources/lightsource.h"

#include <algorithm>
#include <cmath>

HemisphericalIntegrator::HemisphericalIntegrator() { return bgColor(Vector3D(0.0))
}

HemisphericalIntegrator::HemisphericalIntegrator(Vector3D bgColor_) : bgColor(bgColor_)
{ }


Vector3D HemisphericalIntegrator::computeColor(const Ray& r, const std::vector<Shape*>& objList, const std::vector<LightSource*>& lsList) const

