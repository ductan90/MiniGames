/*Author Xiaojie Li, CMU*/
#include <math.h>
#include <stdio.h>
#include "fssimplewindow.h"

#define PI 3.1415927
#define SCREENY 800
#define SCREENX 600

void DrawBall(int cx, int cy) {// black ball
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	double r = 10;// radius is 1 m
	glColor4ub(255, 0, 0,128);
	glLineWidth(1);
	glBegin(GL_TRIANGLE_FAN);
	for (int i = 0; i < 64; i++) {
		double angle = (double)i*PI / 32.0;
		double x = cx + cos(angle)*r;
		double y = cy + sin(angle)*r;
		glVertex2d(x, y);
	}
	glEnd();
	glDisable(GL_BLEND);
}

void DrawMissileBall(int cx, int cy) {// missile ball
	double r = 10;
	glColor3ub(0, 0, 0);
	glLineWidth(1);
	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < 64; i++) {
		double angle = (double)i*PI / 32.0;
		double x = cx + cos(angle)*r;
		double y = cy + sin(angle)*r;
		glVertex2d(x, y);
	}
	glEnd();
}

void MoveBall(double &x, double &y, double &vx, double &vy, double dt, double m) {
	x = x + vx*dt;
	y = y + vy*dt;

	double nF = m*9.8;
	double a = -(0.3*nF); // dynamic moving fraction, 0.3 is the ¦Ì
	vx = vx + a*(vx/fabs(vx))*dt; // not accurate simulation, here is an approximation. Assume the fraction is the same for x and y
	vy = vy + a*(vy /fabs(vy))*dt;
}

void BounceBall(double &vx, double &vy, double x, double y) {// if the ball hit the boundary
	if (x <= 10 || x >= 590) {
		vx = -vx;
	}
	if (y <= 10 || y >= 790) {
		vy = -vy;
	}
}

void ProcessCollision(double &vx1, double &vy1, double &vx2, double &vy2,
	double x1, double y1, double x2, double y2){ // balls collide with each other
	double dx = x2 - x1;
	double dy = y2 - y1;
	double distance = sqrt(dx*dx + dy*dy);
	if (distance < 20.0) {
		double rvx = vx2 - vx1;
		double rvy = vy2 - vy1;

		double dot = dx*rvx + dy*rvy;
		if (dot < 0.0) {
			double nx, ny;
			nx = dx / distance;
			ny = dy / distance;

			double k1 = vx1*nx + vy1*ny;
			double k2 = vx2*nx + vy2*ny;

			vx1 = vx1 + nx*(k2 - k1);
			vy1 = vy1 + ny*(k2 - k1);

			vx2 = vx2 + nx*(k1 - k2);
			vy2 = vy2 + ny*(k1 - k2);
		}
	}
}

void DrawPlayer(int x) { // use a rectangle to represent a player
	glColor3ub(0, 0, 0);
	glBegin(GL_QUADS);

	glVertex2i(x - 10, SCREENY);
	glVertex2i(x + 10, SCREENY);
	glVertex2i(x + 10, SCREENY - 10);
	glVertex2i(x - 10, SCREENY - 10);
	glEnd();
}

void DrawPin(double degree, int playerx) {// 3m length
	glColor3ub(0, 0, 0);
	glLineWidth(1);

	glBegin(GL_LINES);
	glVertex2d(playerx, SCREENY - 10);
	glVertex2d(playerx + 30 * cos(PI*(degree / 180.0)), SCREENY - 10 - 30 * sin(PI*(degree / 180.0)));
	glEnd();
}

void DrawTarget(int x, int y) {// input the left top coordinates
	glColor3ub(127, 127, 127);
	glBegin(GL_QUADS);
	glVertex2i(x, y);
	glVertex2i(x + 50, y);
	glVertex2i(x + 50, y + 50);
	glVertex2i(x, y + 50);
	glEnd();
}

int CheckedHitTarget(int missileX, int missileY, int targetX, int targetY, int targetSizeX, int targetSizeY) { // check if the ball fall into the hole
	int relativeX = missileX - targetX;
	int relativeY = missileY - targetY;

	if (relativeX >= 0 && relativeX < targetSizeX && relativeY >= 0 && relativeY < targetSizeY) {
		return 1;
	}
	else {
		return 0;
	}
}

void DrawStipple(int lx1, int ly1, int lx2, int ly2) { // Draw the aim line
	glEnable(GL_LINE_STIPPLE);
	glLineStipple(1, 0xff00);
	glBegin(GL_LINES);
	glColor3ub(0, 0, 0);
	glVertex2i(lx1, ly1);// origin of the stipple
	glVertex2i(lx2, ly2);
	glEnd();
	glDisable(GL_LINE_STIPPLE);
}

