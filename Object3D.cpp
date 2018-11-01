#include "Object3D.h"

bool Sphere::intersect(const Ray &r, float tmin, Hit &h) const
{
    // BEGIN STARTER

    // We provide sphere intersection code for you.
    // You should model other intersection implementations after this one.

    // Locate intersection point ( 2 pts )
    const Vector3f &rayOrigin = r.getOrigin(); //Ray origin in the world coordinate
    const Vector3f &dir = r.getDirection();

    Vector3f origin = rayOrigin - _center;      //Ray origin in the sphere coordinate

    float a = dir.absSquared();
    float b = 2 * Vector3f::dot(dir, origin);
    float c = origin.absSquared() - _radius * _radius;

    // no intersection
    if (b * b - 4 * a * c < 0) {
        return false;
    }

    float d = sqrt(b * b - 4 * a * c);

    float tplus = (-b + d) / (2.0f*a);
    float tminus = (-b - d) / (2.0f*a);

    // the two intersections are at the camera back
    if ((tplus < tmin) && (tminus < tmin)) {
        return false;
    }

    float t = 10000;
    // the two intersections are at the camera front
    if (tminus > tmin) {
        t = tminus;
    }

    // one intersection at the front. one at the back 
    if ((tplus > tmin) && (tminus < tmin)) {
        t = tplus;
    }

    if (t < h.getT()) {
        Vector3f normal = r.pointAtParameter(t) - _center;
        normal = normal.normalized();
        h.set(t, this->material, normal);
        return true;
    }
    // END STARTER
    return false;
}

// Add object to group
void Group::addObject(Object3D *obj) {
    m_members.push_back(obj);
}

// Return number of objects in group
int Group::getGroupSize() const {
    return (int)m_members.size();
}

bool Group::intersect(const Ray &r, float tmin, Hit &h) const
{
    // BEGIN STARTER
    // we implemented this for you
    bool hit = false;
    for (Object3D* o : m_members) {
        if (o->intersect(r, tmin, h)) {
            hit = true;
        }
    }
    return hit;
    // END STARTER
}

Plane::Plane(const Vector3f &normal, float d, Material *m) : Object3D(m) {
    _normal = normal;
    _d = d;
}
bool Plane::intersect(const Ray &r, float tmin, Hit &h) const
{
    float t = -(-_d + Vector3f::dot(_normal, r.getOrigin())) / Vector3f::dot(_normal, r.getDirection());
    if(t <= tmin)
    {
        return false;
    }
    if (t < h.getT()) {
        Vector3f normal = _normal;
        normal = normal.normalized();
        h.set(t, this->material, normal);
        return true;
    }
    return false;
}
bool Triangle::intersect(const Ray &r, float tmin, Hit &h) const 
{
    // TODO implement
    Matrix3f A = Matrix3f(_v[0]-_v[1], _v[0]-_v[2], r.getDirection());
    Vector3f sol = A.inverse()*(_v[0]-r.getOrigin());
    float beta = sol[0];
    float gamma = sol[1];
    float alpha = 1 - beta - gamma;
    float t = sol[2];
    
    if(!(beta + gamma <= 1 && beta >= 0 && gamma >=0) || t<=tmin)
    {
        return false;
    }
    
    if (t < h.getT()) {
        Vector3f normal = alpha*_normals[0] + beta*_normals[1] + gamma*_normals[2];
        normal = normal.normalized();
        h.set(t, this->material, normal);
        return true;
    }
    return false;
}


Transform::Transform(const Matrix4f &m,
    Object3D *obj) : _object(obj) {
    // TODO implement Transform constructor
    _M = m;
}
bool Transform::intersect(const Ray &r, float tmin, Hit &h) const
{
//    return _object->intersect(r, tmin, h);
    Matrix4f M_inverse = _M.inverse();
    Vector4f newOrigin = M_inverse*Vector4f(r.getOrigin(),1);
    Vector4f newDirection = M_inverse*Vector4f(r.getDirection(),0);
    Ray new_r = Ray(newOrigin.xyz(), newDirection.xyz());
    bool result = _object->intersect(new_r, tmin, h);
    if(result)
    {
        Vector4f normal = Vector4f(h.getNormal(),0);
        Matrix4f M_inverse_transpose = M_inverse;
        M_inverse_transpose.transpose();
        Vector3f newNormal = (M_inverse_transpose*normal).xyz();
        newNormal.normalize();
        h.set(h.getT(), h.material, newNormal);
    }
    return result;
}

