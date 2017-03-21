#include "VBTerrain.h"
#include "DDSTextureLoader.h"
#include "Helper.h"
#include <iostream>
#include <random>
#include <math.h>

VBTerrain::~VBTerrain()
{
	delete[] m_heightmap;
	delete[] m_p;
	delete[] gradsX;
	delete[] gradsY;
}

void VBTerrain::init(ID3D11Device* GD)
{
	//calculate number of vertices and primitives
	m_numVerts = 6 * (m_width - 1) * (m_height - 1);

	//calculate chunks (implement later for optimisation)
	int current = 0;
	while (current < m_numVerts)
	{
		if (current % m_verticesPerChunk == 0)
		{
			m_chunkNum++;
		}
		current++;
	}

	m_numPrims = m_numVerts / 3;
	m_vertices = new myVertex[m_numVerts];
	m_indices = new WORD[m_numVerts];

	//as using the standard VB shader set the tex-coords somewhere safe
	for (int i = 0; i < m_numVerts; i++)
	{
		m_indices[i] = i;
		m_vertices[i].texCoord = Vector2::One;
	}

	//in each loop create the two traingles for the matching sub-square on each of the six faces
	int vert = 0;
	for (int i = 0; i < m_height - 1; i++)
	{
		for (int j = 0; j < m_width - 1; j++)
		{
			//The comments below represent the vertex number in the current quad 
			//1
			m_vertices[vert].Color = Color(1.0f, 1.0f, 1.0f, 1.0f);
			m_vertices[vert].Pos = Vector3((float)i, (float)(0), (float)j);
			m_vertices[vert].texCoord.x = 1.0f;
			m_vertices[vert].texCoord.y = 1.0f;
			vert++;

			//2
			m_vertices[vert].Color = Color(1.0f, 1.0f, 1.0f, 1.0f);
			m_vertices[vert].Pos = Vector3((float)i, (float)(0), (float)(j + 1));
			m_vertices[vert].texCoord.x = 1.0f;
			m_vertices[vert].texCoord.y = 0.0f;
			vert++;

			//3
			m_vertices[vert].Color = Color(1.0f, 1.0f, 1.0f, 1.0f);
			m_vertices[vert].Pos = Vector3((float)(i + 1), (float)(0), (float)j);
			m_vertices[vert].texCoord.x = 0.0f;
			m_vertices[vert].texCoord.y = 1.0f;
			vert++;

			//3
			m_vertices[vert].Color = Color(1.0f, 1.0f, 1.0f, 1.0f);
			m_vertices[vert].Pos = Vector3((float)(i + 1), (float)(0), (float)j);
			m_vertices[vert].texCoord.x = 0.0f;
			m_vertices[vert].texCoord.y = 1.0f;
			vert++;

			//2
			m_vertices[vert].Color = Color(1.0f, 1.0f, 1.0f, 1.0f);
			m_vertices[vert].Pos = Vector3((float)i, (float)(0), (float)(j + 1));
			m_vertices[vert].texCoord.x = 1.0f;
			m_vertices[vert].texCoord.y = 0.0f;
			vert++;

			//4
			m_vertices[vert].Color = Color(1.0f, 1.0f, 1.0f, 1.0f);
			m_vertices[vert].Pos = Vector3((float)(i + 1), (float)(0), (float)(j + 1));
			m_vertices[vert].texCoord.x = 0.0f;
			m_vertices[vert].texCoord.y = 0.0f;
			vert++;
		}
	}

	//load texture
	//HRESULT hr = CreateDDSTextureFromFile(GD, Helper::charToWChar("../Debug/Ground.dds"), nullptr, &m_pTextureRV);
}

void VBTerrain::initialiseNormals()
{
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
}

void VBTerrain::buildMesh(ID3D11Device* GD)
{
	BuildIB(GD, m_indices);
	BuildVB(GD, m_numVerts, m_vertices);


	delete[] m_indices;    //this is no longer needed as this is now in the index Buffer
	delete[] m_vertices; //this is no longer needed as this is now in the Vertex Buffer
	delete[] m_heightmap;
	m_vertices = nullptr;
	m_heightmap = nullptr;
}

void VBTerrain::raiseTerrain()
{
	int vert = 0;
	int currentHeightMap = 0;

	for (int i = 0; i < m_width - 1; i++)
	{
		for (int j = 0; j < m_height - 1; j++)
		{
			//The comments below represent the vertex number in the current quad 
			//1
			m_vertices[vert++].Pos.y = m_heightmap[currentHeightMap].height;

			//2
			m_vertices[vert++].Pos.y = m_heightmap[currentHeightMap + 1].height;

			//3
			m_vertices[vert++].Pos.y = m_heightmap[currentHeightMap + m_height].height;
			
			//3
			m_vertices[vert++].Pos.y = m_heightmap[currentHeightMap + m_height].height;

			//2
			m_vertices[vert++].Pos.y = m_heightmap[currentHeightMap + 1].height;

			//4
			m_vertices[vert++].Pos.y = m_heightmap[currentHeightMap + m_height + 1].height;
			currentHeightMap++;
		}
		currentHeightMap++;
	}
}

