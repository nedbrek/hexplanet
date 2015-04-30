#include <iostream>
#include <vector>
#include <assert.h>

// must include windows.h before GL on win32
#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

// GL and related includes
#include <GL/gl.h>
#ifdef WIN32
#include <GL/glaux.h>
#endif
#include <GL/glu.h>

#include <GL/glut.h>
#include <GL/glui.h>

#include <OpenEXR/ImathVec.h>

#include "load_texture.h"
#include "gamefontgl.h"
#include "planet_gui.h"
#include "map_data.h"
#include <fstream>

float PlanetGui::_ident[16] = {1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };
PlanetGui *PlanetGui::s_theGUI = NULL;
const int PlanetGui::kMaxSubdivisionLevel = 6;

//=============================
// PlanetGui ctor
//=============================
PlanetGui::PlanetGui( int gluiMainWin ) :
	m_gluiMainWin( gluiMainWin ),
	m_cursorHex( 0 ),
	m_mouseOnSurface( false ),
	m_numFrames( 0 ),
	m_lastFPS( 0 ),
	m_beautyMode( 1 ),
	m_showStats( 1 ),
	m_subdLevel ( 3 ),
	m_drawMode( HexPlanet::DrawMode_TERRAIN ),
	m_paintTile( Terrain_DESERT ),
	m_terrRandom( 0.17f ),
	m_terrWatery( 0.5f ),
	m_tileData(new MapData<uint8_t>),
	m_altitude(-2500.f)
{
	// singleton
	assert( s_theGUI == NULL );
	s_theGUI = this;

	m_previousTime = glutGet(GLUT_ELAPSED_TIME);

	// initialize planet rotation
	memcpy( m_rotPlanet, _ident, sizeof( _ident ) );
	memcpy( m_rotSun, _ident, sizeof( _ident ) );

	// Load the font image
	m_glFontTexId_console = loadTextureDDS("datafiles/console_dxt1a.dds");

	// Create a font by passing in an opengl texture id
	m_fntConsole = gfCreateFont( m_glFontTexId_console );

	// A .finfo file contains the metrics for a font. These
	// are generated by the Fontpack utility.
	gfLoadFontMetrics( m_fntConsole, "datafiles/console.finfo");

	printf("Console Font Loaded: %d chars\n", gfGetFontMetric( m_fntConsole, GF_FONT_NUMCHARS ) );
	gfEnableFont( m_fntConsole, 20 );	
	

	// Construct the planet
	std::ifstream is("test/sphere6.fixed.obj");
	//m_planet  = new HexPlanet( m_subdLevel, m_terrRandom, m_terrWatery );
	m_planet  = new HexPlanet();
	m_planet->read(is);
	m_planet->projectToSphere();

	FILE *terrainFile = fopen("test/terrain6.bin", "rb");
	m_tileData->read(terrainFile);

	// Initialize OpenGL Resources
	m_planetDlist = glGenLists( 1 );

	// load star textures
	m_texStars = loadTextureDDS( "datafiles/stars.dds" );		
}

