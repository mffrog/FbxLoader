//
//  Shader.h
//  OpenGL_Tutorial
//
//  Created by Tomoya Fujii on 2017/10/12.
//  Copyright © 2017年 TomoyaFujii. All rights reserved.
//

#ifndef Shader_h
#define Shader_h

#include <GL/glew.h>
#include <string>
#include <memory>

namespace Shader {
    
    class Program;
    typedef std::shared_ptr<Program> ProgramPtr; /// プログラムオブジェクトポインタ型
    
    /**
     * シェーダプログラムクラス
     */
    class Program {
    public:
        /**
         * シェーダプログラム生成関数
         *
         * @param   vsFilename  バーテックスシェーダファイル名
         * @param   fsFilename  フラグメントシェーダファイル名
         *
         * @return シェーダプログラム
         */
        static ProgramPtr Create(const char* vsFilename, const char* fsFilename);
        
        /**
         * シェーダプログラム生成関数
         *
         * @param   vsFilename  バーテックスシェーダファイル名
         * @param   gsFilename  ジオメトリシェーダファイル名
         * @param   fsFilename  フラグメントシェーダファイル名
         * @param   input_type  ジオメトリシェーダの入力タイプ
         * @param   output_type ジオメトリシェーダからの出力タイプ
         * @param   vertex_out  ジオメトリシェーダの最大出力頂点数
         * @return シェーダプログラム
         */
        static ProgramPtr Create(const char* vsFilename, const char* gsFilename, const char* fsFilename, GLint input_type, GLint output_type, GLint vertex_out = -1);
        
        bool UniformBlockBinding(const char* blockName, GLuint bindingPoint);
        void UseProgram();
        void BindTexture(GLenum unit, GLenum type, GLuint texture);
        
    private:
        Program() = default;
        ~Program();
        Program(const Program&) = delete;
        Program& operator=(const Program&) = delete;
        
    private:
        GLuint program = 0;         ///プログラムオブジェクト
        GLint samplerLocation = -1; ///サンプラーの位置
        int samplerCount = 0;       ///サンプラーの数
        std::string name;           ///プログラム名
    };
    
    GLuint CreateProgramFromFile(const char* vsFilename, const char* fsFilename);
    GLuint CreateProgramFromFile(const char* vsFilename, const char* gsFilename, const char* fsFilename, GLint input_type, GLint output_type, GLint vertex_out);

}

#endif /* Shader_h */
