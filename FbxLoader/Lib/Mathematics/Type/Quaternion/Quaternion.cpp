//
//  Quaternion.cpp
//  MatrixCalculater
//
//  Created by Tomoya Fujii on 2017/11/26.
//  Copyright © 2017年 TomoyaFujii. All rights reserved.
//

#include "Quaternion.h"

namespace myTools{
    Vector3 Quaternion::rotate(const Vector3 &v) const {
        Quaternion vq(v.x,v.y,v.z,0);
        vq = *this * vq * Quaternion(-this->v.x,-this->v.y,-this->v.z,this->w);
        Vector3 ret;
        ret.x = vq.v.x;
        ret.y = vq.v.y;
        ret.z = vq.v.z;
        return ret;
    }
    
    Quaternion operator+(const Quaternion& q1, const Quaternion& q2){
        return Quaternion(q1.v + q2.v, q1.w + q2.w);
    }
    Quaternion operator-(const Quaternion& q1, const Quaternion& q2){
        return Quaternion(q1.v - q2.v, q1.w - q2.w);
    }
    Quaternion operator*(const Quaternion& q, float scaler){
        return Quaternion(q.v * scaler, q.w * scaler);
    }
    Quaternion operator*(float scaler, const Quaternion& q){
        return Quaternion(q.v * scaler, q.w * scaler);
    }
    Quaternion operator/(const Quaternion& q, float scaler){
        return Quaternion(q.v / scaler, q.w / scaler);
    }
    Quaternion operator/(float scaler, const Quaternion& q){
        return Quaternion(q.v / scaler, q.w / scaler);
    }
    
    Quaternion operator*(const Quaternion& q1, const Quaternion& q2){
        return Quaternion(cross(q1.v, q2.v) + q2.w * q1.v + q1.w * q2.v,
                          q1.w * q2.w - dot(q1.v, q2.v));
    }
    
    
    Vector3 operator*(const Quaternion& q, const Vector3& v){
        Vector3 ret;
        Quaternion tmp = q * Quaternion(v.x,v.y,v.z,0.0);
        ret = tmp.v;
        return ret;
    }
    Vector3 operator*(const Vector3& v, const Quaternion& q){
        Vector3 ret;
        Quaternion tmp = Quaternion(v.x,v.y,v.z,0.0) * q;
        ret = tmp.v;
        return ret;
    }
    
    Quaternion MakeQuat(const Vector3& v, const float& rad){
        float sin = sinf(rad * 0.5f);
        Vector3 norm = Normalize(v);
        return Quaternion(norm.x * sin, norm.y * sin, norm.z * sin, cosf(rad * 0.5));
    }
    
    Quaternion MakeQuatVectorToVector(const Vector3& start, const Vector3& end){
        Vector3 normalizedS = Normalize(start);
        Vector3 normalizedE = Normalize(end);
        float cos = dot(normalizedS, normalizedE);
        Vector3 axis;
        if(cos < -1 + 0.0001f){
            axis = cross(Vector3(0,0,1), start);
            if(axis.Length() < 0.0001f){
                axis = cross(Vector3(1.0,0.0,0.0), start);
            }
            axis = Normalize(axis);
            return MakeQuat(axis, M_PI);
        }
        axis = Normalize(cross(normalizedS, normalizedE));
        float cosDev = sqrtf((1 + cos) * 0.5f);
        
        float sinDev = sqrtf((1 - cos) * 0.5f);
        return Quaternion(
                          axis.x * sinDev,
                          axis.y * sinDev,
                          axis.z * sinDev,
                          cosDev
                          );
    }
    
    Matrix4x4 QuatToMat(const Quaternion& q){
        Matrix4x4 retval;
        float xx = q.v.x * q.v.x;
        float yy = q.v.y * q.v.y;
        float zz = q.v.z * q.v.z;
        
        float xy = q.v.x * q.v.y;
        float xz = q.v.x * q.v.z;
        float xw = q.v.x * q.w;
        
        float yz = q.v.y * q.v.z;
        float yw = q.v.y * q.w;
        
        float zw = q.v.z * q.w;
        
        retval[0][0] = 1 - 2 * ( yy + zz);
        retval[0][1] = 2 * (xy + zw);
        retval[0][2] = 2 * (xz - yw);
        retval[0][3] = 0;
        
        retval[1][0] = 2 * (xy - zw);
        retval[1][1] = 1 - 2 * (xx + zz);
        retval[1][2] = 2 * (yz + xw);
        retval[1][3] = 0;
        
        retval[2][0] = 2 * (xz + yw);
        retval[2][1] = 2 * (yz - xw);
        retval[2][2] = 1 - 2 * (xx + yy);
        retval[2][3] = 0;
        
        retval[3][0] = 0;
        retval[3][1] = 0;
        retval[3][2] = 0;
        retval[3][3] = 1;
        
        return retval;
    }
    
    float dot(const Quaternion& q1, const Quaternion& q2){
        return q1.w * q2.w + dot(q1.v, q2.v);
    }
}// namespace myTools

