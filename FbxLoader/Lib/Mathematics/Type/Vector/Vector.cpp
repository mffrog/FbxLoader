//
//  Vector.cpp
//  MatrixCalculater
//
//  Created by Tomoya Fujii on 2017/11/26.
//  Copyright © 2017年 TomoyaFujii. All rights reserved.
//

#include "Vector.h"
#include <math.h>
namespace myTools{
    
    //  Vector2
    float Vector2::Norm() const{
        return sqrt(x * x + y * y);
    }
    
    Vector2 operator+(const Vector2& v1, const Vector2& v2){
        return Vector2(v1.x + v2.x, v1.y + v2.y);
    }
    Vector2 operator-(const Vector2& v1, const Vector2& v2){
        return Vector2(v1.x - v2.x, v1.y - v2.y);
    }
    Vector2 operator*(const Vector2& v, float scaler){
        return Vector2(v.x * scaler, v.y * scaler);
    }
    Vector2 operator*(float scaler, const Vector2& v){
        return Vector2(v.x * scaler, v.y * scaler);
    }
    Vector2 operator/(const Vector2& v,float scaler){
        return Vector2(v.x / scaler, v.y / scaler);
    }
    Vector2 operator/(float scaler, const Vector2& v){
        return Vector2(v.x / scaler, v.y / scaler);
    }
    
    bool operator==(const Vector2& v1, const Vector2 v2){
        return v1.x == v2.x && v1.y == v2.y;
    }
    
    Vector2 operator-(const Vector2& v){
        return Vector2(-v.x, -v.y);
    }
    
    float dot(const Vector2& a, const Vector2& b){
        return a.x * b.x + a.y * b.y;
    }
    float cross(const Vector2& a, const Vector2& b){
        return a.x * b.y - a.y * b.x;
    }
    Vector2 Normalize(const Vector2& v){
        float sq = sqrtf(dot(v, v));
        float normalize = sq != 0 ?  1 / sq : 0;
        return Vector2(v.x * normalize, v.y * normalize);
    }
    
    bool IsParallel(const Vector2& v1, const Vector2& v2){
        return fabsf(cross(v1, v2)) < MT_EPSILON;
    }
    
    void print(const Vector2& v){
        std::cout << "x = " << v.x << " y = " << v.y << std::endl;
    }
    
    //  Vector3
    float Vector3::Length() const {
        return sqrt(x * x + y * y + z * z);
    }
    float Vector3::LengthSq() const {
        return x * x + y * y + z * z;
    }
    
    Vector3 operator+(const Vector3& v1, const Vector3& v2){
        return Vector3(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
    }
    Vector3 operator-(const Vector3& v1, const Vector3& v2){
        return Vector3(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
    }
    Vector3 operator*(const Vector3& v, float scaler){
        return Vector3(v.x * scaler, v.y * scaler, v.z * scaler);
    }
    Vector3 operator*(float scaler, const Vector3& v){
        return Vector3(v.x * scaler, v.y * scaler, v.z * scaler);
    }
    Vector3 operator/(const Vector3& v,float scaler){
        return Vector3(v.x / scaler, v.y / scaler, v.z / scaler);
    }
    Vector3 operator/(float scaler, const Vector3& v){
        return Vector3(v.x / scaler, v.y / scaler, v.z / scaler);
    }
    
    bool operator==(const Vector3& v1, const Vector3& v2){
        return fabsf(v1.x - v2.x) < MT_EPSILON && fabsf(v1.y - v2.y) < MT_EPSILON && fabsf(v1.z - v2.z) < MT_EPSILON;
    }
    bool operator==(const Vector3& v, const float& t){
        return fabsf(v.x - t) < MT_EPSILON && fabsf(v.y - t) < MT_EPSILON && fabsf(v.z - t) < MT_EPSILON;
    }

    Vector3 operator-(const Vector3& v){
        return Vector3(-v.x, -v.y, -v.z);
    }
    
    float dot(const Vector3& a, const Vector3& b ){
        return a.x * b.x + a.y * b.y + a.z * b.z;
    }
    Vector3 cross(const Vector3& a, const Vector3& b){
        return Vector3(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
    }
    Vector3 Normalize(const Vector3& v){
        float sq = sqrtf(dot(v,v));
        float normalize = sq != 0 ?  1 / sq : 0;
        return Vector3(v.x * normalize, v.y * normalize, v.z * normalize);
    }
    
    bool IsParallel(const Vector3& v1, const Vector3& v2){
        return cross(v1, v2) == 0.0f;
        //TODO : 要チェック
//        Vector3 c = cross(v1, v2);
//        return fabsf(c.x) < MT_EPSILON && fabsf(c.y) < MT_EPSILON && fabsf(c.z) < MT_EPSILON;
    }
    
    void print(const Vector3& v){
        std::cout << "x = " << v.x << " y = " << v.y << " z = " << v.z << std::endl;
    }
    
    //  Vector4
    float Vector4::Norm() const {
        return sqrt(x * x + y * y + z * z + w * w);
    }
    Vector4 operator+(const Vector4& v1, const Vector4& v2){
        return Vector4(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z, v1.w + v2.w);
    }
    Vector4 operator-(const Vector4& v1, const Vector4& v2){
        return Vector4(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z, v1.w - v2.w);
    }
    Vector4 operator*(const Vector4& v, float scaler){
        return Vector4(v.x * scaler, v.y * scaler, v.z * scaler, v.w * scaler);
    }
    Vector4 operator*(float scaler, const Vector4& v){
        return Vector4(v.x * scaler, v.y * scaler, v.z * scaler, v.w * scaler);
    }
    Vector4 operator/(const Vector4& v,float scaler){
        return Vector4(v.x / scaler, v.y / scaler, v.z / scaler, v.w / scaler);
    }
    Vector4 operator/(float scaler, const Vector4& v){
        return Vector4(v.x / scaler, v.y / scaler, v.z / scaler, v.w / scaler);
    }
    
    bool operator==(const Vector4& v1, const Vector4& v2){
        return v1.x == v2.x && v1.y == v2.y && v1.z == v2.z && v1.w == v2.w;
    }
    
    Vector4 operator-(const Vector4& v){
        return Vector4(-v.x, -v.y, -v.z, -v.w);
    }
    
    float dot(const Vector4& a, const Vector4& b){
        return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
    }
    Vector4 Normalize(const Vector4& v){
        float sq = sqrtf(dot(v, v));
        float normalize = sq != 0 ?  1 / sq : 0;
        return Vector4(v.x * normalize, v.y * normalize, v.z * normalize, v.w * normalize);
    }
    void print(const Vector4& v){
        std::cout << "x = " << v.x << " y = " << v.y << " z = " << v.z << " w = " << v.w << std::endl;
    }
    
    Vector3 GetRightVector(const Vector3& orientation){
        return Normalize(cross(orientation, defaultUpVector));
    }
    Vector3 GetRightVector(const Vector3& orientation, const Vector3& UpVector){
        return Normalize(cross(orientation, UpVector));
    }
    
    Vector3 ToVector3(const Vector4& v){
        return Vector3(v.x,v.y,v.z);
    }
    
}// namespace myTools

