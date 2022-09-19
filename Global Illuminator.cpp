#include <math.h>
#include <ctime>
#include <cstdlib>
#include <stdlib.h>
#include <stdio.h>

#include <iostream>

// global coefficient 
double kd = 0.6;                        // diffuse coefficient
double ks = 0.6;                        // specular coefficient
double lambda = 0.001;                  // reflect decay coefficient
enum Material { DIFF, SPEC, REFR };     // reflection type, DIFF = diffusive, SPEC = specular, REFR = refractive


struct Vector3 
{
    double x, y, z;
    Vector3 (double x_ = 0, double y_ = 0, double z_ = 0) {
        x = x_;
        y = y_;
        z = z_;
    }

    Vector3 operator+ (Vector3 vec) { return Vector3(x+vec.x, y+vec.y, z+vec.z);}

    Vector3 operator- (Vector3 vec) { return Vector3(x-vec.x, y-vec.y, z-vec.z);}

    Vector3 operator* (double scalar) { return Vector3(x*scalar, y*scalar, z*scalar);}

    Vector3 operator% (double scalar) { return Vector3(x/scalar, y/scalar, z/scalar);}

    Vector3 Mul (Vector3 vec) { return Vector3(x*vec.x, y*vec.y, z*vec.z);}

    double Dot (Vector3 vec) { return x*vec.x + y*vec.y + z*vec.z;}

    Vector3 Cross (Vector3 vec) { return Vector3(y*vec.z - z*vec.y, z*vec.x - x*vec.z, x*vec.y - y*vec.x);}

    Vector3 Norm() { return sqrt(x*x + y*y + z*z);}

    Vector3 UnitVector () {
        double norm = sqrt(x*x + y*y + z*z);
        return Vector3(x/norm, y/norm, z/norm);
    }
};


struct Ray 
{
    Vector3 position, direction;
    Ray (Vector3 position_, Vector3 direction_) {
        position = position_;
        direction = direction_;
    }
};

struct Sphere
{
    double radius;
    Vector3 position, color;
    Material material;

    Sphere (Vector3 position_, double radius_, Vector3 color_, Material material_) {
        position = position_;
        radius = radius_;
        color = color_;
        material = material_;
    }

    double Intersection (Ray ray) {
        // solving: (d.d)*t*t - [d.(e-c)]*2*t + (e-c).(e-c) - R*R = 0
        Vector3 e2c = ray.position - position;       // the vector between ray origin and sphere center, (e-c)
        double eps = 1e-4; 
        double a = ray.direction.Dot(ray.direction); // d is a unit vector, hence d.d = 1
        double b = ray.direction.Dot(e2c) * 2;       // we exclude coefficient 2 for easier compute, d.(e-c)
        double c = e2c.Dot(e2c) - radius*radius;     // (e-c).(e-c) - R*R
        double det = b*b - a*c*4;                    // we exclude coefficient 4 for easier compute, 

        if (det < 0) { return 0; }

        // solving t
        double ans1 = (-b + sqrt(det)) / 2*a;
        double ans2 = (-b - sqrt(det)) / 2*a;
        return (ans2)>eps ? ans2 : (ans1>eps ? ans1 : 0); 
    }
};


