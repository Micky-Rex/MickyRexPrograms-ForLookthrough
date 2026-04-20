#pragma once
#include "Basic.h"
#include "../common/interval.h"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"

struct image
{
public:
	int width;
	int height;
	vec4* data;
	unsigned int frame_number; // Ö¡µþ¼ÓµÄ²ãÊý

	image() = default;
	image(int _width, int _height)
	{
		this->width = _width;
		this->height = _height;
		this->data = new vec4[width * height]{};
		this->frame_number = 1;
		return;
	}
	image(const image& image)
	{
		*this = image;
		frame_number = image.frame_number;
		return;
	}
	void pixel(int x, int y, vec4 color)
	{
		if (x > width or y > height)
		{
			printf("[Error]: Array out of bounds!\n");
			return;
		}
		data[x + y * width] = color;
		return;
	}
	vec4 pixel(int x, int y) const
	{
		return data[x % width + y * width];
	}
	void* get_data(int& _width, int& _height) const
	{
		_width = this->width;
		_height = this->height;
		return this->data;
	}
	vec2 size() const
	{
		return vec2(width, height);
	}
	void saveToFile(string filename) const
	{
		interval itv(0, 255);
		float scaling = 255.0 / frame_number;
		char* tmp_data = new char[width*height*4];

		#pragma omp parallel for
		for (int i = 0; i < width * height; i++)
		{
			tmp_data[i * 4] = itv.clamp(data[i].r * scaling);
			tmp_data[i * 4 + 1] = itv.clamp(data[i].g * scaling);
			tmp_data[i * 4 + 2] = itv.clamp(data[i].b * scaling);
			tmp_data[i * 4 + 3] = 255;
		}
		stbi_write_png(filename.c_str(), width, height, 4, tmp_data, width * 4);
		delete[] tmp_data;
		return;
	}
	void loadFromFile(string filename)
	{
		int* comp = new int(4);
		//Uint8* img;
		stbi_uc* img;
		img = stbi_load(filename.c_str(), &width, &height, comp, 4);

		float scaling = 1.0 / 255.0;
		#pragma omp parallel for
		for (int i = 0; i < width * height; i++)
		{
			data[i].r = img[i] * scaling;
			data[i].g = img[i] * scaling;
			data[i].b = img[i] * scaling;
			data[i].a = 1;
		}
		delete comp;
		delete[] img;
		frame_number = 1;
		//stbi_write_png("output.png", width, height, 4, getData(width, height), width * 4);
		return;
	}
	image operator + (const image& other_image)
	{
		if (this->width != other_image.width
			or this->height != other_image.height)
		{
			printf("Failed to superposition two images!\n");
			return image();
		}
		image result = image(width, height);

		memcpy(data, other_image.data, width * height * sizeof(vec4));
		//#pragma omp parallel for
		//for (int i = 0; i < width * height; i++)
		//{
		//	result.data[i] = this->data[i] + other_image.data[i];
		//}
		frame_number += other_image.frame_number;
		return result;
	}
	void operator += (const image& other_image)
	{
		if (this->width != other_image.width
			or this->height != other_image.height)
		{
			printf("Failed to superposition two images!\n");
			return;
		}

		#pragma omp parallel for
		for (int i = 0; i < width * height; i++)
		{
			data[i] = data[i] + other_image.data[i];
		}
		frame_number += other_image.frame_number;
		return;
	}
};