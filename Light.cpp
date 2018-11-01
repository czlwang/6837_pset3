#include "Light.h"
    void DirectionalLight::getIllumination(const Vector3f &p, 
                                 Vector3f &tolight, 
                                 Vector3f &intensity, 
                                 float &distToLight) const
    {
        // the direction to the light is the opposite of the
        // direction of the directional light source

        // BEGIN STARTER
        tolight = -_direction;
        intensity  = _color;
        distToLight = std::numeric_limits<float>::max();
        // END STARTER
    }
    void PointLight::getIllumination(const Vector3f &p, 
                                 Vector3f &tolight, 
                                 Vector3f &intensity, 
                                 float &distToLight) const
    {
        
        // tolight, intensity, distToLight are outputs
        Vector3f d = _position-p;
        Vector3f d_normalize = d;
        d_normalize.normalize();
        
        tolight = d_normalize;
        intensity = _color/(d.absSquared()*_falloff);
        distToLight = d.abs();
    }

