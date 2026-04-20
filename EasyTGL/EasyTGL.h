/*
* MIT License
* 
* Copyright (c) 2026 Micky-Rex
* 
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* 
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
* 
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/

/*
* EasyTGL - 使用便捷的单头文件终端图形接口
* 轻量的, 单头文件的 C++ 终端图形库
* 
* EasyTGL - An Easy-to-Use Terminal Graphics Interface
* A lightweight, header-only C++ library for terminal graphics
*/

#pragma once

/*
* 如需要运行时切换 EASYTGL_USE_256_COLOR_ONLY 的值,
* 可以取消注释命名空间内的 EASYTGL_USE_256_COLOR_ONLY 变量的声明,
* 并注释掉以下三行关于 EASYTGL_USE_256_COLOR_ONLY 的宏定义,
* 在运行时动态改变命名空间内变量的值, 实现动态调整使用8bit还是24bit颜色
*/
///*
#ifndef EASYTGL_USE_256_COLOR_ONLY
#define EASYTGL_USE_256_COLOR_ONLY true
#endif
//*/

#ifndef EASYTGL_DEFAULT_MAIN_BUFFER_SIZE
#define EASYTGL_DEFAULT_MAIN_BUFFER_SIZE 65536*4096
#endif
#ifndef EASYTGL_DEFAULT_FRAME_BUFFER_SIZE
#define EASYTGL_DEFAULT_FRAME_BUFFER_SIZE 65536*2048
#endif

#include <map>
#include <string>
#include <vector>
#include <csignal>
#include <cstdlib>
#include <cstring>
#include <concepts>
#include <iostream>
#include <algorithm>
#include <unordered_map>
namespace EasyTGL {
	/*
	* 如需要运行时切换 EASYTGL_USE_256_COLOR_ONLY 的值,
	* 可以取消注释以下 EASYTGL_USE_256_COLOR_ONLY 变量的声明,
	* 并注释掉文件头部三行关于 EASYTGL_USE_256_COLOR_ONLY 的宏定义,
	* 在运行时动态改变命名空间内变量的值, 实现动态调整使用8bit还是24bit颜色
	*/
	//bool EASYTGL_USE_256_COLOR_ONLY = true;

	using std::string, std::vector, std::shared_ptr;
	using std::is_same_v, std::to_string;

	/*
	* @brief vector2模板类, 实现二维向量基本操作
	*/
	template <typename T> struct vector2
	{
	public:
		T x, y;	///< 向量的两个分量
		vector2() = default;
		vector2(T _x, T _y);

		/*
		* @brief 判断点是否在给出的矩形内
		* @param _min 范围矩形的左上角坐标
		* @param _max 范围矩形的左上角坐标
		*/
		bool in_area(vector2<T> _min, vector2<T> _max) const;
	};
	template <typename T> vector2<T> operator - (const vector2<T>& vec);
	template <typename T> vector2<T> operator + (const vector2<T>& a, const vector2<T>& b);
	template <typename T> vector2<T> operator - (const vector2<T>& a, const vector2<T>& b);
	template <typename T> bool operator == (const vector2<T>& a, const vector2<T>& b);
	template <typename T> bool operator != (const vector2<T>& a, const vector2<T>& b);

	using vec2i = vector2<int>;
	using vec2 = vector2<float>;

	/*
	* @brief vector3模板类, 实现三维向量基本操作
	*/
	template <typename T> struct vector3
	{
		T x, y, z;
		vector3() = default;
		vector3(T _x, T _y, T _z);

		/*
		* @brief 判断点是否在给出的立方体内
		* @param _min 范围体积的左上角坐标
		* @param _max 范围体积的左上角坐标
		*/
		bool in_area(vector3<T> _min, vector3<T> _max) const;
	};
	template <typename T> vector3<T> operator - (const vector3<T>& vec);
	template <typename T> vector3<T> operator + (const vector3<T>& a, const vector3<T>& b);
	template <typename T> vector3<T> operator - (const vector3<T>& a, const vector3<T>& b);

