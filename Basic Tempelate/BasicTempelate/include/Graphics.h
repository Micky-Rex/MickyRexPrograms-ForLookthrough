/*
* “¿¿µsfml-system.lib, opengl32.dll
*/
#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <string>
#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "ray_tracing/shader.h"
namespace Graphics
{
	using namespace std;
	class VertexData
	{
	public:
		VertexData(const vector<float>& _vertices, const vector<int>& _vertex_define) {
			this->vertices = _vertices;
			this->vertex_define = _vertex_define;
			int i = 0; vertex_size = 0;
			while (i < _vertex_define.size()) {
				vertex_size += _vertex_define[i++];
			}
			this->vertex_count = _vertices.size() / vertex_size;
			return;
		}
		vector<float> data() const {
			return vertices;
		}
		vector<int> define() const {
			return vertex_define;
		}
		int size() const {
			return vertex_count;
		}
		int vertexsize() const {
			return vertex_size;
		}

	private:
		size_t vertex_size;
		size_t vertex_count;
		vector<float> vertices;
		vector<int> vertex_define;
	};
	class VertexBuffer
	{
	public:
		unsigned int VBO;
		VertexBuffer() {
			glGenBuffers(1, &VBO);
			return;
		}
		~VertexBuffer() {
			glDeleteBuffers(1, &VBO);
			return;
		}
		void bind() const {
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			return;
		}
		void unbind() const {
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			return;
		}
		void set(const VertexData& vertices, GLenum usage) const {
			this->bind();
			//VAO.bind();
			glBufferData(GL_ARRAY_BUFFER, vertices.data().size() * sizeof(float), vertices.data().data(), usage);
			this->unbind();
			//VAO.unbind();
			return;
		}
	};
	class VertexArray
	{
	public:
		unsigned int VAO;
		VertexArray() {
			glGenVertexArrays(1, &VAO);
			return;
		}
		~VertexArray() {
			glDeleteVertexArrays(1, &VAO);
			return;
		}
		void bind() const {
			glBindVertexArray(VAO);
			return;
		}
		void unbind() const {
			glBindVertexArray(0);
			return;
		}
		void set(const VertexBuffer& VBO, const VertexData& vertices) const {
			this->bind();
			VBO.bind();
			int cur = 0, offset = 0;
			while (cur < vertices.define().size()) {
				glVertexAttribPointer(cur, vertices.define()[cur], GL_FLOAT, GL_FALSE, vertices.vertexsize() * sizeof(float), (void*)(offset * sizeof(float)));
				glEnableVertexAttribArray(cur);
				offset += vertices.define()[cur++];
			}
			VBO.unbind();
			this->unbind();
			return;
		}
	};
	class RenderObject
	{
	public:
		VertexArray VAO;
		VertexBuffer VBO;
		unsigned int vertex_count;
		void init(GLFWwindow* window, const VertexData& vertices, GLenum usage) {
			glfwMakeContextCurrent(window);
			VBO.set(vertices, usage);
			VAO.set(VBO, vertices);
			vertex_count = vertices.size();
			return;
		}
		void circle(float radius, unsigned int vertex_count, vec4 color) {
		}
	};

	//const shader _common_shader = shader("resource/shaders/ray_tracing.vert", "resource/shaders/ray_tracing.frag");

	void framebuffer_size_callback(GLFWwindow* window, int width, int height)
	{
		glfwMakeContextCurrent(window);
		glViewport(0, 0, width, height);
		return;
	}
	int setViewport(GLFWwindow* window, int width, int height)
	{
		glfwMakeContextCurrent(window);
		glViewport(0, 0, width, height);
		glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
		return 0;
	}

