//
//  Transform.h
//  MatrixCalculater
//
//  Created by Tomoya Fujii on 2017/12/01.
//  Copyright © 2017年 TomoyaFujii. All rights reserved.
//

#ifndef Transform_h
#define Transform_h
#include "Matrix.h"
#include "Quaternion.h"

namespace myTools{
    Matrix4x4 Translate(const Vector3& pos);
    Matrix4x4& Translate(const Matrix4x4& mat, const Vector3& pos);
    
    Matrix4x4 Rotate(const Vector3& axis, const float& radians);
    
    enum class EulerType : int {
        XYZ,
    };
    
    Matrix4x4 Rotate(const Vector3& euler, const EulerType type = EulerType::XYZ);
    
    Matrix4x4 RotateX(const float& radians);
    Matrix4x4 RotateY(const float& radians);
    Matrix4x4 RotateZ(const float& radians);
    
    Matrix4x4 Scale(const Vector3& scale);
    
    Matrix4x4 MakeTRS(const Matrix4x4& t, const Matrix4x4& r, const Matrix4x4& s);
    Matrix4x4 MakeTRS(const Vector3& pos, const Quaternion& quat, const Vector3& scale);
    Matrix4x4 MakeTRS(const Vector3& pos, const Vector3& euler, const Vector3& scale);
    Matrix4x4 LookAt(const Vector3& position, const Vector3& center, const Vector3& up);
    Matrix4x4 ViewMat(const Vector3& position, const Quaternion& q);
    Matrix4x4 CameraViewMat(const Vector3& position, const Vector3& orientation, const Vector3& up);
    Matrix4x4 Perspective(const float& fovy, const float& aspect, const float& near, const float& far);
}// namespace myTools
#endif /* Transform_h */

