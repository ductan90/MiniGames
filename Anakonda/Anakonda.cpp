#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "fssimplewindow.h"
#include <time.h>

#define SCREENX 800
#define SCREENY 600
#define NODE_WIDTH 10
#define UP -10
#define DOWN 10
#define RIGHT 1
#define LEFT -1

class Node {
public:
	int x;
	int y;
	Node *next;
	Node *prev;

	Node(int inputx, int inputy) {
		this->x = inputx;
		this->y = inputy;
	}
};

class Snake {
public:
	Node *head;
	Node *tail;
	int _nodecount;
	int _score;

	bool containsNode(int x, int y) {
		Node *tempnode = this -> head;
		while (tempnode != this -> tail) {
			if (abs(tempnode->x - x) < 6 && abs(tempnode->y - y) < 6) {
				return true;
			}
			tempnode = tempnode->next;
		}
		if (abs(tempnode->x - x) < 6 && abs(tempnode->y - y) < 6) {
			return true;
		}
		return false;
	}

	bool eatNode(Node *newhead, int x, int y) {
		return (abs(newhead->x - x) < 6) && (abs(newhead->y - y) < 6);
	}

	void addHead(Node *newhead, int foodx, int foody, int &flag) {
		if (eatNode(newhead, foodx, foody)) {
			this->head->prev = newhead;
			newhead->next = this->head;
			this->head = newhead;
			this->_score+=1;
			this->_nodecount+=1;
			flag = 1;
		}
		else {
			this->head->prev = newhead;
			newhead->next = this->head;
			this->head = newhead;
			Node *oldtail = this->tail;
			this->tail = oldtail->prev;
			this->tail->next = NULL;
			free(oldtail);
		}
	}

	bool Check_Eat_Wall() {
		if (this->head->x < 0 || this->head->x > SCREENX || this->head->y <  0|| this->head->y > SCREENY) {
			return true;
		}
		Node *tempnode = this->head->next;
		while (tempnode != this->tail) {
			if (eatNode(this->head,(tempnode->x), (tempnode->y))) {
				return true;
			}
			tempnode = tempnode->next;
		}
		if (eatNode(this->head, (tempnode->x), (tempnode->y))) {
			return true;
		}
		return false;
	}
};


void Init_Snake(Snake * &snake_ins) {
	snake_ins = new Snake();
	Node *headnode = new Node(0, 10);
	Node *tailnode = new Node(0, 0);
	headnode->next = tailnode;
	tailnode->prev = headnode;
	snake_ins->head = headnode;
	snake_ins->tail = tailnode;
	snake_ins->_nodecount = 2;
	snake_ins->_score = 0;
}

void Draw_Square(int lefttopx, int lefttopy) {
	glLineWidth(1);
	glBegin(GL_QUADS);
	glVertex2i(lefttopx, lefttopy);
	glVertex2i(lefttopx + 10, lefttopy);
	glVertex2i(lefttopx + 10, lefttopy + 10);
	glVertex2i(lefttopx, lefttopy + 10);
	glEnd();
}

int main() {
	FsOpenWindow(16, 16, SCREENX, SCREENY, 1);
	int terminate = 0;
	int direction = RIGHT;
	int flag = 1;
	int foodx, foody;
	Snake *snake_ins;
	Init_Snake(snake_ins);

	while (terminate == 0) {
		FsPollDevice();
		int key = FsInkey();

		switch (key) {
		case FSKEY_ESC: terminate = 1; break;
		case FSKEY_LEFT: direction = LEFT; break;
		case FSKEY_RIGHT: direction = RIGHT; break;
		case FSKEY_UP: direction = UP; break;
		case FSKEY_DOWN: direction = DOWN; break;
		}

		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		/*generate new food*/
		if (flag == 1) {
			srand((int)time(NULL));
			foodx = rand() % 790;
			foody = rand() % 590;
			while (snake_ins->containsNode(foodx, foody)) {
				foodx = rand() % 790;
				foody = rand() % 590;
			}
			flag = 0;
		}
		/*draw the snake and food*/
		Node *tempnode = snake_ins->head;
		while (tempnode != snake_ins -> tail) {
			glColor3ub(0, 0, 0);
			Draw_Square(tempnode->x, tempnode->y);
			tempnode = tempnode->next;
		}
		Draw_Square(snake_ins->tail->x, snake_ins->tail->y);
		glColor3ub(255, 0, 0);
		Draw_Square(foodx, foody);

		/*next move*/
		int newx = snake_ins->head->x + (direction % 10)*10;
		int newy = snake_ins->head->y + (direction / 10)*10;
		Node *newnode = new Node(newx, newy); // new head node
		snake_ins->addHead(newnode, foodx, foody, flag);
		if (snake_ins->Check_Eat_Wall()) {
			terminate = 1;
		}
		FsSwapBuffers();
		FsSleep(25);
	}
	printf("You have scored %d", snake_ins->_score);
}