int main() {
	FsOpenWindow(16, 16, SCREENX, SCREENY, 1);
	int terminate = 0;
	/*player*/
	int playerx = 300;
	/*stick*/
	double degree = 90;
	/*missile ball*/
	int missileBall = 0;
	double missileX, missileY;// update status
	double v = 40 * 10; // 40m/s
	int nShot = 0;
	double vx = 0.0, vy = 0.0; 

	/*target ball*/
	double targetX = 300.0, targetY = 300.0;
	double vTx = 0, vTy = 0;


	/*draw the stipple line*/
	int virBall = 0;
	double virBallX, virBallY;
	double originalX, originalY; // Draw the Stipple line's origin
	double vV = 100 * 10; // 100m/s
	double vVx = 0.0, vVy = 0.0;

	while (0 == terminate) {
		FsPollDevice();
		int key = FsInkey();

		switch (key) {
		case FSKEY_ESC: terminate = 1; break;
		case FSKEY_LEFT:
			playerx -= 10;
			if (playerx <= 0) {
				playerx = 0;
			}
			break;
		case FSKEY_RIGHT:
			playerx += 10;
			if (playerx >= 600) {
				playerx = 600;
			}
			break;
		case FSKEY_UP:
			degree += 2;
			if (degree >= 180) {
				degree = 180;
			}
			break;
		case FSKEY_DOWN:
			degree -= 2;
			if (degree <= 0) {
				degree = 0;
			}
			break;
		case FSKEY_SPACE:
			if (0 == missileBall) {
				missileBall = 1;
				missileX = playerx + 30 * cos(PI*(degree / 180.0));
				missileY = SCREENY - 10 - 30 * sin(PI*(degree / 180.0));
				vx = v* cos(PI*(degree / 180));
				vy = -(v* sin(PI*(degree / 180.0)));
				nShot++;
			}
			break;
		case FSKEY_CTRL:
			if (0 == virBall) {
				virBall = 1;
				virBallX = playerx + 30 * cos(PI*(degree / 180.0));
				virBallY = SCREENY - 10 - 30 * sin(PI*(degree / 180.0));
				originalX = virBallX;
				originalY = virBallY;
				vVx = vV* cos(PI*(degree / 180));
				vVy = -(vV* sin(PI*(degree / 180.0)));
			}
		}
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		DrawPlayer(playerx);
		DrawPin(degree, playerx);
		DrawBall(targetX, targetY);
		DrawTarget(0, 0);
		DrawTarget(SCREENX - 50, 0);
		DrawTarget(SCREENX - 50, SCREENY - 50);
		DrawTarget(0, SCREENY - 50);
		DrawTarget(0, (SCREENY / 2) - 25);
		DrawTarget(SCREENX - 50, (SCREENY / 2) - 25);

		if (missileBall != 0) {
			MoveBall(missileX, missileY, vx, vy, 0.025, 8);// affected by fraction 
			if (missileY > 800 || missileX > 600 || missileY < 0 || missileX < 0 || (fabs(vx) < 2 && fabs(vy) < 2)) {
				missileBall = 0;
			}
			DrawMissileBall(missileX, missileY);
			ProcessCollision(vx, vy, vTx, vTy, missileX, missileY, targetX, targetY);
		}
		if (vTx != 0 && vTy != 0) {
			MoveBall(targetX, targetY, vTx, vTy, 0.025, 8);// affected by fraction
			BounceBall(vTx, vTy, targetX, targetY);
		}
		if (virBall != 0) {// the aim line
			MoveBall(virBallX, virBallY, vVx, vVy, 0.025,8);
			if (virBallY > 800 || virBallX > 600 || virBallY < 0 || virBallX < 0) {
				virBall = 0;
			}
			DrawStipple(originalX, originalY, virBallX, virBallY);
		}
		FsSwapBuffers();
		FsSleep(25);
		terminate += CheckedHitTarget(targetX, targetY, 0, 0, 50, 50);
		terminate += CheckedHitTarget(targetX, targetY, SCREENX - 50, 0, 50, 50);
		terminate += CheckedHitTarget(targetX, targetY, SCREENX - 50, SCREENY - 50, 50, 50);
		terminate += CheckedHitTarget(targetX, targetY, 0, SCREENY - 50, 50, 50);
		terminate += CheckedHitTarget(targetX, targetY, 0,(SCREENY / 2) - 25,50, 50);
		terminate += CheckedHitTarget(targetX, targetY, SCREENX - 50,(SCREENY / 2) - 25,50, 50);
	}
	printf("Congratulations! You succeeded. You have tried %d times", nShot);
}