	using vec3i = vector3<int>;
	using vec3 = vector3<float>;

	/*
	* @brief 表示RGB的结构体, 包含alpha通道, 但暂时没有计划支持alpha通道
	*/
	struct rgb
	{
		uint8_t r, g, b, a;
		rgb() = default;
		rgb(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 0);
		static const rgb white;
		static const rgb black;
		static const rgb red;
		static const rgb green;
		static const rgb blue;
		static const rgb orange;
		static const rgb light_yellow;
	};
	inline const rgb rgb::white = rgb(255, 255, 255);
	inline const rgb rgb::black = rgb(0, 0, 0);
	inline const rgb rgb::red = rgb(255, 0, 0);
	inline const rgb rgb::green = rgb(0, 255, 0);
	inline const rgb rgb::blue = rgb(0, 0, 255);
	inline const rgb rgb::orange = rgb(255, 170, 0);
	inline const rgb rgb::light_yellow = rgb(255, 255, 0);
	bool operator == (const rgb& a, const rgb& b);
	bool operator != (const rgb& a, const rgb& b);
	uint8_t rgb_to_ansi256(const rgb& color);

	/*
	* @brief 每个像素点RGB的集合, 还包含图像的宽和高
	*/
	struct image
	{
		int width, height;	///< 图像的宽和高(单位 像素)
		vector<rgb> data;

		image(int _width, int _height, const vector<rgb>& _data);
	};
	

	inline vector<char> MAIN_BUFFER;	///< 程序运行期间, 所有stdout输出的缓冲区
	

	/*
	* @brief 初始化EasyTGL, 在所有EasyTGL函数调用前调用, 主要包括缓冲区重定向, 内存预分配等操作
	* @param main_buffer_size 程序运行期间, 所有stdout输出使用的缓冲区大小(EasyTGL::MAIN_BUFFER), 默认为EASYTGL_DEFAULT_MAIN_BUFFER_SIZE; 缓冲区过小可能导致不必要的缓冲刷新, 影响性能(单位 字节/字符, ASCII编码下1个字符大小为1字节, 因此二者在缓冲区大小上等价)
	*/
	void init(uint32_t main_buffer_size = EASYTGL_DEFAULT_MAIN_BUFFER_SIZE);


	/*
	* @brief 还原终端输出至调用EasyTGL之前
	*/
	void shut();


	/*
	* @brief EasyTGL绘图使用的视口, 使用空格作为基本字符, 点(0,0)为左上角像素坐标
	*/
	class screen
	{
	public:
		/*
		* @brief EasyTGL绘图使用的视口, 使用空格作为基本字符, 点(0,0)为左上角像素坐标
		* @param _width			视口宽度(单位 逻辑像素, 即实际绘图宽度为 _width*_pixel_width)
		* @param _height		视口高度(单位 像素/字符, 像素高度固定为1字符, 因此二者在高度上等价)
		* @param pos_x			视口左上角横坐标(单位 字符)
		* @param pos_y			视口左上角纵坐标(单位 字符/像素, 像素高度固定为1字符, 因此二者在纵坐标上等价)
		* @param _pixel_width	视口使用的逻辑像素点的宽度(单位 字符), 默认为2, 即表现为像素点形状接近正方形
		* @param buffer_size	视口使用的缓冲区大小(单位 字符/字节, 在ASCII编码下, 字符大小为1字节, 因此二者在缓冲区大小上等价)
		*/
		screen(int _width, int _height, int pos_x = 1, int pos_y = 1, int _pixel_width = 2, uint32_t buffer_size = EASYTGL_DEFAULT_FRAME_BUFFER_SIZE);


		/*
		* @brief 视口析构函数
		*/
		~screen();

		#pragma region 2D Drawing Operation

