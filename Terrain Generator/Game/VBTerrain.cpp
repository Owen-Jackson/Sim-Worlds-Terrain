#include "VBTerrain.h"
#include <iostream>
#include <random>

void VBTerrain::init(int _size, ID3D11Device* GD)
{
	// this is most certainly not the most efficent way of doing most of this
	//but it does give you a very clear idea of what is actually going on

	m_size = _size;

	//calculate number of vertices and primitives
	int numVerts = 6 * (m_width) * (m_height);
	int gridPoints = (m_width + 1) * (m_height + 1);
	m_heightmap = new myVertex[gridPoints];
	m_numPrims = numVerts / 3;
	m_vertices = new myVertex[numVerts];
	WORD* indices = new WORD[numVerts];

	//as using the standard VB shader set the tex-coords somewhere safe
	for (int i = 0; i<numVerts; i++)
	{
		indices[i] = i;
		m_vertices[i].texCoord = Vector2::One;
	}

	//initialise heightmap
	for (int i = 0; i < gridPoints - 1; i++)
	{
		m_heightmap[i].Pos = Vector3::Zero;
		m_heightmap[i].Pos.y = rand() % 4 + 0;
	}

	//m_heightmap[7].Pos.y = 1.0f;
	//m_heightmap[8].Pos.y = 3.0f;

	//in each loop create the two traingles for the matching sub-square on each of the six faces
	int vert = 0;
	for (int i = 0; i < m_height - 1; i++)
	{
		for (int j = 0; j < m_width - 1; j++)
		{
			//top
			//1
			m_vertices[vert].Color = Color(1.0f, 0.0f, 0.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3((float)i, (float)(0), (float)j);

			//2
			m_vertices[vert].Color = Color(1.0f, 1.0f, 0.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3((float)i, (float)(0), (float)(j + 1));

			//3
			m_vertices[vert].Color = Color(0.0f, 1.0f, 1.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3((float)(i + 1), (float)(0), (float)j);

			//3
			m_vertices[vert].Color = Color(1.0f, 0.0f, 1.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3((float)(i + 1), (float)(0), (float)j);

			//2
			m_vertices[vert].Color = Color(1.0f, 1.0f, 1.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3((float)i, (float)(0), (float)(j + 1));

			//4
			m_vertices[vert].Color = Color(0.0f, 0.0f, 0.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3((float)(i + 1), (float)(0), (float)(j + 1));

		}
	}

	m_vertices[0].Color = Color(1.0f, 0.0f, 0.0f, 1.0f);
	m_vertices[numVerts - 1].Color = Color(0.0f, 0.0f, 0.0f, 1.0f);
	
	//carry out some kind of transform on these vertices to make this object more interesting
	Transform();

	//calculate the normals for the basic lighting in the base shader
	for (int i = 0; i<m_numPrims; i++)
	{
		WORD V1 = 3 * i;
		WORD V2 = 3 * i + 1;
		WORD V3 = 3 * i + 2;

		//build normals
		Vector3 norm;
		Vector3 vec1 = m_vertices[V1].Pos - m_vertices[V2].Pos;
		Vector3 vec2 = m_vertices[V3].Pos - m_vertices[V2].Pos;
		norm = vec1.Cross(vec2);
		norm.Normalize();

		m_vertices[V1].Norm = norm;
		m_vertices[V2].Norm = norm;
		m_vertices[V3].Norm = norm;
	}

	BuildIB(GD, indices);
	BuildVB(GD, numVerts, m_vertices);

	delete[] indices;    //this is no longer needed as this is now in the index Buffer
	delete[] m_vertices; //this is no longer needed as this is now in the Vertex Buffer
	delete[] m_heightmap;
	m_vertices = nullptr;
	m_heightmap = nullptr;
}

void VBTerrain::Transform()
{
	int vert = 0;
	int currentHeightMap = 0;

	//std::cout << m_heightmap[currentHeightMap].Pos.y;

	for (int i = 0; i < m_width; i++)
	{
		for (int j = 0; j < m_height; j++)
		{
			//top
			//1
			m_vertices[vert++].Pos.y = m_heightmap[currentHeightMap].Pos.y;

			//2
			m_vertices[vert++].Pos.y = m_heightmap[currentHeightMap + 1].Pos.y;

			//3
			m_vertices[vert++].Pos.y = m_heightmap[currentHeightMap + m_height + 1].Pos.y;

			//3
			m_vertices[vert++].Pos.y = m_heightmap[currentHeightMap + m_height + 1].Pos.y;

			//2
			m_vertices[vert++].Pos.y = m_heightmap[currentHeightMap + 1].Pos.y;

			//4
			m_vertices[vert++].Pos.y = m_heightmap[currentHeightMap + m_width + 1].Pos.y;
			//std::cout << m_heightmap[currentHeightMap].Pos.y << "\n";

			currentHeightMap++;
		}
		currentHeightMap++;
	}
}