void VBTerrain::normaliseHeightmap()
{
	//loop through each height value and divide by a factor to lower the variance in height levels
	for (int i = 0; i < m_width; i++)
	{
		for (int j = 0; j < m_height; j++)
		{
			m_heightmap[(m_height * i) + j].height /= 10.0f;
		}
	}
}

//Heightmap functions
void VBTerrain::initWithHeightMap(ID3D11Device* GD, char* _filename)
{
	readFromBmp(_filename);

	init(GD);
	raiseTerrain();
	initialiseNormals();
}

//This tutorial was used as a guide to creating this function http://www.rastertek.com/dx11ter02.html
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

	//Output the data from the loaded file (used to reverse engineer my write to bitmap function)
	//std::cout << "\nbfOffbits: " << bitmapFileHeader.bfOffBits
	//	<< "\nbfSize: " << bitmapFileHeader.bfSize
	//	<< "\nbfType: " << bitmapFileHeader.bfType
	//	<< "\nbiBitCount: " << bitmapInfoHeader.biBitCount
	//	<< "\nbiClrImportant: " <<bitmapInfoHeader.biClrImportant
	//	<< "\nbiClrUsed: " << bitmapInfoHeader.biClrUsed
	//	<< "\nbiCompression: " << bitmapInfoHeader.biCompression
	//	<< "\nbiHeight: " << bitmapInfoHeader.biHeight
	//	<< "\nbiPlanes: " << bitmapInfoHeader.biPlanes
	//	<< "\nbiSize: " << bitmapInfoHeader.biSize
	//	<< "\nbiSizeImage: " << bitmapInfoHeader.biSizeImage
	//	<< "\nbiWidth: " << bitmapInfoHeader.biWidth
	//	<< "\nbiXPelsPerMeter: " << bitmapInfoHeader.biXPelsPerMeter
	//	<< "\nbiYPelsPerMeter: " << bitmapInfoHeader.biYPelsPerMeter
	//	<< std::endl;

	//get the dimensions of the terrain
	m_width = bitmapInfoHeader.biWidth;
	m_height = bitmapInfoHeader.biHeight;

	//calculate the size of the image data
	int imageSize = m_width * m_height * 3;

	//if image size is odd add another byte to each line
	if (m_width % 2 == 1)
	{
		imageSize = ((m_width * 3) + 1) * m_height;
	}

	std::cout << imageSize << std::endl;

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
	m_heightmap = new HeightMap[m_width * m_height];
	if (!m_heightmap)
	{
		return false;
	}

	//initialise the start position for the image data
	int position = 0;
	//...and the height value to be read in
	unsigned char height;

	int index;

	//read the image data into the heightmap
	for (int i = 0; i < m_width; i++)
	{
		for (int j = 0; j < m_height; j++)
		{
			height = bitmapImage[position];

			index = (m_width * i) + j;

			m_heightmap[index].height = (float)height;

			position += 3;
		}
		//need to increment again to compensate for the extra byte for odd sizes
		if (m_width % 2 == 1)
		{
			position++;
		}
	}

	normaliseHeightmap();

	//release the bitmap data
	delete[] bitmapImage;
	bitmapImage = 0;

	return true;
}

bool VBTerrain::writeToBmp(std::string _filename)
{
	FILE* filePtr;
	BITMAPFILEHEADER fileHeader;
	BITMAPINFOHEADER infoHeader;
	std::string fileName = "../Assets/HeightMaps/" + _filename;
	unsigned char* image;
	int padding = 0;	//padding for end of each row if width is not multiple of 4

	std::cout << "padding: " << m_width << " %  4 = " << m_width % 4 << std::endl;

	if ((m_width * 3) % 4 != 0)
	{
		padding = 4 - ((m_width * 3) % 4);
	}
	std::cout << padding << std::endl;

	//set file header and info header variables
	fileHeader.bfType = 'MB';
	fileHeader.bfSize = 54 + ((3 * m_width) + padding) * m_height;
	fileHeader.bfReserved1 = 0;
	fileHeader.bfReserved2 = 0;
	fileHeader.bfOffBits = 54;
	
	infoHeader.biSize = 40;
	infoHeader.biHeight = m_height;
	infoHeader.biWidth = m_width;
	infoHeader.biPlanes = 1;
	infoHeader.biBitCount = 24;
	infoHeader.biCompression = 0;
	infoHeader.biSizeImage = ((3* m_width) + padding) * m_height;
	infoHeader.biXPelsPerMeter = 3780;
	infoHeader.biYPelsPerMeter = 3780;
	infoHeader.biClrUsed = 0;
	infoHeader.biClrImportant = 0;

	image = new unsigned char[((3 * m_width + padding) * m_height)];
	double greyscaleValue;
	long index = 0;
	int ind = 0;
	for (int i = 0; i < m_height; i++)
	{
		for (int j = 0; j < m_width; j++)
		{
			ind = (m_width * i) + j;
			greyscaleValue = m_heightmap[ind].height * 255;

			image[index] = (float)greyscaleValue;
			index++;
			image[index] = (float)greyscaleValue;
			index++;
			image[index] = (float)greyscaleValue;
			index++;
		}
		for (int k = 0; k < padding; k++)
		{
			std::cout << "adding padding" << std::endl;
			image[index++] = (float) 0.0f;
		}
	}

	int error = 0;

	//open a new file to write to
	error = fopen_s(&filePtr, fileName.c_str(), "wb");
	if (error != 0)
	{
		std::cout << "error : could not open file to write to\n";
		return false;
	}

	//write in the file header
	error = fwrite(&fileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);
	if (error != 1)
	{
		std::cout << "error: could not write file header\n";
		return false;
	}

	//write in the bitmap info header
	error = fwrite(&infoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);
	if (error != 1)
	{
		std::cout << "error: could not write bitmap info header\n";
		return false;
	}

	//std::cout << infoHeader.biSizeImage << std::endl;
	//write in the pixel values
	fwrite(image, 1, infoHeader.biSizeImage, filePtr);

	error = fclose(filePtr);
	if (error != 0)
	{
		std::cout << "error: did not close file\n";
		return false;
	}

	delete[] image;
	image = 0;

	delete[] m_heightmap;
	m_heightmap = nullptr;

	return true;
}