		/*
		* @brief 在视口中依据像素坐标绘制点(范围为 (0,0)~(width, height) )
		* @param point	像素在视口中的坐标(单位 像素)
		* @param color	像素的颜色, 只支持RGB三通道, 不包括alpha通道
		*/
		inline void draw_pixel(const vec2i& point, const rgb& color);


		/*
		* @brief 在视口中依据像素坐标绘制点(范围为 (0,0)~(width, height) ), 但不经过坐标检查
		* !!注意, 超出范围的坐标可能导致内存越界!!
		* @param point	像素在视口中的坐标(单位 像素)
		* @param color	像素的颜色, 只支持RGB三通道, 不包括alpha通道
		*/
		inline void draw_pixel_without_check(const vec2i& point, const rgb& color);


		/*
		* @brief 在视口中依据像素坐标绘制线(坐标范围为 (0,0)~(width, height), 若两点重合则不会绘制任何内容)
		* @param _p1	线段端点在视口中的坐标(单位 像素)
		* @param _p2	线段端点在视口中的坐标(单位 像素)
		* @param color	线段的颜色, 只支持RGB三通道, 不包括alpha通道
		*/
		void draw_line(const vec2i& _p1, const vec2i& _p2, const rgb& color);


		/*
		* @brief 在视口中依据横坐标坐标绘制水平线(坐标范围为 (0,0)~(width, height))
		* @param y		水平线段各点在视口中的纵坐标(单位 像素)
		* @param _x1	端点的横坐标(单位 像素)
		* @param _x2	端点的横坐标(单位 像素)
		* @param color	线段的颜色, 只支持RGB三通道, 不包括alpha通道
		*/
		void draw_hline(const int& y, const int& _x1, const int& _x2, const rgb& color);


		/*
		* @brief 在视口中依据横坐标坐标绘制竖直线(坐标范围为 (0,0)~(width, height))
		* @param y		竖直线段各点在视口中的横坐标(单位 像素)
		* @param _x1	端点的横坐标(单位 像素)
		* @param _x2	端点的横坐标(单位 像素)
		* @param color	线段的颜色, 只支持RGB三通道, 不包括alpha通道
		*/
		void draw_vline(const int& x, const int& _y1, const int& _y2, const rgb& color);


		/*
		* @brief 在视口中依据顶点坐标坐标绘制三角形(坐标范围为 (0,0)~(width, height))
		* @param p1		顶点在视口中的坐标(单位 像素)
		* @param p2		顶点在视口中的坐标(单位 像素)
		* @param p3		顶点在视口中的坐标(单位 像素)
		* @param color	三角形的颜色, 暂不支持顶点颜色混合; 只支持RGB三通道, 不包括alpha通道
		* @param fiiled	是否填充, 否则为线框
		*/
		void draw_triangle(vec2i p1, vec2i p2, vec2i p3, rgb color, bool filled = true);


		/*
		* @brief 将视口的绘图缓冲区的内容打印至终端
		* @param all_redraw	是否全部重绘, 否则为差量(脏区域)绘制, 作为性能优化手段
		*/
		void display(bool all_redraw = false);


		/*
		* @brief 判断像素点是否在视口内(不被裁剪)
		* @param p 像素点的坐标(单位 像素)
		*/
		inline bool in_screen(const vec2i& p);


		/*
		* @brief 判断像素点是否在视口内(不被裁剪)
		* @param x 像素点的横坐标(单位 像素)
		* @param y 像素点的纵坐标(单位 像素)
		*/
		inline bool in_screen(int x, int y);


		/*
		* @brief 在视口中无变换地绘制图像
		* @param _pos	图像左上角的坐标(单位 像素, 从(0,0)开始)
		* @param _image	指向图片image的指针
		*/
		void draw_image(vec2i _pos, image* _image);
#pragma endregion


		/*
		* @brief 获取视口内的内容, 作为RGB图像返回结果
		*/
		image capture();


