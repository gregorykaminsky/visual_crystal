#include <iostream>
#ifndef VEC3D_H
#define VEC3D_H

using namespace std;

/*
  * This class defines the object called Vec3d - which is just a 3 dimensional vector with required operations
  *  addition, subtraction, multiplication, dot multiplication, division and a cross product
*/

class Vec3d
{
    public:
        double x;
        double y;
        double z;

        Vec3d (double ix, double iy, double iz) { x = ix; y = iy; z = iz;}
        Vec3d (){x = 0; y = 0; z = 0;}
        Vec3d (const Vec3d &vec) {x = vec.x; y = vec.y; z = vec.z;}

        inline friend Vec3d operator+(const Vec3d &a, const Vec3d &b)
        {
            return Vec3d(a.x + b.x, a.y + b.y, a.z + b.z);
        }

        inline friend Vec3d operator-(const Vec3d &a, const Vec3d &b)
        {
            return Vec3d(a.x - b.x, a.y - b.y, a.z - b.z);
        }

        inline Vec3d operator*(double mult)
        {
            return Vec3d(this->x * mult, this->y * mult, this->z * mult);
        }

        inline Vec3d operator/(double div)
        {
            return Vec3d(this->x / div, this->y / div, this->z / div);
        }

        inline friend double operator*(const Vec3d a, const Vec3d b)
        {
            return a.x * b.x + a.y * b.y + a.z * b.z;
        }

        inline friend Vec3d operator*(double mul, const Vec3d b)
        {
            return Vec3d(b.x*mul, b.y*mul, b.z*mul);
        }

        Vec3d cross(Vec3d input)
        {
            Vec3d answer;
            answer.x = this->y*input.z - this->z*input.y;
            answer.y = -(this->x*input.z - this->z*input.x);
            answer.z = this->x*input.y - this->y*input.x;

            return answer;
        }

        inline friend bool operator==(Vec3d one, Vec3d two)
        {
            if(one.x != two.x) return false;
            if(one.y != two.y) return false;
            if(one.z != two.z) return false;

            return true;
        }
        inline double norm() { return sqrt(x*x + y*y + z*z);}

        inline void normalize()
        {
            x = x/norm();
            y = y/norm();
            z = z/norm();
        }

        inline Vec3d normalized()
        {
            return Vec3d (x/norm(), y/norm(), z/norm());
        }


   //prints out the vector in the terminal.
        inline void show(){ cout << "x: " << this->x << " y: " << this->y << " z: " << this->z << "\n";}
};
#endif   //VEC3D_H
