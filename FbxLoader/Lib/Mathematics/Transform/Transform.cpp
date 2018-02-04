//
//  Transform.cpp
//  MatrixCalculater
//
//  Created by Tomoya Fujii on 2017/12/01.
//  Copyright © 2017年 TomoyaFujii. All rights reserved.
//

#include "Transform.h"
#include <math.h>

namespace myTools{
    Matrix4x4 Translate(const Vector3& pos){
        Matrix4x4 mat;
        mat[3][0] = pos.x;
        mat[3][1] = pos.y;
        mat[3][0] = pos.z;
        return mat;
    }
    Matrix4x4& Translate(Matrix4x4& mat, const Vector3& pos){
        mat[3][0] += pos.x;
        mat[3][1] += pos.y;
        mat[3][2] += pos.z;
        return mat;
    }
    
    Matrix4x4 Rotate(const Vector3& axis, const float& radians){
        Matrix4x4 ret;
        const float sin = sinf(radians);
        const float cos = cosf(radians);
        
        float omc = 1 - cos;
        
        ret[0][0] = axis.x * axis.x * omc + cos;
        ret[0][1] = axis.x * axis.y * omc + axis.z * sin;
        ret[0][2] = axis.x * axis.z * omc - axis.y * sin;
        
        ret[1][0] = axis.x * axis.y * omc - axis.z * sin;
        ret[1][1] = axis.y * axis.y * omc + cos;
        ret[1][2] = axis.y * axis.z * omc + axis.x * sin;
        
        ret[2][0] = axis.x * axis.z * omc + axis.y * sin;
        ret[2][1] = axis.y * axis.z * omc - axis.x * sin;
        ret[2][2] = axis.z * axis.z * omc + cos;
        
        return ret;
    }
    
    Matrix4x4 Rotate(const Vector3& euler,const EulerType type){
        switch (type) {
            case EulerType::XYZ:
                return RotateX(ToRadian(euler.x)) * RotateY(ToRadian(euler.y)) * RotateZ(ToRadian(euler.z));
            default:
                return Matrix4x4();
                break;
        }
    }
    
    Matrix4x4 RotateX(const float& radians){
        Matrix4x4 ret;
        const float sin = sinf(radians);
        const float cos = cosf(radians);
        
        ret[1][1] = cos;
        ret[1][2] = sin;
        
        ret[2][1] = -sin;
        ret[2][2] = cos;
        
        return ret;
    }
    Matrix4x4 RotateY(const float& radians){
        Matrix4x4 ret;
        const float sin = sinf(radians);
        const float cos = cosf(radians);
        
        ret[0][0] = cos;
        ret[0][2] = -sin;
        
        ret[2][0] = sin;
        ret[2][2] = cos;
        
        return ret;
    }
    Matrix4x4 RotateZ(const float& radians){
        Matrix4x4 ret;
        const float sin = sinf(radians);
        const float cos = cosf(radians);
        
        ret[0][0] = cos;
        ret[0][1] = sin;
        
        ret[1][0] = -sin;
        ret[1][1] = cos;
        
        return ret;
    }
    
    Matrix4x4 Scale(const Vector3& scale){
        return Matrix4x4(
                         Vector4(scale.x,0,0,0),
                         Vector4(0,scale.y,0,0),
                         Vector4(0,0,scale.z,0),
                         Vector4(0,0,0,1)
                         );
    }
    
    Matrix4x4 MakeTRS(const Matrix4x4& t, const Matrix4x4& r, const Matrix4x4& s){
        Matrix4x4 ret;
        
        ret[3][0] = t[3][0];
        ret[3][1] = t[3][1];
        ret[3][2] = t[3][2];
        
        ret[0][0] = s[0][0] * r[0][0];
        ret[1][0] = s[0][0] * r[1][0];
        ret[2][0] = s[0][0] * r[2][0];
        
        ret[0][1] = s[1][1] * r[0][1];
        ret[1][1] = s[1][1] * r[1][1];
        ret[2][1] = s[1][1] * r[2][1];
        
        ret[0][2] = s[2][2] * r[0][2];
        ret[1][2] = s[2][2] * r[1][2];
        ret[2][2] = s[2][2] * r[2][2];
        
        return ret;
    }
    
