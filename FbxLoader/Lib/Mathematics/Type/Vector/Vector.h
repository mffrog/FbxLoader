//
//  Vector.h
//  MatrixCalculater
//
//  Created by Tomoya Fujii on 2017/11/26.
//  Copyright © 2017年 TomoyaFujii. All rights reserved.
//

#ifndef Vector_h
#define Vector_h

#include <iostream>
#include <float.h>

#define MT_EPSILON  0.00001f


namespace myTools{
    struct Vector2{
        union {
            float d[2];
            struct { float x, y; };
            struct { float s, t; };
        };
        
        Vector2(float x = 0, float y = 0)
        : x(x), y(y)
        {}
        
        float Norm() const ;
        
        Vector2& operator+=(const Vector2& v){
            x += v.x;
            y += v.y;
            return *this;
        }
        Vector2& operator-=(const Vector2& v){
            x -= v.x;
            y -= v.y;
            return *this;
        }
        Vector2& operator*=(float scaler){
            x *= scaler;
            y *= scaler;
            return *this;
        }
        Vector2& operator/=(float scaler){
            float dev = 1.0f / scaler;
            x *= dev;
            y *= dev;
            return *this;
        }
        float& operator[](int idx){
            return d[idx];
        }
    };
    
    Vector2 operator+(const Vector2& v1, const Vector2& v2);
    Vector2 operator-(const Vector2& v1, const Vector2& v2);
    Vector2 operator*(const Vector2& v, float scaler);
    Vector2 operator*(float scaler, const Vector2& v);
    Vector2 operator/(const Vector2& v, float scaler);
    Vector2 operator/(float scaler, const Vector2& v);
    
    bool operator==(const Vector2& v1, const Vector2 v2);
    
    Vector2 operator-(const Vector2&);
    struct Vector3 {
        union {
            float d[3];
            struct {float x,y,z;};
            struct {float r,g,b;};
            struct {float s,t,p;};
        };
        
        Vector3(float x = 0, float y = 0, float z = 0)
        : x(x), y(y), z(z)
        {
        }
        
        Vector3(Vector2 v, float z = 0)
        : x(v.x), y(v.y), z(z)
        {
        }
        
        float Length() const ;
        float LengthSq() const ;
        
        Vector3& operator+=(const Vector3& v){
            x += v.x;
            y += v.y;
            z += v.z;
            return *this;
        }
        Vector3& operator-=(const Vector3& v){
            x -= v.x;
            y -= v.y;
            z -= v.z;
            return *this;
        }
        Vector3& operator*=(float scaler){
            x *= scaler;
            y *= scaler;
            z *= scaler;
            return *this;
        }
        Vector3& operator/=(float scaler){
            float dev = 1.0f / scaler;
            x *= dev;
            y *= dev;
            z *= dev;
            return *this;
        }
        float& operator[](int idx) {
            return d[idx];
        }
        float const& operator[](int idx) const{
            return d[idx];
        }
    };
    
    Vector3 operator+(const Vector3& v1, const Vector3& v2);
    Vector3 operator-(const Vector3& v1, const Vector3& v2);
    Vector3 operator*(const Vector3& v, float scaler);
    Vector3 operator*(float scaler, const Vector3& v);
    Vector3 operator/(const Vector3& v, float scaler);
    Vector3 operator/(float scaler, const Vector3& v);
    
    bool operator==(const Vector3& v1, const Vector3& v2);
    bool operator==(const Vector3& v, const float& t);
    Vector3 operator-(const Vector3&);
    
    struct Vector4 {
        union {
            float d[4];
            struct { float x,y,z,w;};
            struct { float r,g,b,a;};
            struct { float s,t,p,q;};
        };
        
        Vector4(float x = 0, float y = 0, float z = 0, float w = 1)
        : x(x), y(y), z(z), w(w)
        {
        }
        
        Vector4(Vector3 v, float w = 1)
        : x(v.x), y(v.y), z(v.z), w(w)
        {
        }
        
        Vector4(Vector2 v, float z = 0, float w = 0)
        : x(v.x), y(v.y), z(z), w(w)
        {
        }
        
        float Norm() const ;
        
        Vector4& operator+=(const Vector4& v){
            x += v.x;
            y += v.y;
            z += v.z;
            w += v.w;
            return *this;
        }
        Vector4& operator-=(const Vector4& v){
            x -= v.x;
            y -= v.y;
            z -= v.z;
            w -= v.w;
            return *this;
        }
        Vector4& operator*=(float scaler){
            x *= scaler;
            y *= scaler;
            z *= scaler;
            w *= scaler;
            return *this;
        }
        Vector4& operator/=(float scaler){
            float dev = 1.0f / scaler;
            x *= dev;
            y *= dev;
            z *= dev;
            w *= dev;
            return *this;
        }
        float& operator[](int idx){
            return d[idx];
        }
        float const& operator[](int idx) const{
            return d[idx];
        }
    };
    
    Vector4 operator+(const Vector4& v1, const Vector4& v2);
    Vector4 operator-(const Vector4& v1, const Vector4& v2);
    Vector4 operator*(const Vector4& v, float scaler);
    Vector4 operator*(float scaler, const Vector4& v);
    Vector4 operator/(const Vector4& v, float scaler);
    Vector4 operator/(float scaler, const Vector4& v);
    
    bool operator==(const Vector4& v1, const Vector4& v2);
    
    Vector4 operator-(const Vector4&);
    
    void print(const Vector2&);
    void print(const Vector3&);
    void print(const Vector4&);
    
    float dot(const Vector2&, const Vector2&);
    float dot(const Vector3&, const Vector3&);
    float dot(const Vector4&, const Vector4&);
    float cross(const Vector2&, const Vector2&);
    Vector3 cross(const Vector3&, const Vector3&);
    
    Vector2 Normalize(const Vector2& v);
    Vector3 Normalize(const Vector3& v);
    Vector4 Normalize(const Vector4& v);
    
    bool IsParallel(const Vector2& v1, const Vector2& v2);
    bool IsParallel(const Vector3& v1, const Vector3& v2);
    
    static Vector3 defaultUpVector(0.0f,1.0f,0.0f);
    Vector3 GetRightVector(const Vector3& orientation);
    Vector3 GetRightVector(const Vector3& orientation, const Vector3& UpVector);
    Vector3 ToVector3(const Vector4& v);
    //点
    typedef Vector2 Point2D;
    typedef Vector3 Point;
}// namespace myTools
#endif /* Vector_h */

