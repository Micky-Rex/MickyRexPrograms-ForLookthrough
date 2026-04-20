#pragma once

/* Platforms Supported : Windows, Linux*/
#define EASYTGL_PLATFORM "Windows"
#define EASYTGL_DEFAULT_BUFFER_SIZE 65536*1024

#include <map>
#include <string>
#include <vector>
#include <cstring>
#include <iostream>
#include <algorithm>

namespace EasyTGL {
	#pragma region Define vec2
	struct vec2
	{
		int x, y;
		vec2() = default;
		vec2(int _x, int _y)
		{
			this->x = _x, this->y = _y;
			return;
		}
		// 判断点是否在给出的矩形内
		bool in_area(vec2 _min, vec2 _max) const
		{
			int _x = this->x;
			int _y = this->y;
			if (_x < _min.x or _x > _max.x or _y < _min.y or _y > _max.y)
				return false;
			return true;
		}
	};
	vec2 operator - (const vec2& vec)
	{
		return vec2(-vec.x, -vec.y);
	}
	vec2 operator + (const vec2& a, const vec2& b)
	{
		return vec2(a.x + b.x, a.y + b.y);
	}
	vec2 operator - (const vec2& a, const vec2& b)
	{
		return a + (-b);
	}
#pragma endregion
	#pragma region Define RGB
	struct rgb
	{
		int r, g, b;
		rgb() { r = 0, g = 0, b = 0; }
		rgb(int r, int g, int b) : r(r), g(g), b(b) {};
	};
	bool operator == (const rgb& a, const rgb& b)
	{
		return a.r == b.r and a.g == b.g and a.b == b.b;
	}
	bool operator != (const rgb& a, const rgb& b)
	{
		return !(a == b);
	}
#pragma endregion
	#pragma region Define image
	struct image
	{
		int width, height;	// 窗口的宽和高
		std::vector<rgb> data;

