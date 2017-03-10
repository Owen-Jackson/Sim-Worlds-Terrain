#include "VBTerrain.h"
#include <iostream>
#include <random>
#include <array>

void VBTerrain::init(int _size, ID3D11Device* GD)
{
	// this is most certainly not the most efficent way of doing most of this
	//but it does give you a very clear idea of what is actually going on

	m_size = _size;
	readFromBmp("../Assets/HeightMaps/TestMap.bmp");
	//calculate number of vertices and primitives
	int numVerts = 6 * (m_width - 1) * (m_height - 1);
	int gridPoints = (m_width) * (m_height);
	//m_heightmap = new myVertex[gridPoints];

	int current = 0;
	while (current < numVerts)
	{
		if (current % m_verticesPerChunk == 0)
		{
			//std::cout << current << std::endl;
			m_chunkNum++;
		}
		current++;
	}

	//std::cout << m_chunkNum << std::endl;
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
	//for (int i = 0; i < gridPoints - 1; i++)
	//{
	//	m_heightmap[i].Pos = Vector3::Zero;
	//	m_heightmap[i].Pos.y = rand() % 4 + 0;
	//}

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

	//carry out some kind of transform on these vertices to make this object more interesting
	Transform();

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

bool VBTerrain::readFromBmp(char* _filename)
{
	//create file related variables
	FILE* filePtr;
	int error;
	unsigned int count;
	BITMAPFILEHEADER bitmapFileHeader;
	BITMAPINFOHEADER bitmapInfoHeader;
	unsigned char* bitmapImage;

	//open the height map file in binary
	error = fopen_s(&filePtr, _filename, "rb");
	if (error != 0)
	{
		std::cout << "error: file did not open\n";
		return false;
	}

	//read in the file header
	count = fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);
	if (count != 1)
	{
		std::cout << "error: could not read file header\n";
		return false;
	}

	//read in the bitmap info header
	count = fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);
	if (count != 1)
	{
		std::cout << "error: could not read bitmap info header\n";
		return false;
	}

	//get the dimensions of the terrain
	m_width = bitmapInfoHeader.biWidth;
	m_height = bitmapInfoHeader.biHeight;

	//calculate the size of the image data
	int imageSize = m_width * m_height * 3;

	//allocate memory for the bitmap image data
	bitmapImage = new unsigned char[imageSize];
	if (!bitmapImage)
	{
		return false;
	}

	//move to the beginning of the bitmap data
	fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

	//read in the image data
	count = fread(bitmapImage, 1, imageSize, filePtr);
	if (count != imageSize)
	{
		std::cout << "error: did not read the full image\n";
		return false;
	}

	//close the file
	error = fclose(filePtr);
	if (error != 0)
	{
		std::cout << "error: did not close file\n";
		return false;
	}

	//create the heightmap that stores the data
	m_heightmap = new myVertex[m_width * m_height];
	if (!m_heightmap)
	{
		return false;
	}

	//initialise the start position for the image data
	int position = 0;
	//...and the height value to be read in
	unsigned char height;

	int index;

	//read the iamge data into the heightmap
	for (int i = 0; i < m_width; i++)
	{
		for (int j = 0; j < m_height; j++)
		{
			height = bitmapImage[position];

			index = (m_height * i) + j;

			m_heightmap[index].Pos.x = (float)j;
			m_heightmap[index].Pos.y = (float)height;
			m_heightmap[index].Pos.z = (float)i;

			position+=3;
		}
	}

	normaliseHeightmap();

	//release the bitmap data
	delete[] bitmapImage;
	bitmapImage = 0;

	return true;
}

void VBTerrain::normaliseHeightmap()
{
	//loop through each height value and divide by a factor to lower the variance in height levels
	for (int i = 0; i < m_width; i++)
	{
		for (int j = 0; j < m_height; j++)
		{
			m_heightmap[(m_height * i) + j].Pos.y /= 5.0f;
		}
	}
}