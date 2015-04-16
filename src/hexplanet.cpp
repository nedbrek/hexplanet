#include <string>
#include <iostream>
#include <vector>
#include <algorithm>

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

#include "load_texture.h"
#include "hexplanet.h"
#include <set>

bool HexPlanet::m_initStaticRes = false;
GLuint HexPlanet::g_texTemplate;
GLuint HexPlanet::g_texTileset;
GLuint HexPlanet::g_texTilesetGrid;

float HexPlanet::kPlanetRadius = 1000.0f;

HexTile::HexTile( Imath::V3f p ) :
	m_vertPos( p )
{
	m_terrain = HexTile::Terrain_DESERT;
}

Imath::V3f HexTile::normal() const
{
	return m_vertPos.normalized();
}

HexTri::HexTri( size_t a, size_t b, size_t c) :
	m_hexA( a ), m_hexB( b ), m_hexC( c )
{
	m_nbAB = NULL;
	m_nbBC = NULL;
	m_nbCA = NULL;

	// Mark newvert as uninitialized
	m_tmp.m_newvert = std::string::npos;

	// DBG color. For debugging
	//m_dbgColor.r = (float)rand() / (float)RAND_MAX;
	//m_dbgColor.g = (float)rand() / (float)RAND_MAX;
	//m_dbgColor.b = (float)rand() / (float)RAND_MAX;
	
}

Imath::V3f HexTri::getCenter( const std::vector<HexTile> &hexes )
{
	Imath::V3f pntCenter = hexes[ m_hexA ].m_vertPos;
	pntCenter += hexes[ m_hexB ].m_vertPos;
	pntCenter += hexes[ m_hexC ].m_vertPos;
	pntCenter /= 3.0f;

	return pntCenter;
}

HexPlanet::HexPlanet() :
	m_subdLevel( 0 )
{
}

HexPlanet::HexPlanet( int subd_level, float trandom, float twatery  ) :
	m_subdLevel( 0 )
{
	// build initial (level 0) mesh
	buildLevel0( twatery );

	// subdivide until desired level
	while (m_subdLevel < subd_level)
	{
		subdivide( trandom, twatery);
	}

	// planetize if we're at level 0
	if (subd_level == 0)
	{
		projectToSphere();
	}
}

void HexPlanet::write( std::ostream &o )
{
	o << "# " << m_hexes.size() << " Vertices" << std::endl;
	for (std::vector<HexTile>::const_iterator i = m_hexes.begin(); i != m_hexes.end(); ++i)
	{
		  const Imath::V3f nrm = i->normal();
		  o << 'v'
		  << ' ' << nrm[0]
		  << ' ' << nrm[1]
		  << ' ' << nrm[2]
		  << std::endl;
	}

	o << "# " << m_hexdual.size() << " Faces" << std::endl;
	for (std::vector<HexTri>::const_iterator i = m_hexdual.begin(); i != m_hexdual.end(); ++i)
	{
		o << 'f'
		  << ' ' << i->m_hexA +1
		  << ' ' << i->m_hexB +1
		  << ' ' << i->m_hexC +1
		  << std::endl;
	}
}

void HexPlanet::read( std::istream &is )
{
	std::string line;
	for (std::getline(is, line); !is.eof(); std::getline(is, line))
	{
		std::istringstream iss(line);
		char firstChar;
		iss >> firstChar;
		if (firstChar == '#')
		{
			// comment - do nothing
		}
		else if (firstChar == 'v')
		{
			// vertex - 3 coordinates
			// make into a hex
			float x, y, z;
			iss >> x >> y >> z;
			m_hexes.push_back(HexTile(Imath::V3f(x, y, z)));
		}
		else if (firstChar == 'f')
		{
			// face - 3 vert indices
			size_t x, y, z;
			iss >> x >> y >> z;
			m_hexdual.push_back(HexTri(x-1, y-1, z-1));
		}
	}
}