Sphere spheres[] = {//Scene: position, radius, color, material
    // Sphere(Vector3(0,  0,        6),        5,  Vector3(.75, .25, .25), DIFF),
    Sphere(Vector3(-1e5+1.25,  500,        500),        1e5, Vector3(.75, .25, .25), DIFF),   //Left 0 
    Sphere(Vector3(1e5+998.75, 500,        500),        1e5, Vector3(.25, .25, .75), DIFF),   //Rght 1 
    Sphere(Vector3(500,        500,        1e5+998.75), 1e5, Vector3(.75, .75, .75), DIFF),   //Back 2 
    Sphere(Vector3(500,        500,        -1e5),       1e5, Vector3(             ), DIFF),   //Frnt 3 
    Sphere(Vector3(500,        -1e5+1.25,  500),        1e5, Vector3(.75, .75, .75), DIFF),   //Botm 4 
    Sphere(Vector3(500,        1e5+998.75, 500),        1e5, Vector3(.75, .75, .75), DIFF),   //Top  5
    Sphere(Vector3(150,        400,        100),        100, Vector3(1, 1, 1)*.999,  SPEC),   //Mirr 6 
    Sphere(Vector3(350,        200,        110),        100, Vector3(1, 1, 1)*.999,  SPEC),   //Mirr 7
    Sphere(Vector3(350,        800,        110),        100, Vector3(1, 1, 1)*.999,  SPEC),   //Mirr 8
    Sphere(Vector3(600,        500,        110),        100, Vector3(1, 1, 1)*.999,  DIFF),   //Norm 9
    Sphere(Vector3(850,        750,        450),        100, Vector3(1, 1, 1)*.999,  DIFF),   //Norm 10  
    Sphere(Vector3(500,        1e5+998,    500),        1e5, Vector3(),              DIFF)    //Light 11

};


double Random() {
    return (double)rand() / (double)RAND_MAX;
}


double Max(double x, double y) {
    return x > y ? x : y;
}


double Clamp (double x) {
    return x < 0 ? 0 : x > 1 ? 1 : x;               // CLAMP function,  0 < x < 1
}


int ToInt (double x) {
    return int(pow(Clamp(x), 1/2.2) * 255 + 0.5);   // convert floats to integers to be save in ppm file, 2.2 is gamma correction
}


bool IsIntersect (Ray ray, double &t, int &id) {
    int sphere_num = sizeof(spheres) / sizeof(Sphere);
    double temp;
    double inf = 1e20;
    t = inf;

    for (int i = 0; i < int(sphere_num); i++) {
        temp = spheres[i].Intersection(ray);
        if(temp != 0 && temp < t) {                 // compute the closest intersection
            t = temp;
            id = i;
        }
    }
    return t < inf;
}


