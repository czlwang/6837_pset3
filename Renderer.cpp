#include "Renderer.h"

#include "ArgParser.h"
#include "Camera.h"
#include "Image.h"
#include "Ray.h"
#include "VecUtils.h"

#include <limits>


Renderer::Renderer(const ArgParser &args) :
    _args(args),
    _scene(args.input_file)
{
}

void
Renderer::Render()
{
    int w = _args.width;
    int h = _args.height;

    Image image(w, h);
    Image nimage(w, h);
    Image dimage(w, h);

    // loop through all the pixels in the image
    // generate all the samples

    // This look generates camera rays and callse traceRay.
    // It also write to the color, normal, and depth images.
    // You should understand what this code does.
    Camera* cam = _scene.getCamera();
    for (int y = 0; y < h; ++y) {
        float ndcy = 2 * (y / (h - 1.0f)) - 1.0f;
        for (int x = 0; x < w; ++x) {
            float ndcx = 2 * (x / (w - 1.0f)) - 1.0f;
            // Use PerspectiveCamera to generate a ray.
            // You should understand what generateRay() does.
            Ray r = cam->generateRay(Vector2f(ndcx, ndcy));

            Hit h;
            Vector3f color = traceRay(r, cam->getTMin(), _args.bounces, h);

            image.setPixel(x, y, color);
            nimage.setPixel(x, y, (h.getNormal() + 1.0f) / 2.0f);
            float range = (_args.depth_max - _args.depth_min);
            if (range) {
                dimage.setPixel(x, y, Vector3f((h.t - _args.depth_min) / range));
            }
        }
    }
    // END SOLN

    // save the files 
    if (_args.output_file.size()) {
        image.savePNG(_args.output_file);
    }
    if (_args.depth_file.size()) {
        dimage.savePNG(_args.depth_file);
    }
    if (_args.normals_file.size()) {
        nimage.savePNG(_args.normals_file);
    }
}


Vector3f
Renderer::traceRay(const Ray &r,
    float tmin,
    int bounces,
    Hit &h) const
{
    float epsilon = 0.01;
    // The starter code only implements basic drawing of sphere primitives.
    // You will implement phong shading, recursive ray tracing, and shadow rays.
    if (_scene.getGroup()->intersect(r, tmin, h)) {
        Vector3f shade = Vector3f::ZERO;
        Vector3f p = r.pointAtParameter(h.getT());
        for(int i=0; i<_scene.getNumLights(); i++){
            Vector3f toLight;
            Vector3f intensity;
            float distanceToLight;
            _scene.getLight(i)->getIllumination(p, toLight, intensity, distanceToLight);
            
            Vector3f shadowDirection = toLight.normalized();
            Vector3f shadowOrigin = p + shadowDirection*epsilon;
            Hit shadowHit;
            Ray shadowRay = Ray(shadowOrigin, shadowDirection);
            bool shadowed = false;
            if(_scene.getGroup()->intersect(shadowRay, tmin, shadowHit)){
                float shadowDistance = (p - shadowRay.pointAtParameter(shadowHit.getT())).abs();
                if(shadowDistance < distanceToLight){
                    shadowed = true && _args.shadows;
                }
            }
            if(!shadowed){
                shade += h.getMaterial()->shade(r, h, toLight, intensity);
            }
        }
        Vector3f ambient = _scene.getAmbientLight()*h.getMaterial()->getDiffuseColor();
        Vector3f direct = shade + ambient;
        Vector3f indirect = Vector3f::ZERO;
        if(bounces > 0){
            Vector3f reflected = r.getDirection() - 2*Vector3f::dot(r.getDirection(), h.getNormal().normalized())*h.getNormal().normalized();
            reflected = reflected.normalized();
            Vector3f reflectedOrigin = p + reflected*epsilon;
            Ray reflectedRay = Ray(reflectedOrigin, reflected);
            Hit reflectHit;
            indirect = traceRay(reflectedRay, tmin, bounces - 1, reflectHit);
            indirect = h.getMaterial()->getSpecularColor()*indirect;
        }
        return direct+indirect;
    }
    return _scene.getBackgroundColor(r.getDirection());
}

