#include "Material.h"
using namespace std;
Vector3f Material::shade(const Ray &ray,
    const Hit &hit,
    const Vector3f &dirToLight,
    const Vector3f &lightIntensity)
{
    // TODO implement Diffuse and Specular phong terms
    float dot = Vector3f::dot(hit.getNormal(), -dirToLight);
    float clamp = dot>0 ? dot:0;
    Vector3f diffuse = clamp*lightIntensity*_diffuseColor;
    
    Vector3f reflected = ray.getDirection() - 2*Vector3f::dot(ray.getDirection(), hit.normal)*hit.normal;
    dot = Vector3f::dot(-dirToLight, reflected);
    clamp = dot>0 ? dot:0;
    clamp = std::pow(clamp, _shininess);
    Vector3f specular = clamp*lightIntensity*_specularColor;
    
    return specular + diffuse;
}