		image(int _width, int _height, const std::vector<rgb>& _data)
		{
			width = _width;
			height = _height;
			memcpy(data.data(), _data.data(), sizeof(rgb) * _data.size());
			return;
		}
	};
#pragma endregion
	// screen中像素坐标以(0,0)为最左上角
	class screen
	{
	public:
		screen(int _width, int _height, int pos_x = 1, int pos_y = 1, int _pixel_width = 2, uint32_t buffer_size = EASYTGL_DEFAULT_BUFFER_SIZE)
		{
			_text_buffer.resize(buffer_size);	// 自定义输出缓冲区大小
			memset(_text_buffer.data(), 0, _text_buffer.size() * sizeof(char));
			setvbuf(stdout, _text_buffer.data(), _IOFBF, _text_buffer.size());	// 设置终端的自定义输出缓冲区

			this->width = _width;
			this->height = _height;
			this->pixel_width = _pixel_width;
			this->basic_pixel = new char[pixel_width + 1];
			this->global_pos = vec2(pos_x, pos_y);
			this->screen_buffer.resize(_width * _height);
			this->lastframe_screen_buffer.clear();
			for (int i = 0; i < pixel_width; i++) basic_pixel[i] = ' ';
			basic_pixel[pixel_width] = '\0';
			this->clear();
			return;
		}
		~screen()
		{
			delete[] this->basic_pixel;
			setvbuf(stdout, NULL, _IONBF, 0);
			return;
		}
		inline void draw_pixel(const vec2& point, const rgb& color)
		{
			if (!in_screen(point)) return;
			screen_buffer[point.x + point.y * width] = color;
			return;
		}
		inline void draw_pixel_without_check(const vec2& point, const rgb& color)
		{
			screen_buffer[point.x + point.y * width] = color;
			return;
		}
		void draw_line(const vec2& _p1, const vec2& _p2, const rgb& color)
		{
			vec2 p1 = _p1, p2 = _p2;
			if (!in_screen(p1)) {
				if (in_screen(p2)) std::swap(p1, p2);
			}
			
			int x1 = p1.x, y1 = p1.y;
			int x2 = p2.x, y2 = p2.y;

			int dx = abs(x2 - x1);
			int dy = abs(y2 - y1);
			int sx = (x1 < x2) ? 1 : -1;  // x方向步进
			int sy = (y1 < y2) ? 1 : -1;  // y方向步进
			int err = dx - dy;  // 误差项

			// 循环直到到达终点
			while (!in_screen(x1, y1)) {
				// 到达终点时退出循环
				if (x1 == x2 && y1 == y2) break;

				// 计算误差调整
				int e2 = 2 * err;
				if (e2 > -dy) {  // 误差累积到需要调整x方向
					err -= dy;
					x1 += sx;
				}
				if (e2 < dx) {   // 误差累积到需要调整y方向
					err += dx;
					y1 += sy;
				}
			}
			// 循环直到到达终点
			while (in_screen(x1,y1)) {
				draw_pixel(vec2(x1, y1), color);  // 绘制当前像素

				// 到达终点时退出循环
				if (x1 == x2 && y1 == y2) break;

				// 计算误差调整
				int e2 = 2 * err;
				if (e2 > -dy) {  // 误差累积到需要调整x方向
					err -= dy;
					x1 += sx;
				}
				if (e2 < dx) {   // 误差累积到需要调整y方向
					err += dx;
					y1 += sy;
				}
			}
		}
		void draw_hline(const int& y, const int& _x1, const int& _x2, const rgb& color)
		{
			if (y < 0 or y >= height) return;
			int x1 = _x1, x2 = _x2;
			x1 = std::clamp(x1, 0, width - 1);
			x2 = std::clamp(x2, 0, width - 1);
			if (x1 > x2) std::swap(x1, x2);
			for (; x1 <= x2; x1++) {
				draw_pixel_without_check(vec2(x1, y), color);
			}
		}
		void draw_vline(const int& x, const int& _y1, const int& _y2, const rgb& color)
		{
			if (x < 0 or x >= width) return;
			int y1 = _y1, y2 = _y2;
			y1 = std::clamp(y1, 0, height - 1);
			y2 = std::clamp(y2, 0, height - 1);
			if (y1 > y2) std::swap(y1, y2);
			for (; y1 <= y2; y1++) {
				draw_pixel_without_check(vec2(x, y1), color);
			}
		}
		void draw_triangle(vec2 p1, vec2 p2, vec2 p3, rgb color, bool filled = true)
		{
			if (filled) draw_fill_triangle(p1, p2, p3, color);
			else draw_line_triangle(p1, p2, p3, color);
			return;
		}
		void display(bool all_redraw = false)
		{
			printf("\033[0m");
			if (all_redraw) {
				//printf("\033[%d;%dH", global_pos_y, global_pos_x);
				for (int idx = 0; idx < width * height; idx++) {
					printf("\033[%d;%dH", global_pos.y + (idx / width), global_pos.x + (idx % width) * pixel_width);
					printf("\033[48;2;%d;%d;%dm%s", screen_buffer[idx].r, screen_buffer[idx].g, screen_buffer[idx].b, basic_pixel);
				}
				lastframe_screen_buffer = screen_buffer;
				//std::swap(lastframe_screen_buffer, screen_buffer);
			}
			else {
				std::vector<int> difference = get_difference();
				for (int i = 0; i < difference.size(); i++) {
					int idx = difference[i];
					int x = idx % width;
					int y = idx / width;
					rgb* color = &screen_buffer[idx];

					printf("\033[%d;%dH", global_pos.y + y, global_pos.x + x * pixel_width);
					if (i == 0 or screen_buffer[difference[i - 1]] != *color) {
						printf("\033[48;2;%d;%d;%dm", color->r, color->g, color->b);
					}
					printf("%s", basic_pixel);
					lastframe_screen_buffer[idx] = screen_buffer[idx];
				}
			}
			fflush(stdout);
			printf("\033[0m\033[38;2;255;255;255m");
			return;
		}
		inline bool in_screen(const vec2& p)
		{
			return p.in_area(vec2(0, 0), vec2(width - 1, height - 1));
		}
		inline bool in_screen(int x, int y)
		{
			return (x >= 0 and x < width) and (y >= 0 and y < height);
		}

