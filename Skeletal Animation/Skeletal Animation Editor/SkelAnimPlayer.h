#pragma once
#include "SkelAnim.h"
#include <cmath>

#ifndef PI
#define PI 3.1415926
#endif

class SkelAnimPlayer
{
public:
	/**
	* @brief Default constructor 
	*/
	SkelAnimPlayer();

	/**
	* @param skel Pass in a Skeletal Animation to play it
	* @overload SkelAnimPlayer::SkelAnimPlayer()
	*/
	SkelAnimPlayer(SkelAnim* skel);

	/**
	* @brief Init player state
	*/
	void init(float time = 0.0f);

	/**
	* @brief play animation
	* @param anim_name Animation name
	* @param delta delta per frame
	*/
	void play(string anim_name, float delta);

	/**
	* @brief play animation reverse
	* @param anim_name Animation name
	* @param delta delta per frame
	*/
	void play_reverse(string anim_name, float delta);

	void print_anim_list();	// print animation name list
	float get_play_time();	// get animation playing time

private:
	SkelAnim *skel;
	float play_time;

	float lerp(float from, float to, float proportion);	// linear interpolation
	float slerp(float from, float to, float proportion);	// Linear interpolation of radians
};