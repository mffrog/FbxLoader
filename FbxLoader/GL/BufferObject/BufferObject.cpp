//
//  BufferObject.cpp
//  TFEngine
//
//  Created by Tomoya Fujii on 2018/01/16.
//  Copyright © 2018年 TomoyaFujii. All rights reserved.
//

#include "BufferObject.h"

namespace TFEngine {
    namespace GL{
        
/**
 * バッファオブジェクトを作成する
 *
 * @param target    バッファオブジェクトの種類
 * @param size    頂点データのサイズ
 * @param data    頂点データのポインタ
 * @param usage    バッファオブジェクトのアクセスタイプ
 */
void BufferObject::Init(GLenum target, GLsizeiptr size, const GLvoid* data, GLenum usage) {
    Destroy();
    glGenBuffers(1, &bufferId);
    glBindBuffer(target, bufferId);
    glBufferData(target, size, data, usage);
    glBindBuffer(target, 0);
}

/**
 * Buffer Objectを破棄する
 */
void BufferObject::Destroy() {
    if (bufferId) {
        glDeleteBuffers(1, &bufferId);
        bufferId = 0;
    }
}

        /**
         * VAOを作成する
         *
         * @param vbo   VBOのID
         */
        void VertexArrayObject::Init(GLuint vbo){
            Destroy();
            glGenVertexArrays(1, &vao);
            glBindVertexArray(vao);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBindVertexArray(0);
        }
        
/**
 * VAOを作成する
 *
 * @param vbo    頂点バッファオブジェクトのID
 * @param ibo    インデックスバッファオブジェクトのID
 */
void VertexArrayObject::Init(GLuint vbo, GLuint ibo) {
    Destroy();
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBindVertexArray(0);
}

/**
 * VAO を破棄する
 */
void VertexArrayObject::Destroy() {
    if (vao) {
        glDeleteVertexArrays(1, &vao);
        vao = 0;
    }
}

/**
 * 頂点アトリビュートを設定する
 *
 * @param index        頂点アトリビュートのインデックス
 * @param size        頂点アトリビュートの要素数
 * @param type        頂点アトリビュートの型
 * @param normalized    GL_TRUE = 要素を正規化する、 GL_FALSE = 要素を正規化しない
 * @param stride        次の同頂点データまでのバイト数
 * @param offset        頂点データ先頭からアトリビュートのバイトオフセット
 */
void VertexArrayObject::VertexAttribPointer(GLuint index, GLint size, GLenum type,
                                            GLboolean normalized, GLsizei stride, size_t offset) {
    glEnableVertexAttribArray(index);
    switch (type) {
        case GL_FLOAT:
            glVertexAttribPointer(index, size, type, normalized, stride, reinterpret_cast<GLvoid*>(offset));
            break;
        case GL_INT:
        case GL_UNSIGNED_INT:
            glVertexAttribIPointer(index, size, type, stride, reinterpret_cast<GLvoid*>(offset));
            break;
        default:
            glVertexAttribPointer(index, size, type, normalized, stride, reinterpret_cast<GLvoid*>(offset));
            break;
    }
}
/**
 * VAOを処理対象にする
 */
void VertexArrayObject::Bind() const { glBindVertexArray(vao); }

/**
 * VAOを処理対象から外す
 */
void VertexArrayObject::Unbind() const { glBindVertexArray(0); }


    }// namespace GL
}//namespace TFEngine
