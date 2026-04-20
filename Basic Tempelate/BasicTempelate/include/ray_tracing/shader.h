#pragma once
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include "Basic.h"
namespace Graphics
{
    using namespace std;
    class shader
    {
    public:
        // 程序ID
        unsigned int ID;

        shader() { ID = 0; };
        // 构造器读取并构建着色器
        shader(const char* vertexPath, const char* fragmentPath);
        // 使用/激活程序
        void use() const;
        // uniform工具函数
        void setBool(const string& name, bool value) const;
        void setInt(const string& name, int value) const;
        void setFloat(const string& name, float value) const;
        void setVec(const string& name, vec2 value) const;
        void setVec(const string& name, vec3 value) const;
        void setVec(const string& name, vec4 value) const;
        void setMat(const string& name, mat3 mat) const;
        void setMat(const string& name, mat4 mat) const;
    };
}   // namespace Graphics