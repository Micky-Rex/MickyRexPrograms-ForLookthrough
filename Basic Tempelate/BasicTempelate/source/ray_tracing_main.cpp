/*
* 统计代码量：正则表达式
* ^b*[^:b#/]+.*$
*/
#define _CRT_SECURE_NO_WARNINGS
#include <map>
#include "common/timer.h"
#include "common/transform.h"
#include "Graphics.h"
#include "ray_tracing.h"
#include "ray_tracing/obj_loader.h"
#include "ray_tracing/gpu_data_setter.h"
#include <thread>

/*namespace OBJLoader {
	void rotate_model(Model& model, const vec3& pivot, const vec3& axis, double angle_degrees) {
		double angle_rad = radians(angle_degrees);  // 角度转弧度

		// 并行化旋转每个顶点 (C++17 支持)
		#if __cplusplus >= 201703L
		std::for_each(std::execution::par_unseq,
			model.vertices.begin(),
			model.vertices.end(),
			[&](vec3& vertex) {
				vertex = rotate_point(vertex, pivot, axis, angle_rad);
			});
		#else
// 单线程回退
		for (vec3& vertex : model.vertices) {
			vertex = rotate_point(vertex, pivot, axis, angle_rad);
		}
#endif
	}
}*/
using namespace Graphics;
//using namespace BasicRayTracing;
hittable_list world;
camera cam(1, 1000);

map<shared_ptr<material>, int> materials;
/*
void init()
{
	timer clock;

	srand(time(NULL));
	shared_ptr<texture> checker = make_shared<checker_texture>(1, make_shared<solid_color>(0.8, 0.8, 0.0), make_shared<solid_color>(1.0, 1.0, 1.0));
	shared_ptr<texture> earth = make_shared<image_texture>("earthmap.jpg");
	shared_ptr<material> light = make_shared<dissuse_light>(vec3(150, 150, 150));
	shared_ptr<material> material_earth = make_shared<lambertian>(earth);
	//shared_ptr<material> material_ground = make_shared<lambertian>(vec3(0.1, 0.1, 0.1));
	shared_ptr<material> material_ground = make_shared<lambertian>(checker);
	//shared_ptr<material> material_center = make_shared<lambertian>(vec3(0.1, 0.2, 0.5));
	shared_ptr<material> material_center = make_shared<metal>(vec3(1, 1, 1), 0.02);
	//shared_ptr<material> material_center = make_shared<lambertian>(vec3(1, 1, 1));
	shared_ptr<material> material_left = make_shared<lambertian>(vec3(0.8, 0.0, 0.0));
	shared_ptr<material> material_bubble = make_shared<dielectric>(1.5);
	shared_ptr<material> material_right = make_shared<metal>(vec3(0.8, 0.6, 0.2), 0.1);
	shared_ptr<material> material_pot = make_shared<lambertian>(vec3(0.8, 0.8, 0.8));

	//world.add(make_shared<sphere>(vec3(0.0, 100.0, -100.0), 100.0, material_sun));
	world.add(make_shared<sphere>(vec3(0.0, -100.5, -1.0), 100.0, material_ground));
	//world.add(make_shared<sphere>(vec3(0.0, 0.0, -1.2), 0.5, material_center));
	world.add(make_shared<sphere>(vec3(-1.0, 0.0, -1.0), 0.4, material_left));
	world.add(make_shared<sphere>(vec3(-1.0, 0.0, -1.0), 0.5, material_bubble));
	world.add(make_shared<sphere>(vec3(1.0, 0.0, -1.0), 0.5, material_right));
	world.add(make_shared<sphere>(vec3(0.0, 0.0, -1.0), 0.2, light));
	world.add(make_shared<triangle>(vec3(1.0, 0.0, -2), vec3(-0.6, 0.0, -0.5), vec3(-0.6, -1.0, -0.5), material_center));

	//world.objects[4]->transformation.set_scale(vec3(0.4,0.4,0.4));
	clock.restart();
	printf("Start Loading...");
	model _model("temp.obj");
	_model.translate(vec3(0, 0.1, 0));
	_model.rotate_x(radians(40));
	_model.scale(vec3(0.6, 0.6, 0.6));
	_model.set_position(vec3(0, -0.2, -1));
	printf("complete    time: %fs\n", clock.getTime());
	//world.add(make_shared<model>(_model));

	// ============== Camera ==============
	cam.fov = 90;
	cam.aspect_ratio = 16.0 / 9.0;
	cam.lookfrom = vec3(0, 0, 0);
	cam.lookat = vec3(0, 0, -1);
	cam.vup = vec3(0, 1, 0);
	cam.samples_per_pixel = 1;
	cam.max_depth = 100;
	cam.background = vec3(0, 0, 0);

	return;
}
void cornell_box()
{
	shared_ptr<material> red	=	make_shared<lambertian>(vec3(0.65, 0.05, 0.05));
	shared_ptr<material> white	=	make_shared<lambertian>(vec3(0.73, 0.73, 0.73));
	shared_ptr<material> green	=	make_shared<lambertian>(vec3(0.12, 0.45, 0.15));
	shared_ptr<material> light	=	make_shared<dissuse_light>(vec3(500, 500, 500));
	materials[red] = 0;
	materials[white] = 1;
	materials[green] = 2;
	materials[light] = 3;

	world.clear();
	world.add(make_shared<quad>(vec3(555, 0, 0), vec3(0, 555, 0), vec3(0, 0, 555), green));
	world.add(make_shared<quad>(vec3(0, 0, 0), vec3(0, 555, 0), vec3(0, 0, 555), red));
	world.add(make_shared<quad>(vec3(343, 554, 332), vec3(-130, 0, 0), vec3(0, 0, -105), light));
	world.add(make_shared<quad>(vec3(0, 0, 0), vec3(555, 0, 0), vec3(0, 0, 555), white));
	world.add(make_shared<quad>(vec3(555, 555, 555), vec3(-555, 0, 0), vec3(0, 0, -555), white));
	world.add(make_shared<quad>(vec3(0, 0, 555), vec3(555, 0, 0), vec3(0, 555, 0), white));

	// ============== Camera ==============
	cam.fov = 40;
	cam.aspect_ratio = 1;
	cam.lookfrom = vec3(278, 278, -800);
	cam.lookat = vec3(278, 278, 0);
	cam.vup = vec3(0, 1, 0);
	cam.samples_per_pixel = 1;
	cam.max_depth = 10;
	cam.background = vec3(0, 0, 0);

}
*/
//float vertices[] = {
//	// 位置              // 颜色
//	 0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // 右下
//	-0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // 左下
//	 0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f    // 顶部
//};
float vertices[] = {
	// 位置              // UV
	 1.f, -1.f, 0.0f,  1.0f, 0.0f,	// 右下
	-1.f, -1.f, 0.0f,  0.0f, 0.0f,   // 左下
	 1.f, 1.f, 0.0f,  1.0f, 1.0f,    // 右上
	// 位置              // UV
	 1.f, 1.f, 0.0f,  1.0f, 1.0f,   // 右上
	-1.f, 1.f, 0.0f,  0.0f, 1.0f,   // 左下
	 -1.f, -1.f, 0.0f,  0.0f, 0.0f    // 左上
};