void HexPlanet::repairNormals()
{
	for (std::vector<HexTri>::iterator i = m_hexdual.begin(); i != m_hexdual.end(); ++i)
	{
		// pull three verts of triangle
		const Imath::V3f p0 = m_hexes[i->m_hexA].normal();
		const Imath::V3f p1 = m_hexes[i->m_hexB].normal();
		const Imath::V3f p2 = m_hexes[i->m_hexC].normal();
		const Imath::V3f n = (p2 - p0).cross(p1 - p0);
		const float d = n.dot(p0);
		if ((d >= 0 && d < 0.001) || (d < 0 && d > -0.001))
			std::cerr << "Triangle intersects origin" << std::endl;
		else if (d > 0)
		{
			// reverse points
			const size_t oldC = i->m_hexC;
			i->m_hexC = i->m_hexA;
			i->m_hexA = oldC;
		}
		else
		{
			// good
		}
	}
}

//=============================
// buildLevel0 -- builds the initial icosahedron
// for the planet mesh
//=============================
void HexPlanet::buildLevel0( float twatery )
{
	// hard code an icosahedron (20 sided die)
	m_hexes.erase( m_hexes.begin(), m_hexes.end() );	
	m_hexes.push_back( HexTile( Imath::V3f( 0.723606f, 0.0f, 1.17082f )));
	m_hexes.push_back( HexTile( Imath::V3f( 0.0f, 1.17082f, 0.723606f )));
	m_hexes.push_back( HexTile( Imath::V3f( -0.723606f, 0.0f, 1.17082f )));
	m_hexes.push_back( HexTile( Imath::V3f( 0.0f, -1.17082f,  0.723606f )));
	m_hexes.push_back( HexTile( Imath::V3f( 0.723606f, 0.0f, -1.17082f )));
	m_hexes.push_back( HexTile( Imath::V3f( 0.0f, -1.17082f, -0.723606f )));
	m_hexes.push_back( HexTile( Imath::V3f( -0.723606f, 0.0f, -1.17082f )));
	m_hexes.push_back( HexTile( Imath::V3f( 0.0f, 1.17082f, -0.723606f )));
	m_hexes.push_back( HexTile( Imath::V3f( 1.17082f, -0.723606f, 0.0f )));
	m_hexes.push_back( HexTile( Imath::V3f( 1.17082f, 0.723606f, 0.0f )));
	m_hexes.push_back( HexTile( Imath::V3f( -1.17082f, 0.723606f, 0.0f )));
	m_hexes.push_back( HexTile( Imath::V3f( -1.17082f, -0.723606f,  0.0f )));

	// set initial terrain types
	for (std::vector<HexTile>::iterator hi = m_hexes.begin();
		hi != m_hexes.end(); hi++)
	{
		(*hi).m_terrain = getRandomTerrain( twatery );
	}

	m_hexdual.push_back( HexTri( 5, 11, 6 ));
	m_hexdual.push_back( HexTri( 1, 2, 0 ));
	m_hexdual.push_back( HexTri( 0, 2, 3 ));
	m_hexdual.push_back( HexTri( 5, 6, 4 ));
	m_hexdual.push_back( HexTri( 4, 6, 7 ));
	m_hexdual.push_back( HexTri( 9, 1, 0 ));
	m_hexdual.push_back( HexTri( 10, 2, 1 ));
	m_hexdual.push_back( HexTri( 2, 10, 11 ));
	m_hexdual.push_back( HexTri( 11, 3, 2 ));
	m_hexdual.push_back( HexTri( 8, 9, 0 ));
	m_hexdual.push_back( HexTri( 0, 3, 8 ));
	m_hexdual.push_back( HexTri( 11, 10, 6 ));
	m_hexdual.push_back( HexTri( 4, 7, 9 ));
	m_hexdual.push_back( HexTri( 9, 8, 4 ));
	m_hexdual.push_back( HexTri( 7, 6, 10 ));
	m_hexdual.push_back( HexTri( 1, 9, 7 ));
	m_hexdual.push_back( HexTri( 10, 1, 7 ));
	m_hexdual.push_back( HexTri( 5, 4, 8 ));
	m_hexdual.push_back( HexTri( 3, 11, 5 ));
	m_hexdual.push_back( HexTri( 8, 3, 5 ));	

	// make planet sized
	// projectToSphere();

	// assign neighbors
	findNeighbors();
}

