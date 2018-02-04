//
//  GLFWEW.cpp
//  TFEngine
//
//  Created by Tomoya Fujii on 2018/01/16.
//  Copyright © 2018年 TomoyaFujii. All rights reserved.
//

#include "GLFWEW.h"
#include <iostream>

namespace TFEngine {
    namespace GL{

        /**
         * @desc     GLFW からのエラーコールバックを処理する
         *
         * @param   error   エラー番号
         * @param   desc    エラー内容
         */
        void ErrorCallback(int error, const char* desc){
            std::cerr << "ERROR: " << desc << std::endl;
        }
        
        /**
         * @desc    インスタンス取得
         */
        Window& Window::GetInstance(){
            static Window instance;
            return instance;
        }
        
        /**
         * @desc    コンストラクタ
         */
        Window::Window(): isInitialized(false),isGLFWInitialized(false),window(nullptr){
        }
        
        /**
         * @desc    デストラクタ
         */
        Window::~Window(){
            if(isGLFWInitialized){
                glfwTerminate();
            }
        }
        
        /**
         * @desc    初期化関数
         *
         * @param   width   ウィンドウの描画範囲の幅（ピクセル）
         * @param   height  ウィンドウの描画範囲の高さ（ピクセル）
         * @param   title   ウィンドウのタイトル(UTF-8 の 0終端文字列)
         *
         * @retval  true    初期化成功
         * @retval  false   初期化失敗
         */
        bool Window::Init(int width, int height, const char* title){
            if(isInitialized){
                std::cerr << "ERRROR: Window had already initialized." << std::endl;
                return false;
            }
            
            if(!isGLFWInitialized){
                glfwSetErrorCallback(ErrorCallback);
                if(glfwInit() != GL_TRUE){
                    std::cerr << "ERROR: glfwInit() is failed." << std::endl;
                    return false;
                }
                isGLFWInitialized = true;
            }
            //OpenGL Version 4.1 Core Profile を選択
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

            if(!window){
                window = glfwCreateWindow(width, height, title, nullptr, nullptr);
                if(!window){
                    return false;
                }
                glfwMakeContextCurrent(window);
            }
            
            if(glewInit() != GLEW_OK){
                std::cerr << "ERROR: glewInit() is failed." << std::endl;
                return false;
            }
            
            glfwSwapInterval(1);
            
            //-----
            //各種コールバック設定
            //-----
            
            
            const GLubyte* renderer = glGetString(GL_RENDERER);
            std::cout << "Renderer:\t" << renderer << std::endl;
            const GLubyte* version = glGetString(GL_VERSION);
            std::cout << "Version:\t" << version << std::endl;
            const GLubyte* SLversion = glGetString(GL_SHADING_LANGUAGE_VERSION);
            std::cout << "SLversion:\t" << SLversion << std::endl;
            isInitialized = true;
            
            this->width = width;
            this->height = height;
            
            return true;
        }
        
        /**
         * @desc    ウィンドウを閉じるべきか調べる
         *
         * @retval  true    閉じる
         * @retval  false   閉じない
         */
        bool Window::ShouldClose(){
            return glfwWindowShouldClose(window);
        }
        
        /**
         * バッファの切り替えと入力更新処理
         */
        void Window::SwapBuffers(){
            glfwPollEvents();
            glfwSwapBuffers(window);
        }
        



    }// namespace GL
}// namespace TFEngine
