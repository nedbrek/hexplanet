#ifndef CONTROLS_H
#define CONTROLS_H

struct GLFWwindow;
class Camera;

class Controls
{
public:
	Controls();

	void beginFrame(GLFWwindow *main_window, Camera *cp);

private:
	double lastTime_;
};

#endif

