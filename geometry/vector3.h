//
// Created by RainSure on 24-8-23.
//

#ifndef POISSONRECONCGAL_VECTOR3_H
#define POISSONRECONCGAL_VECTOR3_H

#include <iostream>

struct Vector3 {
    double x, y, z;

    Vector3(double x = 0, double y = 0, double z = 0) : x(x), y(y), z(z) {}

    Vector3 operator-(const Vector3& v) const {
        return Vector3(x - v.x, y - v.y, z - v.z);
    }

    Vector3 cross(const Vector3& v) const {
        return Vector3(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
    }

    Vector3 operator+(const Vector3& v) const {
        return Vector3(x + v.x, y + v.y, z + v.z);
    }

    Vector3 operator*(double c) const {
        return Vector3(x * c, y * c, z * c);
    }

    Vector3 operator/(double c) const {
        return Vector3(x / c, y / c, z / c);
    }

    double magnitude() const {
        return sqrt(x * x + y * y + z * z);
    }

    Vector3 normalize() const {
        double mag = magnitude();
        return mag == 0 ? Vector3(0, 0, 0) : *this / mag;
    }

    bool operator<(const Vector3& v) const {
        if (x != v.x) return x < v.x;
        if (y != v.y) return y < v.y;
        return z < v.z;
    }
};

// 输出向量
std::ostream& operator<<(std::ostream& os, const Vector3& v) {
    os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
    return os;
}


#endif //POISSONRECONCGAL_VECTOR3_H