//=============================
// buildInterface
//=============================
void PlanetGui::buildInterface()
{
	// create bottom panel
	m_glui = GLUI_Master.create_glui_subwindow( m_gluiMainWin, 
										GLUI_SUBWINDOW_BOTTOM );
	m_glui->set_main_gfx_window( m_gluiMainWin );

	// Main panel
	GLUI_Panel *panel = m_glui->add_panel( "", GLUI_PANEL_NONE );

	// Rotation Panel
	GLUI_Panel *rot_panel = m_glui->add_panel_to_panel( panel, "", GLUI_PANEL_NONE );

	GLUI_Rotation *rotPlanet = m_glui->add_rotation_to_panel( rot_panel,
													"Planet", m_rotPlanet );
	rotPlanet->set_spin( 1.0 );	

	m_glui->add_column_to_panel( rot_panel, false );
	GLUI_Rotation *rotSun = m_glui->add_rotation_to_panel( rot_panel,
													"Sun", m_rotSun );
	rotSun->set_spin( 1.0 );	

	m_glui->add_column_to_panel( rot_panel, false );
	m_glui->add_translation_to_panel( rot_panel, "Altitude", GLUI_TRANSLATION_Z, &m_altitude);

	m_glui->add_column_to_panel( panel, false );
	// Planet Construction Panel
	GLUI_Panel *construct_pane = m_glui->add_panel_to_panel(
											panel, "Construction" );
	
	GLUI_Spinner *spinner =
		m_glui->add_spinner_to_panel( construct_pane, "Subdivision:", 
				GLUI_SPINNER_INT,
				&(m_subdLevel), ID_SUBDIVISION );
	spinner->set_int_limits( 0, kMaxSubdivisionLevel );

	// Paint
	GLUI_Listbox *list = m_glui->add_listbox_to_panel( construct_pane, 
													"Draw:", &m_paintTile );  
    list->add_item( Terrain_WATER, "Water" );
	list->add_item( Terrain_DESERT, "Desert" );
	list->add_item( Terrain_GRASSLAND, "Grassland" );
	list->add_item( Terrain_FOREST, "Forest" );
	list->add_item( Terrain_MOUNTAIN, "Mountains" );


	// add button
	m_glui->add_button_to_panel( construct_pane, "Rebuild", 
								  ID_BTN_REBUILD, control_cb );

	m_glui->add_column_to_panel( construct_pane, true );

	spinner =
		m_glui->add_spinner_to_panel( construct_pane, "Randomness:", 
				GLUI_SPINNER_FLOAT,
				&(m_terrRandom), ID_TERRRAND );
	spinner->set_float_limits( 0.0f, 1.0f );

	spinner =
		m_glui->add_spinner_to_panel( construct_pane, "Water:", 
				GLUI_SPINNER_FLOAT,
				&(m_terrWatery), ID_TWATER );
	spinner->set_float_limits( 0.0f, 1.0f );

		
	m_glui->add_button_to_panel( construct_pane, "Subdivide", 
								  ID_BTN_SUBDIV, control_cb );


	// Planet display panel
	m_glui->add_column_to_panel( panel, false );
	GLUI_Panel *display_pane = m_glui->add_panel_to_panel(
											panel, "Display" );

	GLUI_RadioGroup *radio;
	radio = m_glui->add_radiogroup_to_panel( display_pane,&m_drawMode, 
												ID_DRAWMODE, control_cb );
	m_glui->add_radiobutton_to_group( radio, "Template" );
	m_glui->add_radiobutton_to_group( radio, "Terrain" );
	m_glui->add_radiobutton_to_group( radio, "Terrain+Grid" );	

	m_glui->add_column_to_panel( display_pane, true );
	m_glui->add_checkbox_to_panel( display_pane, "Beauty Mode", &m_beautyMode );
	m_glui->add_checkbox_to_panel( display_pane, "Show Stats", &m_showStats );

	m_glui->add_statictext_to_panel( panel, " by Joel Davis <joeld42@yahoo.com>" ); 
}

