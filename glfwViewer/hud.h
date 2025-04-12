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

class LineVar : public Line
{
public:
	LineVar(const char *prefix, const std::string &initVal)
	: prefix_(prefix)
	, var_(initVal)
	{
	}

	virtual std::string current() const
	{
		return prefix_ + var_;
	}

	void update(const std::string &newVal)
	{
		var_ = newVal;
	}

protected:
	std::string prefix_;
	std::string var_;
};

template<>
inline
void Hud::updateVarLine<std::string>(size_t id, const std::string &newVal)
{
	LineVar *l = id < lines_.size() ? dynamic_cast<LineVar*>(lines_[id]) : NULL;
	if (l)
	{
		l->update(newVal);
	}
}

#endif

