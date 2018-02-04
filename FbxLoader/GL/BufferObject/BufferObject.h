//
//  BufferObject.h
//  TFEngine
//
//  Created by Tomoya Fujii on 2018/01/16.
//  Copyright © 2018年 TomoyaFujii. All rights reserved.
//

#ifndef BufferObject_h
#define BufferObject_h

#include <GL/glew.h>

namespace TFEngine {
    namespace GL{

/**
 * VBO, IBO
 */
class BufferObject {
public:
    BufferObject() = default;
    ~BufferObject(){ Destroy(); }
    BufferObject(const BufferObject&) = delete;
    BufferObject& operator=(const BufferObject&) = delete;
    
    /**
     * バッファオブジェクトを作成する
     *
     * @param target    バッファオブジェクトの種類
     * @param size    頂点データのサイズ
     * @param data    頂点データのポインタ
     * @param usage    バッファオブジェクトのアクセスタイプ
     */
    void Init(GLenum target, GLsizeiptr size, const GLvoid* data = nullptr, GLenum usage = GL_STATIC_DRAW);
    
    /**
     * Buffer Objectを破棄する
     */
    void Destroy();
    
    /**
     * ID 取得
     */
    GLuint Id() const {return bufferId;}
private:
    GLuint bufferId = 0;
};

/**
 * VAO
 */
class VertexArrayObject{
public:
    VertexArrayObject() = default;
    ~VertexArrayObject() { Destroy();}
    VertexArrayObject(const VertexArrayObject&) = delete;
    VertexArrayObject& operator=(VertexArrayObject&) = delete;
    
    /**
     * VAOを作成する
     *
     * @param vbo   VBOのID
     */
    void Init(GLuint vbo);
    
    /**
     * VAOを作成する
     *
     * @param vbo    頂点バッファオブジェクトのID
     * @param ibo    インデックスバッファオブジェクトのID
     */
    void Init(GLuint vbo, GLuint ibo);
    
    /**
     * VAO を破棄する
     */
    void Destroy();
    
    /**
     * 頂点アトリビュートを設定する
     *
     * @param index        頂点アトリビュートのインデックス
     * @param size        頂点アトリビュートの要素数
     * @param type        頂点アトリビュートの型
     * @param normalized    GL_TRUE = 要素を正規化する、 GL_FALSE = 要素を正規化しない
     * @param stride        次の頂点データまでのバイト数
     * @param offset        頂点データ先頭からのバイトオフセット
     *
     * @tips    バインドのし忘れに注意
     */
    void VertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, size_t offset);
    
    /**
     * VAO を処理対象にする
     */
    void Bind() const;
    
    /**
     * VAO を処理対象から外す
     */
    void Unbind() const;
    
    GLuint Id() const { return vao;}
private:
    GLuint vao = 0;
};
    }//namespace GL
}// namespace TFEngine
#endif /* BufferObject_h */
