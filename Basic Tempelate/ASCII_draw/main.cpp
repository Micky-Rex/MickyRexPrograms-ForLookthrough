#include "draw.h"
#include <math.h>
#include <thread>
#include <unistd.h>
#include <termios.h>
using namespace std;
#define MAP_SIZE 39

int player_dir = 1;
int player_size = 3;
int g[MAP_SIZE][MAP_SIZE]{0};	// g[x][y]

screen scr(MAP_SIZE, MAP_SIZE, 1, 1, 2);
int main()
{
	printf("\033[?25l");

	srand(time(NULL));

	termios old_settings, new_settings;
	tcgetattr(STDIN_FILENO, &old_settings);
	new_settings = old_settings;
	new_settings.c_lflag &= ~(ICANON | ECHO);	// 关闭规范模式和回显
	new_settings.c_cc[VMIN] = 0;	// 至少读取一个字符
	new_settings.c_cc[VTIME] = 0;	// 无超时
	tcsetattr(STDIN_FILENO, TCSANOW, &new_settings);	// 应用新设置

	clear_all();

	for (int i = 0; i < MAP_SIZE; i++)
	{
		for (int j = 0; j < MAP_SIZE; j++)
		{
			g[i][j] = 0;
		}
	}
	g[0][0] = 1;
	g[1][0] = 2;
	g[2][0] = 3;
	g[15][15] = -1;

	char input;
	bool lastframe_eaten = false;
	while (true)
	{
		if (read(STDIN_FILENO, &input, 1) == 1)	// 读取一个字符
		{
			if (input == '\033') break;
			if (input == 'w' and player_dir != 2) player_dir = 0;
			if (input == 'd' and player_dir != 3) player_dir = 1;
			if (input == 's' and player_dir != 0) player_dir = 2;
			if (input == 'a' and player_dir != 1) player_dir = 3;
		}
		scr.clear();

		vec2 head_pos = vec2(-100,-1);
		for (int i = 0; i < MAP_SIZE; i++)
		{
			for (int j = 0; j < MAP_SIZE; j++)
			{
				if (g[i][j] == 0) continue;
				if (g[i][j] > 0)
				{
					scr.draw_pixel(vec2(i, j), rgb(0, 150, 0));
					if (!lastframe_eaten) g[i][j]--;
					if (g[i][j] != player_size - 1 or head_pos.x != -100) continue;
					int x = i, y = j;
					if (player_dir == 0) y--;
					if (player_dir == 1) x++;
					if (player_dir == 2) y++;
					if (player_dir == 3) x--;
					head_pos = vec2(x, y);
				}
				if(g[i][j] == -1)
					scr.draw_pixel(vec2(i, j), rgb(255, 255, 0));
			}
		}
		if (!head_pos.in_area(vec2(0, 0), vec2(MAP_SIZE-1, MAP_SIZE-1))) break;
		lastframe_eaten = false;
		int this_grid = g[head_pos.x][head_pos.y];
		if (this_grid > 0) break;
		g[head_pos.x][head_pos.y] = player_size;
		if (this_grid == -1)
		{
			player_size++;
			int x = head_pos.x, y = head_pos.y;
			while (g[x][y] != 0) x = rand() % MAP_SIZE, y = rand() % MAP_SIZE;
			g[x][y] = -1;
			lastframe_eaten = true;
		}
		scr.draw_pixel(head_pos, rgb(0, 255, 0));
		scr.display();
		this_thread::sleep_for(chrono::milliseconds(100));
	}
	printf("\033[41;1H\033[?25hGame Over!\n");
	tcsetattr(STDIN_FILENO, TCSANOW, &old_settings);	// 还原设置
}