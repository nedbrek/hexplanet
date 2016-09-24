#ifndef CONTROLS_H
#define CONTROLS_H

class Camera;

class Controls
{
public:
	Controls();

	void beginFrame(Camera *cp);

private:
	double lastTime_;
};

#endif