		/*
		* @brief 清除视口的绘图缓冲区
		* @param color 清屏的背景颜色(RGB), 只支持RGB三通道, 不包括alpha通道
		*/
		void clear(rgb color = rgb::black);


		/*
		* @brief 快速清除视口的绘图缓冲区(使用memset优化性能), 但只能清除为黑色RGB(0,0,0)
		*/
		void fast_clear();


		/*
		* @brief 移动窗口, 仍以左上角为基准, (1,1)为在终端上的原点
		* @param pos	视口左上角横坐标(单位 字符); 视口左上角纵坐标(单位 字符/像素, 像素高度固定为1字符, 因此二者在纵坐标上等价)
		*/
		//void move_to(const vec2i& pos);

	private:
		string screen_buffer;	///< 视口的输出缓冲区, 绘制像素的控制码输出于此
		//string text_buffer;	///< 视口的输出缓冲区, 绘制文字的控制码输出于此, 此缓冲区内所有内容将在screen_buffer刷新后处理, 即绘制的文字必定覆盖画面
		vec2i global_pos;	///< 视口左上角相对于终端左上角的全局位置, 从(1,1)开始
		vec2i last_global_pos;	///< 上一帧的窗口位置, 用于差量绘制优化
		int width, height;	///< 视口的宽和高(单位 像素)
		int pixel_width;	///< 像素的宽度(单位 字符) (像素高度固定为1个字符)
		char* basic_pixel;	///< 逻辑像素使用的图元(字符串, 默认使用空格作为基本字符)
		vector<rgb> frame_buffer;	///< 当前帧的RGB缓冲区
		vector<rgb> lastframe_frame_buffer;	///< 上一帧的RGB缓冲区, 用于差量绘制优化
		

		/*
		* @brief 根据直线的xy变化, 计算直线斜率
		* @param dx 直线的x变化量
		* @param dy 直线的y变化量(dx变化相对的dy)
		*/
		float calc_slope(float dx, float dy);


		/*
		* @brief 根据直线经过的两点, 计算直线斜率
		* @param a 直线经过的点坐标(单位 像素)
		* @param b 直线经过的点坐标(单位 像素)
		*/
		template <typename T>
		float calc_slope(const vector2<T>& a, const vector2<T>& b);


		/*
		* @brief 绘制线框三角形
		* @param p1		三角形的顶点坐标(单位 像素)
		* @param p2		三角形的顶点坐标(单位 像素)
		* @param p3		三角形的顶点坐标(单位 像素)
		* @param color	三角形的线框颜色, 只支持RGB三通道, 不包括alpha通道
		*/
		void draw_line_triangle(vec2i p1, vec2i p2, vec2i p3, rgb color);


		/*
		* @brief 绘制填充(实心)三角形
		* @param p1		三角形的顶点坐标(单位 像素)
		* @param p2		三角形的顶点坐标(单位 像素)
		* @param p3		三角形的顶点坐标(单位 像素)
		* @param color	三角形的颜色, 只支持RGB三通道, 不包括alpha通道
		*/
		void draw_fill_triangle(vec2i p1, vec2i p2, vec2i p3, rgb color);


		/*
		* @brief 获取脏区域包含的像素索引(需要差量更新的像素点的索引集合)
		*/
		vector<int> get_difference();
	};


	/*
	* @brief 清除终端所有内容, 并直接刷新缓冲
	*/
	void clear_all();
}

#ifdef EASYTGL_IMPLEMENTATION
#undef EASYTGL_IMPLEMENTATION
namespace EasyTGL {

