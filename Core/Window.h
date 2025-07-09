#ifndef PROJECT1_WINDOW_H
#define PROJECT1_WINDOW_H


namespace Window
{
	enum direction { LEFT, FORWARD, RIGHT, BACK, UP, DOWN };

	extern double deltaTime;

	void updateTime();
	double getCurrentTime();
	void input();
	void initWindow();
	void terminateWindow();
	bool shouldCloseWindow();
	void pollSwapWindow();
}

#endif