	bool GLFW_has_inited = false;
	bool GLAD_has_inited = false;
	int initGLFW() {
		if (GLFW_has_inited) return 1;
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		GLFW_has_inited = true;
		return 0;
	}
	int initGLAD() {
		if (GLAD_has_inited) return 1;
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			printf("Failed to initialize GLAD\n");
			return -1;
		}
		GLAD_has_inited = true;
		return 0;
	}
	class Window
	{
	public:
		shared_ptr<shader> _shader;
		GLFWwindow* window;
		Window(int width, int height, string title, GLFWwindow* sharedContext = NULL, const shared_ptr<shader> _shader = make_shared<shader>(shader())) {
			initGLFW();
			closed = false;
			window = glfwCreateWindow(width, height, title.c_str(), NULL, sharedContext);
			if (window == NULL)
			{
				printf("Failed to create GLFW window!\n");
				glfwDestroyWindow(window);
				return;
			}
			glfwMakeContextCurrent(window);
			glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

			initGLAD();

			setViewport(window, width, height);
			if (_shader->ID != 0) this->_shader = _shader;
			else this->_shader = make_shared<shader>(shader("resource/shaders/common.vert", "resource/shaders/common.frag"));
			return;
		}
		~Window() {
			close();
			return;
		}
		void makeCurrent() const {
			glfwMakeContextCurrent(window);
			return;
		}
		void processInput() const {
			if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
				glfwSetWindowShouldClose(window, true);
		}
		void clear(vec4 color = vec4(0, 0, 0, 0), GLbitfield mask = GL_COLOR_BUFFER_BIT) const {
			glClearColor(color.r, color.g, color.b, color.a);
			glClear(mask);
		}
		void useShader(const shader& _shader = shader()) {
			if (_shader.ID != 0) _shader.use();
			else this->_shader->use();
			return;
		}
		void draw(GLenum mode, const RenderObject& obj, GLuint first = 0, int count = -1) const {
			if (count == -1) count = obj.vertex_count;
			obj.VAO.bind();
			glDrawArrays(mode, first, count);
			obj.VAO.unbind();
			return;
		}
		void display() const {
			glfwSwapBuffers(window);
			return;
		}
		bool shouldClose() const {
			if (closed) return true;
			return glfwWindowShouldClose(window);
		}
		void close() {
			closed = true;
			glfwDestroyWindow(window);
			return;
		}
		bool closed = false;
	};

	//class WindowGroup
	//{
	//private:
	//	vector<shared_ptr<Window> > windows;
	//public:
	//	WindowGroup() {
	//		windows.clear();
	//	}
	//	shared_ptr<Window> createWindow(int width, int height, const string& title, const shared_ptr<shader> _shader = make_shared<shader>(shader())) {
	//		shared_ptr<Window> latest_window = windows.empty()? NULL : windows.back();
	//		shared_ptr<Window> win = make_shared<Window>(Window(width, height, title, latest_window == NULL ? NULL : latest_window->window, (_shader->ID == 0 and latest_window != NULL) ? latest_window->_shader : _shader));
	//		latest_window = win;
	//		windows.push_back(latest_window);
	//		return win;
	//	}
	//	bool shouldCloseAll() {
	//		bool returned = true;
	//		for (int i = 0; i < windows.size(); i++) {
	//			if (!windows[i]->shouldClose()) {
	//				returned = false;
	//			}
	//			else if (!windows[i]->closed) {
	//				windows[i]->close();
	//			}
	//		}
	//		return returned;
	//	}
	//};
	//class WindowGroup
	//{
	//private:
	//	vector<shared_ptr<Window>> windows;
	//public:
	//	WindowGroup() {
	//		windows.clear();
	//	}
	//	shared_ptr<Window> createWindow(int width, int height, const string& title, const shared_ptr<shader> _shader = make_shared<shader>(shader())) {
	//		shared_ptr<Window> latest_window = windows.empty() ? nullptr : windows.back();
	//		shared_ptr<Window> win = make_shared<Window>(width, height, title, latest_window == nullptr ? nullptr : latest_window->window, (_shader->ID == 0 && latest_window != nullptr) ? latest_window->_shader : _shader);
	//		windows.push_back(win);
	//		return win;
	//	}
	//	bool shouldCloseAll() {
	//		bool returned = true;
	//		for (int i = 0; i < windows.size(); i++) {
	//			if (!windows[i]->shouldClose()) {
	//				returned = false;
	//			}
	//			else if (!windows[i]->closed) {
	//				windows[i]->close();
	//			}
	//		}
	//		return returned;
	//	}
	//};
}	// namespace Graphics