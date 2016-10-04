/*Author: Xiaojie Li*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "fssimplewindow.h"

void DrawTarget(int x, int y){//(x,y) is the lefthigh corner
	glColor3ub(255, 0, 0);

	glBegin(GL_QUADS);
	glVertex2i(x, y);
	glVertex2i(x + 50, y);
	glVertex2i(x + 50, y + 50);
	glVertex2i(x, y + 50);
	glEnd();
}

void MoveTarget(int &x, int &y, int scrnSizeX, int scrnSizeY) {// move per second
	y += 0.25 * 10;
	if (y + 50 > scrnSizeY) {//as soon as it reaches the bottom
		y = 60; //I think this should be 60 pixels instead of 60 meters
	}
}

void DrawCannon() {// 1 meter width and 1 meter height
	int cannonRightUpX = 50 + 10;
	int cannonRightUpY = 600 - 50 - 10;
	int cannonLeftUpX = 50;
	int cannonLeftUpY = 600 - 50 - 10;
	int cannonLeftDownX = 50;
	int cannonLeftDownY = 600 - 50;
	int cannonRightDownX = 50 + 10;
	int cannonRightDownY = 600 - 50;
	glColor3ub(0, 0, 255);

	glBegin(GL_QUADS);
	glVertex2i(cannonLeftDownX, cannonLeftDownY);
	glVertex2i(cannonRightDownX, cannonRightDownY);
	glVertex2i(cannonRightUpX, cannonRightUpY);
	glVertex2i(cannonLeftUpX, cannonLeftUpY);
	glEnd();
}

void DrawPin(double time) {
	const double PI = 3.1415927;
	double cannonRightUpX = 50.0 + 10;
	double cannonRightUpY = 600.0 - 50.0 - 10.0;
	glColor3ub(0, 0, 255);
	glLineWidth(1);

	glBegin(GL_LINES);
	glVertex2d(cannonRightUpX, cannonRightUpY);
	glVertex2d(cannonRightUpX + 30 * cos(0.5*PI*(time / 90.0)), cannonRightUpY - 30 * sin(0.5*PI*(time / 90.0))); // 3 meters length
	glEnd();
}

void DrawBall(int cx, int cy) {
	const double PI = 3.1415927;
	double r = 2.0;// radius is 0.2 m
	glColor3ub(255, 0, 0);
	glLineWidth(1);
	glBegin(GL_TRIANGLE_FAN);
	for (int i = 0; i < 64; i++) {
		double angle = (double)i*PI / 32.0;
		double x = cx + cos(angle)*r;
		double y = cy + sin(angle)*r;
		glVertex2d(x, y);
	}
	glEnd();
}

void MoveBall(double &x, double &y, double &vx, double &vy, double m, double dt) {
	double fx, fy, ax, ay;
	// based on the	Newton 2nd Law
	x = x + vx*dt;
	y = y + vy*dt;

	fx = 0.0;
	fy = m*9.8;//Gravity 

	ax = fx / m;
	ay = fy / m;

	vx = vx + ax*dt;
	vy = vy + ay*dt;
}

int CheckHitTarget(int missileX, int missileY, int targetX, int targetY, int targetSizeX, int targetSizeY) {
	int relativeX, relativeY;
	relativeX = missileX - targetX;
	relativeY = missileY - targetY;
	if (0 <= relativeX && relativeX < targetSizeX && 0 <= relativeY && relativeY < targetSizeY) {
		return 1;
	}
	else {
		return 0;
	}
}

void DrawObstacle(int x, int y, int w, int h) {//(x,y) is the lefthigh corner
	glColor3ub(0, 255, 0);

	glBegin(GL_QUADS);
	glVertex2i(x, y);
	glVertex2i(x + w, y);
	glVertex2i(x + w, y + h);
	glVertex2i(x, y + h);
	glEnd();
}

void DrawEmptyRec(int x, int y, int w, int h) {
	glColor3ub(0, 255, 0);

	glBegin(GL_LINE_LOOP);
	glVertex2i(x, y);
	glVertex2i(x + w, y);
	glVertex2i(x + w, y + h);
	glVertex2i(x, y + h);
	glEnd();
}

void DrawStipple(int lx1, int ly1, int lx2, int ly2) {
	glEnable(GL_LINE_STIPPLE);
	glLineStipple(1, 0xff00);
	glBegin(GL_LINES);
	glColor3ub(0, 0, 0);
	glVertex2i(lx1, ly1);// origin of the stipple
	glVertex2i(lx2, ly2);
	glEnd();
	glDisable(GL_LINE_STIPPLE);
}

int main(){
	FsOpenWindow(16, 16, 800, 600, 1);// open the double buffer
	const double PI = 3.1415927;
	int terminate = 0;
	int targetX = 75 * 10;
	int targetY = 60 * 10;
	int base_pin = 30.0; // start from 30 degrees
	double cannonRightUpX = 50.0 + 10;
	double cannonRightUpY = 600.0 - 50.0 - 10.0;
	int missile = 0; // flag of missile
	double missileX = 0, missileY = 0; // coordinate of missile
	int nShot = 0;
	double v = 40 * 10; // 40m/s
	double vx=0.0, vy=0.0; // x and y derection vector of v

	/*generate five random obstacal*/
	srand((int)time(NULL));
	int x1 = rand() % 651;//0-650
	int y1 = rand() % 451;//0-450
	int w1 = 80 + rand() % 71;//80-150
	int h1 = 80 + rand() % 71;//80-150
	int x2 = rand() % 651;
    int y2 = rand() % 451;
	int w2 = 80 + rand() % 71;
	int h2 = 80 + rand() % 71;
	int x3 = rand() % 651;
	int y3 = rand() % 451;
	int w3 = 80 + rand() % 71;
	int h3 = 80 + rand() % 71;
	int x4 = rand() % 651;
	int y4 = rand() % 451;
	int w4 = 80 + rand() % 71;
	int h4 = 80 + rand() % 71;
	int x5 = rand() % 651;
	int y5 = rand() % 451;
	int w5 = 80 + rand() % 71;
	int h5 = 80 + rand() % 71;
	/*whether to show/ detect the obstacles*/
	bool flag1 = true; 
	bool flag2 = true;
	bool flag3 = true;
	bool flag4 = true;
	bool flag5 = true;

	int originalX = 0, originalY = 0;

	while (0 == terminate) {
		FsPollDevice();
		int key = FsInkey();

		switch (key) {
		case FSKEY_ESC: terminate = 1; break;
		case FSKEY_SPACE:
			if (0 == missile) {
				missile = 1;
				missileX = cannonRightUpX + 30 * cos(0.5*PI*((base_pin) / 90.0));
				missileY = cannonRightUpY - 30 * sin(0.5*PI*(base_pin / 90.0));
				originalX = missileX;
				originalY = missileY;
				vx = v*cos(0.5*PI*(base_pin / 90.0));
				vy = -(v*sin(0.5*PI*(base_pin / 90.0))); // orginal vy is at the negative direction of y axis
				nShot++;
			}
			break;
		}

		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		DrawCannon();
		DrawPin(base_pin);
		DrawTarget(targetX, targetY);
		MoveTarget(targetX, targetY, 800, 600);
		if (flag1) {
			DrawObstacle(x1, y1, w1, h1);
		}
		else {
			DrawEmptyRec(x1, y1, w1, h1);
		}
		if (flag2) {
			DrawObstacle(x2, y2, w2, h2);
		}
		else {
			DrawEmptyRec(x2, y2, w2, h2);
		}
		if (flag3) {
			DrawObstacle(x3, y3, w3, h3);
		}
		else {
			DrawEmptyRec(x3, y3, w3, h3);
		}
		if (flag4) {
			DrawObstacle(x4, y4, w4, h4);
		}
		else {
			DrawEmptyRec(x4, y4, w4, h4);
		}
		if (flag5) {
			DrawObstacle(x5, y5, w5, h5);
		}
		else {
			DrawEmptyRec(x5, y5, w5, h5);
		}
		if (missile != 0) {
			MoveBall(missileX, missileY, vx, vy, 1, 0.025);// adjust the missileX and missileY
			DrawStipple(originalX, originalY, missileX, missileY);// draw the trajectory
			if (missileY > 600 || missileX > 800 || missileY < 0) {// missile disappear from screen
				missile = 0;
			}
			DrawBall(missileX, missileY);
		}
		base_pin++;
		if (base_pin == 90.0) {
			base_pin = 0.0; // back to horizon 
		}
		FsSwapBuffers();
		FsSleep(25);

		if (CheckHitTarget(missileX, missileY, targetX, targetY, 50, 50) == 1) {
			printf("Hit!\n");
			printf("You have shotted %d balls\n", nShot);
			terminate = 1;
		}
		if (flag1) {
			if (CheckHitTarget(missileX, missileY, x1, y1, w1, h1) == 1) {
				printf("Destroy an obstacle\n");
				printf("You have shotted %d balls\n", nShot);
				missile = 0;
				flag1 = false;
			}
		}
		if (flag2) {
			if (CheckHitTarget(missileX, missileY, x2, y2, w2, h2) == 1) {
				printf("Destroy an obstacle\n");
				printf("You have shotted %d balls\n", nShot);
				missile = 0;
				flag2 = false;
			}
		}
		if (flag3) {
			if (CheckHitTarget(missileX, missileY, x3, y3, w3, h3) == 1) {
				printf("Destroy an obstacle\n");
				printf("You have shotted %d balls\n", nShot);
				missile = 0;
				flag3 = false;
			}
		}
		if (flag4) {
			if (CheckHitTarget(missileX, missileY, x4, y4, w4, h4) == 1) {
				printf("Destroy an obstacle\n");
				printf("You have shotted %d balls\n", nShot);
				missile = 0;
				flag4 = false;
			}
		}

		if (flag5) {
			if (CheckHitTarget(missileX, missileY, x5, y5, w5, h5) == 1) {
				printf("Destroy an obstacle\n");
				printf("You have shotted %d balls\n", nShot);
				missile = 0;
				flag5 = false;
			}
		}
	}
	return 0;
}
