#include "hud.h"
#include <FTGL/ftgl.h>
#include <string>

static int FONT_OFFSET = 3;

//----------------------------------------------------------------------------
Line::~Line()
{
}

//----------------------------------------------------------------------------
class StaticLine : public Line
{
public:
	StaticLine(const char *str)
	: line_(str)
	{
	}

	virtual std::string current() const
	{
		return line_;
	}

protected:
	std::string line_;
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

//----------------------------------------------------------------------------
Hud::Hud()
{
}

Hud::~Hud()
{
	for(std::vector<Line*>::const_iterator i = lines_.begin(); i != lines_.end(); ++i)
		delete *i;
}

void Hud::addStaticLine(const char *str)
{
	lines_.push_back(new StaticLine(str));
}

size_t Hud::addVarLine(const char *prefix, const std::string &initVal)
{
	size_t ret = lines_.size();
	lines_.push_back(new LineVar(prefix, initVal));
	return ret;
}

template<>
void Hud::updateVarLine<std::string>(size_t id, const std::string &newVal)
{
	LineVar *l = id < lines_.size() ? dynamic_cast<LineVar*>(lines_[id]) : NULL;
	if (l)
	{
		l->update(newVal);
	}
}

void Hud::render(FTFont &font)
{
	FTPoint linePos(FONT_OFFSET, FONT_OFFSET);
	for(std::vector<Line*>::const_iterator i = lines_.begin(); i != lines_.end(); ++i)
	{
		std::string curLine = (**i).current();

		const char *const curStr = curLine.c_str();
		font.Render(curStr, -1, linePos);
		linePos += FTPoint(0, font.BBox(curStr).Upper().Y() + FONT_OFFSET);
	}
}

