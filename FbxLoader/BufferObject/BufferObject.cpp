//
//  BufferObject.cpp
//  OpenGL_Base
//
//  Created by Tomoya Fujii on 2017/11/14.
//  Copyright © 2017年 TomoyaFujii. All rights reserved.
//

#include "BufferObject.h"

/**
 * バッファオブジェクト初期化
 *
 * @param type  バッファオブジェクトの種類
 * @param size  データのサイズ
 * @param data  データへのポインタ
 * @param usage バッファオブジェクトのアクセスタイプ
 */
void BufferObject::Init(GLenum type, GLsizeiptr size, GLvoid* data, GLenum usage){
    Destroy();
    glGenBuffers(1,&bufferId);
    glBindBuffer(type, bufferId);
    glBufferData(type, size, data, usage);
    glBindBuffer(type, 0);
}

/**
 * オブジェクトの破棄
 */
void BufferObject::Destroy(){
    if(bufferId){
        glDeleteBuffers(1,&bufferId);
        bufferId = 0;
    }
}

/**
 * VAO作成
 *
 * @param vbo   頂点バッファオブジェクトのID
 * @param ibo   インデックスバッファオブジェクトのID
 */
void VertexArrayObject::Init(GLuint vbo, GLuint ibo){
    Destroy();
    glGenVertexArrays(1,&vaoId);
    glBindVertexArray(vaoId);
    glBindBuffer(GL_ARRAY_BUFFER,vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,ibo);
    glBindVertexArray(0);
}

/**
 * VAOの破棄
 */
void VertexArrayObject::Destroy(){
    if(vaoId){
        glDeleteVertexArrays(1,&vaoId);
        vaoId = 0;
    }
}

/**
 * 頂点アトリビュートの設定
 *
 * @param index         設定する頂点アトリビュートのインデックス（何番目に設定するか）
 * @param size          頂点アトリビュートの要素数
 * @param type          頂点アトリビュートの型
 * @param normalized    GL_TRUE = 要素を正規化する、 GL_FALSE = 正規化しない
 * @param stride        次の頂点までのバイト数
 * @param offset        頂点データ先頭から頂点アトリビュートまでのオフセット
 */
void VertexArrayObject::VerteAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, size_t offset){
    glEnableVertexAttribArray(index);
    glVertexAttribPointer(index, size, type, normalized, stride, reinterpret_cast<GLvoid*>(offset));
}

/**
 * VAOを処理対象にする
 */
void VertexArrayObject::Bind() const {
    glBindVertexArray(vaoId);
}

/**
 * VAOを処理対象から外す
 */
void VertexArrayObject::Unbind() const {
    glBindVertexArray(0);
}