//=============================
// redraw
//=============================
void PlanetGui::redraw()
{
	if (m_beautyMode)
	{
		glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
	}
	else 
	{
		glClearColor( 0.2f, 0.2f, 0.6f, 1.0f );
	}
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glMatrixMode( GL_MODELVIEW );

	
	
	// Add the sunlight before rotation :)
	if (!m_beautyMode) 
	{
		glDisable( GL_LIGHTING );
	}
	else
	{
		glPushMatrix();
		// Rotate to the sun light's alignment		
		//glLoadIdentity();
		glMultMatrixf( m_rotSun );

		glDisable( GL_LIGHTING );				

		glEnable( GL_LIGHTING );
		glEnable( GL_LIGHT0 );
		glEnable( GL_LIGHT1 );
		glEnable( GL_LIGHT2 );

		glColorMaterial( GL_FRONT_AND_BACK, GL_DIFFUSE );
		glEnable( GL_COLOR_MATERIAL );		

		//printf( "sun: %f %f %f\n", 
					//m_rotSun[8], 
					//m_rotSun[9], 
					//m_rotSun[10] );
			
		GLfloat sun_intensity = 5.5f;
		GLfloat zero_amb[] = { 0.0f, 0.0f, 0.0f, 1.0f };		
		GLfloat sun_color[] = { 0.961f,  0.882f, 0.699f, 1.0f };		
		GLfloat sun_amb[] = { 0.0f, 0.0f, 0.2f, 1.0f };		
		GLfloat sun_direction[] = { .97, 0.176, -0.1235, 0.0f };		

		sun_color[0] *= sun_intensity;
		sun_color[1] *= sun_intensity;
		sun_color[2] *= sun_intensity;

		// Rim  & fill don't track with sun rotation
		GLfloat fill_color[] = { 0.1f, 0.1f, 0.4f, 1.0f };
		GLfloat fill_dir[] = { 0.0f, 0.0f, 1.0f, 0.0f };
		
		GLfloat rim_color[] = { 1.0f, 1.0f, 1.2f, 1.0f };
		GLfloat rim_dir[] = { -0.015461f, 0.64879f, -0.760808, 0.0f };
		
		// zero out other amb
		glLightfv(GL_LIGHT1, GL_AMBIENT,  zero_amb );
		glLightfv(GL_LIGHT2, GL_AMBIENT,  zero_amb );

		// Sun light
		glLightfv( GL_LIGHT0, GL_POSITION, sun_direction );
		glLightfv( GL_LIGHT0, GL_DIFFUSE, sun_color );	
		glLightfv(GL_LIGHT0, GL_AMBIENT,  sun_amb );
		
		glPopMatrix();

		// Fill light
		glLightfv( GL_LIGHT1, GL_POSITION, fill_dir );
		glLightfv( GL_LIGHT1, GL_DIFFUSE, fill_color );		
	
		// Rim light
		glLightfv( GL_LIGHT2, GL_POSITION, rim_dir);
		glLightfv( GL_LIGHT2, GL_DIFFUSE, rim_color );	
	}

	

	// camera distance
	glLoadIdentity();
	glTranslatef( 0.0, 0.0, m_altitude );				

	// draw the "atmosphere"	
	if (m_beautyMode)
	{
		glDepthMask( GL_FALSE );
		drawAtmosphere();		
	}

	// Rotate to the planet's alignment
	glMultMatrixf( m_rotPlanet );	

	if (m_beautyMode)
	{
		drawStarfield();
		glDepthMask( GL_TRUE );
	}

	//glPushMatrix();

	// grab the current MVP
	glGetDoublev( GL_MODELVIEW_MATRIX, m_modelView );
	glGetDoublev( GL_PROJECTION_MATRIX, m_projection );	
	glGetIntegerv( GL_VIEWPORT, m_viewport );	
		

	// Update the cursor, probably not the best place to
	// call this, but I don't really have a sim loop in
	// this crappy demo
	updateCursorPos();

	glColor3f( 1.0f, 1.0f, 1.0f );
	glLineWidth( 1.0f );

	// draw once with no lights
	glDisable( GL_LIGHTING );
		
	if (!m_beautyMode)
	{
		glColor3f( 1.0f, 1.0f, 1.0f );		
	}
	else
	{		
		glColor3f( 0.2f, 0.2f, 0.2f );
	}

	if (m_planetDirty)
	{		
		glNewList( m_planetDlist, GL_COMPILE_AND_EXECUTE );
		m_planet->draw( m_drawMode, *m_tileData );	
		glEndList();

		m_planetDirty = false;
	}
	else
	{		
		glCallList( m_planetDlist );
	}

	if (m_beautyMode)
	{
		// GL regular lighting doesn't allow you to
		// get values brighter than the base texture,
		// overdraw with lighting instead to get more
		// range.
		glDepthMask( GL_FALSE );
		glEnable( GL_LIGHTING );
		glDisable( GL_TEXTURE_2D );
		glColor4f( 0.5f, 0.5f, 0.5f, 0.2f );
		glEnable( GL_BLEND );
		glBlendFunc( GL_ONE, GL_ONE );		
		glutSolidSphere( 1020.0f, 30, 30 );
		

		// draw again at lower opacity to back off
		// the overblown lighting (stupid GL lighting)
		glDisable( GL_LIGHTING );
		glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );		
		glColor4f( 1.0f, 1.0f, 1.0f, 0.3f );
		glCallList( m_planetDlist );

		glDisable( GL_BLEND );
		glDepthMask( GL_TRUE );
		
	}

	// draw the "cursor" hex
	std::vector<uint32_t> nb;	
	if (m_mouseOnSurface)
	{
		std::vector<Imath::V3f> cursorPoly;
		if (m_cursorHex < m_planet->m_hexes.size() )
		{
			m_planet->getPolygon( m_cursorHex, cursorPoly );

			glColor3f( 0.0f, 1.0f, 1.0f );
			glLineWidth( 3.0 );
			glDisable( GL_TEXTURE_2D );
			glBegin( GL_LINE_LOOP );
			for ( std::vector<Imath::V3f>::iterator pi = cursorPoly.begin();
				  pi != cursorPoly.end(); pi++) 
			{
				glVertex3f( (*pi).x, (*pi).y, (*pi).z );
				//printf("%f %f %f\n", (*pi).x, (*pi).y, (*pi).z );
			}
			glEnd();
			//printf("---\n" );
		}


		// DBG: Draw dots on neighbor hexes		
		m_planet->getNeighbors( m_cursorHex, nb );
#if 0
		glPointSize( 3.0f );
		glColor3f( 0.0f, 0.0f, 1.0f );
		glBegin( GL_POINTS );
		for (std::vector<size_t>::iterator ni = nb.begin();
			 ni != nb.end(); ni++)
		{
			Imath::V3f v = m_planet->m_hexes[*ni].m_vertPos;
			glVertex3f( v.x, v.y, v.z );
		}
		glEnd();
#endif
	}

	//glPopMatrix();

	// Draw status text
	glColor3f( 1.0f, 1.0f, 1.0f );
	if (m_showStats)
	{
		int tx, ty, tw, th;
		GLUI_Master.get_viewport_area( &tx, &ty, &tw, &th );

		gfEnableFont( m_fntConsole, 20 );			

		const int ROW_SIZE = 20;
		int pixelRow = th - ROW_SIZE;

		gfBeginText();

		glPushMatrix();	
		glTranslated( 20, pixelRow, 0 );
		gfDrawStringFmt( "# of hexes: %d\n", m_planet->getNumHexes() );
		glPopMatrix();	
		pixelRow -= ROW_SIZE;

		glPushMatrix();	
		glTranslated( 20, pixelRow, 0 );
		gfDrawStringFmt( "# of Triangles: %d\n", m_planet->m_hexdual.size() );
		glPopMatrix();	
		pixelRow -= ROW_SIZE;

		glPushMatrix();	
		glTranslated( 20, pixelRow, 0 );
		gfDrawStringFmt( "FPS: %f\n", m_lastFPS );
		glPopMatrix();	
		pixelRow -= ROW_SIZE;

		if (m_mouseOnSurface)
		{
			glPushMatrix();	
			glTranslated( 20, pixelRow, 0 );
			gfDrawStringFmt( "Curr Hex: %d\n", m_cursorHex );
			glPopMatrix();	
			pixelRow -= ROW_SIZE;

			glPushMatrix();	
			glTranslated( 20, pixelRow, 0 );
			gfDrawStringFmt( "Curr Terrain: %d\n", 
					(*m_tileData)[ m_cursorHex ] );
			glPopMatrix();	
			pixelRow -= ROW_SIZE;

			glPushMatrix();	
			glTranslated( 20, pixelRow, 0 );
			gfDrawStringFmt( "Num Neighbors: %d", nb.size() );
			glPopMatrix();	
			pixelRow -= ROW_SIZE;
		}

		gfEndText();
	}

	glDisable( GL_TEXTURE_2D );

	++m_numFrames;
	const int currentTime = glutGet(GLUT_ELAPSED_TIME);
	const int deltaTime = currentTime - m_previousTime;
	if (deltaTime > 1000)
	{
		m_lastFPS = m_numFrames / (deltaTime / 1000.f);
		m_numFrames = 0;
		m_previousTime = currentTime;
	}
}

