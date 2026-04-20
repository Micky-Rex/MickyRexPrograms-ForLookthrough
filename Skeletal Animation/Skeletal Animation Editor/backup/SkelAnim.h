#pragma once
#include <map>
#include <stack>
#include <vector>
#include <string>
#include <fstream>

#define MAX_BONE 1000

using namespace std;

struct KeyFrame
{
	float time_stamp;
	float key_position_x;	// 根节点坐标关键帧
	float key_position_y;	// 根节点坐标关键帧
	float key_rotation[MAX_BONE];
};

struct Bone
{
	unsigned int ID;
	float pos_x, pos_y;	// 骨骼头部坐标
	float rotation;	// 骨骼相对父节点朝向的朝向 (radians)
	float _rotation;	// 骨骼的全局朝向
	float length;	// 骨骼的长度 (px)
	vector<unsigned int> children;	// 子骨骼 (ID)

	Bone();
	Bone(unsigned int ID, float x, float y, float dir, float len);
	void move(float dx, float dy);
	void move_to(float x, float y);
	void rotate(float change_value);
	void set_rotation(float rotation);
	void update();
	void update(Bone& father);
	void add_child(unsigned int id);
	float get_direction();
};

class SkelAnim
{
public:
	Bone bones[MAX_BONE];
	map<string, vector<KeyFrame> > key_frame;	// 关键帧

	SkelAnim();
	int load_from_file(string file_name);
	int save_as_file(string file_name);
	int laod_from_binary_file(string file_name);
	int save_as_binary_file(string file_name);
	void add_bone(unsigned int father, Bone bone);
	void add_bone(Bone& father, Bone bone);
	void update(unsigned int root);
	void add_key_frame(string anim_name, KeyFrame key_frame);
};