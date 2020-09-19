#include "Shape.h"
#include "Scene.h"
#include <cstdio>
#include <limits>

Shape::Shape(void)
{
}

Shape::Shape(int id, int matIndex)
    : id(id), matIndex(matIndex)
{
}

Sphere::Sphere(void)
{}

/* Constructor for sphere. You will implement this. */
Sphere::Sphere(int id, int matIndex, int cIndex, float R, vector<Vector3f> *pVertices)
    : Shape(id, matIndex)
{
    /***********************************************
     *                                             *
     * TODO: Implement this function               *
     *                                             *
     ***********************************************
     */
    this->R = R;
    this->center = pVertices->at(cIndex-1);
}

/* Sphere-ray intersection routine. You will implement this. 
Note that ReturnVal structure should hold the information related to the intersection point, e.g., coordinate of that point, normal at that point etc. 
You should to declare the variables in ReturnVal structure you think you will need. It is in defs.h file. */
ReturnVal Sphere::intersect(const Ray & ray) const
{
    /***********************************************
     *                                             *
     * TODO: Implement this function               *
     *                                             *
     ***********************************************
     */
    ReturnVal return_val;

    Vector3f o_c;
    o_c.x = ray.origin.x - this->center.x;
    o_c.y = ray.origin.y - this->center.y;
    o_c.z = ray.origin.z - this->center.z;

    Vector3f direction;
    direction.x = ray.direction.x;
    direction.y = ray.direction.y;
    direction.z = ray.direction.z;

    float dd = direction.x*direction.x + direction.y*direction.y + direction.z*direction.z;


    float do_c = direction.x*o_c.x + direction.y*o_c.y + direction.z*o_c.z;

    float delta = ((4*do_c*do_c) - 4*dd*((o_c.x*o_c.x + o_c.y*o_c.y + o_c.z*o_c.z)-R*R));


    float t;

    if(delta < 0){
        return_val.intersection_check = false;

    }
    else{
        if(delta == 0){
            t = (-2*do_c + sqrt(delta))/(2*dd);
        }
        else if(delta > 0){
            t = (-2*do_c + sqrt(delta))/(2*dd);
            float t2 = (-2*do_c - sqrt(delta))/(2*dd);

            if(t2<t){
                t = t2;
            }
        }
        if(t<0){
            return_val.intersection_check = false;
        }
        else{
            Vector3f point = ray.getPoint(t);

            return_val.intersection_point = point;


            Vector3f surface_vector;
            surface_vector.x = point.x - this->center.x;
            surface_vector.y = point.y - this->center.y;
            surface_vector.z = point.z - this->center.z;

            float length = sqrt(surface_vector.x*surface_vector.x + surface_vector.y*surface_vector.y + surface_vector.z*surface_vector.z);

            return_val.surface_normal.x = surface_vector.x/length;
            return_val.surface_normal.y = surface_vector.y/length;
            return_val.surface_normal.z = surface_vector.z/length;

            return_val.t = t;
            return_val.intersection_check = true;
            return_val.id = this->id;
            return_val.matIndex = this->matIndex;
        }
    }

    return return_val;
}

Triangle::Triangle(void)
{}

/* Constructor for triangle. You will implement this. */
Triangle::Triangle(int id, int matIndex, int p1Index, int p2Index, int p3Index, vector<Vector3f> *pVertices)
    : Shape(id, matIndex)
{
    /***********************************************
     *                                             *
     * TODO: Implement this function               *
     *                                             *
     ***********************************************
     */
    this->a = pVertices->at(p1Index-1);
    this->b = pVertices->at(p2Index-1);
    this->c = pVertices->at(p3Index-1);
}

