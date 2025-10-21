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

    float x = rand() / (float) RAND_MAX ;

	float y = rand() / (float) RAND_MAX ;
    
    return myAreaLightsource->corner + myAreaLightsource->v1 * x + myAreaLightsource->v2 * y;
}