void PlanetGui::drawAtmosphere()
{	
	// disable the lighting for the atmosphere
	glDisable (GL_LIGHTING);
	glEnable( GL_BLEND );

	glBegin( GL_QUAD_STRIP );
	for ( float t = 0.0f; t <= 2.0f * M_PI + 0.001; t += M_PI / 10.0f )
	{
		Imath::V3f p( cos(t), sin(t), 0.0f );
		glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
		glNormal3f( p[0], p[1], 0.0 );
		glVertex3f( p[0]*1000.0f, p[1]*1000.0f, 0.0f );

		glColor4f( 0.0f, 0.5f, 0.9f, 0.1f );
		glNormal3f( 0.0, 0.0, -1.0 );
		glVertex3f( p[0]*1300.0f, p[1]*1300.0f, 0.0f );
	}
	glEnd();

	glEnable (GL_LIGHTING);
}


// Draws a starfield
void PlanetGui::drawStarfield()
{
	while (m_stars.size() < 200) 
	{
		Imath::V3f spos;
		spos.x = ((float)rand() / (float)RAND_MAX)*2.0f - 1.0f;
		spos.y = ((float)rand() / (float)RAND_MAX)*2.0f - 1.0f;
		spos.z = ((float)rand() / (float)RAND_MAX)*2.0f - 1.0f;
			

		float d = spos.length2();
		
		// reject points outside of the unit sphere to
		// create an even distribution (no clumps at "corners")
		if (d > 1.0f) continue;

		Star s;
		spos = spos.normalize();		
		s.p = spos * 3000.0f;
		s.radius = 500.0f + ((float)rand() / (float)RAND_MAX) * 500.0f;
		s.index = rand() % 8;
		m_stars.push_back( s );		
	}

	glEnable( GL_TEXTURE_2D );

	glBindTexture( GL_TEXTURE_2D, m_texStars );
	glColor3f( 0.5f, 0.5f, 0.5f );

	glEnable( GL_BLEND );
	glBlendFunc( GL_ONE, GL_ONE );

	// draw the stars
	glDisable( GL_LIGHTING );
	glBegin( GL_QUADS );					
	
	// get billboard directions -- transpose of the planet rotation
	Imath::V3f right( m_rotPlanet[0], m_rotPlanet[4], m_rotPlanet[8] );
	Imath::V3f up( m_rotPlanet[1], m_rotPlanet[5], m_rotPlanet[9] );		

	float r;
	float sz = 1.0f / 8.0f;
	for (std::vector<Star>::iterator si = m_stars.begin();
		 si != m_stars.end(); si++)
	{
		Imath::V3f p;
		r = (*si).radius;

		p = (*si).p + right*r + up*r;
		glTexCoord2f( sz*((*si).index+1), 0.0f );
		glVertex3f( p.x, p.y, p.z );

		p = (*si).p + right*r - up*r;
		glTexCoord2f( sz*((*si).index+1), 1.0f );
		glVertex3f( p.x, p.y, p.z );

		p = (*si).p - right*r - up*r;
		glTexCoord2f( sz*(*si).index, 1.0f );
		glVertex3f( p.x, p.y, p.z );

		p = (*si).p - right*r + up*r;
		glTexCoord2f( sz*(*si).index, 0.0f );
		glVertex3f( p.x, p.y, p.z );
	}

	glEnd();
	glEnable( GL_LIGHTING );

	glDisable( GL_BLEND );
}

