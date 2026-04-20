#include "SkelAnimPlayer.h"

SkelAnimPlayer::SkelAnimPlayer()
{
	this->play_time = 0.0f;
	this->is_reverse_playing = false;
	return;
}

SkelAnimPlayer::SkelAnimPlayer(SkelAnim* skel)
{
	this->skel = skel;
	this->play_time = 0.0f;
	this->is_reverse_playing = false;
	return;
}

void SkelAnimPlayer::init(float time)
{
	this->play_time = time;
	return;
}

void SkelAnimPlayer::play(float delta)
{
	if (skel->key_frame.size() < 1)
	{
		return;
	}
	int key_frame_cur = -1;
	this->play_time += delta;

	for (int i = 0; i < skel->key_frame.size() - 1; i++)
	{
		if (skel->key_frame.size() == 1)
		{
			key_frame_cur = 0;
			break;
		}
		if (this->play_time >= skel->key_frame[i].time_stamp and
			this->play_time <= skel->key_frame[i + 1].time_stamp)
		{
			key_frame_cur = i;
			break;
		}
	}
	if (key_frame_cur == -1)
	{
		this->skel->bones[0].move_to(this->skel->key_frame.back().key_position_x, this->skel->key_frame.back().key_position_y);
		for (int i = 0; i < MAX_BONE; i++)
		{
			this->skel->bones[i].set_rotation(this->skel->key_frame.back().key_rotation[i]);
		}
		this->skel->update(0);
		return;
	}
	float t1, t2;
	t1 = skel->key_frame[key_frame_cur].time_stamp;
	t2 = skel->key_frame[key_frame_cur + 1].time_stamp;
	float position_x, position_y, rotation, proportion;
	proportion = (this->play_time - t1) / (t2 - t1);
	position_x = lerp(skel->key_frame[key_frame_cur].key_position_x, skel->key_frame[key_frame_cur + 1].key_position_x, proportion);
	position_y = lerp(skel->key_frame[key_frame_cur].key_position_y, skel->key_frame[key_frame_cur + 1].key_position_y, proportion);
	skel->bones[0].move_to(position_x, position_y);

	for (int i = 0; i < MAX_BONE; i++)
	{
		rotation = slerp(skel->key_frame[key_frame_cur].key_rotation[i], skel->key_frame[key_frame_cur + 1].key_rotation[i], proportion);
		skel->bones[i].set_rotation(rotation);
	}
	skel->update(0);
	return;
}

void SkelAnimPlayer::play_reverse(float delta)
{
	if (skel->key_frame.size() < 1)
	{
		return;
	}
	int key_frame_cur = -1;
	this->play_time -= delta;

	for (int i = 0; i < skel->key_frame.size() - 1; i++)
	{
		if (skel->key_frame.size() == 1)
		{
			key_frame_cur = 0;
			break;
		}
		if (this->play_time >= skel->key_frame[i].time_stamp and
			this->play_time <= skel->key_frame[i + 1].time_stamp)
		{
			key_frame_cur = i;
			break;
		}
	}
	if (key_frame_cur == -1)
	{
		this->skel->bones[0].move_to(this->skel->key_frame.front().key_position_x, this->skel->key_frame.front().key_position_y);
		for (int i = 0; i < MAX_BONE; i++)
		{
			this->skel->bones[i].set_rotation(this->skel->key_frame.front().key_rotation[i]);
		}
		this->skel->update(0);
		return;
	}
	float t1, t2;
	float rotation, proportion;
	for (int i = 0; i < MAX_BONE; i++)
	{
		t1 = skel->key_frame[key_frame_cur].time_stamp;
		t2 = skel->key_frame[key_frame_cur + 1].time_stamp;
		proportion = (this->play_time - t1) / (t2 - t1);
		rotation = slerp(skel->key_frame[key_frame_cur].key_rotation[i], skel->key_frame[key_frame_cur + 1].key_rotation[i], proportion);
		skel->bones[i].set_rotation(rotation);
	}
	skel->update(0);
	return;
}

float SkelAnimPlayer::get_play_time()
{
	return this->play_time;
}

float SkelAnimPlayer::lerp(float from, float to, float proportion)	// 在 from, to 两值间线性插值 proportion (0~1, 0: 在from; 1: 在to)表示插值位置
{
	proportion = max(min(proportion, 1.0f), 0.0f);
	//return (proportion - from) / (to - from);
	//return (from * (1 - proportion) + to * proportion) / 2.0f;
	return from + proportion * (to - from);
}

float SkelAnimPlayer::slerp(float from, float to, float proportion)
{
	proportion = max(min(proportion, 1.0f), 0.0f);
	if (abs(from - to) >= PI)
	{
		if (from > to)
		{
			to += PI * 2;
		}
		else
		{
			from += PI * 2;
		}
	}
	return lerp(from, to, proportion);
}