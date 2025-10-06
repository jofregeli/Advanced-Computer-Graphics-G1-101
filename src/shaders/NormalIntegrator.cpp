#include "NormalIntegrator.h"
#include "../core/utils.h"

NormalIntegrator::NormalIntegrator() : Shader(Vector3D(0.0, 0.0, 0.0)) { }

NormalIntegrator::NormalIntegrator(Vector3D bgColor_) : Shader(bgColor_) { }

Vector3D NormalIntegrator::computeColor(const Ray& r,
    const std::vector<Shape*>& objList,
    const std::vector<LightSource*>& lsList) const
{
    Intersection its;
    // Check for intersection and get intersection info
    if (Utils::getClosestIntersection(r, objList, its))
    {
        // Get normal and normalize
        Vector3D n = its.normal.normalized();

        // Map normal from [-1,1] to [0,1]
        Vector3D color = (n + Vector3D(1.0, 1.0, 1.0)) * 0.5;

        return color;
    }
    else
    {
        return bgColor;
    }
}