Vector3 TracingAndShading (Ray ray, int depth) {

    double t;
    int id = 0;
    depth ++;

    if (depth > 15) {
        return Vector3(-1.0, -1.0, -1.0);           // ray can reflect for 15 times in maximum, if larger than 15, assume the ray is not from light source
    }
    if (IsIntersect(ray, t, id) == false) {         // if not hit any object, which means the ray is from the void
        return Vector3(-1.0, -1.0, -1.0);
    } 
    if (id == 11) {                                 // if hits light source, end ray tracing recursion
        return Vector3();
    }   

    Sphere obj = spheres[id];
    Vector3 p = ray.position + ray.direction * t;       // ray-sphere hit point
    Vector3 n = (p - obj.position).UnitVector();        // sphere normal vector at hit point 
    Vector3 nl = n.Dot(ray.direction) < 0 ? n : n*-1;   // properly oriented sphere normal vector, telling the ray in entering or exiting.

    if (obj.material == DIFF) {                         // ideal diffusive reflaction
        double r1 = 2 * M_PI * Random();                // angle around
        double r2 = Random();                           // distance from center

        Vector3 w = nl;                                 // w = sphere normal 
        Vector3 u = ((fabs(w.x) > 0.1 ? Vector3(0,1) : Vector3(1)).Cross(w)).UnitVector();  // u is perpendicular to w
        Vector3 v = w.Cross(u);                                                             // v is perpendicular to w and v
        Vector3 d = (u*cos(r1)*sqrt(r2) + v*sin(r1)*sqrt(r2) + w*sqrt(1-r2)).UnitVector();  // random generated reflection ray direction
        Vector3 h = (ray.direction*-1 + d).UnitVector();                                    // the vector of bisector of origin ray and reflected ray

        Ray reflect_ray(p, d);
        Vector3 reflect_color = TracingAndShading(reflect_ray, depth);
        if (reflect_color.x == -1.0) {                  // if the ray is not from light source
            return reflect_color;
        } 
        return obj.color*kd*Max(0, n.Dot(d)) + obj.color*ks*Max(0, n.Dot(h)) + reflect_color*lambda;
    }
    
    // ideal specular reflaction
    Vector3 d = ray.direction - n*2*n.Dot(ray.direction); 
    Ray reflect_ray(p, d);
    Vector3 reflect_color = TracingAndShading(reflect_ray, depth); 
    if (reflect_color.x == -1.0) {
        return reflect_color;
    }
    return obj.color*kd*Max(0, n.Dot(d)) + obj.color*ks + reflect_color*lambda;

    /* ideal dielectric refraction */
    // Ray reflect_ray = (p, ray.direction - n*2*n.Dot(ray.direction));
    // bool into = n.Dot(nl) > 0;                  // ray from outside going in?
    // double nc = 1;
    // double nt = 1.5;
    // double nnt = into ? nc/nt : nt/nc;
    // double ddn = ray.direction.Dot(nl); 
    // double cos2t = 1 - nnt*nnt(1-ddn*ddn);

    // if (cos2t < 0) {                            // total internal reflection
    //     Vector3 reflect_color = TracingAndShading(reflect_ray, depth);
    //     if (reflect_color.x == -1.0) {
    //         return reflect_color;
    //     }
    //     return obj.color*kd + obj.color*ks + reflect_color*lambda;
    // }

    // Vector3 tdir = (ray.direction*nnt - n*((into?1:-1)*(ddn*nnt + sqrt(cos2t)))).UnitVector();
    // double a = nt - nc;
    // double b = nt + nc;
    // double c = 1 - (into ? -ddn : tdir.Dot(n));
    // double R0 = a*a/(b*b);
    // double Re = R0 + (1-R0)*c*c*c*c*c;
    // double Tr = 1 - Re;
    // double P = 0.25 + 0.5*Re;
    // double RP = Re / P;
    // double TP = Tr / (1-P);

    // return obj.color*kd + obj.color*ks + (depth > 2 ? (Random < P ? 
    //     TracingAndShading(reflect_ray, depth)*RP : TracingAndShading(Ray(p,tdir), depth)*TP) : 
    //     TracingAndShading(reflect_ray, depth)*Re + TracingAndShading(Ray(p,tdir), depth)*Tr);
}


int main(int argc, char* argv[]) {
    int hight = 1000;
    int width = 1000;
    unsigned seed = time(0);
    srand(seed);

    Vector3 origin(0.0, 0.0, 0.0);
    Vector3 direction(0.0, 0.0, 1.0);
    Vector3 *image = new Vector3[width * hight];

    /* main ray tracing loop */
    for (int row = 0; row < hight; row++) {                     // looping over image rows
        for (int col = 0; col < width; col++) {                 // looping over image colomns
            int index = (hight-row-1)*width + col;              // map 2D pixel location to 1D vector
            Vector3 pixel_color(0.0, 0.0, 0.0);

            for (int srow = 0; srow < 2; srow++) {              // looping over 2x2 subsample rows
                for (int scol = 0; scol < 2; scol ++) {         // looping over 2x2 subsample colomns
                    Vector3 offset(double(row)+double(srow), double(col)+double(scol), 0.0);
                    Ray camera(offset, direction);
                    int depth = 0;

                    pixel_color = pixel_color + TracingAndShading(camera, depth) * 0.25;
                }
            }
            image[index] = image[index] + Vector3(Clamp(pixel_color.x), Clamp(pixel_color.y), Clamp(pixel_color.z));
        }
    }

    FILE *file = fopen("image.ppm", "w");
    fprintf(file, "P3\n%d %d\n%d\n", width, hight, 255);
    for (int i = 0; i < width*hight; i++) {
        fprintf(file, "%d %d %d ", ToInt(image[i].x), ToInt(image[i].y), ToInt(image[i].z));
    }
}