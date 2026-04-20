#pragma once

#include <chrono>
#include <thread>
#include <algorithm>
namespace EasyTime {
	double to_second(long long milliseconds);
	struct timer
	{
	public:
		timer();
		void restart();
		long long elapsed() const;
		static void sleep(double time/*秒*/);
		static void sleep_milliseconds(long long time/*毫秒*/);
		static void wait_until(long long timestamp);
		/*
		* 传入本地时间，不是UTC
		*/
		static void wait_until(int year, int month, int day, int hour = 0, int minute = 0, int second = 0);
		/*
		* @param date 年月日 传入8位字符串 如"19000101"
		* @param time 时分秒 传入6位字符串 如"161905"
		* */
		static void wait_until(const std::string& date, const std::string& time);
		/*
		* 获取系统毫秒时间戳
		*/
		static long long getTime();
	private:
		long long start_stamp;	// 计时器开始的时间
	};
}

#ifdef EASYTIME_IMPLEMENTATION
#undef EASYTIME_IMPLEMENTATION
namespace EasyTime {
	double to_second(long long milliseconds) {
		return milliseconds * 0.001;
	}
	timer::timer() {
		restart();
		return;
	}
	void timer::restart() {
		start_stamp = getTime();
		return;
	}
	long long timer::elapsed() const {
		return getTime() - start_stamp;
	}
	void timer::sleep(double time) {
		sleep_milliseconds(time * 1e3);
		return;
	}
	void timer::sleep_milliseconds(long long time)
	{
		long long t = getTime() + time;
		std::this_thread::sleep_for(std::chrono::milliseconds(std::max(time - 50, 0LL)));
		while (getTime() < t) std::this_thread::yield();
	}
	void timer::wait_until(long long timestamp)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(std::max(timestamp - getTime() - 50, 0LL)));
		while (getTime() < timestamp) std::this_thread::yield();
	}
	void timer::wait_until(int year, int month, int day, int hour, int minute, int second)
	{
		std::tm time = {};
		time.tm_year = year - 1900;
		time.tm_mon = month - 1;
		time.tm_mday = day;
		time.tm_hour = hour;
		time.tm_min = minute;
		time.tm_sec = second;

		wait_until(
			std::chrono::duration_cast<std::chrono::milliseconds>(
				std::chrono::system_clock::from_time_t(
					mktime(&time)
				).time_since_epoch()
			).count()
		);

		return;
	}
	void timer::wait_until(const std::string& date, const std::string& time)
	{
		wait_until(
			std::atoi(date.substr(0, 4).c_str()),
			std::atoi(date.substr(4, 2).c_str()),
			std::atoi(date.substr(6, 2).c_str()),
			std::atoi(time.substr(0, 2).c_str()),
			std::atoi(time.substr(2, 2).c_str()),
			std::atoi(time.substr(4, 2).c_str())
		);
	}
	long long timer::getTime()
	{
		return std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::system_clock::now().time_since_epoch()
		).count();
	}
}
#endif