/* Triangle-ray intersection routine. You will implement this. 
Note that ReturnVal structure should hold the information related to the intersection point, e.g., coordinate of that point, normal at that point etc. 
You should to declare the variables in ReturnVal structure you think you will need. It is in defs.h file. */
ReturnVal Triangle::intersect(const Ray & ray) const
{
    /***********************************************
     *                                             *
     * TODO: Implement this function               *
     *                                             *
     ***********************************************
     */
    float beta, gamma, t;

    Vector3f a = this->a;
    Vector3f b = this->b;
    Vector3f c = this->c;

    float d, e, f, g, h, i, j, k, l, r1, r2, r3;
    d = a.x - b.x;
    e = a.y - b.y;
    f = a.z - b.z;
    g = a.x - c.x;
    h = a.y - c.y;
    i = a.z - c.z;
    j = ray.direction.x;
    k = ray.direction.y;
    l = ray.direction.z;
    r1 = a.x - ray.origin.x;
    r2 = a.y - ray.origin.y;
    r3 = a.z - ray.origin.z;

    float A[3][3] = {{d, g, j},
                     {e, h, k},
                     {f, i, l}};

    float A_beta[3][3] = {{r1, g, j},
                     {r2, h, k},
                     {r3, i, l}};

    float A_gamma[3][3] = {{d, r1, j},
                     {e, r2, k},
                     {f, r3, l}};

    float A_t[3][3] = {{d, g, r1},
                     {e, h, r2},
                     {f, i, r3}};

    float detA = d * (h * l - k * i) - e * (g * l - j * i) + f * (g * k - h * j);
    float detA_beta = r1 * (h * l - k * i) - r2 * (g * l - j * i) + r3 * (g * k - h * j);
    float detA_gamma = d * (r2 * l - k * r3) - e * (r1 * l - j * r3) + f * (r1 * k - r2 * j);
    float detA_t = d * (h * r3 - r2 * i) - e * (g * r3 - r1 * i) + f * (g * r2 - h * r1);

    beta = detA_beta / detA;
    gamma = detA_gamma / detA;
    t = detA_t / detA;

    ReturnVal returnVal;
    float eps = pScene->intTestEps;
    if((t > pScene->shadowRayEps) && (beta + gamma <= 1) && (-eps <= beta) && (-eps <= gamma)){
        returnVal.t = t;
        returnVal.id = this->id;
        returnVal.matIndex = this->matIndex;
        returnVal.intersection_check = true;
        returnVal.intersection_point = ray.getPoint(t);
        Vector3f ab, bc, normal;
        ab.x = b.x - a.x;
        ab.y = b.y - a.y;
        ab.z = b.z - a.z;
        bc.x = c.x - b.x;
        bc.y = c.y - b.y;
        bc.z = c.z - b.z;
        normal.x = ab.y * bc.z - ab.z * bc.y;
        normal.y = ab.z * bc.x - ab.x * bc.z;
        normal.z = ab.x * bc.y - ab.y * bc.x;
        float length = sqrt(pow(normal.x, 2) + pow(normal.y, 2) + pow(normal.z, 2));
        normal.x = normal.x / length;
        normal.y = normal.y / length;
        normal.z = normal.z / length;
        returnVal.surface_normal = normal;
    }
    else{
        returnVal.intersection_check = false;
    }
    
    return returnVal;

}

Mesh::Mesh()
{}

/* Constructor for mesh. You will implement this. */
Mesh::Mesh(int id, int matIndex, const vector<Triangle>& faces, vector<Vector3f> *pVertices)
    : Shape(id, matIndex)
{
    /***********************************************
     *                                             *
     * TODO: Implement this function               *
     *                                             *
     ***********************************************
     */
    this->faces = faces;
}

/* Mesh-ray intersection routine. You will implement this. 
Note that ReturnVal structure should hold the information related to the intersection point, e.g., coordinate of that point, normal at that point etc. 
You should to declare the variables in ReturnVal structure you think you will need. It is in defs.h file. */
ReturnVal Mesh::intersect(const Ray & ray) const
{
    /***********************************************
     *                                             *
     * TODO: Implement this function               *
     *                                             *
     ***********************************************
     */

    ReturnVal returnVal, value;
    returnVal.intersection_check = false;
    int len = this->faces.size();
    int i;
    float t_min=std::numeric_limits<float>::max();
    for(i=0; i<len; i++){
        value = this->faces[i].intersect(ray);
        if(value.intersection_check && value.t < t_min){
            returnVal.t = value.t;
            returnVal.id = this->id;
            returnVal.matIndex = this->matIndex;
            returnVal.intersection_point = value.intersection_point;
            returnVal.surface_normal = value.surface_normal;
            returnVal.intersection_check = value.intersection_check;
            t_min = returnVal.t;
        }
    }
    
    return returnVal;
}
