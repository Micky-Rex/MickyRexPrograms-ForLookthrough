#include "SkelAnim.h"

Bone::Bone()
{
	this->ID = 0;
	this->pos_x = 0;
	this->pos_y = 0;
	this->rotation = 0;
	this->_rotation = 0;
	this->length = 50;
	this->children.clear();
	return;
}

Bone::Bone(unsigned int ID, float x, float y, float dir, float len)
{
	this->ID = ID;
	this->pos_x = x;
	this->pos_y = y;
	this->rotation = dir;
	this->_rotation = 0;
	this->length = len;
	this->children.clear();
	return;
}

void Bone::move(float dx, float dy)
{
	this->pos_x += dx;
	this->pos_y += dy;
	return;
}

void Bone::move_to(float x, float y)
{
	this->pos_x = x;
	this->pos_y = y;
	return;
}

void Bone::rotate(float change_value)
{
	this->rotation += change_value;
	return;
}

void Bone::set_rotation(float rotation)
{
	this->rotation = rotation;
	return;
}

void Bone::update()
{
	this->_rotation = this->rotation;
	return;
}

void Bone::update(Bone& father)
{
	this->pos_x = father.pos_x + cos(father._rotation) * father.length;
	this->pos_y = father.pos_y + sin(father._rotation) * father.length;
	this->_rotation = this->rotation + father._rotation;
	return;
}

void Bone::add_child(unsigned int id)
{
	this->children.push_back(id);
	return;
}

float Bone::get_direction()
{
	return this->_rotation;
}

SkelAnim::SkelAnim()
{
	memset(bones, 0, sizeof(bones));
	bones[0] = Bone();
	return;
}

int SkelAnim::load_from_file(string file_name)
{
	int tmp, bone_num;
	ifstream fin(file_name.c_str());
	fin >> bone_num;
	printf("Bone number: %d\n", bone_num);
	if (bone_num > MAX_BONE)
	{
		printf("[Info] : Too many bones, discard excess parts!\n");
		printf(" MAX_BONE: %d\n", MAX_BONE);
		bone_num = MAX_BONE;
	}
	for (int i = 0; i < bone_num; i++)
	{
		fin >> this->bones[i].ID;
		fin >> this->bones[i].length;
		fin >> tmp;
		this->bones[i].children.resize(tmp);
		for (int j = 0; j < tmp; j++)
		{
			fin >> this->bones[i].children[j];
		}
	}
	fin >> tmp;
	this->key_frame.resize(tmp);
	for (int i = 0; i < tmp; i++)
	{
		fin >> this->key_frame[i].time_stamp >> this->key_frame[i].key_position_x >> this->key_frame[i].key_position_y;
		for (int j = 0; j < bone_num; j++)
		{
			fin >> this->key_frame[i].key_rotation[j];
		}
	}
	return 0;
}

int SkelAnim::save_as_file(string file_name)
{
	ofstream fout(file_name.c_str());
	// Bones
	fout << MAX_BONE << endl;
	for (int i = 0; i < MAX_BONE; i++)
	{
		fout << this->bones[i].ID << endl;
		fout << this->bones[i].length << endl;
		fout << this->bones[i].children.size() << endl;
		for (int j = 0; j < this->bones[i].children.size(); j++)
		{
			fout << this->bones[i].children[j] << ' ';
		}
		fout << endl;
	}
	fout << this->key_frame.size() << endl;
	for (int i = 0; i < this->key_frame.size(); i++)
	{
		fout << this->key_frame[i].time_stamp << ' ' << this->key_frame[i].key_position_x << ' ' << this->key_frame[i].key_position_y << endl;
		for (int j = 0; j < MAX_BONE; j++)
		{
			fout << this->key_frame[i].key_rotation[j] << ' ';
		}
		fout << endl;
	}
	return 0;
}

int SkelAnim::laod_from_binary_file(string file_name)
{
	return 0;
}

int SkelAnim::save_as_binary_file(string file_name)
{
	return 0;
}

void SkelAnim::add_bone(unsigned int father, Bone bone)
{
	bones[bone.ID] = bone;
	bones[father].add_child(bone.ID);
	return;
}

void SkelAnim::add_bone(Bone& father, Bone bone)
{
	bones[bone.ID] = bone;
	bones[father.ID].add_child(bone.ID);
	return;
}

void SkelAnim::update(unsigned int root)
{
	bones[root].update();
	bool vis[MAX_BONE + 5];
	memset(vis, false, sizeof(vis));
	stack<unsigned int> update_stk;
	update_stk.push(root);
	while (!update_stk.empty())
	{
		unsigned int cur = update_stk.top();
		update_stk.pop();
		for (int i = 0; i < bones[cur].children.size(); i++)
		{
			unsigned int w = bones[cur].children[i];
			if (vis[w]) continue;
			bones[w].update(bones[cur]);
			vis[w] = true;
			update_stk.push(w);
		}
	}
	return;
}
