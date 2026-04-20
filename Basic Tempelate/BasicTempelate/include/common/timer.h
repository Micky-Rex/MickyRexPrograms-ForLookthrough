#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include "glad/glad.h"
#include <thread>
#include <GLFW/glfw3.h>
struct timer
{
public:
	timer() { restart(); }
	void restart() {
		start_stamp = getTime();
		return;
	}
	static void sleep(double time/*秒*/) {
		sleep_milliseconds(time * 1e6);
		return;
	}
	static void sleep_milliseconds(long long time/*毫秒*/) {
		float t = getTime();
		//this_thread::sleep_for(chrono::milliseconds(max(long long(time - 1e5), 0LL)));
		while (getTime() < t);
		return;
	}
	static float getTime() {
		return glfwGetTime();
	}
private:
	double start_stamp;	// 计时器开始的时间
};