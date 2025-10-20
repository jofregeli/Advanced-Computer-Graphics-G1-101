#include "arealightsource.h"

AreaLightSource::AreaLightSource(Square* areaLightsource_) :
    myAreaLightsource(areaLightsource_)
{ }



Vector3D AreaLightSource::getIntensity() const
{
    return myAreaLightsource->getMaterial().getEmissiveRadiance();
}


Vector3D AreaLightSource::sampleLightPosition()   const
{
    //FILL(...)
    // 
    //New Randam Pos inside Area Lightsource



    //Select random point: 

    float x = rand() / (float) RAND_MAX *myAreaLightsource->v1.length();

	float y = rand() / (float) RAND_MAX *myAreaLightsource->v2.length();
    
    return  Vector3D(x, y, 0);
}

