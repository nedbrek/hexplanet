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