	struct display_color
	{
		union
		{
			uint8_t color256;
			rgb color_rgb;
		};
		display_color() = delete;
		display_color(const rgb& color)
		{
			*this = color;
			return;
		}
		bool operator == (const display_color& other) const
		{
			if (EASYTGL_USE_256_COLOR_ONLY) return other.color256 == this->color256;
			return other.color_rgb == this->color_rgb;
		}
		bool operator != (const display_color& other) const
		{
			return !(*this == other);
		}
		void operator = (const display_color& other)
		{
			if (EASYTGL_USE_256_COLOR_ONLY) this->color256 = other.color256;
			else this->color_rgb = other.color_rgb;
			return;
		}
		void operator = (const uint8_t& color)
		{
			if (EASYTGL_USE_256_COLOR_ONLY)
				color256 = color;
			return;
		}
		void operator = (const rgb& color)
		{
			if (!EASYTGL_USE_256_COLOR_ONLY)
				color_rgb = color;
			else
				color256 = rgb_to_ansi256(color);
			return;
		}
	};

	template <typename T> vector2<T>::vector2(T _x, T _y)
	{
		this->x = _x, this->y = _y;
		return;
	}
	template <typename T> bool vector2<T>::in_area(vector2<T> _min, vector2<T> _max) const
	{
		T _x = this->x;
		T _y = this->y;
		if (_x < _min.x or _x > _max.x or _y < _min.y or _y > _max.y)
			return false;
		return true;
	}
	template <typename T> vector2<T> operator - (const vector2<T>& vec)
	{
		return vector2<T>(-vec.x, -vec.y);
	}
	template <typename T> vector2<T> operator + (const vector2<T>& a, const vector2<T>& b)
	{
		return vector2<T>(a.x + b.x, a.y + b.y);
	}
	template <typename T> vector2<T> operator - (const vector2<T>& a, const vector2<T>& b)
	{
		return a + (-b);
	}
	template <typename T> bool operator == (const vector2<T>& a, const vector2<T>& b)
	{
		return a.x == b.x and a.y == b.y;
	}
	template <typename T> bool operator != (const vector2<T>& a, const vector2<T>& b)
	{
		return a.x != b.x or a.y != b.y;
	}

	template <typename T> vector3<T>::vector3(T _x, T _y, T _z)
	{
		this->x = _x, this->y = _y, this->z = _z;
		return;
	}
	template <typename T> bool vector3<T>::in_area(vector3<T> _min, vector3<T> _max) const
	{
		T _x = this->x;
		T _y = this->y;
		if (_x < _min.x or _x > _max.x or _y < _min.y or _y > _max.y)
			return false;
		return true;
	}
	template <typename T> vector3<T> operator - (const vector3<T>& vec)
	{
		return vector3<T>(-vec.x, -vec.y);
	}
	template <typename T> vector3<T> operator + (const vector3<T>& a, const vector3<T>& b)
	{
		return vector3<T>(a.x + b.x, a.y + b.y);
	}
	template <typename T> vector3<T> operator - (const vector3<T>& a, const vector3<T>& b)
	{
		return a + (-b);
	}

	rgb::rgb(uint8_t r, uint8_t g, uint8_t b, uint8_t a) : r(r), g(g), b(b), a(a) {
		return;
	}
	bool operator == (const rgb& a, const rgb& b)
	{
		return a.r == b.r and a.g == b.g and a.b == b.b;
	}
	bool operator != (const rgb& a, const rgb& b)
	{
		return !(a == b);
	}
	uint8_t rgb_to_ansi256(const rgb& color)
	{
		const uint8_t& r = color.r, g = color.g, b = color.b;
		if (r == g and g == b) {
			if (color.r == 0) return 16;
			uint8_t gray_idx = (r * 23) / 255;
			return 232 + gray_idx;
		}
		uint8_t ans = 16;
		// 色立方, 将RGB分别从[0,255]映射到[0,5]
		ans += (r / 43) * 36;
		ans += (g / 43) * 6;
		ans += b / 43;
		return ans;
	}

	image::image(int _width, int _height, const vector<rgb>& _data)
	{
		width = _width;
		height = _height;
		data.resize(_data.size());
		memcpy(data.data(), _data.data(), sizeof(rgb) * _data.size());
		return;
	}

