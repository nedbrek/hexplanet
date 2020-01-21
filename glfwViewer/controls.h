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
	float theta_ = 0;
	float phi_ = 0;
	float dist_ = -2.5;
};

#endif

