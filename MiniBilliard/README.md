# MiniBilliard

This is a mini version of billiards.You must shoot a ball in order to hit another pink ball which is existing on the table at the beginning of the game. The goal of this game is to let the pink ball(by alpha blending) fall into one of the six holes.


Use the space key to shoot a ball. I use a rectangle to simulate a player. The pin is to simulate the stick. You can use [left arrow] and [right arrow] to move the player and [up arrow] and [down arrow] to adjust the stick.


You can use [ctrl] to draw an aim line(Stipple) in order to aim the target. However, the aim line is only a simulation in ideal situation. You should not rely on this. Because the ball is moving with fraction.


You can exit the game at any time by pressing [ESC]. 


You can only shoot another ball when the last shot ball has been out of the window or been stopped by fraction. If a shot ball has been stopped, it will disappeared automatically. 


When you win this game, you will receive a summary from console. 


Requirement Satisfaction:


1/ Used the double-buffering.

2/ Run the animation in a while loop. And user can exit the game by pressing [ESC] key.

3/ Used line stipple(aim line) and alpha blending(pink ball).

4/ Used GL_LINE_LOOP (shooting ball), GL_TRIANGLE_FAN (pink ball), GL_QUADS(targets and player) and GL_LINE (stick).

5/ Used <math.h> and Euler's method (The fraction is considered when the ball is moving).


