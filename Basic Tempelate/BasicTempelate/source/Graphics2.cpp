#include "Graphics2.h"
#include <iostream>
#include <cassert>
#include <unordered_map>

// ===================== 基本数学类型 =====================
struct vec4 { float r, g, b, a; vec4(float r=0, float g=0, float b=0, float a=1):r(r),g(g),b(b),a(a){} };

// ===================== VertexData =====================
namespace Graphics {

VertexData::VertexData(const std::vector<float>& _vertices, const std::vector<int>& _vertex_define)
    : vertices_(_vertices), vertex_define_(_vertex_define)
{
    vertex_size_ = 0;
    for (int sz : _vertex_define) vertex_size += sz;
    vertex_count = _vertices.size() / vertex_size;
}

std::vector<float> VertexData::data() const { return vertices; }
std::vector<int> VertexData::define() const { return vertex_define; }
int VertexData::size() const { return static_cast<int>(vertex_count); }
int VertexData::vertexsize() const { return static_cast<int>(vertex_size); }

// ===================== VertexBuffer =====================
VertexBuffer::VertexBuffer() {
    glGenBuffers(1, &VBO);
}
VertexBuffer::~VertexBuffer() {
    glDeleteBuffers(1, &VBO);
}
void VertexBuffer::bind() const {
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
}
void VertexBuffer::unbind() const {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
void VertexBuffer::set(const VertexData& vertices, GLenum usage) const {
    bind();
    glBufferData(GL_ARRAY_BUFFER, vertices.data().size() * sizeof(float), vertices.data().data(), usage);
    unbind();
}

// ===================== VertexArray =====================
VertexArray::VertexArray() {
    glGenVertexArrays(1, &VAO);
}
VertexArray::~VertexArray() {
    glDeleteVertexArrays(1, &VAO);
}
void VertexArray::bind() const {
    glBindVertexArray(VAO);
}
void VertexArray::unbind() const {
    glBindVertexArray(0);
}
void VertexArray::set(const VertexBuffer& VBO, const VertexData& vertices) const {
    bind();
    VBO.bind();
    int cur = 0, offset = 0;
    while (cur < vertices.define().size()) {
        glVertexAttribPointer(cur, vertices.define()[cur], GL_FLOAT, GL_FALSE,
            vertices.vertexsize() * sizeof(float), (void*)(offset * sizeof(float)));
        glEnableVertexAttribArray(cur);
        offset += vertices.define()[cur];
        ++cur;
    }
    VBO.unbind();
    unbind();
}

// ===================== RenderObject =====================
RenderObject::RenderObject() : vertex_count(0) {}

void RenderObject::setup(const VertexData& data, GLenum usage) {
    VBO.set(data, usage);
    VAO.set(VBO, data);
    vertex_count = data.size();
}

void RenderObject::setShader(std::shared_ptr<shader> shd) { shader_ = shd; }
std::shared_ptr<shader> RenderObject::getShader() const { return shader_; }

void RenderObject::draw(GLenum mode, int first, int count) const {
    if (count == -1) count = vertex_count;
    if (shader_) shader_->use();
    VAO.bind();
    glDrawArrays(mode, first, count);
    VAO.unbind();
}

int RenderObject::vertexCount() const { return vertex_count; }

VertexArray& RenderObject::getVAO() { return VAO; }
VertexBuffer& RenderObject::getVBO() { return VBO; }

// ===================== 资源池 ResourcePool =====================
ResourcePool::ResourcePool() {}
ResourcePool::~ResourcePool() {
    shaders.clear();
    objects.clear();
}

// ===================== Window =====================
Window::Window(int width, int height, const std::string& title)
    : closed(false), shared_context_(nullptr)
{
    initGLFW();
    window_ = glfwCreateWindow(width, height, title.c_str(), NULL, shared_context_);
    if (!window_) {
        std::cerr << "Failed to create GLFW window!" << std::endl;
        return;
    }
    makeCurrent();
    glfwSetFramebufferSizeCallback(window_, framebuffer_size_callback);
    initGLAD(window_);
    setViewport(window_, width, height);
}
Window::~Window() { close(); }

void Window::makeCurrent() const {
    glfwMakeContextCurrent(window_);
}
void Window::clear(const vec4& color, GLbitfield mask) const {
    glClearColor(color.r, color.g, color.b, color.a);
    glClear(mask);
}
void Window::swapBuffers() const {
    glfwSwapBuffers(window_);
}
bool Window::shouldClose() const {
    return closed || glfwWindowShouldClose(window_);
}
void Window::close() {
    closed = true;
    if (window_) glfwDestroyWindow(window_);
}
void Window::useShader(const std::shared_ptr<shader>& shd) const {
    if (shd) shd->use();
}
void Window::drawObject(const RenderObject& obj, GLenum mode, int first, int count) const {
    obj.draw(mode, first, count);
}
void Window::setSharedContext(GLFWwindow* shared) {
    shared_context_ = shared;
}

// ===================== WindowGroup =====================
WindowGroup::WindowGroup()
    : resource_pool_(std::make_shared<ResourcePool>()) {}

WindowGroup::~WindowGroup() { closeAll(); }

std::shared_ptr<Window> WindowGroup::createWindow(int width, int height, const std::string& title) {
    GLFWwindow* shared = windows_.empty() ? nullptr : windows_.back()->handle();
    auto win = std::make_shared<Window>(width, height, title);
    if (shared) win->setSharedContext(shared);
    windows_.push_back(win);
    return win;
}

void WindowGroup::closeAll() {
    for (auto& win : windows_) win->close();
    windows_.clear();
}
bool WindowGroup::shouldCloseAll() const {
    for (const auto& win : windows_)
        if (!win->shouldClose()) return false;
    return true;
}
void WindowGroup::pollEvents() const { glfwPollEvents(); }

// ===================== OpenGL/GLFW 初始化工具 =====================
int initGLFW() {
    static bool GLFW_has_inited = false;
    if (GLFW_has_inited) return 1;
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW!" << std::endl;
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFW_has_inited = true;
    return 0;
}
int initGLAD(GLFWwindow* window) {
    static bool GLAD_has_inited = false;
    if (GLAD_has_inited) return 1;
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD!" << std::endl;
        return -1;
    }
    GLAD_has_inited = true;
    return 0;
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glfwMakeContextCurrent(window);
    glViewport(0, 0, width, height);
}
void setViewport(GLFWwindow* window, int width, int height) {
    glfwMakeContextCurrent(window);
    glViewport(0, 0, width, height);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
}

} // namespace Graphics