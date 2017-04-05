#ifndef _VB_TERRAIN_H_
#define _VB_TERRAIN_H_
#include "VBGO.h"
#include "vertex.h"
#include <vector>
#include <string>

//=================================================================
//procedurally generate a VBGO Terrain
//each side be divided in to _size * _size squares (2 triangles per square)
//=================================================================

class VBTerrain : public VBGO
{
public:
	VBTerrain() {};
	virtual ~VBTerrain();

	//initialise the Veretx and Index buffers for the cube
	void init(ID3D11Device* _GD);

	void normaliseHeightmap();
	void initialiseNormals();
	void buildMesh(ID3D11Device* _GD);

	//Heightmap/bitmap related functions	
	void initWithHeightMap(ID3D11Device* _GD, char* _filename);
	bool readFromBmp(char* _filename);
	bool writeToBmp(std::string _filename);

	//Perlin related functions
	void initWithPerlin(int size, ID3D11Device* _GD);
	void raiseTerrain();


protected:
	int m_chunkNum;
	int m_verticesPerChunk = 10000;
	int m_numVerts = 0;
	struct HeightMap
	{
		double height;
	};

	WORD* m_indices;
	myVertex* m_vertices;
	HeightMap* m_heightmap;
	float m_normaliseMultiple = 10.0f;
	int m_width = 1024;
	int m_height = 1024;
};

#endif