		/*
		* _pos 图像左上角坐标(从0开始)
		* _image 图片
		*/
		void draw_image(vec2 _pos, image* _image)
		{
			for (int i = 0; i < _image->width; i++) {
				for (int j = 0; j < _image->height; j++) {
					draw_pixel(vec2(i + _pos.x, j + _pos.y), _image->data[j * _image->width + i]);
				}
			}
			return;
		}
		image capture()
		{
			return image(width, height, screen_buffer);
		}
		void clear(rgb color = rgb(0, 0, 0))
		{
			for (int i = 0; i < screen_buffer.size(); i++) {
				screen_buffer[i] = color;
			}
		}
		void fast_clear()
		{
			memset(screen_buffer.data(), 0x00, sizeof(rgb) * screen_buffer.size());
			return;
		}

	private:
		std::vector<char> _text_buffer;
		vec2 global_pos;	// 窗口左上角在命令行中的全局位置从(1,1)开始
		int width, height;	// 窗口的宽和高
		int pixel_width;	// 像素的宽度(单位 字符)
		// 像素高度固定为1个字符

		char* basic_pixel;
		std::vector<rgb> screen_buffer;
		std::vector<rgb> lastframe_screen_buffer;

		float calc_slope(float dx, float dy)
		{
			if (dy == 0) return 0;  // 水平线，斜率为0
			return dx / dy;
		}
		float calc_slope(const vec2& a, const vec2& b)
		{
			float dx = a.x - b.x;
			float dy = a.y - b.y;

			if (dy == 0) return 0;  // 水平线，斜率为0
			return dx / dy;
		}

		void sort_by_y(vec2* a, vec2* b, vec2* c)
		{
			if (a->y > b->y) std::swap(*a, *b);
			if (a->y > c->y) std::swap(*a, *c);
			if (b->y > c->y) std::swap(*b, *c);
			return;
		}

		void draw_line_triangle(vec2 p1, vec2 p2, vec2 p3, rgb color)
		{
			draw_line(p1, p2, color);
			draw_line(p1, p3, color);
			draw_line(p2, p3, color);
			return;
		}
		void draw_fill_triangle(vec2 p1, vec2 p2, vec2 p3, rgb color)
		{
			sort_by_y(&p1, &p2, &p3);
			float k1 = calc_slope(p1, p3);
			float k2 = calc_slope(p1, p2);
			float k3 = calc_slope(p2, p3);

			//上半部分 从p1到p2 水平扫描线
			float x_left = p1.x, x_right = p1.x;
			for (int y = p1.y; y < p2.y; y++) {
				draw_hline(y, x_left, x_right, color);
				x_left += k2;  // 左边界
				x_right += k1; // 右边界
			}

			//下半部分 从p2到p3 水平扫描线
			x_left = p2.x;  // 重新开始
			for (int y = p2.y; y <= p3.y; y++) {
				draw_hline(y, x_left, x_right, color);
				x_left += k3;  // 左边界
				x_right += k1; // 右边界
			}
			return;
		}
		// 返回两帧之间变化(脏区域)像素索引
		std::vector<int> get_difference()
		{
			std::vector<int> result;
			for (int i = 0; i < screen_buffer.size(); i++) {
				if (i >= lastframe_screen_buffer.size()) {
					result.push_back(i);
					lastframe_screen_buffer.push_back(rgb());
					continue;
				}
				if (screen_buffer[i] != lastframe_screen_buffer[i]) {
					result.push_back(i);
					//printf("\033[0m1111111111111111");
				}
			}
			return result;
		}

		std::map<int, int> get_triangle_scan_line(vec2 p1, vec2 p2)
		{
			std::map<int, int> result;
			int x1 = p1.x, y1 = p1.y;
			int x2 = p2.x, y2 = p2.y;

			int dx = abs(x2 - x1);
			int dy = abs(y2 - y1);
			int sx = (x1 < x2) ? 1 : -1;  // x方向步进
			int sy = (y1 < y2) ? 1 : -1;  // y方向步进
			int err = dx - dy;  // 误差项

			// 循环直到到达终点
			while (true) {
				result[y1] = x1;

				// 到达终点时退出循环
				if (x1 == x2 && y1 == y2) break;

				// 计算误差调整
				int e2 = 2 * err;
				if (e2 > -dy) {  // 误差累积到需要调整x方向
					err -= dy;
					x1 += sx;
				}
				if (e2 < dx) {   // 误差累积到需要调整y方向
					err += dx;
					y1 += sy;
				}
			}
			return result;
		}
	};

	void clear_all()
	{
		printf("\033[2J");
	}
}
