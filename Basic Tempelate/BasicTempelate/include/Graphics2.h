#pragma once
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>
#include <iostream>
#include <cassert>
#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include "ray_tracing/shader.h"

namespace Graphics {

using std::string;
using std::vector;
using std::shared_ptr;
using std::unordered_map;
using std::function;
using std::make_shared;

// 顶点数据结构
class VertexData {
public:
    VertexData(const vector<float>& vertices, const vector<int>& vertex_define);
    const vector<float>& data() const { return vertices_; }
    const vector<int>& define() const { return vertex_define_; }
    int count() const { return vertex_count_; }
    int stride() const { return stride_; }
private:
    vector<float> vertices_;
    vector<int> vertex_define_;
    int vertex_count_;
    int stride_;
};

// 顶点缓冲
class VertexBuffer {
public:
    VertexBuffer();
    ~VertexBuffer();
    void set(const VertexData& data, GLenum usage = GL_STATIC_DRAW);
    void bind() const;
    void unbind() const;
private:
    GLuint vbo_;
};

// 顶点数组对象
class VertexArray {
public:
    VertexArray();
    ~VertexArray();
    void set(const VertexBuffer& vbo, const VertexData& data);
    void bind() const;
    void unbind() const;
private:
    GLuint vao_;
};


// 渲染对象，包含VAO/VBO可选shader
class RenderObject {
public:
    RenderObject();
    void setup(const VertexData& data, GLenum usage = GL_STATIC_DRAW);
    void draw(GLenum mode, int first = 0, int count = -1) const;
    void setShader(shared_ptr<shader> shd) { shader_ = shd; }
    shared_ptr<shader> getShader() const { return shader_; }
    int vertexCount() const { return vertex_count_; }
    VertexArray& VAO() { return vao_; }
    VertexBuffer& VBO() { return vbo_; }
private:
    VertexArray vao_;
    VertexBuffer vbo_;
    int vertex_count_;
    shared_ptr<shader> shader_;
};


// 窗口类
class Window {
public:
    Window(int width, int height, const string& title);
    ~Window();

    void makeCurrent() const;
    void clear(const vec4& color = vec4(0,0,0,1), GLbitfield mask = GL_COLOR_BUFFER_BIT) const;
    void swapBuffers() const;
    bool shouldClose() const;
    void pollEvents() const { glfwPollEvents(); }
    void close();
    GLFWwindow* handle() const { return window_; }

    // 渲染操作
    void useShader(const shared_ptr<shader>& shd) const;
    void drawObject(const RenderObject& obj, GLenum mode, int first=0, int count=-1) const;

    // 资源共享接口
    void setSharedContext(GLFWwindow* shared) { shared_context_ = shared; }
private:
    GLFWwindow* window_;
    bool closed_;
    GLFWwindow* shared_context_;
};


/// 资源池用于跨窗口共享（着色器、纹理、渲染对象等）
class ResourcePool {
public:
    unordered_map<string, shared_ptr<shader>> shaders;
    unordered_map<string, shared_ptr<RenderObject>> objects;
    // 可继续扩展纹理等
};

/// 窗口组管理器
class WindowGroup {
public:
    WindowGroup();
    ~WindowGroup();

    shared_ptr<Window> createWindow(int width, int height, const string& title);
    void closeAll();
    bool shouldCloseAll() const;
    void pollEvents() const;
    size_t size() const { return windows_.size(); }
    shared_ptr<ResourcePool> resources() { return resource_pool_; }
    const vector<shared_ptr<Window>>& windows() const { return windows_; }

private:
    vector<shared_ptr<Window>> windows_;
    shared_ptr<ResourcePool> resource_pool_;
};


// OpenGL/GLFW初始化工具函数
inline int initGLFW();
inline int initGLAD(GLFWwindow* window);
inline void framebuffer_size_callback(GLFWwindow* window, int width, int height);
inline void setViewport(GLFWwindow* window, int width, int height);

} // namespace Graphics

// 实现部分建议放到Graphics.cpp（这里只是接口声明）