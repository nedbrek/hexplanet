#ifndef HUD_H
#define HUD_H

#include <sstream>
#include <string>
#include <vector>
class FTFont;

/// one line of text
class Line
{
public:
	virtual ~Line() = 0;

	virtual std::string current() const = 0;
};

/// manages the 2D overlay in front of the camera
class Hud
{
public:
	Hud();
	~Hud();

	/// a line which is always the same (mostly for debugging)
	void addStaticLine(const char *str);

	/// a line with a variable portion
	size_t addVarLine(const char *prefix, const std::string &initVal);

	template<typename T>
	void updateVarLine(size_t id, const T &newVal)
	{
		std::ostringstream os;
		os << newVal;
		updateVarLine(id, os.str());
	}

	/// render all lines
	void render(FTFont &font);

protected:
	std::vector<Line*> lines_;
};

#endif

