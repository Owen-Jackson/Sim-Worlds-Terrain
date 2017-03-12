#include "VBTerrain.h"
#include "DDSTextureLoader.h"
#include "Helper.h"
#include <iostream>
#include <random>

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

	//carry out some kind of transform on these vertices to make this object more interesting
	//Transform();

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
			m_vertices[vert++].Pos.y = m_heightmap[currentHeightMap];

			//2
			m_vertices[vert++].Pos.y = m_heightmap[currentHeightMap + 1];

			//3
			m_vertices[vert++].Pos.y = m_heightmap[currentHeightMap + m_height];
			
			//3
			m_vertices[vert++].Pos.y = m_heightmap[currentHeightMap + m_height];

			//2
			m_vertices[vert++].Pos.y = m_heightmap[currentHeightMap + 1];

			//4
			m_vertices[vert++].Pos.y = m_heightmap[currentHeightMap + m_height + 1];

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
			m_heightmap[(m_height * i) + j] /= 10.0f;
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
	buildMesh(GD);
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

	std::cout << "\nbfOffbits: " << bitmapFileHeader.bfOffBits
		<< "\nbfSize: " << bitmapFileHeader.bfSize
		<< "\nbfType: " << bitmapFileHeader.bfType
		<< "\nbiBitCount: " << bitmapInfoHeader.biBitCount
		<< "\nbiClrImportant: " <<bitmapInfoHeader.biClrImportant
		<< "\nbiClrUsed: " << bitmapInfoHeader.biClrUsed
		<< "\nbiCompression: " << bitmapInfoHeader.biCompression
		<< "\nbiHeight: " << bitmapInfoHeader.biHeight
		<< "\nbiPlanes: " << bitmapInfoHeader.biPlanes
		<< "\nbiSize: " << bitmapInfoHeader.biSize
		<< "\nbiSizeImage: " << bitmapInfoHeader.biSizeImage
		<< "\nbiWidth: " << bitmapInfoHeader.biWidth
		<< "\nbiXPelsPerMeter: " << bitmapInfoHeader.biXPelsPerMeter
		<< "\nbiYPelsPerMeter: " << bitmapInfoHeader.biYPelsPerMeter
		<< std::endl;

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
	m_heightmap = new int[m_width * m_height];
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

			index = (m_width * i) + j;

			m_heightmap[index] = (float)height;

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
	FILE* filePtr = nullptr;
	BITMAPFILEHEADER fileHeader;
	BITMAPINFOHEADER infoHeader;
	std::string fileName = "../Assets/HeightMaps/" + _filename;
	unsigned char* image;
	int padding = 0;	//padding for end of each row if width is not multiple of 4

	if ((m_width * 3) % 4 != 0)
	{
		padding = 4 - ((m_width * 3) % 4);
	}

	//set file header and info header variables
	fileHeader.bfType = 19778;
	fileHeader.bfOffBits = 54;
	fileHeader.bfReserved1 = 0;
	fileHeader.bfReserved2 = 0;
	fileHeader.bfSize = 54 + (( 3 * m_width + padding) * m_height);
	
	infoHeader.biBitCount = 24;
	infoHeader.biCompression = 0;
	infoHeader.biSize = 40;
	infoHeader.biClrImportant = 0;
	infoHeader.biClrUsed = 0;
	infoHeader.biSizeImage = ((3 * m_width + padding) * m_height);
	infoHeader.biXPelsPerMeter = 3780;
	infoHeader.biYPelsPerMeter = 3780;
	infoHeader.biPlanes = 1;
	infoHeader.biHeight = m_height;
	infoHeader.biWidth = m_width;

	image = new unsigned char[infoHeader.biSizeImage];
	float greyscaleValue = 0;
	int index = 0;
	for (int i = 0; i < m_height - 1; i++)
	{
		for (int j = 0; j < m_width - 1; j++)
		{
			greyscaleValue = rand() % 255 + 0;
			image[index++] = (float)greyscaleValue;
			image[index++] = (float)greyscaleValue;
			image[index++] = (float)greyscaleValue;
		}
		for (int k = 0; k < padding - 1; k++)
		{
			image[index++] = (float) 0.0f;
		}
	}

	int error = 0;

	//open a new file to write to
	error = fopen_s(&filePtr, fileName.c_str(), "w");
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

	//write in pixel values
	fwrite(image, sizeof(DWORD), m_width * m_height, filePtr);

	//close the file
	error = fclose(filePtr);
	if (error != 0)
	{
		std::cout << "error: did not close file\n";
		return false;
	}

	delete[] image;
	image = 0;

	return true;
}

