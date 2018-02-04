//
//  GLFWEW.h
//  TFEngine
//
//  Created by Tomoya Fujii on 2018/01/16.
//  Copyright © 2018年 TomoyaFujii. All rights reserved.
//

#ifndef GLFWEW_h
#define GLFWEW_h

#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace TFEngine {
    namespace GL{
        class Window {
        public:
            static Window& GetInstance();
            bool Init(int width, int height, const char* title);
            bool ShouldClose();
            void SwapBuffers();
            
            double GetWidth() {return width;}
            double GetHeight() {return height;}
            
            GLFWwindow* Get(){return window;}
            
        private:
            Window();
            ~Window();
            Window(const Window&) = delete;
            Window& operator=(const Window&) = delete;
            
            GLFWwindow* window = nullptr;
            
            int width;
            int height;
            
            bool isInitialized = false;
            bool isGLFWInitialized = false;
        };
    }// namespace GL
}// namespace TFEngine

#endif /* GLFWEW_h */
