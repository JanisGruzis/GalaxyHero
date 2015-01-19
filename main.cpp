#include <iostream>
#include <ncurses.h>
#include <list>
#include <cstdlib>
#include <time.h>
#define PLAIN_SPEED 1
using namespace std;

const int WIDTH = 50;

struct point
{
	int x, y;
} start, size;

class Item
{
public:

	int cordCount;
	int **cord;
	int shootWay;

	point pos;
	list<point> shoots;

	Item()
	{
		this->cordCount = 0;
	}

	void draw()
	{
		for (int i = 0; i < this->cordCount; ++i)
		{
			int x = start.x + this->pos.x + this->cord[i][1];
			int y = start.y + this->pos.y + this->cord[i][0];
			mvaddch(y, x, (char)this->cord[i][2]);
		}

		this->drawShoots();
	}

	void drawShoots()
	{
		list<point>::iterator it;
		for (it = this->shoots.begin(); it != this->shoots.end(); ++it)
		{
			(*it).y += this->shootWay;
			int x = start.x + (*it).x;
			int y = start.y + (*it).y;
			mvaddch(y, x, '*');
		}
	}

	void shoot()
	{
		this->shoots.push_back(this->pos);
	}

	bool collision(const Item &item)
	{
		for (int i = 0; i < item.cordCount; ++i)
		{
			point p;
			p.x = item.pos.x + item.cord[i][1];
			p.y = item.pos.y + item.cord[i][0];
			if (this->pointCollision(p)) return true;
		}

		return false;
	}

	bool pointCollision(point p)
	{
		for (int i = 0; i < this->cordCount; ++i)
		{
			int x = this->pos.x + this->cord[i][1];
			int y = this->pos.y + this->cord[i][0];
			if (x == p.x && y == p.y) return true;
		}

		return false;
	}
};

class Plain : public Item
{
public:
	Plain()
	{
		this->cordCount = 7;
		this->shootWay = -1;
		this->cord = new int*[this->cordCount];
		this->cord[0] = new int[3] {2, 0, '#'};
		this->cord[1] = new int[3] {2, -1, '#'};
		this->cord[2] = new int[3] {2, 1, '#'};
		this->cord[3] = new int[3] {1, 0, '|'};
		this->cord[4] = new int[3] {0, 0, 'T'};
		this->cord[5] = new int[3] {3, 1, 'S'};
		this->cord[6] = new int[3] {3, -1, 'S'};
	}
} plain;

class Monster1 : public Item
{
public:
	Monster1()
	{
		this->cordCount = 4;
		this->shootWay = -1;
		this->cord = new int*[this->cordCount];
		this->cord[0] = new int[3] {0, 0, '#'};
		this->cord[1] = new int[3] {-1, 0, '#'};
		this->cord[2] = new int[3] {-1, 1, '#'};
		this->cord[3] = new int[3] {-1, -1, '#'};
	}
};

list<Item> monsters;

void init()
{
	initscr(); // Init screen for ncurses.
	cbreak(); // Allow OS signals do their work and disable line buffering.
	noecho(); // Dont output input.
	keypad(stdscr, true); // Catch special key events like F1 and arrows.
	halfdelay(1); // Non blocking input.
	getmaxyx(stdscr, size.y, size.x);

	start.y = 0;
	start.x = (size.x - WIDTH) / 2;
	plain.pos.x = WIDTH/2;
	plain.pos.y = size.y - 5;
}

void close()
{
	endwin(); // Close screen for ncurses.
}

int readInput()
{
	int ch = getch();
	switch (ch)
	{
		case KEY_LEFT:
			plain.pos.x = max(0, plain.pos.x - PLAIN_SPEED);
			break;
		case KEY_RIGHT:
			plain.pos.x = min(WIDTH, plain.pos.x + PLAIN_SPEED);
			break;
		case ' ':
			plain.shoot();
			break; 
	}
	return ch;
}

bool generateMonsters()
{
	int cnt = rand() % 2;
	mvprintw(start.x, start.y, "100");
	
	for (int i = 0; i < cnt; ++i)
	{
		Monster1 tmp;
		tmp.pos.y = 0;
		tmp.pos.x = rand() % WIDTH;
		monsters.push_back(tmp);
	}

	list<Item>::iterator it;
	list<point>::iterator its;
	for (it = monsters.begin(); it != monsters.end(); ++it)
	{
		(*it).pos.y += 1;
		if ((*it).pos.y >= size.y)
			it = monsters.erase(it);
		else
			for (its = plain.shoots.begin(); its != plain.shoots.end(); ++its)
			{
				if ((*it).pointCollision(*its))
				{
					it = monsters.erase(it);
				}
			}

		if (plain.collision(*it))
		{
			return true;
		}
	}

	for (it = monsters.begin(); it != monsters.end(); ++it)
	{
		(*it).draw();
	}

	return false;
}

void game()
{
	bool gameOver = false;
	while (1)
	{
		int ch = readInput();
		if (ch == 'q') break;
		clear();
		if (gameOver)
		{
			mvprintw(start.y, start.x, "GAME OVER");
		} else {
			gameOver = generateMonsters();
			plain.draw();
			curs_set(0);
		}
		refresh();
	}
}

int main()
{
	srand(time(NULL));
	init();
	game();	
	close();

    return 0;
}
