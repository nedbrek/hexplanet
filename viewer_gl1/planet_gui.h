#ifndef PLANET_GUI_H
#define PLANET_GUI_H

#include "../src/hexplanet.h"
#include "load_texture.h"

class GLUI;
template<typename T> class MapData;
class Neighbors;

// A star billboard in the starfield
struct Star
{
	int index;
	Imath::V3f p;
	float radius;
};

//=====================================================
// Main interface class, also draws the background
// starfield and lighting
//=====================================================
class PlanetGui
{
public:
	enum {
		DrawMode_CONSTRUCTION, // Show construction
		DrawMode_TERRAIN,      // Terrain with no grid
		DrawMode_TERRAINGRID,  // Terrain with grid
	};

	PlanetGui(int gluiMainWin);

	void buildInterface();

	void redraw();	

	void paintTile();

protected: // methods
	// Updates the hex cursor pos
	void updateCursorPos();

	// callbacks
	void cbControl(int id);

	// helpers for drawing in beauty mode
	void drawAtmosphere();
	void drawStarfield();

	void drawHexes(const MapData<uint8_t> &terrainData);

protected: // data
	// font stuff
	GLuint m_glFontTexId_console;
	GLuint m_fntConsole;

	// Glut and GLUI widgets
	GLUI *m_glui;
	int m_gluiMainWin;

	// Widget IDs
	enum {
		ID_SUBDIVISION,
		ID_TERRRAND,
		ID_TWATER,
		ID_BTN_REBUILD,
		ID_BTN_SUBDIV,
		ID_DRAWMODE,

		ID_LAST
	};

	// Index of the hex the cursor is over
	uint32_t m_cursorHex;

	// location of the mouse cursor on the planet's surface
	// (last know location if mouse is not touching planet)
	Imath::V3f m_mouseSurfacePos;
	bool m_mouseOnSurface;

	// planet displaylist
	GLuint m_planetDlist;
	bool m_planetDirty;	

	// FPS state
	int m_numFrames;
	int m_previousTime;
	float m_lastFPS;

	// display params
	int m_beautyMode;
	int m_showStats;

	// planet params
	int m_subdLevel;  // subdivision level 
	int m_drawMode;	  // draw mode
	int m_paintTile;  // which tile to "paint"

	// "terraforming" params
	float m_terrRandom;
	float m_terrWatery;

	// the planet map
	HexPlanet *m_planet;
	MapData<uint8_t> *m_tileData;
	MapData<uint8_t> *m_plateData;
	Neighbors *m_neighbors;

	// Planet Rotation
	float m_rotPlanet[16];
	float m_rotSun[16];

	float m_altitude;

	// view from opengl
	double m_modelView[16], m_projection[16];
	int m_viewport[4];

	GLuint m_texStars;
	std::vector<Star> m_stars;

	// default rotations
	static float _ident[16];

	// singleton
	static PlanetGui *s_theGUI;

	// static resources
	static bool m_initStaticRes;
	static GLuint g_texTemplate;
	static GLuint g_texTileset;
	static GLuint g_texTilesetGrid; // Tileset with outline

public:
	// updated from main
	int m_mouseX, m_mouseY;

	//Callbacks
	static void control_cb(int control);

	// max subd level
	static const int kMaxSubdivisionLevel;	
};

#endif
