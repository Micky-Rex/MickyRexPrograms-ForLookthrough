
#include <string>
#include <conio.h>
#include <fstream>
#include <iostream>
#include <Windows.h>
#include "SFML/Graphics.hpp"

#include "SkelAnimPlayer.h"
#define key_down(key_name) ((GetAsyncKeyState(key_name))?1:0)	//用来检键盘按键的点击事件 

using namespace sf;
using namespace std;

POINT mouse;
HWND window_hwnd;
HWND last_window_hwnd;

void get_mouse();
float get_time_second(Clock& clock);
float radians(float degrees);
float degrees(float radians);
float distance(float x1, float y1, float x2, float y2);
void print_help_info();

string instruction = "none";
string file_name;
string anim_name = "";

unsigned int focus_bone_id = -1;

bool playing_anim = false;
int playing_state = 0;

SkelAnim* anim = new SkelAnim();
SkelAnimPlayer anim_player(anim);

struct auxiliary_line
{
	Vertex line[2];

	void init(float x1, float y1, float x2, float y2)
	{
		line[0].position = Vector2(x1, y1);
		line[1].position = Vector2(x2, y2);
		line[0].color = Color::White;
		return;
	}
	void render(RenderWindow* window)
	{
		window->draw(line, 2, Lines);
		return;
	}
};

auxiliary_line auxiliary_lines[100];

DWORD WINAPI instruction_thread(LPVOID lp_param)
{
	#pragma region Initial Page
	string str;
	printf("======  Skeletal Animation Editor - MickyRex  ======\n");
#pragma endregion
	while (true)
	{
		printf("> : ");
		cin >> str;
		//printf("%s", str.c_str());
		if (!strcmp(str.c_str(), "new"))
		{
			*anim = SkelAnim();
			anim_name = "";
			playing_anim = false;
		}
		else if (!strcmp(str.c_str(), "open"))
		{
			*anim = SkelAnim();
			anim_name = "";
			cin >> file_name;
			anim->load_from_file(file_name);
			playing_anim = false;
		}
		else if (!strcmp(str.c_str(), "save"))
		{
			anim->save_as_file(file_name);
		}
		else if (!strcmp(str.c_str(), "save_as"))
		{
			cin >> file_name;
			anim->save_as_file(file_name);
		}
		else if (!strcmp(str.c_str(), "new_anim"))
		{
			cin >> anim_name;
			anim->key_frame[anim_name].clear();
		}
		else if (!strcmp(str.c_str(), "key_frame"))
		{
			float time_stamp;
			cin >> anim_name >> time_stamp;
			KeyFrame key_frame;
			key_frame.time_stamp = time_stamp;
			key_frame.key_position_x = anim->bones[0].pos_x;
			key_frame.key_position_y = anim->bones[0].pos_y;
			for (int i = 0; i < MAX_BONE; i++)
			{
				key_frame.key_rotation[i] = anim->bones[i].rotation;
				key_frame.key_scaling[i] = anim->bones[i].length;
			}
			anim->add_key_frame(anim_name, key_frame);
		}
		else if (!strcmp(str.c_str(), "move_to"))
		{
			float x, y;
			cin >> x >> y;
			anim->bones[0].move_to(x, y);
		}
		else if (!strcmp(str.c_str(), "set_rotation"))
		{
			float r;
			cin >> r;
			if (focus_bone_id == -1)
			{
				printf("> : [Info] : No any bones selected\n");
			}
			else
			{
				anim->bones[focus_bone_id].set_rotation(r);
			}
		}
		else if (!strcmp(str.c_str(), "toward"))
		{
			float x, y;
			cin >> x >> y;
			if (focus_bone_id == -1)
			{
				printf("> : [Info] : No any bones selected\n");
			}
			else
			{
				Bone& father = anim->bones[anim->get_bone_father(focus_bone_id)];
				anim->bones[focus_bone_id].set_rotation(atan2(y - father.pos_y, x - father.pos_x) - father.get_direction());
			}
		}
		else if (!strcmp(str.c_str(), "resize"))
		{
			int length;
			cin >> length;
			if (focus_bone_id == -1)
			{
				printf("> : [Info] : No any bones selected\n");
			}
			else
			{
				anim->bones[focus_bone_id].length = length;
			}
		}
		else if (!strcmp(str.c_str(), "del_anim"))
		{
			string name;
			cin >> name;
			anim->key_frame.erase(name);
			anim_name = "";
		}
		else if (!strcmp(str.c_str(), "del_key_frame"))
		{
			bool f = false;
			string name;
			float ts;
			cin >> name >> ts;
			if (!anim->delete_key_frame(name, ts)) printf("> : [Info] Can't find key_frame at %fs!\n", ts);
		}
		else if (!strcmp(str.c_str(), "set_play_time"))
		{
			float t;
			cin >> t;
			anim_player.init(t);
			if (anim_name != "")
			{
				anim_player.play(anim_name, 0);
			}
			playing_anim = false;
		}
		else if (!strcmp(str.c_str(), "play"))
		{
			cin >> anim_name;
			playing_anim = true;
			playing_state = 0;
			anim_player.init(0.0f);
		}
		else if (!strcmp(str.c_str(), "play_reverse"))
		{
			cin >> anim_name;
			playing_anim = true;
			playing_state = 1;
			anim_player.init(anim->key_frame[anim_name].back().time_stamp);
		}
		else if (!strcmp(str.c_str(), "ls"))
		{
			anim_player.print_anim_list();
		}
		else if (!strcmp(str.c_str(), "get_pos"))
		{
			if (focus_bone_id != -1)
			{
				printf("> : (%.2f, %.2f)\n", anim->bones[focus_bone_id].pos_x, anim->bones[focus_bone_id].pos_y);
			}
			else
			{
				printf("> : [Info] : No any bones selected\n");
			}
		}
		else if (!strcmp(str.c_str(), "get_rotation"))
		{
			if (focus_bone_id != -1)
			{
				printf("> : %.5f\n", anim->bones[focus_bone_id].rotation);
			}
			else
			{
				printf("> : [Info] : No any bones selected\n");
			}
		}
		else if (!strcmp(str.c_str(), "get_size"))
		{
			if (focus_bone_id != -1)
			{
				printf("> : %.2f\n", anim->bones[focus_bone_id].length);
			}
			else
			{
				printf("> : [Info] : No any bones selected\n");
			}
		}
		else if (!strcmp(str.c_str(), "exit"))
		{
			char c;
			printf("Are you sure? (Y/n) : ");
			cin >> c;
			if (c == 'Y')
			{
				delete anim;
				exit(0);
			}
		}
		else if (!strcmp(str.c_str(), "close"))
		{
			char c;
			printf("Are you sure? (Y/n) : ");
			cin >> c;
			if (c == 'Y')
			{
				*anim = SkelAnim();
				anim_name = "";
				playing_anim = false;
			}
		}
		else if (!strcmp(str.c_str(), "help"))
		{
			print_help_info();
		}
		else
		{
			printf("> : Can't find instruction named %s! Input \"help\" to print help information\n", str.c_str());
		}
	}
	return 0;
}

