//
//  UniformBuffer.cpp
//  OpenGL_Tutorial
//
//  Created by Tomoya Fujii on 2017/10/16.
//  Copyright c 2017年 TomoyaFujii. All rights reserved.
//

#include "UniformBuffer.h"
#include <iostream>

/**
 * Uniform バッファを作成する
 *
 * @param size          バッファのバイトサイズ
 * @param bindingPoint  バッファを割り当てるバインディング・ポイント
 * @param name          バッファ名(デバッグ用)
 *
 * @return  作成した Uniform バッファポインタ
 */
UniformBufferPtr UniformBuffer::Create(GLsizeiptr size, GLuint bindingPoint, const char *name) {
    struct Impl : UniformBuffer {};
    UniformBufferPtr p = std::make_shared<Impl>();
    if (!p) {
        std::cerr << "ERROR: UBO '" << name << "'の作成に失敗" << std::endl;
        return {};
    }
    
    glGenBuffers(1, &p->ubo);
    glBindBuffer(GL_UNIFORM_BUFFER, p->ubo);
    glBufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, p->ubo);
    const GLenum result = glGetError();
    if (result != GL_NO_ERROR) {
        std::cerr << "ERROR: UBO '" << name << "'の作成に失敗" << std::endl;
        return {};
    }
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    
    p->size = size;
    p->bindingPoint = bindingPoint;
    p->name = name;
    return p;
}

/**
 * デストラクタ
 */
UniformBuffer::~UniformBuffer() {
    if (ubo) {
        glDeleteBuffers(1, &ubo);
    }
}

/**
 * Uniformバッファにデータを転送する
 *
 * @param data      転送するデータへのポインタ
 * @param offset    転送先のバイトオフセット
 * @param size      転送するバイト数
 *
 * @retval true     転送成功
 * @retval false    転送失敗
 *
 * offset と size の両方が0の場合、バッファサイズ = UBO サイズとして転送される
 */
bool UniformBuffer::BufferSubData(const GLvoid *data, GLintptr offset, GLsizeiptr size) {
    if (offset + size > this->size) {
        std::cerr << "ERROR(" << name << "): 転送範囲がバッファサイズを超えています (buffer = " << this->size << "offset = " << offset << " size = " << size << ")" << std::endl;
        return false;
    }
    if (offset == 0 && size == 0) {
        size = this->size;
    }
    
    glBindBuffer(GL_UNIFORM_BUFFER, ubo);
    glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint,ubo);
    glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
    
    return true;
}

/**
 * 指定された範囲をバインディング・ポイントに割り当てる
 *
 * @param offset    割り当てる範囲のバイトオフセット
 * @param size      割り当てる範囲のバイト数
 */
void UniformBuffer::BindBufferRange(GLintptr offset, GLsizeiptr size) const {
    glBindBufferRange(GL_UNIFORM_BUFFER, bindingPoint, ubo, offset, size);
}

/**
 * UBOをシステムメモリにマップ
 *
 * @return マップしたメモリへのポインタ
 */
void* UniformBuffer::MapBuffer() const {
    glBindBuffer(GL_UNIFORM_BUFFER, ubo);
    return glMapBufferRange(GL_UNIFORM_BUFFER, 0, size, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
}

/**
 * バッファの割り当てを解除する
 */
void UniformBuffer::UnmapBuffer() const {
    glUnmapBuffer(GL_UNIFORM_BUFFER);
}
