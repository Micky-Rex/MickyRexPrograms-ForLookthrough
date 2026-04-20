#pragma once
#include <map>
#include <stdio.h>
#include <string>
#include <vector>

using namespace std;
struct vec2
{
	int x, y;
	vec2() = default;
	vec2(int _x, int _y)
	{
		this->x = _x, this->y = _y;
		return;
	}
	// 判断向量表示的点是否在给出的矩形内
	bool in_area(vec2 _min, vec2 _max)
	{
		int _x = this->x;
		int _y = this->y;
		if (_x < _min.x or _x > _max.x or _y < _min.y or _y > _max.y)
			return false;
		return true;
	}
};
vec2 operator - (vec2 vec)
{
	return vec2(-vec.x, -vec.y);
}
vec2 operator + (vec2 a, vec2 b)
{
	return vec2(a.x + b.x, a.y + b.y);
}
vec2 operator - (vec2 a, vec2 b)
{
	return a + (-b);
}
struct rgb
{
	int r, g, b;
	rgb() { r = 0, g = 0, b = 0; }
	rgb(int r, int g, int b) : r(r), g(g), b(b) {};
};
bool operator == (rgb& a, rgb& b)
{
	return a.r == b.r and a.g == b.g and a.b == b.b;
}
bool operator != (rgb& a, rgb& b)
{
	return !(a==b);
}

struct image
{
	int width, height;	// 窗口的宽和高
	vector<rgb> data;

	image(int _width, int _height, vector<rgb>& _data)
	{
		width = _width;
		height = _height;
		data = _data;
		return;
	}
};

// screen中像素坐标以(0,0)为最左上角
class screen
{
public:
	vec2 global_pos;	// 窗口左上角在命令行中的全局位置从(1,1)开始
	int width, height;	// 窗口的宽和高
	int pixel_width;	// 像素的宽度(单位 字符)

	screen(int _width, int _height, int pos_x = 0, int pos_y = 0, int _pixel_width = 1)
	{
		this->width = _width;
		this->height = _height;
		this->pixel_width = _pixel_width;
		this->basic_pixel = new char[pixel_width];
		this->global_pos = vec2(pos_x, pos_y);
		this->screen_buffer.resize(_width * _height);
		this->lastframe_screen_buffer.clear();
		for (register int i = 0; i < pixel_width; i++) basic_pixel[i] = ' ';
		this->clear();
		return;
	}

	void draw_pixel(vec2 point, rgb color)
	{
		if (!point.in_area(vec2(0, 0), vec2(width, height))) return;
		screen_buffer[point.x + point.y * width] = color;
		return;
	}

	void draw_line(vec2 p1, vec2 p2, rgb color) {
		int x1 = p1.x, y1 = p1.y;
		int x2 = p2.x, y2 = p2.y;

		int dx = abs(x2 - x1);
		int dy = abs(y2 - y1);
		int sx = (x1 < x2) ? 1 : -1;  // x方向步进
		int sy = (y1 < y2) ? 1 : -1;  // y方向步进
		int err = dx - dy;  // 误差项

		// 循环直到到达终点
		while (true) {
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

	void draw_triangle(vec2 p1, vec2 p2, vec2 p3, rgb color)
	{
		draw_line(p1, p2, color);
		draw_line(p1, p3, color);
		draw_line(p2, p3, color);
		return;
	}

	/*
	void draw_fill_triangle(vec2 p1, vec2 p2, vec2 p3, rgb color)
	{
		map<int, int> l[3];
		l[0] = get_triangle_scan_line(p1, p2);
		l[1] = get_triangle_scan_line(p1, p3);
		l[2] = get_triangle_scan_line(p2, p3);

		int _max_idx;
		float _max_height = max(max(abs(p1.y - p2.y), abs(p1.y - p3.y)), abs(p2.y - p3.y));
		if (abs(p1.y - p2.y) == _max_height) _max_idx = 0;
		if (abs(p1.y - p3.y) == _max_height) _max_idx = 1;
		if (abs(p2.y - p3.y) == _max_height) _max_idx = 2;

		for (int i = 0; i < l[_max_idx].size(); i++)
		{
			draw_line()
		}
	}
	*/

	void display(bool all_redraw = false)
	{
		if (all_redraw)
		{
			//printf("\033[%d;%dH", global_pos_y, global_pos_x);
			for (int idx = 0; idx < width * height; idx++)
			{
				printf("\033[%d;%dH", global_pos.y + (idx / width), global_pos.x + (idx % width) * pixel_width);
				printf("\033[48;2;%d;%d;%dm%s", screen_buffer[idx].r, screen_buffer[idx].g, screen_buffer[idx].b, basic_pixel);
				//if (idx % width == width - 1)
				//{
				//	printf("\n");
				//}
			}
			lastframe_screen_buffer = screen_buffer;
		}
		else
		{
			vector<int> difference = get_difference();
			for (int i = 0; i < difference.size(); i++)
			{
				int idx = difference[i];
				int x = idx % width;
				int y = idx / width;
				rgb* color = &screen_buffer[idx];

				printf("\033[%d;%dH", global_pos.y + y, global_pos.x + x * pixel_width);
				if (i==0 or screen_buffer[difference[i-1]] != *color)
				{
					printf("\033[48;2;%d;%d;%dm", color->r, color->g, color->b);
				}
				printf("%s", basic_pixel);
				lastframe_screen_buffer[idx] = screen_buffer[idx];
			}
		}
		fflush(stdout);
		printf("\033[0m");
		return;
	}

	/*
	* _pos 图像左上角坐标(从0开始)
	* _image 图片
	*/
	void draw_image(vec2 _pos, image* _image)
	{
		for (int i = 0; i < _image->width; i++)
		{
			for (int j = 0; j < _image->height; j++)
			{
				draw_pixel(vec2(i + _pos.x, j + _pos.y), _image->data[j * _image->width + i]);
			}
		}
		return;
	}

	image capture()
	{
		return image(width, height, screen_buffer);
	}

	void clear(rgb color = rgb(0,0,0))
	{
		for (int i = 0; i < screen_buffer.size(); i++)
		{
			screen_buffer[i] = color;
		}
	}

private:
	char* basic_pixel;
	vector<rgb> screen_buffer;
	vector<rgb> lastframe_screen_buffer;

	// 返回两帧之间变化(脏区域)像素索引
	vector<int> get_difference()
	{
		vector<int> result;
		for (int i = 0; i < screen_buffer.size(); i++)
		{
			if (i >= lastframe_screen_buffer.size())
			{
				result.push_back(i);
				lastframe_screen_buffer.push_back(rgb());
				continue;
			}
			if (screen_buffer[i] != lastframe_screen_buffer[i])
			{
				result.push_back(i);
				//printf("\033[0m1111111111111111");
			}
		}
		return result;
	}

	map<int, int> get_triangle_scan_line(vec2 p1, vec2 p2)
	{
		map<int, int> result;
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