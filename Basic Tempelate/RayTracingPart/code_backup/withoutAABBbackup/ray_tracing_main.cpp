
#include "RayTracing.h"
#include "objLoader.h"
using namespace BasicRayTracing;

namespace OBJLoader {
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
}
Color operator * (const Color& color, const double& k)
{
	vec3 c = vec3(color.r, color.g, color.b);
	return vec_to_color(c * k);
}

int main()
{
	Clock timer;

	srand(time(NULL));
	shared_ptr<material> material_sun = make_shared<lambertian>(vec3(255, 255, 255));
	//shared_ptr<material> material_ground = make_shared<lambertian>(vec3(0.1, 0.1, 0.1));
	shared_ptr<material> material_ground = make_shared<lambertian>(vec3(0.8, 0.8, 0.0));
	//shared_ptr<material> material_center = make_shared<lambertian>(vec3(0.1, 0.2, 0.5));
	shared_ptr<material> material_center = make_shared<metal>(vec3(1, 1, 1), 0.02);
	//shared_ptr<material> material_center = make_shared<lambertian>(vec3(1, 1, 1));
	shared_ptr<material> material_left = make_shared<lambertian>(vec3(0.8,0,0));
	shared_ptr<material> material_bubble = make_shared<dielectric>(1.5);
	shared_ptr<material> material_right = make_shared<metal>(vec3(0.8, 0.6, 0.2), 0.1);
	shared_ptr<material> material_pot = make_shared<lambertian>(vec3(0.8, 0.8, 0.8));

	hittable_list world;
	//world.add(make_shared<sphere>(vec3(0.0, 100.0, -100.0), 100.0, material_sun));
		world.add(make_shared<sphere>(vec3(0.0, -100.5, -1.0), 100.0, material_ground));
		//world.add(make_shared<sphere>(vec3(0.0, 0.0, -1.2), 0.5, material_center));
		world.add(make_shared<sphere>(vec3(-1.0, 0.0, -1.0), 0.4, material_left));
		world.add(make_shared<sphere>(vec3(-1.0, 0.0, -1.0), 0.5, material_bubble));
		world.add(make_shared<sphere>(vec3(1.0, 0.0, -1.0), 0.5, material_right));
		world.add(make_shared<triangle>(vec3(1.0, 0.0, -2), vec3(-0.6, 0.0, -0.5), vec3(-0.6, -1.0, -0.5), material_center));
	//world.objects[4]->transformation.set_scale(vec3(0.4,0.4,0.4));
	Sprite spr;
	Texture tex;
	camera cam(window_aspect_ratio, window_width);

	cam.fov = 90;
	//cam.lookfrom = vec3(-2, 2, 1);
	//cam.lookat = vec3(0, 0, -1);
	//cam.vup = vec3(0, 1, 0);
	cam.lookfrom = vec3(0, 0, 0);
	cam.lookat = vec3(0, 0, -1);
	cam.vup = vec3(0, 1, 0);

	//tex.loadFromImage(cam.render(world));
	//window.clear();
	//spr.setTexture(tex);
	//window.draw(spr);
	//window.display();

	cam.samples_per_pixel = 1;
	cam.max_depth = 100;
	//sf::Image image = cam.render(world);
	//image.saveToFile("output.png");

	cam.samples_per_pixel = 1;
	cam.max_depth = 30;

	timer.restart();
	printf("Start Loading...");
	OBJLoader::Model mod;
	model _model;
	mod = OBJLoader::loadOBJ("temp.obj");
	OBJLoader::rotate_model(mod, vec3(0, -0.1, 0), vec3(1, 0, 0), 40);
	printf("complete    time: %fs\n", timer.getElapsedTime().asSeconds());

	//world.clear();
	int cur = 0;
	printf("Adding Model...");
	world.add(make_shared<sphere>(vec3(0.0, -100.5, -1.0), 100.0, material_ground));
	while (!mod.triangles.empty() and cur < mod.triangles.size())
	{
		_model.add(make_shared<triangle>(
			mod.vertices[mod.triangles[cur].vertexIndices[0]]*0.6 + vec3(0, -0.2, -1),
			mod.vertices[mod.triangles[cur].vertexIndices[1]]*0.6 + vec3(0, -0.2, -1),
			mod.vertices[mod.triangles[cur].vertexIndices[2]]*0.6 + vec3(0, -0.2, -1),
			material_pot)
		);
		cur++;
	}
	world.add(make_shared<model>(_model));
	printf("complete    time: %fs\n", timer.getElapsedTime().asSeconds());

#ifndef COMPILE_WITHOUT_GRAPHICS
	RenderWindow window;
	window.create(VideoMode(window_width, window_height), "RayTracing");

	result_image this_frame(window_width, window_height);
	result_image last_frame;
	this_frame.frame_number = 0;
	while (window.isOpen())
	{
		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed)
			{
				window.close();
			}
		}
		//last_frame = this_frame;
		printf("frame %d...", this_frame.frame_number);
		timer.restart();

		this_frame += cam.render(world);
		printf("complete    time: %fs\n", timer.getElapsedTime().asSeconds());

		struct color_array
		{
			Color* data;
			unsigned int w, h;
			color_array(unsigned int w, unsigned int h) :w(w), h(h)
			{
				data = new Color[w * h];
				return;
			}
			color_array(const result_image& image)
			{
				w = image.width;
				h = image.height;
				data = new Color[w * h];
				double scale = 1.f / image.frame_number;
				#pragma omp parallel for
				for (int i = 0; i < w * h; i++)
				{
					data[i] = vec_to_color(image.data[i].xyz * scale);
					//printf("%f, %f, %f\n", image.data[i].x * 1.f / image.frame_number, image.data[i].y * 1.f / image.frame_number, image.data[i].z * 1.f / image.frame_number);
					//printf("%d, %d, %d\n",data[i].r, data[i].g, data[i].b);
				}
				return;
			}
			~color_array()
			{
				delete[] data;
				return;
			}
			void operator = (const result_image& image)
			{
				w = image.width;
				h = image.height;
				delete[] data;
				data = new Color[w * h];
				double scale = 1.f / image.frame_number;
				#pragma omp parallel for
				for (int i = 0; i < w * h; i++)
				{
					data[i] = vec_to_color(image.data[i].xyz * scale);
					//printf("%f, %f, %f\n", image.data[i].x * 1.f / image.frame_number, image.data[i].y * 1.f / image.frame_number, image.data[i].z * 1.f / image.frame_number);
					//printf("%d, %d, %d\n",data[i].r, data[i].g, data[i].b);
				}
				return;
			}
		};

		Image image;
		image.create(this_frame.width, this_frame.height);
		memcpy((void*)image.getPixelsPtr(), color_array(this_frame).data, image.getSize().x* image.getSize().y * 4);
		tex.loadFromImage(image);
		window.clear();
		spr.setTexture(tex);
		window.draw(spr);
		window.display();
		//world.objects[1]->transformation.set_position(world.objects[1]->transformation.get_position() + vec3(0, -0.01, 0));
		if (this_frame.frame_number % 500 == 0) image.saveToFile(string("output-spp") + to_string(this_frame.frame_number) + string(".png"));
	}
	//Image this_frame;
	//Image last_frame;
	//this_frame = cam.render(world);
	//while (window.isOpen())
	//{
	//	Event event;
	//	while (window.pollEvent(event))
	//	{
	//		if (event.type == Event::Closed)
	//		{
	//			window.close();
	//		}
	//	}
	//	last_frame = this_frame;
	//	this_frame = cam.render(world);
	//	tex.loadFromImage(this_frame);
	//	window.clear();
	//	spr.setTexture(tex);
	//	window.draw(spr);
	//	window.display();
	//}
#endif
}