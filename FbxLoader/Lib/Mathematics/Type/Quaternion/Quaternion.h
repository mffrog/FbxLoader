//
//  Quaternion.h
//  MatrixCalculater
//
//  Created by Tomoya Fujii on 2017/11/26.
//  Copyright © 2017年 TomoyaFujii. All rights reserved.
//

#ifndef Quaternion_h
#define Quaternion_h

#include "Vector.h"
#include "Matrix.h"
#include <math.h>
namespace myTools{
    
    inline float ToRadian(float angle){
        return angle * M_PI / 180;
    }
    
    struct Quaternion{
        Vector3 v;
        float w;
        Quaternion(float x = 0, float y = 0, float z = 0, float w = 1)
        : v(x,y,z), w(w)
        {}
        Quaternion(Vector3 v, float w)
        : v(v), w(w)
        {}
        
        Vector3 rotate(const Vector3& v) const;
    };
    
    Quaternion operator+(const Quaternion& q1, const Quaternion& q2);
    Quaternion operator-(const Quaternion& q1, const Quaternion& q2);
    Quaternion operator*(const Quaternion& q, float scaler);
    Quaternion operator*(float scaler, const Quaternion& q);
    Quaternion operator/(const Quaternion& q, float scaler);
    Quaternion operator/(float scaler, const Quaternion& q);
    
    Quaternion operator*(const Quaternion& q1, const Quaternion& q2);
    
    Vector3 operator*(const Quaternion& q, const Vector3& v);
    Vector3 operator*(const Vector3& v, const Quaternion& q);
    
    
    Quaternion MakeQuat(const Vector3& v, const float& rad);
    Quaternion MakeQuatVectorToVector(const Vector3& start, const Vector3& end);
    Matrix4x4 QuatToMat(const Quaternion& q);
    
    float dot(const Quaternion& q1, const Quaternion& q2);
}// namespace myTools
#endif /* Quaternion_h */

