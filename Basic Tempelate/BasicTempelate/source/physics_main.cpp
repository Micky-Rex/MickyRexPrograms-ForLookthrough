#include "physics/physics.h"
#include "common/timer.h"
#include "Graphics.h"
//#include "ray_tracing.h"
#include "Basic.h"

#define obj(id) (objects[sys.objs[id]])

using namespace Physics;
using namespace Graphics;

int main()
{
	timer clock;
	//WindowGroup window_group;

	shared_ptr<Window> window = make_shared<Window>(800, 600, "Physics Program");
	shared_ptr<Window> window2 = make_shared<Window>(400, 300, "Physics Child Program");
	//shared_ptr<Window> window = window_group.createWindow(800, 600, "Physics Program");

	vector<float> data = {
		-0.1, 0, 0, 1, 1, 1, 0, 0,
		0.1, -0.1, 0, 1, 1, 1, 0, 0,
		0.1, 0.1, 0, 1, 1, 1, 0, 0
	};
	VertexData vertices(data, vector<int>({3,3,2}));

	RenderObject sprite;
	sprite.init(window->window, vertices, GL_STATIC_DRAW);

	//shared_ptr<Window> window2 = window_group.createWindow(800, 600, "Physics Program2");

	System sys;
	shared_ptr<Point> A = make_shared<Point>(Point()), B = make_shared<Point>(Point());
	A->p = vec3(0, 0, 0);
	B->p = vec3(10, 0, 0);
	A->m = 1; B->m = 1;
	sys.add(A); sys.add(B);
	Tie tie(A, B);
	tie.length = 5;
	tie.k = 1; tie.type = SPRING;
	sys.add(tie);

	window->makeCurrent();
	//while (!window_group.shouldCloseAll()) {
	while (!window->shouldClose()) {
		sys.tick(1);
		window->processInput();
		window->useShader();
		window->clear(vec4(0.2f, 0.3f, 0.2f, 1.f));
		window->_shader->setBool("useTexture", GL_FALSE);
		window->draw(GL_TRIANGLES, sprite);
		window->display();
		glfwPollEvents();
	}
	glfwTerminate();
}

#undef obj(id)