	void init(uint32_t main_buffer_size)
	{
		MAIN_BUFFER.reserve(main_buffer_size);
		MAIN_BUFFER.resize(main_buffer_size);
		setvbuf(stdout, MAIN_BUFFER.data(), _IOLBF, EASYTGL_DEFAULT_MAIN_BUFFER_SIZE);	// 设置终端的自定义输出缓冲区
		return;
	}
	void shut()
	{
		//printf("\n\n\033[0m\033[49m\033[2J\033c\n");	// 还原输出格式
		fflush(stdout);
		fprintf(stderr, "\033[0m\033[2J\033[39m\033c\n");	// stderr默认无缓冲, 保证恢复终端状态
		return;
	}

	screen::screen(int _width, int _height, int pos_x, int pos_y, int _pixel_width, uint32_t buffer_size)
	{
		screen_buffer.reserve(EASYTGL_DEFAULT_FRAME_BUFFER_SIZE);
		this->width = _width;
		this->height = _height;
		this->pixel_width = _pixel_width;
		this->basic_pixel = new char[pixel_width + 1];
		this->global_pos = vec2i(pos_x, pos_y);
		this->frame_buffer.resize(_width * _height);
		this->lastframe_frame_buffer.clear();
		for (int i = 0; i < pixel_width; i++) basic_pixel[i] = ' ';
		basic_pixel[pixel_width] = '\0';
		this->fast_clear();
		return;
	}
	screen::~screen()
	{
		delete[] this->basic_pixel;
		return;
	}
	inline void screen::draw_pixel(const vec2i& point, const rgb& color)
	{
		if (!in_screen(point)) return;
		frame_buffer[point.x + point.y * width] = color;
		return;
	}
	inline void screen::draw_pixel_without_check(const vec2i& point, const rgb& color)
	{
		frame_buffer[point.x + point.y * width] = color;
		return;
	}
	void screen::draw_line(const vec2i& _p1, const vec2i& _p2, const rgb& color)
	{
		vec2i p1 = _p1, p2 = _p2;
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
		while (in_screen(x1, y1)) {
			draw_pixel(vec2i(x1, y1), color);  // 绘制当前像素

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
		return;
	}
	void screen::draw_hline(const int& y, const int& _x1, const int& _x2, const rgb& color)
	{
		if (y < 0 or y >= height) return;
		int x1 = _x1, x2 = _x2;
		x1 = std::clamp(x1, 0, width - 1);
		x2 = std::clamp(x2, 0, width - 1);
		if (x1 > x2) std::swap(x1, x2);
		if (x1 == x2) return;
		for (; x1 <= x2; x1++) {
			draw_pixel_without_check(vec2i(x1, y), color);
		}
		return;
	}
	void screen::draw_vline(const int& x, const int& _y1, const int& _y2, const rgb& color)
	{
		if (x < 0 or x >= width) return;
		int y1 = _y1, y2 = _y2;
		y1 = std::clamp(y1, 0, height - 1);
		y2 = std::clamp(y2, 0, height - 1);
		if (y1 > y2) std::swap(y1, y2);
		if (y1 == y2) return;
		for (; y1 <= y2; y1++) {
			draw_pixel_without_check(vec2i(x, y1), color);
		}
		return;
	}
	void screen::draw_triangle(vec2i p1, vec2i p2, vec2i p3, rgb color, bool filled)
	{
		if (filled) draw_fill_triangle(p1, p2, p3, color);
		else draw_line_triangle(p1, p2, p3, color);
		return;
	}
	void screen::display(bool all_redraw)
	{
		display_color last_color(rgb::white);

		screen_buffer.reserve(EASYTGL_DEFAULT_FRAME_BUFFER_SIZE);
		/*
		* @brief lambda表达式 移动终端光标
		* @param x	像素点的横坐标(单位 像素)
		* @param y	像素点的纵坐标(单位 像素)
		*/
		auto set_place = [&](const int& x, const int& y) -> void
			{
				screen_buffer += "\033[";
				screen_buffer += to_string(y + global_pos.y);
				screen_buffer += ";";
				screen_buffer += to_string(x + global_pos.x);
				screen_buffer += "H";
				return;
			};

		/*
		* @brief lambda表达式 改变输出(背景)颜色
		* @param color	目标颜色, 只支持RGB三通道, 不包括alpha通道
		*/
		auto set_color = [&](const display_color& color) -> void
			{
				if (EASYTGL_USE_256_COLOR_ONLY) {
					// ~11 Byte per Pixel
					screen_buffer += "\033[48;5;";
					screen_buffer += to_string(color.color256);
					screen_buffer += "m";
				}
				else {
					// ~19 Byte per Pixel
					screen_buffer += "\033[48;2;";
					screen_buffer += to_string(color.color_rgb.r);
					screen_buffer += ";";
					screen_buffer += to_string(color.color_rgb.g);
					screen_buffer += ";";
					screen_buffer += to_string(color.color_rgb.b);
					screen_buffer += "m";
				}
				return;
			};

		set_color(last_color);
		//if (abs(global_pos.x - last_global_pos.x) % 2 == 1) all_redraw = true;
		//if (last_global_pos != global_pos) all_redraw = true;
		if (all_redraw) {
			/*
			* @brief lambda表达式 绘制像素到屏幕缓冲区
			* @param x		像素点的横坐标(单位 像素)
			* @param y		像素点的纵坐标(单位 像素)
			* @param color	像素点的颜色, 只支持RGB三通道, 不包括alpha通道
			* @param last_c	上一像素点的颜色, 用于ASCII控制码优化
			*/
			auto put_pixel = [&](const int& x, const int& y, const display_color& color, const display_color& last_c) -> void
				{
					if (x==0) set_place(x, y);
					if (color != last_c)
						set_color(color);
					screen_buffer += basic_pixel;
					return;
				};

			for (int idx = 0; idx < width * height; idx++) {
				int x = (idx % width) * pixel_width;
				int y = idx / width;
				put_pixel(
					x, y,
					display_color(frame_buffer[idx]),
					last_color
				);
				last_color.color_rgb = frame_buffer[idx];
			}
			lastframe_frame_buffer = frame_buffer;
		}
		else {
			/*
			* @brief lambda表达式 绘制像素到屏幕缓冲区
			* @param x		像素点的横坐标(单位 像素)
			* @param y		像素点的纵坐标(单位 像素)
			* @param color	像素点的颜色, 只支持RGB三通道, 不包括alpha通道
			* @param last_c	上一像素点的颜色RGB, 用于ASCII控制码优化
			* @param last_x	上一像素点的横坐标, 用于ASCII控制码优化(单位 像素)
			* @param last_y	上一像素点的纵坐标, 用于ASCII控制码优化(单位 像素)
			*/
			auto put_pixel = [&](const int& x, const int& y, const display_color& color, const display_color& last_c, const int& last_x, const int& last_y) -> void
				{
					if (last_x + 1 != x or last_y != y) set_place(x, y);
					if (color != last_c) set_color(color);
					screen_buffer += basic_pixel;
					return;
				};
			
			int last_x = 0;
			int last_y = 0;
			set_place(last_x, last_y);
			vector<int> difference = get_difference();
			for (int i = 0; i < difference.size(); i++) {
				int idx = difference[i];
				int x = (idx % width) * pixel_width;
				int y = idx / width;

				put_pixel(
					x, y, display_color(frame_buffer[idx]),
					last_color, last_x, last_y
				);

				last_x = x; last_x = y;
				last_color = frame_buffer[idx];
				lastframe_frame_buffer[idx] = frame_buffer[idx];
			}
		}
		last_global_pos = global_pos;
		fwrite(screen_buffer.data(), sizeof(char), screen_buffer.size(), stdout);
		//fwrite(text_buffer.data(), sizeof(char), text_buffer.size(), stdout);
		fflush(stdout);
		screen_buffer = "";
		//text_buffer = "";
		return;
	}
	inline bool screen::in_screen(const vec2i& p)
	{
		return p.in_area(vec2i(0, 0), vec2i(width - 1, height - 1));
	}
	inline bool screen::in_screen(int x, int y)
	{
		return (x >= 0 and x < width) and (y >= 0 and y < height);
	}
	void screen::draw_image(vec2i _pos, image* _image)
	{
		/*
		* _pos 图像左上角坐标(从0开始)
		* _image 图片
		*/
		for (int i = 0; i < _image->width; i++) {
			for (int j = 0; j < _image->height; j++) {
				draw_pixel(vec2i(i + _pos.x, j + _pos.y), _image->data[j * _image->width + i]);
			}
		}
		return;
	}
	image screen::capture()
	{
		return image(width, height, frame_buffer);
	}
	void screen::clear(rgb color)
	{
		for (int i = 0; i < frame_buffer.size(); i++) {
			frame_buffer[i] = color;
		}
		return;
	}
	void screen::fast_clear()
	{
		memset(frame_buffer.data(), 0x00, sizeof(rgb) * frame_buffer.size());
		return;
	}
	float screen::calc_slope(float dx, float dy)
	{
		if (dy == 0) return 0;  // 水平线，斜率为0
		return dx / dy;
	}
	template <typename T> float screen::calc_slope(const vector2<T>& a, const vector2<T>& b)
	{
		float dx = a.x - b.x;
		float dy = a.y - b.y;

		if (dy == 0) return 0;  // 水平线，斜率为0
		return dx / dy;
	}
	void screen::draw_line_triangle(vec2i p1, vec2i p2, vec2i p3, rgb color)
	{
		draw_line(p1, p2, color);
		draw_line(p1, p3, color);
		draw_line(p2, p3, color);
		return;
	}
	void screen::draw_fill_triangle(vec2i p1, vec2i p2, vec2i p3, rgb color)
	{
		if (p1.y > p2.y) std::swap(p1, p2);
		if (p1.y > p3.y) std::swap(p1, p3);
		if (p2.y > p3.y) std::swap(p2, p3);
		double k1 = calc_slope(p1, p3);
		double k2 = calc_slope(p1, p2);
		double k3 = calc_slope(p2, p3);

		//上半部分 从p1到p2 水平扫描线
		double x_left = p1.x, x_right = p1.x;
		for (int y = p1.y; y < p2.y; y++) {
			draw_hline(y, (int)std::floor(x_left), (int)std::floor(x_right), color);
			x_left += k2;  // 左边界
			x_right += k1; // 右边界
		}

		//下半部分 从p2到p3 水平扫描线
		x_left = p2.x;  // 重新开始
		for (int y = p2.y; y <= p3.y; y++) {
			draw_hline(y, (int)std::floor(x_left), (int)std::floor(x_right), color);
			x_left += k3;  // 左边界
			x_right += k1; // 右边界
		}
		return;
	}
	vector<int> screen::get_difference()
	{
		vector<int> result;
		result.reserve(width * height + 16);
		//#pragma omp parallel for num_threads(a)
		for (int i = 0; i < frame_buffer.size(); i++) {
			if (i >= lastframe_frame_buffer.size()) {
				result.push_back(i);
				lastframe_frame_buffer.push_back(rgb());
				continue;
			}
			//int x = i % width;
			//int y = i / width;
			//int last_idx = i + ((y - last_global_pos.y) * (width-1) + x - last_global_pos.x);
			if (//last_idx < 0 or last_idx >= (width -1) * (height-1) or 
				frame_buffer[i] != lastframe_frame_buffer[i]//last_idx]
				) result.push_back(i);
		}
		return result;
	}
	void clear_all() {
		printf("\033[2J");
		fflush(stdout);
		return;
	}
}
#endif