// Used in subdivide
void createTrisFromEdge( std::vector< std::pair< size_t, size_t > > &edgeDone,
							 std::vector<HexTri> &trilist,
							 HexTri &tri, HexTri &otherTri,
							 size_t eA, size_t eB )
{
	std::pair<size_t, size_t> eid( std::min( eA, eB ), std::max( eA, eB ) );
	if (std::find( edgeDone.begin(), edgeDone.end(), eid ) == edgeDone.end() )
	{
		trilist.push_back( HexTri( eA, tri.m_tmp.m_newvert, otherTri.m_tmp.m_newvert ) );					
		trilist.push_back( HexTri( tri.m_tmp.m_newvert, otherTri.m_tmp.m_newvert, eB ) );
		edgeDone.push_back( eid );
	}
}

//=============================
// subdivide()
// Perform sqrt(3) subdivision on
// the mesh
//=============================
void HexPlanet::subdivide( float trandom, float twatery )
{


	// Subdivide by creating two triangles in 
	// the next level mesh for every edge in the
	// src mesh. Keep track of which edges have
	// already been handled
	std::vector< std::pair< size_t, size_t > > edgeDone;

	// The new mesh that will be created	
	std::vector<HexTri> newHexdual;

	// Go through each triangle in the old mesh and create
	// a new vert at the center of each one
	for (std::vector<HexTri>::iterator ti = m_hexdual.begin();
		 ti != m_hexdual.end(); ti++ )
	{
		// Create a new vert at the center of the triangle
		Imath::V3f pNewVert;
		(*ti).m_tmp.m_newvert = m_hexes.size();
		pNewVert = (*ti).getCenter( m_hexes );		

		// add it to the list of hexes
		m_hexes.push_back( HexTile( pNewVert ) );


		// if we're below the random threshold
		// inherit the terrain from an arbitrary parent		
		if  ( ((float)rand() / (float)RAND_MAX) > trandom )
		{
			int tr = rand() % 3;
			switch (tr)
			{
			case 0:
				m_hexes.back().m_terrain = m_hexes[ (*ti).m_hexA ].m_terrain;
				break;
			case 1:
				m_hexes.back().m_terrain = m_hexes[ (*ti).m_hexB ].m_terrain;
				break;
			case 2:
				m_hexes.back().m_terrain = m_hexes[ (*ti).m_hexC ].m_terrain;
				break;
			}
		}
		else
		{
			// Go with a random tile
			m_hexes.back().m_terrain = getRandomTerrain( twatery );
		}
	}

	// Go through each triangle in the old mesh and create
	// a new pair of triangles for each edge
	for (std::vector<HexTri>::iterator ti = m_hexdual.begin();
		 ti != m_hexdual.end(); ti++ )
	{
		HexTri &t = (*ti);

		// Create a pair for edge AB
		createTrisFromEdge( edgeDone, newHexdual, t, *(t.m_nbAB), t.m_hexA, t.m_hexB );

		// Create a pair for edge BC
		createTrisFromEdge( edgeDone, newHexdual, t, *(t.m_nbBC), t.m_hexB, t.m_hexC );
		
		// Create a pair for edge CA
		createTrisFromEdge( edgeDone, newHexdual, t, *(t.m_nbCA), t.m_hexC, t.m_hexA );
		
	}

	// replace the current set of hexes with the dual
	m_hexdual = newHexdual;

	// find new neighbors
	findNeighbors();

	// reproject back to sphere
	projectToSphere();

	// note the subdivision
	m_subdLevel++;
}


