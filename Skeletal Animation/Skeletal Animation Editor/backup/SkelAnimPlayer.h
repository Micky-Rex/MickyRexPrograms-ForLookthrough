#pragma once
#include "SkelAnim.h"
#include "defines.h"

class SkelAnimPlayer
{
public:
	SkelAnimPlayer();
	SkelAnimPlayer(SkelAnim* skel);
	void init(float time = 0.0f);
	void play(float delta);
	void play_reverse(float delta);
	float get_play_time();
private:
	bool is_reverse_playing;
	SkelAnim *skel;
	float play_time;	// 动画已播放的事件

private:
	float lerp(float from, float to, float proportion);
	float slerp(float from, float to, float proportion);
};