//Perlin functions
void VBTerrain::initWithPerlin(int _size, ID3D11Device* GD)
{
	m_width = _size;
	m_height = _size;
	m_p = new int[512];
	for (int i = 0; i < 512; i++)
	{
		m_p[i] = permutations[i % 256];
	}
	m_heightmap = new HeightMap[m_width * m_height];
	gradsX = new double[_size];
	gradsY = new double[_size];
	//initialise the gradient arrays used in Perlin noise
	for (int i = 0; i < _size; i++)
	{
		gradsX[i] = double(rand()) / (RAND_MAX / 2) - 1.0f;
		gradsY[i] = double(rand()) / (RAND_MAX / 2) - 1.0f;
		//std::cout << gradsX[i] << " , " << gradsY[i] << std::endl;
	}

	int index = 0;

	for (int i = 0; i < m_height; i++)
	{
		for (int j = 0; j < m_width; j++)
		{
			index = (m_width * j) + i;
			double x = (double)j / (double)m_width;
			double y = (double)i / (double)m_height;
			//std::cout << x << " , " << y << std::endl;
			int random = rand() % 256;
			m_heightmap[index].gradx = gradsX[random];
			m_heightmap[index].grady = gradsY[random];
			m_heightmap[index].height = generatePerlin(x, y) * 256;
			//std::cout << m_heightmap[index].height << std::endl;
		}
	}
	init(GD);
	normaliseHeightmap();
	//raiseTerrain();
	//initialiseNormals();

}

//This algorithm is a combination of these two tutorials:
//http://www.angelcode.com/dev/perlin/perlin.html
//http://flafla2.github.io/2014/08/09/perlinnoise.html
double VBTerrain::generatePerlin(double x, double y)
{
	//calculate where the square the input falls into is located
	int x0 = (int)floorf(x);
	int x1 = x0 + 1;

	int y0 = (int)floorf(y);
	int y1 = y0 + 1;

	//calculate input point's position within the unit square
	float pX0 = x - floorf(x);
	float pX1 = pX0 - 1;

	float pY0 = y - floorf(y);
	float pY1 = pY0 - 1;

	//permutate values to get indices to use with the gradient look-up tables
	//then get the dot product between the gradient and sample position vector
	int index = m_p[(y0 + m_p[x0])];
	//std::cout << index << std::endl;
	double vecAA = gradsX[index] * pX0 + gradsY[index] * pY0;

	index = m_p[(y0 + m_p[x1])];
	double vecAB = gradsX[index] * pX1 + gradsY[index] * pY0;

	index = m_p[(y1 + m_p[x0])];
	double vecBA = gradsX[index] * pX0 + gradsY[index] * pY1;

	index = m_p[(y1 + m_p[x1])];
	double vecBB = gradsX[index] * pX1 + gradsY[index] * pY1;
	//std::cout << vecAA << " , " << vecAB << " , " << vecBA << " , " << vecBB << std::endl;
	
	//compute the fade curves for each value
	double u = fade(x);
	double v = fade(y);

	//interpolate the resulting dot products
	double r1, r2, result;

	r1 = lerp(vecAA, vecAB, u);
	r2 = lerp(vecBA, vecBB, u);

	result = lerp(r1, r2, v);
	
	//std::cout << result << std::endl;
	
	return (result + 1)/2;
}

double VBTerrain::fade(double t)
{
	return t * t * t * (t * (t * 6 - 15) + 10);	//Ken Perlin's improved function
}

//linearly interpolates the two points by a weight factor
double VBTerrain::lerp(double a, double b, double weight)
{
	return a + weight * (b - a);
}