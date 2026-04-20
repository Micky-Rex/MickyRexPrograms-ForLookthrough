```cpp
#include "Graphics.h"
#include <iostream>
using namespace Graphics;

int main() {
    // 初始化GLFW
    if (initGLFW() != 0) return -1;

    // 创建窗口组
    WindowGroup group;

    // 创建窗口A和B（共享资源）
    auto winA = group.createWindow(640, 480, "Window A");
    auto winB = group.createWindow(640, 480, "Window B");

    // 加载shader到资源池
    auto shd = make_shared<shader>("resource/shaders/common.vert", "resource/shaders/common.frag");
    group.resources()->shaders["common"] = shd;

    // 创建三角形顶点数据
    std::vector<float> triangle = {
        // x, y, z, r, g, b
         0.f, 0.5f, 0.f, 1.f, 0.f, 0.f,
        -0.5f,-0.5f, 0.f, 0.f, 1.f, 0.f,
         0.5f,-0.5f, 0.f, 0.f, 0.f, 1.f
    };
    std::vector<int> layout = {3, 3}; // 位置3，颜色3

    VertexData vdata(triangle, layout);

    // 构造渲染对象，放入资源池
    auto obj = make_shared<RenderObject>();
    obj->setup(vdata, GL_STATIC_DRAW);
    obj->setShader(shd);
    group.resources()->objects["triangle"] = obj;

    // 主循环
    while (!group.shouldCloseAll()) {
        // 窗口A渲染
        winA->makeCurrent();
        winA->clear(vec4(0.2f,0.3f,0.3f,1.0f));
        winA->useShader(shd);
        winA->drawObject(*obj, GL_TRIANGLES);
        winA->swapBuffers();

        // 窗口B渲染（使用同一对象和shader）
        winB->makeCurrent();
        winB->clear(vec4(0.1f,0.1f,0.1f,1.0f));
        winB->useShader(shd);
        winB->drawObject(*obj, GL_TRIANGLES);
        winB->swapBuffers();

        group.pollEvents();
    }
    group.closeAll();
    return 0;
}
```