#ifndef _VB_TERRAIN_H_
#define _VB_TERRAIN_H_
#include "VBGO.h"
#include "vertex.h"
#include <vector>

//=================================================================
//procedurally generate a VBGO Cube
//each side be divided in to _size * _size squares (2 triangles per square)
//=================================================================

class VBTerrain : public VBGO
{
public:
	VBTerrain() {};
	virtual ~VBTerrain() {};

	//initialise the Veretx and Index buffers for the cube
	void init(int _size, ID3D11Device* _GD);

protected:
	//this is to allow custom versions of this which create the basic cube and then distort it
	//see VBSpiral, VBSpiked and VBPillow
	void Transform();

	int m_size;
	int m_chunkNum;
	int m_verticesPerChunk = 10000;
	myVertex* m_vertices;
	myVertex* m_heightmap;
	int m_width = 200;
	int m_height = 200;
};

#endif