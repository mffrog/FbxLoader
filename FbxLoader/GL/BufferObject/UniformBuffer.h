//
//  UniformBuffer.h
//  OpenGL_Tutorial
//
//  Created by Tomoya Fujii on 2017/10/16.
//  Copyright c 2017年 TomoyaFujii. All rights reserved.
//

#ifndef UniformBuffer_h
#define UniformBuffer_h

#include <GL/glew.h>
#include <string>
#include <memory>

class UniformBuffer;
typedef std::shared_ptr<UniformBuffer> UniformBufferPtr;    ///UBOポインタ型

/**
 * UBO クラス
 */

class UniformBuffer {
public:
    static UniformBufferPtr Create(GLsizeiptr size, GLuint bindingPoint, const char* name);
    bool BufferSubData(const GLvoid* data, GLintptr offset = 0, GLsizeiptr size = 0);
    void BindBufferRange(GLintptr offset, GLsizeiptr size) const;
    void* MapBuffer() const;
    void UnmapBuffer() const;
    
    GLsizeiptr Size() const { return size; }
    
private:
    UniformBuffer() = default;
    ~UniformBuffer();
    UniformBuffer(const UniformBuffer&) = delete;
    UniformBuffer& operator=(const UniformBuffer&) = delete;
    
private:
    GLuint ubo = 0;         ///Uniform Buffer Object
    GLsizeiptr size = 0;    ///UBO のバイトサイズ
    GLuint bindingPoint;    ///UBOの割り当て先バインディングポイント
    std::string name;       ///UBO名
};

#endif /* UniformBuffer_h */