//Perlin functions
void VBTerrain::initWithPerlin(int _size, ID3D11Device* GD)
{
	m_width = _size;
	m_height = _size;
}

//This algorithm is mostly taken from Perlin's own implementation http://mrl.nyu.edu/~perlin/noise/ 
double VBTerrain::generatePerlin(double x, double y, double z)
{
	//create the unit cube that our coordinate is in
	int X = (int)floor(x) & 255;
	int Y = (int)floor(y) & 255;
	int Z = (int)floor(z) & 255;

	//find relative x, y, z of point in the cube
	x -= floor(x);
	y -= floor(y);
	z -= floor(z);

	//compute the fade curves for each value
	double u = fade(x);
	double v = fade(y);
	double w = fade(z);

	//hash coordinates of the 8 cube corners
	int AAA, ABA, AAB, ABB, BAA, BBA, BAB, BBB;
	AAA = m_p[m_p[m_p[X] + Y] + Z];
	ABA = m_p[m_p[m_p[X] + Y + 1] + Z];
	AAB = m_p[m_p[m_p[X] + Y] + Z + 1];
	ABB = m_p[m_p[m_p[X] + Y + 1] + Z + 1];
	BAA = m_p[m_p[m_p[X + 1] + Y] + Z];
	BBA = m_p[m_p[m_p[X + 1] + Y + 1] + Z];
	BAB = m_p[m_p[m_p[X + 1] + Y] + Z + 1];
	BBB = m_p[m_p[m_p[X + 1] + Y + 1] + Z + 1];

	//apply the gradient function and interpolate
	double x1, x2, y1, y2;
	x1 = lerp(gradient(AAA, x, y, z),
		gradient(BAA, x - 1, y, z),
		u);

	x2 = lerp(gradient(ABA, x, y - 1, z),
		gradient(BAA, x - 1, y - 1, z),
		u);

	y1 = lerp(x1, x2, v);

	x1 = lerp(gradient(AAB, x, y, z - 1),
		gradient(BAB, x - 1, y, z - 1),
		u);

	x2 = lerp(gradient(ABB, x, y - 1, z - 1),
		gradient(BBB, x - 1, y - 1, z - 1),
		u);

	y2 = lerp(x1, x2, v);

	return (lerp(y1, y2, w) + 1) / 2;
}

double VBTerrain::fade(double t)
{
	return t * t * t * (t * (t * 6 - 15) + 10);	//Ken Perlin's improved function
}

double VBTerrain::gradient(int _hash, double x, double y, double z)
{
	switch (_hash & 0xF)
	{
	case 0x0: return x + y;
	case 0x1: return -x + y;
	case 0x2: return x - y;
	case 0x3: return -x - y;
	case 0x4: return x + z;
	case 0x5: return -x + z;
	case 0x6: return  x - z;
	case 0x7: return -x - z;
	case 0x8: return  y + z;
	case 0x9: return -y + z;
	case 0xA: return  y - z;
	case 0xB: return -y - z;
	case 0xC: return  y + x;
	case 0xD: return -y + z;
	case 0xE: return  y - x;
	case 0xF: return -y - z;
	default: return 0; // never happens
	}
}

//linearly interpolates the two points by a weight factor
double VBTerrain::lerp(double a, double b, double x)
{
	return a + x * (b - a);
}