// Example Program For EasyTGL
/*

	termios old_settings, new_settings;
	tcgetattr(STDIN_FILENO, &old_settings);
	new_settings = old_settings;
	new_settings.c_lflag &= ~(ICANON | ECHO);	// 关闭规范模式和回显
	new_settings.c_cc[VMIN] = 0;	// 至少读取一个字符
	new_settings.c_cc[VTIME] = 0;	// 无超时
	tcsetattr(STDIN_FILENO, TCSANOW, &new_settings);	// 应用新设置
找到这段代码在Windows上运行的平替，要求必须无第三方依赖，允许使用Windows的API，不强制

*/
#define EASYTGL_IMPLEMENTATION
#define EASYTIME_IMPLEMENTATION

#include "EasyTGL.h"
#include "EasyTime.h"

using EasyTGL::vec2, EasyTGL::vec2i, EasyTGL::rgb;

EasyTime::timer timer;
const int count = 30000;
int frame = 1;

void exit_all()
{
	EasyTGL::shut();
	fprintf(stderr, "\033[0m\033[1;10HFPS: %f\n", frame / EasyTime::to_second(timer.elapsed()));
	return;
}

void signal_handler(int signal_num)
{
	exit(0);
	return;
}

int main()
{
	std::atexit(exit_all);
	std::signal(SIGINT, signal_handler);

	EasyTGL::init();
	timer.restart();
	//(void)getchar();
	EasyTGL::screen scr(256, 140);	// 滚轮缩小八次
	//EasyTGL::screen scr2(560, 320, 400*2);
	//EasyTGL::screen scr(1410, 520);	// 超大吃配置
	//EasyTGL::screen scr(960, 320);	// 超大吃配置
	//EasyTGL::screen scr(128, 74);	// 滚轮缩小四次，终端全屏(或从最小放大六次)
	EasyTGL::clear_all();
	int x = 0;
	double current_frame_time = EasyTime::to_second(timer.getTime()), last_frame_time = EasyTime::to_second(timer.getTime()), delta_time = 0.f;
	//while (true) {
	//	x = sin(frame * 0.001) * 100;
	//	//scr.clear(rgb(0, 0, 0));
	//	scr.fast_clear();
	//	scr.display();
	//	frame++; frame %= 31416;
	//}
	srand(time(NULL));
	while (frame >= 1) {
		x = sin(frame * 0.005) * 100;
		//scr.clear(rgb(0, 0, 0));
		scr.fast_clear();
		scr.draw_line(vec2i(x * 1.5, x * 0.4), vec2i(x + 1000, x + 500), rgb::orange);
		scr.draw_triangle(vec2i(100 + x, 20 - x * 0.5), vec2i(200 + x * 0.1, 150 + x), vec2i(340 - x * 1.2, 50), rgb::white);
		//for (int x = 0; x < 960; x++) for (int y = 0; y < 320; y++) { float tmp = 1.f*rand() / RAND_MAX; scr.draw_pixel(vec2i(x, y), rgb(255 * tmp, 255 * tmp, 255 * tmp)); }
		//for (int x = 0; x < 960; x++) for (int y = 0; y < 320; y++) scr.draw_pixel(vec2i(x, y), rgb(rand(), rand(), rand()));
		//scr.draw_triangle(vec2i(0, 0), vec2i(60 + x, 300 - 0.1 * x), vec2i(1200 + x * 0.2, 20 + 0.7 * x), rgb::white);
		//scr.draw_triangle(vec2i(0, 0), vec2i(60, 0), vec2i(0, 60), rgb(0, 0, 0));
		scr.draw_pixel(vec2i(frame % 128, 0), rgb::red);
		scr.draw_pixel(vec2i(frame % 128, 1), rgb::red);
		scr.draw_pixel(vec2i(frame % 128 + 1, 0), rgb::red);
		scr.draw_pixel(vec2i(frame % 128 + 1, 1), rgb::red);
		//scr.move_to(vec2i(frame * 2 % 256 + ((frame * 2 % 256)%2), frame % 128));
		//EasyTGL::clear_all();
		//scr.display(true);
		scr.display();
		//scr2.fast_clear();
		//scr2.draw_line(vec2i(x * 1.5, x * 0.4), vec2i(x + 1000, x + 500), rgb(255, 170, 0));
		//scr2.draw_triangle(vec2i(100 + x, 20 - x * 0.5), vec2i(200 + x * 0.1, 150 + x), vec2i(340 - x * 1.2, 50), rgb(255, 255, 255));
		//scr2.display();
		frame++; frame %= count + 1;

		if (frame % 20 == 0) {
			current_frame_time = EasyTime::to_second(timer.getTime());
			delta_time = current_frame_time - last_frame_time;
			delta_time *= 0.05;
			last_frame_time = current_frame_time;
		}
	}
	exit(0);
}
//optimized : 37.5 FPS
//without optimize : 32.1 FPS