//=============================
// findNeighbors() -- it would be more
// efficent to just keep track of the neighbors
// during subdivision, but this is easier
//=============================
bool edgeMatch( size_t a, size_t b,
				size_t otherA, size_t otherB, size_t otherC )
{
	if ( ((a==otherA) && (b==otherB)) ||
		 ((a==otherB) && (b==otherC)) ||
		 ((a==otherC) && (b==otherA)) ||
		 ((b==otherA) && (a==otherB)) ||
		 ((b==otherB) && (a==otherC)) ||
		 ((b==otherC) && (a==otherA)) ) return true;
	return false;
}

bool _cmpAngle( HexTri *a, HexTri *b )
{
	return a->m_tmp.m_angle < b->m_tmp.m_angle;
}

void HexPlanet::findNeighbors()
{
	// Find edge adjacentcy -- slow and brute force. Should
	// do this as part of the subdivide step if this were a
	// non-prototype implementation.
	for (std::vector<HexTri>::iterator ti = m_hexdual.begin();
		ti != m_hexdual.end(); ti++)
	{
		// find the neighbors for ti
		for (std::vector<HexTri>::iterator tj = m_hexdual.begin();
			tj != m_hexdual.end(); tj++)
		{

			// Don't match ourselves
			if (ti==tj) continue;

			// Neighbor across edge AB
			if ( edgeMatch( (*ti).m_hexA, (*ti).m_hexB,
							(*tj).m_hexA, (*tj).m_hexB, (*tj).m_hexC ) )
			{
				(*ti).m_nbAB = &(*tj);
			}

			// Neighbor across edge BC
			if ( edgeMatch( (*ti).m_hexB, (*ti).m_hexC,
							(*tj).m_hexA, (*tj).m_hexB, (*tj).m_hexC ) )
			{
				(*ti).m_nbBC = &(*tj);
			}

			// Neighbor across edge CA
			if ( edgeMatch( (*ti).m_hexC, (*ti).m_hexA,
							(*tj).m_hexA, (*tj).m_hexB, (*tj).m_hexC ) )
			{
				(*ti).m_nbCA = &(*tj);
			}
		}
	}
}

//=============================
// projectToSphere()
//=============================
void HexPlanet::projectToSphere()
{
	for (std::vector<HexTile>::iterator ti = m_hexes.begin();
		 ti != m_hexes.end(); ti++ )
	{
		Imath::V3f p = (*ti).m_vertPos;
		p.normalize();
		p *= kPlanetRadius;
		(*ti).m_vertPos = p;
	}
}

