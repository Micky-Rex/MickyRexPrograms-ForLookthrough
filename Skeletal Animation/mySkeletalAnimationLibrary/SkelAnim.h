#pragma once
#include <map>
#include <stack>
#include <vector>
#include <string>
#include <fstream>
#include <assert.h>
#include "SFML/Graphics.hpp"
#include "pch.h"
#define MAX_BONE 1000

using namespace sf;
using namespace std;


struct KeyFrame
{
	float time_stamp;
	float key_position_x;	// key_position_x (root bone)
	float key_position_y;	// key_position_y (root bone)
	float key_rotation[MAX_BONE];
	float key_scaling[MAX_BONE];
};

/**
* @brief Bone for Skeletal Animation
*/
struct Bone
{
	unsigned int ID;	// bone id
	float pos_x, pos_y;	// bone position
	float rotation;	// bone rotation (be relative to father bone)
	float _rotation;	// bone rotation (global)
	float length;	// bone length(size) in pixel
	vector<unsigned int> children;	// children id

	/**
	* @brief Default constructor
	*/
	Bone();

	/**
	* @param ID Bone id
	* @param x x position, usually only valid for the root bone
	* @param y y position, usually only valid for the root bone
	* @param dir rotation
	* @param len length(size)
	* @overload Bone::Bone()
	*/
	Bone(unsigned int ID, float x, float y, float dir, float len);

	/**
	* @brief Move bone, usually only valid for the root bone
	* @param dx x position variation
	* @param dy y position variation
	*/
	void move(float dx, float dy);

	/**
	* @brief Move bone, usually only valid for the root bone
	* @param x new x position
	* @param y new y position
	*/
	void move_to(float x, float y);	/// @brief set bone position

	/**
	* @param pos new position
	* @overload void Bone::move_to(float, float)
	*/
	void move_to(Vector2f pos);

	/**
	* @brief Rotate bone
	* @param change_value rotation variation(radians)
	*/
	void rotate(float change_value);

	/**
	* @brief Set bone rotation
	* @param rotation new rotation(radians)
	*/
	void set_rotation(float rotation);

	/**
	* @brief Update bone state
	* usually only valid for the root bone.
	*/
	void update();

	/**
	* Update bone state
	* @param father this bone's father 
	* Pass in the father bone for transformation.
	* @overload void Bone::update()
	*/
	void update(Bone& father);

	/**
	* @brief Add a child_bone to a bone
	* @param id child bone id
	*/
	void add_child(unsigned int id);

	/**
	* @brief Get bone rotation
	* @return the bone global rotation(radians)
	*/
	float get_direction();
};

class SkelAnim
{
public:
	Bone bones[MAX_BONE];	// all bones in skeletal
	map<string, vector<KeyFrame> > key_frame;	// key_frames [animation name][KeyFrame]

	/**
	* @brief Default constructor
	*/
	SkelAnim();

	/**
	* @brief Load animation
	* load animation from text file
	* @param file_name animation file name
	*/
	int load_from_file(string file_name);

	/**
	* @brief Save animation
	* save animation as text file so you can edit it in text editor
	* \linebreak
	* Save format:
	*	Bone number,
	*	
	*	Bone{id, length, number of children, children id[]},
	*	Bone{id, length, number of children, children id[]},
	*	...
	*	Bone{id, length, number of children, children id[]},
	* 
	*	animations number,
	*	Animation
	* 	{
	*		Key frames number, Animation name,
	*		KeyFrame{time stamp, key position x, key position y, key rotation[Bone number], key scaling[Bone number]},
	*		KeyFrame{time stamp, key position x, key position y, key rotation[Bone number], key scaling[Bone number]},
	*		...
	*		KeyFrame{time stamp, key position x, key position y, key rotation[Bone number], key scaling[Bone number]}
	*	},
	*	Animation
	* 	{
	*		Key frames number, Animation name,
	*		KeyFrame{time stamp, key position x, key position y, key rotation[Bone number], key scaling[Bone number]},
	*		KeyFrame{time stamp, key position x, key position y, key rotation[Bone number], key scaling[Bone number]},
	*		...
	*		KeyFrame{time stamp, key position x, key position y, key rotation[Bone number], key scaling[Bone number]}
	*	},
	*	...
	*	Animation
	* 	{
	*		Key frames number, Animation name,
	*		KeyFrame{time stamp, key position x, key position y, key rotation[Bone number], key scaling[Bone number]},
	*		KeyFrame{time stamp, key position x, key position y, key rotation[Bone number], key scaling[Bone number]},
	*		...
	*		KeyFrame{time stamp, key position x, key position y, key rotation[Bone number], key scaling[Bone number]}
	*	}
	* @param file_name animation file name
	*/
	int save_as_file(string file_name);
	int laod_from_binary_file(string file_name);	/// \brief load animation from binary file
	int save_as_binary_file(string file_name);	/// \brief save animation as binary file so you can't edit it in text editor

	/**
	* @brief Get bone's father id
	* @param child bone id
	* @return father bone id
	*/
	int get_bone_father(unsigned int bone_id);

	/**
	* @param father father bone id
	* @param new bone
	*/
	void add_bone(unsigned int father, Bone bone);

	/**
	* @param father bone
	* @param new bone
	* @overload void SkelAnim::add_bone(unsigned int, Bone)
	*/
	void add_bone(Bone& father, Bone bone);

	/**
	* @brief Update skeletal state
	* @param root root bone id
	*/
	void update(unsigned int root = 0);

	/**
	* Add a key frame to a animation. It will automatically sort keyframes
	* @param anim_name animation name
	* @param key_frame new key_frame
	*/
	void add_key_frame(string anim_name, KeyFrame key_frame);

	void delete_bone(unsigned int id);
	bool delete_key_frame(string anim_name, float time_stamp);

	/**
	* Sort any key frames in a anim_name according to time_stamp
	*/
	void sort_key_frame(string anim_name);
};