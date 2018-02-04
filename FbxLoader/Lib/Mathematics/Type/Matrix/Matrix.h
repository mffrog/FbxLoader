//
//  Matrix.h
//  MatrixCalculater
//
//  Created by Tomoya Fujii on 2017/11/25.
//  Copyright © 2017年 TomoyaFujii. All rights reserved.
//

#ifndef Matrix_h
#define Matrix_h

#include "Vector.h"
#include <stdint.h>
#include <iostream>

namespace myTools{
        struct Matrix4x4{
            Matrix4x4(float d = 1){
                for(int i = 0; i < 16; ++i){
                    m[i] = 0;
                }
                v[0][0] = d;
                v[1][1] = d;
                v[2][2] = d;
                v[3][3] = d;
            }
            
            Matrix4x4(const Vector4& v1, const Vector4& v2, const Vector4& v3, const Vector4& v4)
            {
                v[0] = v1;
                v[1] = v2;
                v[2] = v3;
                v[3] = v4;
            }
            
            Vector4 row(int idx){
                Vector4 retval;
                for(int i = 0; i < 4; ++i){
                    retval[i] = v[i][idx];
                }
                return retval;
            }
            
            Vector4& operator[](int idx){
                return v[idx];
            }
            Vector4 const & operator[](int idx) const {
                return v[idx];
            }
            union {
                Vector4 v[4];
                float m[16];
            };
            
            Matrix4x4& operator+=(const Matrix4x4& mat);
            Matrix4x4& operator-=(const Matrix4x4& mat);
            Matrix4x4& operator*=(const Matrix4x4& mat);
            Matrix4x4& operator*=(const float& s);
            Matrix4x4& operator/=(const float& s);
            
        };
        
        Matrix4x4 operator+(const Matrix4x4& a, const Matrix4x4& b);
        Matrix4x4 operator-(const Matrix4x4& a, const Matrix4x4& b);
        Matrix4x4 operator*(const Matrix4x4& mat, const float& s);
        Matrix4x4 operator/(const Matrix4x4& mat, const float& s);
        
        Matrix4x4 operator*(const Matrix4x4& a, const Matrix4x4& b);
        Vector4 operator*(const Matrix4x4& mat, const Vector4& vec);
        
        void print(const Matrix4x4&);
        
        Matrix4x4 Inverse(Matrix4x4 mat);
        
        Matrix4x4 Transpose(Matrix4x4& mat);
}// namespace myTools
#endif /* Matrix_h */

