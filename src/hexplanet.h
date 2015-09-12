#ifndef HEXPLANET_H
#define HEXPLANET_H

#include <OpenEXR/ImathVec.h>
#include <OpenEXR/ImathColor.h>
#include <stdint.h>
#include <vector>

template<typename T> class MapData;

// Terrain Type
enum Terrain
{
	Terrain_WATER,
	Terrain_DESERT,
	Terrain_GRASSLAND,
	Terrain_FOREST,
	Terrain_MOUNTAIN
};

// A Hextile is a single hex (or sometimes pentagon)
// in the hex tiled planet. It is a single vertex of
// the dual mesh.
struct HexTile
{
	HexTile( Imath::V3f p );	

	Imath::V3f normal() const;

	// the position of the vert in the tile,
	// and the center of the hex in space
	Imath::V3f m_vertPos;		
};

// A hextri is an element of the dual
// of the (mostly) hex tiling
struct HexTri
{
	HexTri( uint32_t a, uint32_t b, uint32_t c );

	// indices into the hex list
	uint32_t m_hexA, m_hexB, m_hexC;
	
	// Returns the center of the tri
	Imath::V3f getCenter( const std::vector<HexTile> &hexes  );

	// Temporaries that don't need to stick around	
	union {
		// Just used in the construction process
		uint32_t m_newvert;

		// angle around tile center
		float m_angle;
	} m_tmp;
	
	// Dbg color
	//Imath::C4f m_dbgColor;
};

class HexPlanet
{
public:
	HexPlanet();
	HexPlanet( int subd_level, float trandom, float twatery );

	void write( std::ostream &o );
	void read( std::istream &i );

	void repairNormals();

	/**
	 * @param colorTextureBar true if data is colors, false if data is texture indexes
	 * @param terrainData data to use for hex terrain
	 */
	void draw( bool colorTextureBar, const MapData<uint8_t> &terrainData );

	size_t getNumHexes() const;

	void subdivide( float trandom, float twatery );

	// returns the polygon representation of this
	// hex. Usually 6-sided but could be a pentagon	
	void getPolygon( uint32_t tileIndex, std::vector<Imath::V3f> &poly, float offset=0.0f );

	// returns the indices of the neighbors of this tile
	// Usually 6, could be 5. These aren't in any order
	void getNeighbors( uint32_t tileNdx, std::vector<uint32_t> &nbrs ) const;
	
	// Returns a point on the planet's surface given a ray
	bool rayHitPlanet( Imath::V3f p, Imath::V3f dir, Imath::V3f &result );

	// Get a hex index from a 3d point .. projects the point
	// down to the planet surface and returns the index of the
	// hex containing it
	size_t getHexIndexFromPoint( Imath::V3f surfPos );

//protected:
	int m_subdLevel;

	// construction operations
	void buildLevel0( float twatery );	
	void projectToSphere();

	// data
	std::vector<HexTile> m_hexes;
	std::vector<HexTri> m_hexdual;

	static float kPlanetRadius;
};

#endif