int main()
{
	printf("Begin to init world and set on GPU...\n");
	shared_ptr<material> blue = make_shared<lambertian>(vec3(0.1, 0.2, 0.5));
	shared_ptr<material> grey = make_shared<lambertian>(vec3(0.0, 0.0, 0.8));
	//shared_ptr<material> grey = make_shared<lambertian>(vec3(0.8, 0.8, 0.8));
	shared_ptr<material> pink = make_shared<dielectric>(1.5);
	shared_ptr<material> green = make_shared<lambertian>(vec3(0.8, 0.8, 0.0));
	//shared_ptr<material> pink = make_shared<metal>(vec3(1,1,1), 0);
	//shared_ptr<material> pink = make_shared<lambertian>(vec3(0.8, 0.3, 0.3));
	shared_ptr<sphere> left = make_shared<sphere>(vec3(0.f, 0.f, 0.f), 0.5f, pink);
	//shared_ptr<sphere> left = make_shared<sphere>(vec3(-1.f, 0.f, -1.f), 0.5f, pink);
	shared_ptr<sphere> center = make_shared<sphere>(vec3(0.f, 0.f, -1.2f), 0.5f, blue);
	shared_ptr<sphere> right = make_shared<sphere>(vec3(1.f, 0.f, -1.f), 0.5f, grey);
	shared_ptr<sphere> ground = make_shared<sphere>(vec3(0.f, -100.5f, -1.f), 100.f, green);
	//shared_ptr<sphere> ground = make_shared<sphere>(vec3(0.f, 2.f, 0.f), 0.5f, green);
	//world.add(left);
	left->center = vec3(0, 0.2, -1);
	world.add(ground);
	world.add(right);
	//world.add(center);
	//ground->center = vec3(0.f, -100.5f, -1.f);
	//ground->transformation = struct transform(vec3(), quat(), vec3());
	//world.objects[0] = ground;
	//cam.image_width = 1600;
	//cam.fov = 90;
	//cam.aspect_ratio = 16.0 / 9.0;
	//cam.lookfrom = vec3(0, 0, 0);
	//cam.lookat = vec3(0, 0, -1);
	//cam.vup = vec3(0, 1, 0);
	//cam.samples_per_pixel = 5;
	//cam.max_depth = 100;
	//cam.background = vec3(0, 0, 0);a
	cam.image_width = 1600;
	cam.vup = vec3(0.f, 1.f, 0.f);
	cam.fov = 90.f;
	cam.samples_per_pixel = 10;
	cam.aspect_ratio = 16.f / 9.f;
	cam.max_depth = 20;
	cam.lookfrom = vec3(0.0, 0.0, 0.0);
	cam.lookat = vec3(0.0, 0.0, -1.0);
	//cam.background = vec3(0.5, 0.7, 1.0); // Light blue background
	cam.background = vec3(1, 1, 1); // Light blue background
	//cam.init();
	cam.init_for_gpu();

	timer clock;
	Graphics::Window window(cam.image_width, cam.image_height, "OpenGL Ray Trace Program");
	shader _shader("resource/shaders/ray_tracing.vert", "resource/shaders/ray_tracing.frag");
	//shader _show_shader("shaders/show.vert", "shaders/show.frag");
	GpuDataSetter::GpuDataSetter setter;
	//setter.clean();
	//setter.add(&world);
	setter.add(right);
	setter.add(ground);
	setter.set_scene();

	_shader.use();
	//printf(string(" mat4(\n  %.2f, %.2f, %.2f, %.2f\n  %.2f, %.2f, %.2f, %.2f\n  %.2f, %.2f, %.2f, %.2f\n  %.2f, %.2f, %.2f, %.2f )\n"), left->transformation);
	//printf(string(" mat4(\n  %.2f, %.2f, %.2f, %.2f\n  %.2f, %.2f, %.2f, %.2f\n  %.2f, %.2f, %.2f, %.2f\n  %.2f, %.2f, %.2f, %.2f )\n"), left->inversed_trans);

	//printf("End...\n\n");
	//printf("Renderer Resolution: width: %d, height: %d\n", cam.image_width, cam.image_height);
	fflush(stdout);
	_shader.setVec("cam.look_from", cam.lookfrom);
	_shader.setInt("cam.max_depth", cam.max_depth);
	_shader.setInt("cam.image_width", cam.image_width);
	_shader.setInt("cam.image_height", cam.image_height);
	_shader.setVec("cam.background", cam.background);
	_shader.setVec("cam.pixel_delta_u", cam.pixel_delta_u);
	_shader.setVec("cam.pixel_delta_v", cam.pixel_delta_v);
	_shader.setVec("cam.pixel00_center", cam.pixel00_center);
	_shader.setInt("cam.samples_per_pixel", cam.samples_per_pixel);
	_shader.setFloat("cam.pixel_samples_scale", cam.pixel_samples_scale);

	//(void)getchar();

	//unsigned int cur_data = 0;
	//unsigned int cur_texture = 1;

	//setObj(world, scene, data, cur_scene, cur_data);

	//unsigned int scene_texture;
	//glGenTextures(1, &scene_texture);
	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, scene_texture);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 2048, 2048, 0, GL_RGBA32F, GL_UNSIGNED_BYTE, scene);


	//unsigned int data_texture;
	//glGenTextures(world.objects.size(), &data_texture);
	//glActiveTexture(GL_TEXTURE1);
	//glBindTexture(GL_TEXTURE_2D, data_texture);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 32, 32, 0, GL_RGBA32F, GL_UNSIGNED_BYTE, data);

	//delete[] data;

	unsigned int VBO;	// 顶点缓冲对象
	glGenBuffers(1, &VBO);

	unsigned int cur = 0;
	unsigned int size = 0;
	unsigned int VAO;	// 顶点数组对象
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(0 * sizeof(float)));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//unsigned int FBO[2], result_tex[2];
	//glGenFramebuffers(2, FBO);
	//glGenTextures(2, result_tex);

	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, result_tex[0]);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, cam.image_width, cam.image_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	//glActiveTexture(GL_TEXTURE1);
	//glBindTexture(GL_TEXTURE_2D, result_tex[1]);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, cam.image_width, cam.image_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	//glBindFramebuffer(GL_FRAMEBUFFER, FBO[0]);
	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, result_tex[0], 0);
	//glBindFramebuffer(GL_FRAMEBUFFER, FBO[1]);
	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, result_tex[1], 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		printf("帧缓冲未完成!\n");
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	srand(timer::getTime());
	unsigned int frame = 0u;
	float this_frame = 0, last_frame = 0, delta;
	srand(timer::getTime());
	while (!glfwWindowShouldClose(window.window))
	{
		window.processInput();
		//(void)getchar();
		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, result_tex[0]);
		//glActiveTexture(GL_TEXTURE1);
		//glBindTexture(GL_TEXTURE_2D, result_tex[1]);

		//glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		//setter.set_scene();
		_shader.setFloat("rdSeed[0]", rand() * randomf());
		_shader.setFloat("rdSeed[1]", rand() * randomf());
		_shader.setFloat("rdSeed[2]", rand() * randomf());
		_shader.setFloat("rdSeed[3]", rand() * randomf());
		_shader.setInt("scene", 0);
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		/*
		glBindFramebuffer(GL_FRAMEBUFFER, FBO[1]);
		_show_shader.use();
		_show_shader.setInt("texture1", 0);
		_show_shader.setInt("texture2", 0);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(0.2f, 0.3f, 0.3f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT);
		_show_shader.use();
		_show_shader.setInt("texture1", 0);
		_show_shader.setInt("texture2", 1);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		*/
		glfwSwapBuffers(window.window);
		glfwPollEvents();

		//getchar();
		this_frame = timer::getTime();
		delta = this_frame - last_frame;
		last_frame = this_frame;
		//printf("%f\n", delta);
		frame++;
		//break;
		//this_thread::sleep_for(chrono::milliseconds(100));
	}
	glfwTerminate();
	return 0;
}