//=============================
// draw()
//=============================
void HexPlanet::draw( int draw_mode )
{
	// Initialize static resources
	if (!m_initStaticRes)
	{
		m_initStaticRes = true;
		g_texTemplate = loadTextureDDS( "datafiles/template.dds" );
		g_texTileset = loadTextureDDS( "datafiles/tileset.dds" );
		g_texTilesetGrid = loadTextureDDS( "datafiles/tileset_grid.dds" );
	}

	//DBG: Draw axes
#if 0
	glBegin( GL_LINES );
	
	glColor3f( 1.0f, 0.0f, 0.0f );
	glVertex3f( 0.0f, 0.0f, 0.0f );
	glVertex3f( 1.0f, 0.0f, 0.0f );


	glColor3f( 0.0f, 1.0f, 0.0f );
	glVertex3f( 0.0f, 0.0f, 0.0f );
	glVertex3f( 0.0f, 1.0f, 0.0f );

	glColor3f( 0.0f, 0.0f, 1.0f );
	glVertex3f( 0.0f, 0.0f, 0.0f );
	glVertex3f( 0.0f, 0.0f, 1.0f );

	glEnd();
#endif
	

	
	// Draw the hexes
	//glColor3f( 1.0f, 1.0f, 1.0f );
	glEnable( GL_TEXTURE_2D );

	
	if (draw_mode == DrawMode_CONSTRUCTION)
	{
		glBindTexture( GL_TEXTURE_2D, g_texTemplate );		
	} 
	else if (draw_mode == DrawMode_TERRAIN)
	{
		glBindTexture( GL_TEXTURE_2D, g_texTileset );
	}
	else // terrain + grid
	{
		glBindTexture( GL_TEXTURE_2D, g_texTilesetGrid );
	}


	glBegin( GL_TRIANGLES );
	for ( std::vector<HexTri>::iterator hi = m_hexdual.begin();
		 hi != m_hexdual.end(); hi++ )
	{
		Imath::V3f pA, pB, pC;
		pA = m_hexes[ (*hi).m_hexA ].m_vertPos;
		pB = m_hexes[ (*hi).m_hexB ].m_vertPos;
		pC = m_hexes[ (*hi).m_hexC ].m_vertPos;	

		if (draw_mode == DrawMode_CONSTRUCTION)
		{
			// Template draw mode
			glTexCoord2f( 0.5f, 0.94999 );
			glVertex3f( pA.x, pA.y, pA.z );

			glTexCoord2f( 0.8897, 0.275 );
			glVertex3f( pB.x, pB.y, pB.z );

			glTexCoord2f( 0.1103f, 0.275 );
			glVertex3f( pC.x, pC.y, pC.z );		
		} 
		else // terrain or terrain_Grid
		{
			// Terrain draw mode
			Imath::V2f txA( 0.5f, 0.94999 ), 
				   txB( 0.8897, 0.275 ), 
				   txC( 0.1103f, 0.275 ),
				   txTileMin, txTileMax;

			// offset texture coords to fit tile
			int ndxA , ndxB, ndxC;
			ndxA = m_hexes[ (*hi).m_hexA ].m_terrain;
			ndxB = m_hexes[ (*hi).m_hexC ].m_terrain;
			ndxC = m_hexes[ (*hi).m_hexB ].m_terrain;

			// TODO: Could use GL texture transform here
			int ndx = (ndxA*25)+(ndxB*5)+ndxC;
			int tile_y = ndx / 12;
			int tile_x = ndx % 12;		
		
			// tileset is 1020 pixels wide, 1020/1024 = 0.996...
			float sz = 0.99609375/12.0f; 

			txA.x = (txA.x * sz) + (tile_x*sz);
			txA.y = (txA.y * sz) + (tile_y*sz);

			txB.x = (txB.x * sz) + (tile_x*sz);
			txB.y = (txB.y * sz) + (tile_y*sz);

			txC.x = (txC.x * sz) + (tile_x*sz);
			txC.y = (txC.y * sz) + (tile_y*sz);

			glTexCoord2f( txA.x, txA.y );
			glVertex3f( pA.x, pA.y, pA.z );

			glTexCoord2f( txB.x, txB.y  );
			glVertex3f( pB.x, pB.y, pB.z );

			glTexCoord2f( txC.x, txC.y  );
			glVertex3f( pC.x, pC.y, pC.z );		
		}
	}
	glEnd();	

#if 0
	// DBG: Draw the dual mesh
	glColor3f( 1.0f, 1.0f, 1.0f );
	glLineWidth( 2.0f );	
	
	for ( std::vector<HexTri>::iterator hi = m_hexdual.begin();
		 hi != m_hexdual.end(); hi++ )
	{
		glBegin( GL_LINE_LOOP );
		Imath::V3f pA, pB, pC;
		pA = m_hexes[ (*hi).m_hexA ].m_vertPos;
		pB = m_hexes[ (*hi).m_hexB ].m_vertPos;
		pC = m_hexes[ (*hi).m_hexC ].m_vertPos;
	
		glVertex3f( pA.x, pA.y, pA.z );
		glVertex3f( pB.x, pB.y, pB.z );
		glVertex3f( pC.x, pC.y, pC.z );		
		
		glEnd();	
	}
#endif
}

size_t HexPlanet::getNumHexes()
{
	return m_hexes.size();
}

int HexPlanet::getRandomTerrain( float twatery )
{
	if ( ((float)rand() / (float)RAND_MAX) > twatery)
	{
		// return a land hex
		int t = (((float)rand() / (float)RAND_MAX) * 4) + 1;
		if (t > HexTile::Terrain_MOUNTAIN) t = HexTile::Terrain_MOUNTAIN;
		return t;
	} 
	else return HexTile::Terrain_WATER;
}