    Matrix4x4 MakeTRS(const Vector3& pos, const Quaternion& quat, const Vector3& scale){
        Matrix4x4 ret;
        
        ret[3][0] = pos[0];
        ret[3][1] = pos[1];
        ret[3][2] = pos[2];
        
        ret[0][0] = scale[0] * ( 1 - 2 * ( quat.v.y * quat.v.y + quat.v.z * quat.v.z));
        ret[1][0] = scale[0] * (2 * ( quat.v.x * quat.v.y + quat.w * quat.v.z));
        ret[2][0] = scale[0] * (2 * ( quat.v.x * quat.v.z + quat.w * quat.v.y));
        
        ret[0][1] = scale[1] * (2 * ( quat.v.x * quat.v.y - quat.w * quat.v.z));
        ret[1][1] = scale[1] * (1 - 2 * ( quat.v.x * quat.v.x + quat.v.z * quat.v.z));
        ret[2][1] = scale[1] * (2 * ( quat.v.y * quat.v.z + quat.w * quat.v.x));
        
        ret[0][2] = scale[2] * (2 * ( quat.v.x * quat.v.z - quat.w * quat.v.y));
        ret[1][2] = scale[2] * (2 * ( quat.v.y * quat.v.z - quat.w * quat.v.x));
        ret[2][2] = scale[2] * (1 - 2 * ( quat.v.x * quat.v.x + quat.v.y * quat.v.y));
        
        return  ret;
    }
    
    Matrix4x4 LookAt(const Vector3& position, const Vector3& center, const Vector3& up){
        Vector3 z = Normalize(position - center);
        Vector3 x = Normalize(cross(Normalize(up),z));
        Vector3 y = cross(z,x);
        
        Matrix4x4 ret;
        ret[0][0] = x.x;
        ret[1][0] = x.y;
        ret[2][0] = x.z;
        ret[0][1] = y.x;
        ret[1][1] = y.y;
        ret[2][1] = y.z;
        ret[0][2] = z.x;
        ret[1][2] = z.y;
        ret[2][2] = z.z;
        ret[3][0] = -dot(x, position);
        ret[3][1] = -dot(y, position);
        ret[3][2] = -dot(z, position);
        return ret;
    }
    
    Matrix4x4 CameraViewMat(const Vector3& position, const Vector3& orientation, const Vector3& up){
        Vector3 z = Normalize(-orientation);
        Vector3 x = Normalize(cross(up, z));
        Vector3 y = cross(z, x);
        
        Matrix4x4 ret;
        ret[0][0] = x.x;
        ret[1][0] = x.y;
        ret[2][0] = x.z;
        ret[0][1] = y.x;
        ret[1][1] = y.y;
        ret[2][1] = y.z;
        ret[0][2] = z.x;
        ret[1][2] = z.y;
        ret[2][2] = z.z;
        ret[3][0] = -dot(x, position);
        ret[3][1] = -dot(y, position);
        ret[3][2] = -dot(z, position);
        return ret;
    }
    
    Matrix4x4 ViewMat(const Vector3& position, const Quaternion& q){
        Matrix4x4 view = QuatToMat(q);
        view = Transpose(view);
        view[3][0] = -dot(position, view[0]);
        view[3][1] = -dot(position, view[1]);
        view[3][2] = -dot(position, view[2]);
        return view;
    }
    
    Matrix4x4 Perspective(const float& fovy, const float& aspect, const float& near, const float& far){
        Matrix4x4 ret;
        const float t = 1 / tanf(fovy * 0.5f);
        float zRate = 1 / ( far - near );
        ret[0][0] = aspect * t;
        ret[1][1] = t;
        ret[2][2] = -far * zRate;
        ret[2][3] = -1;
        ret[3][2] = -near * far * zRate;
        ret[3][3] = 0;
        return ret;
    }
}// namespace myTools

