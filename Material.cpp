#include "Material.h"
using namespace std;
Vector3f Material::shade(const Ray &ray,
    const Hit &hit,
    const Vector3f &dirToLight,
    const Vector3f &lightIntensity)
{
    // TODO implement Diffuse and Specular phong terms
    float dot = Vector3f::dot(hit.normal, dirToLight);
    float clamp = dot>0 ? dot:0;
    return clamp*lightIntensity*_diffuseColor;
}
