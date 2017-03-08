#include "VBTerrain.h"
#include <iostream>
#include <random>
#include <array>

void VBTerrain::init(int _size, ID3D11Device* GD)
{
	// this is most certainly not the most efficent way of doing most of this
	//but it does give you a very clear idea of what is actually going on

	m_size = _size;

	//calculate number of vertices and primitives
	int numVerts = 6 * (m_width - 1) * (m_height - 1);
	int gridPoints = (m_width) * (m_height);
	m_heightmap = new myVertex[gridPoints];

	int current = 0;
	while (current < numVerts)
	{
		if (current % m_verticesPerChunk == 0)
		{
			std::cout << current << std::endl;
			m_chunkNum++;
		}
		current++;
	}

	std::cout << m_chunkNum << std::endl;
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

	//in each loop create the two traingles for the matching sub-square on each of the six faces
	int vert = 0;
	for (int i = 0; i < m_height - 1; i++)
	{
		for (int j = 0; j < m_width - 1; j++)
		{
			//The comments below represent the vertex number in the current quad 
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
	//Transform();
	//std::cout << m_numPrims << std::endl;
	//std::cout << numVerts << std::endl;
	//std::cout << vert;

	//calculate the normals for the basic lighting in the base shader
	int count = 0;
	for (int i = 0; i<m_numPrims; i++)
	{
		int V1 = 3 * i;
		int V2 = 3 * i + 1;
		int V3 = 3 * i + 2;

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
	//myVertex* temp = new myVertex[m_verticesPerChunk];
	//for (int loop = 0; loop < m_chunkNum - 1; loop++)
	//{
	//	for (int j = 0; j < m_verticesPerChunk - 1; j++)
	//	{
	//		temp[j].Pos = m_vertices[m_verticesPerChunk * loop + j].Pos;
	//		temp[j].Color = m_vertices[m_verticesPerChunk * loop + j].Color;
	//	}

	//	BuildVB(GD, m_verticesPerChunk, temp);
	//}
	BuildVB(GD, numVerts - 1, m_vertices);


	delete[] indices;    //this is no longer needed as this is now in the index Buffer
	delete[] m_vertices; //this is no longer needed as this is now in the Vertex Buffer
	delete[] m_heightmap;
	//delete[] temp;
	m_vertices = nullptr;
	m_heightmap = nullptr;
}

void VBTerrain::Transform()
{
	int vert = 0;
	int currentHeightMap = 0;

	//std::cout << m_heightmap[currentHeightMap].Pos.y;

	for (int i = 0; i < m_width - 1 ; i++)
	{
		for (int j = 0; j < m_height - 1; j++)
		{
			//The comments below represent the vertex number in the current quad 
			//1
			m_vertices[vert++].Pos.y = m_heightmap[currentHeightMap].Pos.y;

			//2
			m_vertices[vert++].Pos.y = m_heightmap[currentHeightMap + 1].Pos.y;

			//3
			m_vertices[vert++].Pos.y = m_heightmap[currentHeightMap + m_height].Pos.y;

			//3
			m_vertices[vert++].Pos.y = m_heightmap[currentHeightMap + m_height].Pos.y;

			//2
			m_vertices[vert++].Pos.y = m_heightmap[currentHeightMap + 1].Pos.y;

			//4
			m_vertices[vert++].Pos.y = m_heightmap[currentHeightMap + m_height + 1].Pos.y;

			currentHeightMap++;
		}
		currentHeightMap++;
	}
}