int main()
{
	SetWindowPos(GetForegroundWindow(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	RenderWindow window(VideoMode(1440, 960), string("Skeletal Animation Editor - " + file_name).c_str());
	View camera = window.getView();
	sf::Clock myClock;

	float currentTime, delta;
	float lastTime = get_time_second(myClock);

	Vertex bone_line[] =
	{
		Vertex(),
		Vertex()
	};

	Vertex axis_line[] =
	{
		Vertex(),
		Vertex()
	};
	axis_line[0].color = Color::Color(100, 100, 100, 255);
	axis_line[1].color = Color::Color(100, 100, 100, 255);

	CircleShape bone_node;
	bone_node.setRadius(3);
	bone_node.setOrigin(3, 3);

	bool first_frame = true;
	Vector2f last_mouse_pos;
	float last_mouse_rotation;
	
	float camera_zoom = 1.0f;

	HANDLE thread_handle = CreateThread(NULL, 0, instruction_thread, NULL, 0, NULL);
	if (thread_handle == NULL)
	{
		printf("- [Error] : Create instrcution_process_thread failed\n");
		return -1;
	}

	anim_player.init(0.0f);

	camera.setCenter(0.f, 0.f);
	while (window.isOpen())
	{
		#pragma region Delta Time
			currentTime = get_time_second(myClock);
			delta = currentTime - lastTime;
			lastTime = currentTime;
#pragma endregion
		#pragma region Process Event
			get_mouse();
			window.setTitle(string("Skeletal Animation Editor - " + file_name).c_str());
			Event event;
			Vector2f mouse_pos = Vector2f(mouse.x, mouse.y);

			while (window.pollEvent(event))
			{
				if (event.type == Event::Closed)
				{
					window.close();
				}
				if (event.type == Event::MouseWheelScrolled)
				{
				
					if (event.mouseWheelScroll.delta < 0)
					{
						// Mouse Wheel up
						camera.zoom(1.1f);
						camera_zoom *= 1.1f;
					}
					else if(event.mouseWheelScroll.delta > 0)
					{
						// Mouse Wheel down
						camera.zoom(0.9f);
						camera_zoom *= 0.9f;
					}
				
				}
			}
#pragma endregion
		#pragma region Process Input
			if (window.hasFocus())
			{
				if (key_down(VK_LBUTTON))
				{
					if (!first_frame and window_hwnd == last_window_hwnd)
					{
						camera.move((last_mouse_pos - mouse_pos) * camera_zoom);
					}
			
				}
				if (key_down('A'))
				{

					while (key_down('A'));
				}
				if (key_down('R'))
				{
					if (key_down(VK_CONTROL))
					{
						if (focus_bone_id != -1)
						{
							anim->bones[focus_bone_id].set_rotation(0);
						}
					}
					else if (key_down(VK_MENU))
					{
						anim->bones[0].move_to(0, 0);
					}
					else
					{
						anim_player.init(0.0f);
					}
				}
				if (key_down('C'))
				{
					camera.setCenter(0.f, 0.f);
				}
				if (key_down(VK_MBUTTON))
				{
					camera.zoom(1 / camera_zoom);
					camera_zoom = 1.0f;
				}
				if (key_down(' '))
				{
					playing_anim = !playing_anim;
					while (key_down(' '));
				}
				if (key_down(VK_ESCAPE))
				{
					focus_bone_id = -1;
				}
				if (key_down('E'))
				{
					if (focus_bone_id != -1)
					{
						int id = -1;
						for (int i = 1; i < MAX_BONE; i++)
						{
							if (anim->bones[i].ID != i)
							{
								id = i;
								break;
							}
						}
						if (id == -1)
						{
							printf("[Info] : Can't add bone (over MAX_BONE)\n> :");
						}
						else
						{
							anim->add_bone(focus_bone_id, Bone(id, 0, 0, 0, 50));
							focus_bone_id = id;
						}
					}
					while (key_down('E'));
				}
				if (key_down('X'))
				{
					if (focus_bone_id == 0)
					{
						while (!anim->bones[0].children.empty())
						{
							anim->delete_bone(anim->bones[0].children.front());
						}
						
					}
					else
					{
						int next_focus = anim->get_bone_father(focus_bone_id);	// 指向其父亲
						anim->delete_bone(focus_bone_id);
						focus_bone_id = next_focus;
					}
					while (key_down('X'));
				}
			}
			
		
			last_mouse_pos = mouse_pos;
#pragma endregion

		window.setView(camera);

		if (playing_anim and anim_name != "")
		{
			if (playing_state == 0)
			{
				anim_player.play(anim_name, delta);
			}
			else if (playing_state == 1)
			{
				anim_player.play_reverse(anim_name, delta);
			}
		}
		anim->update(0);

		axis_line[0].position = Vector2f(0.f, -3000.f);
		axis_line[1].position = Vector2f(0.f, +3000.f);
		window.clear();
		window.draw(axis_line, 2, sf::Lines);
		axis_line[0].position = Vector2f(-4000.f, 0.f);
		axis_line[1].position = Vector2f(+4000.f, 0.f);
		window.draw(axis_line, 2, sf::Lines);

		for (register int i = 0; i < MAX_BONE; i++)
		{
			bone_node.setPosition(anim->bones[i].pos_x, anim->bones[i].pos_y);
			if (anim->bones[i].ID != i) continue;
			if (anim->bones[i].ID == 0)
			{
				bone_node.setFillColor(Color::Red);
			}
			else
			{
				bone_node.setFillColor(Color::White);
			}
			Vector2f mp = window.mapPixelToCoords(Vector2i(mouse.x, mouse.y), camera) - Vector2f(anim->bones[0].pos_x, anim->bones[0].pos_y);;
			if (first_frame)
			{
				last_mouse_rotation = atan2(mp.y, mp.x);
			}
			if (key_down(VK_RBUTTON) and key_down('S'))
			{
				anim->bones[0].rotate(atan2(mp.y, mp.x) - last_mouse_rotation);
				anim->update(0);
			}
			else if (focus_bone_id == i and key_down(VK_RBUTTON))
			{
				int father = anim->get_bone_father(i);
				if(father == -1) anim->bones[i].move_to(window.mapPixelToCoords(Vector2i(mouse.x, mouse.y), camera));
				else
				{
					mp = window.mapPixelToCoords(Vector2i(mouse.x, mouse.y), camera) - Vector2f(anim->bones[father].pos_x, anim->bones[father].pos_y);
					anim->bones[i].set_rotation(atan2(mp.y, mp.x) - anim->bones[father].get_direction());
				}
				//anim->update(focus_bone_id);
			}
			last_mouse_rotation = atan2(mp.y, mp.x);

			if (focus_bone_id == i or (bone_node.getGlobalBounds().contains(window.mapPixelToCoords(Vector2i(mouse.x, mouse.y), camera)) and key_down(VK_LBUTTON)))
			{
				focus_bone_id = i;
				bone_node.setFillColor(Color::Blue);
			}
			bone_node.setPosition(anim->bones[i].pos_x, anim->bones[i].pos_y);
			bone_line[0].position = bone_node.getPosition();

			for (register int j = 0; j < anim->bones[i].children.size(); j++)
			{
				bone_line[1].position = Vector2f(anim->bones[anim->bones[i].children[j]].pos_x, anim->bones[anim->bones[i].children[j]].pos_y);
				window.draw(bone_line, 2, sf::Lines);
			}
			window.draw(bone_node);
		}
		window.display();

		first_frame = false;
	}

	CloseHandle(thread_handle);
	delete anim;
	return 0;
}

void get_mouse()
{
	last_window_hwnd = window_hwnd;
	window_hwnd = GetForegroundWindow();
	GetCursorPos(&mouse);
	ScreenToClient(window_hwnd, &mouse);
	return;
}

float get_time_second(Clock& clock)
{
	return clock.getElapsedTime().asSeconds();
}

float radians(float degrees)
{
	return (degrees / 180.0f) * PI;
}

float degrees(float radians)
{
	return (radians / PI) * 180.0f;
}

float distance(float x1, float y1, float x2, float y2)
{
	float result;
	result = sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
	return result;
}

void print_help_info()
{
	printf("===== Key Help =====\n");
	printf("- A : Add an auxiliary line");
	printf("- R : Replay animation\n");
	printf("- C : Reset camera\n");
	printf("- E : Create new bone\n");
	printf("- X : Delete bone\n");
	printf("- Alt + R : Reset root bone position\n");
	printf("- Ctrl + R : Reset bone rotation(selected)\n");
	printf("- Space : Pause/continue animation\n");
	printf("- Mouse move(press left mouse button) : Move camera / Select bone\n");
	printf("- Mouse move(press right mouse button) : Rotate bone\n");
	printf("- Mouse move(press right mouse button and press S) : Rotate root bone\n");
	printf("- Mouse wheel up : Camera zoom in\n");
	printf("- Mouse wheel down : Camera zoom out\n");
	printf("- Press mouse wheel : Reset camera zoom\n");
	printf("- Esc : Deselect bone\n\n");

	printf("===== Instruction Help =====\n");
	printf("- new [file name] : Create a new animation\n");
	printf("- open [file name] : Open a file\n");
	printf("- save : Save animation file\n");
	printf("- save_as : Save as a animation file\n");
	printf("- new_anim [anim_name] : Create a anim\n");
	printf("- key_frame [anim name][time stamp] : Create a new key frame\n");
	printf("- move_to [position_x] [position_y] : Move bone\n");
	printf("- toward [position_x] [position_y] : Toward a point\n");
	printf("- set_rotation [radians] : Set bone rotation");
	printf("- resize [bone length] : Reset bone length\n");
	printf("- del_anim [animation name] : Delete animation\n");
	printf("- del_key_frame [animation name] [time_stamp] : Delete key_frame in animation\n    .if there are multiple key frames have the same time_stamp, will delete the first one\n");
	printf("- ls : Show animation list\n");
	printf("- get_pos : Get bone position\n");
	printf("- get_rotation : Get bone rotation\n");
	printf("- get_size : Get bone length(size)\n");
	printf("- play [animation name] : Play animation\n");
	printf("- set_play_time [time/s] : Set animation playing time\n");
	printf("- play_reverse [animation name] : Play animation(reverse)\n");
	printf("- exit : Exit the program\n");
	printf("- close : Close this file\n");
	printf("- help : print this help infromation\n");
	return;
}
