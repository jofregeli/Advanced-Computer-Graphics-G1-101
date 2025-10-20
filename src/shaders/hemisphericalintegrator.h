        #pragma once

#include <vector>

class HemisphericalIntegrator {
public:
    HemisphericalIntegrator();
    explicit HemisphericalIntegrator(const Vector3D bgColor_);

    Vector3D computeColor(const Ray& r,
                          const std::vector<Shape*>& objList,
                          const std::vector<LightSource*>& lsList, int N) const;

private:
    Vector3D bgColor;
};
