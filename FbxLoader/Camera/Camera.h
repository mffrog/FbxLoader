//
//  Camera.h
//  OpenGL_Practice
//
//  Created by Tomoya Fujii on 2017/10/23.
//  Copyright © 2017年 TomoyaFujii. All rights reserved.
//
#ifndef Camera_h
#define Camera_h
#include "Vector.h"
#include "Matrix.h"
#include "Transform.h"
#include "Quaternion.h"
namespace myTools{
    class Camera {
    public:
        void SetPosition(const Vector3& pos){
            position = pos;
        }
        void SetOrientation(const Vector3& ori){
            orientation = Normalize(ori);
        }
        void SetUpVector(const Vector3& upv){
            upVector = Normalize(upv);
        }
        Vector3 GetPosition(){
            return position;
        }
        Vector3 GetForward(){
            return orientation;
        }
        Vector3 GetOrientation(){
            return orientation;
        }
        Vector3 GetUpVector(){
            return upVector;
        }
        
        
        void Rotate(const Quaternion& quat){
            orientation = quat.rotate(orientation);
        }
        
        Matrix4x4 GetViewMat(){
            return  CameraViewMat(position, orientation, upVector);
        }
        
    private:
        Vector3 position;
        Vector3 orientation;
        Vector3 upVector = Vector3(0.0f,1.0f,0.0f);
    };

}// namespace myTools
#endif /* Camera_h */