// Update the cursor pos
void PlanetGui::updateCursorPos()
{
	// Make a ray from the mouse position
	Imath::V3f rayP, rayDir;

	// unproject at 2 depths
	GLdouble x, y, z;	

	// adjust the viewport for unProject to work	
	m_viewport[1] = 0;
	m_viewport[0] = 0;

	gluUnProject( m_mouseX, m_viewport[3]-m_mouseY, 0.001,
				  m_modelView, m_projection, m_viewport,
				  &x, &y, &z );
	rayP = Imath::V3f( x, y, z );
	
	gluUnProject( m_mouseX, m_viewport[3]-m_mouseY, 0.01,
				  m_modelView, m_projection, m_viewport,
				  &x, &y, &z );
	rayDir = Imath::V3f( x, y, z );
	
	// subtract them to get the ray
	rayDir = rayDir - rayP;
	rayDir = rayDir.normalize();

	Imath::V3f p;
	if (m_planet->rayHitPlanet( rayP, rayDir, p ))
	{
		m_mouseSurfacePos = p;
		m_mouseOnSurface = true;
	}
	else
	{		
		m_mouseOnSurface = false;
	}			

	// Now, find the hex which contains the cursor
	if (m_mouseOnSurface)
	{
		m_cursorHex = m_planet->getHexIndexFromPoint( m_mouseSurfacePos );
	}
}

// Set the tile 
void PlanetGui::paintTile()
{
	updateCursorPos();
	if (m_mouseOnSurface)
	{
		(*m_tileData)[ m_cursorHex ] = m_paintTile;
	}

	// mark the planet as needing redraw
	m_planetDirty = true;
}

// widget callback
void PlanetGui::cbControl( int id )
{
	// Do something
	switch (id)
	{
	case ID_BTN_REBUILD:
		delete m_planet;
		m_planet  = new HexPlanet( m_subdLevel, m_terrRandom, m_terrWatery );
		break;
	case ID_BTN_SUBDIV:		
		if (m_subdLevel < kMaxSubdivisionLevel )
		{
			m_subdLevel++;
			m_planet->subdivide( m_terrRandom, m_terrWatery );
		}
		break;
	};

	// mark the planet as needing rebuild
	m_planetDirty = true;
}

// static callback
void PlanetGui::control_cb( int control )
{
	s_theGUI->cbControl( control );
}

