//
//  Matrix.cpp
//  MatrixCalculater
//
//  Created by Tomoya Fujii on 2017/11/25.
//  Copyright © 2017年 TomoyaFujii. All rights reserved.
//

#include "Matrix.h"
#include <math.h>

namespace myTools{
        Matrix4x4& Matrix4x4::operator+=(const Matrix4x4& mat){
            for(int i = 0; i < 16; ++i){
                m[i] += mat.m[i];
            }
            return *this;
        }
        Matrix4x4& Matrix4x4::operator-=(const Matrix4x4& mat){
            for(int i = 0; i < 16; ++i){
                m[i] -= mat.m[i];
            }
            return *this;
        }
        Matrix4x4& Matrix4x4::operator*=(const Matrix4x4& mat){
            return (*this = *this * mat);
        }
        Matrix4x4& Matrix4x4::operator*=(const float& s){
            for(auto& data : m){
                data *= s;
            }
            return *this;
        }
        Matrix4x4& Matrix4x4::operator/=(const float& s){
            float dev = 1.0f / s;
            for(auto& data : m){
                data *= dev;
            }
            return *this;
        }
        
        Matrix4x4 operator+(const Matrix4x4& a, const Matrix4x4& b){
            Matrix4x4 ret;
            for(int i = 0; i < 16; ++i){
                ret.m[i] = a.m[i] + b.m[i];
            }
            return ret;
        }
        
        Matrix4x4 operator-(const Matrix4x4& a, const Matrix4x4& b){
            Matrix4x4 ret;
            for(int i = 0; i < 16; ++i){
                ret.m[i] = a.m[i] - b.m[i];
            }
            return ret;
        }
        Matrix4x4 operator*(const Matrix4x4& mat, const float& s){
            Matrix4x4 ret;
            for(int i = 0; i < 16; ++i){
                ret.m[i] = mat.m[i] * s;
            }
            return ret;
        }
        Matrix4x4 operator/(const Matrix4x4& mat, const float& s){
            Matrix4x4 ret;
            float dev = 1.0f / s;
            for(int i = 0; i < 16; ++i){
                ret.m[i] = mat.m[i] * dev;
            }
            return ret;
        }
        
        Matrix4x4 operator*(const Matrix4x4& a, const Matrix4x4& b){
            Matrix4x4 ret(0);
            for(int i = 0; i < 4; ++i){
                for(int j = 0; j < 4; ++j){
                    for(int k = 0; k < 4; ++k){
                        ret[j][i] += a[k][i] * b[j][k];
                    }
                }
            }
            return ret;
        }
        Vector4 operator*(const Matrix4x4& mat, const Vector4& vec){
            Vector4 ret(0,0,0,0);
            for(int i = 0; i < 4; ++i){
                for(int j = 0; j < 4; ++j){
                    ret[i] += mat[j][i] * vec[j];
                }
            }
            return ret;
        }
        
        void print(const Matrix4x4& m){
            for(int i = 0; i < 4; ++i){
                for(int j = 0; j < 4; ++j){
                    std::cout << "m[" << j << "][" << i << "] = " << std::fixed << m[j][i] << " ";
                }
                std::cout << std::endl;
            }
            std::cout << std::endl;
        }
        
        Matrix4x4 Inverse(Matrix4x4 mat){
            Matrix4x4 inverse;
            float buf, tmp;
            float big;
            int pivotRow = 0;
            int i, j, k;
            
            for(i = 0; i < 4; ++i){
                big = 0;
                pivotRow = i;
                for(j = i; j < 4; ++j){
                    if(fabs(mat[i][j]) > big ){
                        big = fabs(mat[i][j]);
                        pivotRow = j;
                    }
                }
                if(big == 0.0){
                    return {};
                }
                if( pivotRow != i ){
                    for(j = 0; j < 4; ++j){
                        tmp = mat[j][i];
                        mat[j][i] = mat[j][pivotRow];
                        mat[j][pivotRow] = tmp;
                        
                        tmp = inverse[j][i];
                        inverse[j][i] = inverse[j][pivotRow];
                        inverse[j][pivotRow] = tmp;
                    }
                }
                buf = 1 / mat[i][i];
                for(j = 0; j < 4; ++j){
                    mat[j][i] *= buf;
                    inverse[j][i] *= buf;
                }
                for(j = 0; j < 4; ++j){
                    if(i != j){
                        buf = mat[i][j];
                        for(k = 0; k < 4; ++k){
                            mat[k][j] -= mat[k][i] * buf;
                            inverse[k][j] -= inverse[k][i] * buf;
                        }
                    }
                }
            }
            return inverse;
        }
        
        Matrix4x4 Transpose(Matrix4x4& mat){
            Matrix4x4 tar;
            
            tar[0][0] = mat[0][0];
            tar[0][1] = mat[1][0];
            tar[0][2] = mat[2][0];
            tar[0][3] = mat[3][0];
            
            tar[1][0] = mat[0][1];
            tar[1][1] = mat[1][1];
            tar[1][2] = mat[2][1];
            tar[1][3] = mat[3][1];
            
            tar[2][0] = mat[0][2];
            tar[2][1] = mat[1][2];
            tar[2][2] = mat[2][2];
            tar[2][3] = mat[3][2];
            
            tar[3][0] = mat[0][3];
            tar[3][1] = mat[1][3];
            tar[3][2] = mat[2][3];
            tar[3][3] = mat[3][3];
            
            return tar;
        }
}// namespace myTools

