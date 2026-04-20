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

void Bone::move_to(Vector2f pos)
{
	this->move_to(pos.x, pos.y);
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
	this->_rotation = this->rotation + father._rotation;
	this->pos_x = father.pos_x + cos(this->_rotation) * this->length;
	this->pos_y = father.pos_y + sin(this->_rotation) * this->length;
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
	this->key_frame.clear();
	return;
}

int SkelAnim::load_from_file(string file_name)
{
	string anim_name;
	int tmp, bone_num, anim_num;
	ifstream fin(file_name.c_str());
	fin >> bone_num;
	printf("> : [Info] Bone number: %d\n", bone_num);
	if (bone_num > MAX_BONE)
	{
		printf("> : [Info] Too many bones, discard excess parts!\n");
		printf("> : MAX_BONE: %d\n", MAX_BONE);
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
	fin >> anim_num;
	register int j = 0;
	for (j = 0; j < anim_num; j++)
	{
		fin >> tmp >> anim_name;
		this->key_frame[anim_name].resize(tmp);
		for (int i = 0; i < tmp; i++)
		{
			fin >> this->key_frame[anim_name][i].time_stamp >> this->key_frame[anim_name][i].key_position_x >> this->key_frame[anim_name][i].key_position_y;
			for (int j = 0; j < bone_num; j++)
			{
				fin >> this->key_frame[anim_name][i].key_rotation[j];
			}
			for (int j = 0; j < bone_num; j++)
			{
				fin >> this->key_frame[anim_name][i].key_scaling[j];
			}
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
	register map<string, vector<KeyFrame> >::iterator it;
	for (it = key_frame.begin(); it != key_frame.end(); it++)
	{
		fout << it->second.size() << ' ' << it->first << endl;
		for (int i = 0; i < it->second.size(); i++)
		{
			fout << it->second[i].time_stamp << ' ' << it->second[i].key_position_x << ' ' << it->second[i].key_position_y << endl;
			for (int j = 0; j < MAX_BONE; j++)
			{
				fout << it->second[i].key_rotation[j] << ' ';
			}
			fout << endl;
			for (int j = 0; j < MAX_BONE; j++)
			{
				fout << it->second[i].key_scaling[j] << ' ';
			}
			fout << endl;
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
	while (!update_stk.empty()) update_stk.pop();
	update_stk.push(root);
	while (!update_stk.empty())
	{
		unsigned int cur = update_stk.top();
		update_stk.pop();
		for (int i = 0; i < bones[cur].children.size(); i++)
		{
			if (vis[bones[cur].children[i]]) continue;
			vis[bones[cur].children[i]] = true;
			bones[bones[cur].children[i]].update(bones[cur]);
			update_stk.push(bones[cur].children[i]);
		}
	}
	return;
}

void SkelAnim::add_key_frame(string anim_name, KeyFrame key_frame)
{
	this->key_frame[anim_name].push_back(key_frame);
	this->sort_key_frame(anim_name);
	return;
}

void SkelAnim::delete_bone(unsigned int id)
{
	vector<int> del;
	del.clear();
	bool vis[MAX_BONE + 5];
	memset(vis, false, sizeof(vis));
	stack<unsigned int> update_stk;
	while (!update_stk.empty()) update_stk.pop();
	update_stk.push(id);
	while (!update_stk.empty())
	{
		unsigned int cur = update_stk.top();
		update_stk.pop();
		for (int i = 0; i < bones[cur].children.size(); i++)
		{
			if (vis[bones[cur].children[i]]) continue;
			vis[bones[cur].children[i]] = true;
			del.push_back(bones[cur].children[i]);
			update_stk.push(bones[cur].children[i]);
		}
	}
	this->bones[this->get_bone_father(id)].children.erase(remove(this->bones[this->get_bone_father(id)].children.begin(), this->bones[this->get_bone_father(id)].children.end(), id), this->bones[this->get_bone_father(id)].children.end());
	;
	this->bones[this->get_bone_father(id)].children.pop_back();

	this->bones[id] = Bone(0, 0, 0, 0, 50);
	while (!del.empty())
	{
		this->bones[del.back()] = Bone(0, 0, 0, 0, 50);
		del.pop_back();
	}
	return;
}

bool SkelAnim::delete_key_frame(string anim_name, float time_stamp)
{
	vector<KeyFrame>::iterator it;
	for (it = this->key_frame[anim_name].begin(); it != this->key_frame[anim_name].end(); it++)
	{
		if (it->time_stamp == time_stamp)
		{
			this->key_frame[anim_name].erase(it);
			return true;
		}
	}
	return false;
}

bool key_frame_sorting_rule(KeyFrame x, KeyFrame y)
{
	return x.time_stamp < y.time_stamp;
}

void SkelAnim::sort_key_frame(string anim_name)
{
	vector<KeyFrame>new_key_frame;
	new_key_frame = this->key_frame[anim_name];
	sort(this->key_frame[anim_name].begin(), this->key_frame[anim_name].begin() + this->key_frame[anim_name].size(), key_frame_sorting_rule);
	return;
}

int SkelAnim::get_bone_father(unsigned int bone_id)
{
	for (int i = 0; i < MAX_BONE; i++)
	{
		for (int j = 0; j < this->bones[i].children.size(); j++)
		{
			if (this->bones[i].children[j] == bone_id)
			{
				return i;
			}
		}
	}
	return -1;
}
