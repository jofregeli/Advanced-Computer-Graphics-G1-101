#include "intersectionshader.h"
#include "../core/utils.h"

IntersectionShader::IntersectionShader() :
    hitColor(Vector3D(1, 0, 0))
{ }

IntersectionShader::IntersectionShader(Vector3D hitColor_, Vector3D bgColor_) :
    Shader(bgColor_), hitColor(hitColor_)
{ }

Vector3D IntersectionShader::computeColor(const Ray &r, const std::vector<Shape*> &objList, const std::vector<LightSource*> &lsList) const
{
    //(FILL..)
	Intersection its;
	if (Utils::hasIntersection(r, objList))
        if (its.shape->getMaterial().isEmissive())
            return its.shape->getMaterial().getEmissiveRadiance();
        else {
			return its.shape->getMaterial().getDiffuseReflectance();
        }

    return bgColor;
}
