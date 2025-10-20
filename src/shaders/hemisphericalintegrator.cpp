#include "hemisphericalintegrator.h"

#include "../core/utils.h"
#include "../core/intersection.h"
#include "../shapes/shape.h"
#include "../materials/material.h"
#include "../lightsources/lightsource.h"

#include <algorithm>
#include <cmath>
#include <limits>

HemisphericalIntegrator::HemisphericalIntegrator()
    : bgColor(Vector3D(0.0)) { }

HemisphericalIntegrator::HemisphericalIntegrator(const Vector3D bgColor_)
    : bgColor(bgColor_) { }


Vector3D HemisphericalIntegrator::computeColor(const Ray& r,
                                               const std::vector<Shape*>& objList,
                                               const std::vector<LightSource*>& lsList,int N) const
{

    float L_o_dir = 0.0;




    float L_e = 2;
    float L_o_indir = 2;



    float L_r = L_o_dir + L_o_indir;
    float L_tot = L_e + L_r;
    return L_tot;
}