size_t HexPlanet::getHexIndexFromPoint( Imath::V3f surfPos )
{
	size_t best_hex = 0;
	float best_dot;

	// normalize
	Imath::V3f p = surfPos;
	p = p.normalize();
	best_dot = acos( m_hexes[0].m_vertPos.dot( p ) / kPlanetRadius );


	// clever cheat -- just use the dot product to find the 
	// smallest angle -- and thus the containing hex
	for (size_t ndx = 1; ndx < m_hexes.size(); ndx++)
	{
		float d = acos( m_hexes[ndx].m_vertPos.dot( p ) / kPlanetRadius );
		if (d < best_dot)
		{
			best_hex = ndx;
			best_dot = d;
		}
	}

	return best_hex;
}

// returns the polygon representation of this
// hex. Usually 6-sided but could be a pentagon	
void HexPlanet::getPolygon( size_t tileIndex, std::vector<Imath::V3f> &poly, float offset )
{
	// clear list
	poly.erase( poly.begin(), poly.end() );

	// get neighboring hexes
	std::vector<size_t> neighbors;
	getNeighbors(tileIndex, neighbors);

	// sort edges to make a good polygon
	// ---first assign angles around center
	std::vector<HexTri*> triangles;
	const Imath::V3f firstPos = m_hexdual[neighbors[0]].getCenter(m_hexes);

	for ( std::vector<size_t>::const_iterator i = neighbors.begin(); i != neighbors.end(); ++i )
	{
		triangles.push_back(&m_hexdual[*i]);
		Imath::V3f v1 = firstPos - m_hexes[tileIndex].m_vertPos;
		Imath::V3f nrm = m_hexdual[*i].getCenter(m_hexes);
		Imath::V3f v2 = nrm - m_hexes[tileIndex].m_vertPos;
		nrm.normalize();
		v1.normalize();
		v2.normalize();

		float ang = acos( v1.dot( v2 ) );
		const float dir = nrm.dot( v1.cross( v2 ) );
		if (dir < 0.0f) ang = M_PI + (M_PI - ang);

		m_hexdual[*i].m_tmp.m_angle = ang;
	}
	std::sort( triangles.begin(), triangles.end(), _cmpAngle );

	// construct polygon
	for ( std::vector<HexTri*>::iterator ti = triangles.begin(); ti != triangles.end(); ++ti )
	{
		Imath::V3f p = (**ti).getCenter( m_hexes );
		p.normalize();
		p *= kPlanetRadius + offset;
		poly.push_back( p );
	}
}

// returns the indices of the neighbors of this tile
// Usually 6, could be 5
void HexPlanet::getNeighbors( size_t tileNdx, std::vector<size_t> &nbrs )
{
	// clear list
	nbrs.erase( nbrs.begin(), nbrs.end() );

	std::set<size_t> candidates;

	// find neighbors
	for ( size_t ti = 0; ti != m_hexdual.size(); ++ti)
	{
		if (m_hexdual[ti].m_hexA == tileNdx ||
			 m_hexdual[ti].m_hexB == tileNdx ||
			 m_hexdual[ti].m_hexC == tileNdx)
		{
			candidates.insert(ti);
		}
	}

	nbrs.insert(nbrs.end(), candidates.begin(), candidates.end());
}

// Returns a point on the planet's surface given a ray
bool HexPlanet::rayHitPlanet( Imath::V3f p, Imath::V3f dir, Imath::V3f &result )
{
	float a,b,c,d;
	a = dir.dot(dir);
	b = (2.0f*dir).dot(p);
	c = p.dot(p) - (kPlanetRadius*kPlanetRadius);
	d = b*b - 4.0f*a*c;
	if (d <=0 ) return false;
	result = p + ((-b - sqrtf(d)) / 2.0f*a)*dir;
	return true;
}
