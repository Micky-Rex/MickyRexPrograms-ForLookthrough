#include "SkelAnimPlayer.h"

SkelAnimPlayer::SkelAnimPlayer()
{
	this->play_time = 0.0f;
	return;
}

SkelAnimPlayer::SkelAnimPlayer(SkelAnim* skel)
{
	this->skel = skel;
	this->play_time = 0.0f;
	return;
}

void SkelAnimPlayer::init(float time)
{
	this->play_time = time;
	return;
}

void SkelAnimPlayer::play(string anim_name, float delta)
{
	if (this->skel->key_frame.count(anim_name) == 0)
	{
		printf("[Error] : Can't find anim named %s!\n", anim_name.c_str());
		this->print_anim_list();
		printf("> : Press any key to continue...\n");
		getchar();
		return;
	}
	int key_frame_cur = -1;
	this->play_time += delta;
	
	for (int i = 0; i < this->skel->key_frame[anim_name].size() - 1; i++)
	{
		if (this->skel->key_frame[anim_name].size() == 1)
		{
			key_frame_cur = 0;
			break;
		}
		if (this->play_time >= this->skel->key_frame[anim_name][i].time_stamp and
			this->play_time <= this->skel->key_frame[anim_name][i + 1].time_stamp)
		{
			key_frame_cur = i;
			break;
		}
	}
	if (this->play_time >= this->skel->key_frame[anim_name].back().time_stamp)
	{
		this->skel->bones[0].move_to(this->skel->key_frame[anim_name].back().key_position_x, this->skel->key_frame[anim_name].back().key_position_y);
		for (int i = 0; i < MAX_BONE; i++)
		{
			this->skel->bones[i].set_rotation(this->skel->key_frame[anim_name].back().key_rotation[i]);
			this->skel->bones[i].length = this->skel->key_frame[anim_name].back().key_scaling[i];
		}
		this->skel->update(0);
		return;
	}
	float t1, t2;
	t1 = this->skel->key_frame[anim_name][key_frame_cur].time_stamp;
	t2 = this->skel->key_frame[anim_name][key_frame_cur + 1].time_stamp;
	float position_x, position_y, rotation, scaling, proportion;
	proportion = (this->play_time - t1) / (t2 - t1);
	position_x = lerp(this->skel->key_frame[anim_name][key_frame_cur].key_position_x, this->skel->key_frame[anim_name][key_frame_cur + 1].key_position_x, proportion);
	position_y = lerp(this->skel->key_frame[anim_name][key_frame_cur].key_position_y, this->skel->key_frame[anim_name][key_frame_cur + 1].key_position_y, proportion);
	this->skel->bones[0].move_to(position_x, position_y);

	for (int i = 0; i < MAX_BONE; i++)
	{
		rotation = slerp(this->skel->key_frame[anim_name][key_frame_cur].key_rotation[i], this->skel->key_frame[anim_name][key_frame_cur + 1].key_rotation[i], proportion);
		scaling = lerp(this->skel->key_frame[anim_name][key_frame_cur].key_scaling[i], this->skel->key_frame[anim_name][key_frame_cur + 1].key_scaling[i], proportion);
		this->skel->bones[i].set_rotation(rotation);
		this->skel->bones[i].length = scaling;
	}
	this->skel->update(0);
	return;
}

void SkelAnimPlayer::play_reverse(string anim_name, float delta)
{
	if (this->skel->key_frame.count(anim_name) == 0)
	{
		printf("[Error] : Can't find anim named %s!\n", anim_name.c_str());
		this->print_anim_list();
		printf("> : Press any key to continue...\n");
		getchar();
		return;
	}
	int key_frame_cur = -1;
	this->play_time -= delta;

	for (int i = 0; i < this->skel->key_frame[anim_name].size() - 1; i++)
	{
		if (this->skel->key_frame[anim_name].size() == 1)
		{
			key_frame_cur = 0;
			break;
		}
		if (this->play_time >= this->skel->key_frame[anim_name][i].time_stamp and
			this->play_time <= this->skel->key_frame[anim_name][i + 1].time_stamp)
		{
			key_frame_cur = i;
			break;
		}
	}
	if (this->play_time <= this->skel->key_frame[anim_name].front().time_stamp)
	{
		this->skel->bones[0].move_to(this->skel->key_frame[anim_name].front().key_position_x, this->skel->key_frame[anim_name].front().key_position_y);
		for (int i = 0; i < MAX_BONE; i++)
		{
			this->skel->bones[i].set_rotation(this->skel->key_frame[anim_name].front().key_rotation[i]);
			this->skel->bones[i].length = this->skel->key_frame[anim_name].back().key_scaling[i];
		}
		this->skel->update(0);
		return;
	}
	float t1, t2;
	t1 = this->skel->key_frame[anim_name][key_frame_cur].time_stamp;
	t2 = this->skel->key_frame[anim_name][key_frame_cur + 1].time_stamp;
	float position_x, position_y, rotation, scaling, proportion;
	proportion = (this->play_time - t1) / (t2 - t1);
	position_x = lerp(this->skel->key_frame[anim_name][key_frame_cur].key_position_x, this->skel->key_frame[anim_name][key_frame_cur + 1].key_position_x, proportion);
	position_y = lerp(this->skel->key_frame[anim_name][key_frame_cur].key_position_y, this->skel->key_frame[anim_name][key_frame_cur + 1].key_position_y, proportion);
	this->skel->bones[0].move_to(position_x, position_y);

	for (int i = 0; i < MAX_BONE; i++)
	{
		rotation = slerp(this->skel->key_frame[anim_name][key_frame_cur].key_rotation[i], this->skel->key_frame[anim_name][key_frame_cur + 1].key_rotation[i], proportion);
		scaling = lerp(this->skel->key_frame[anim_name][key_frame_cur].key_scaling[i], this->skel->key_frame[anim_name][key_frame_cur + 1].key_scaling[i], proportion);
		this->skel->bones[i].set_rotation(rotation);
		this->skel->bones[i].length = scaling;
	}
	this->skel->update(0);
	return;
}

void SkelAnimPlayer::print_anim_list()
{
	printf("Animation list:\n");
	map<string, vector<KeyFrame> >::iterator it;
	for (it = this->skel->key_frame.begin(); it != this->skel->key_frame.end(); it++)
	{
		printf("  %s\n", it->first.c_str());
		vector<KeyFrame>::iterator vit;
		for (vit = it->second.begin(); vit != it->second.end(); vit++)
		{
			printf("   Key_frame at %.2fs\n", vit->time_stamp);
		}
	}
	printf("\n");
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