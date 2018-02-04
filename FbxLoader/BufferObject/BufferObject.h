//
//  BufferObject.h
//  OpenGL_Base
//
//  Created by Tomoya Fujii on 2017/11/14.
//  Copyright © 2017年 TomoyaFujii. All rights reserved.
//

#ifndef BufferObject_h
#define BufferObject_h

#include "GL/glew.h"

class BufferObject{
public:
    BufferObject() = default;
    ~BufferObject(){Destroy();}
    BufferObject(const BufferObject&) = delete;
    BufferObject& operator=(const BufferObject&) = delete;
    
    /**
     * バッファオブジェクト作成
     *
     * @param type  バッファオブジェクトの種類
     * @param size  データのサイズ
     * @param data  データへのポインタ
     * @param usage バッファオブジェクトのアクセスタイプ
     */
    void Init(GLenum type, GLsizeiptr size, GLvoid* data = nullptr, GLenum usage = GL_STATIC_DRAW);
    
    /**
     * オブジェクトの破棄
     */
    void Destroy();
    
    /**
     * ID取得
     */
    GLuint Id(){return bufferId;}
    
private:
    GLuint bufferId;
};

class VertexArrayObject{
public:
    VertexArrayObject() = default;
    ~VertexArrayObject() {Destroy();}
    VertexArrayObject(const VertexArrayObject& ) = delete;
    VertexArrayObject& operator=(const VertexArrayObject&) = delete;
    
    GLuint Id(){return vaoId;}
    
    /**
     * VAO作成
     *
     * @param vbo   頂点バッファオブジェクトのID
     * @param ibo   インデックスバッファオブジェクトのID
     */
    void Init(GLuint vbo, GLuint ibo);
    
    /**
     * VAOの破棄
     */
    void Destroy();
    
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
    void VerteAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, size_t offset);
    
    /**
     * VAOを処理対象にする
     */
    void Bind() const;
    
    /**
     * VAOを処理対象から外す
     */
    void Unbind() const;
    
private:
    GLuint vaoId;
};

#endif /* BufferObject_h */
