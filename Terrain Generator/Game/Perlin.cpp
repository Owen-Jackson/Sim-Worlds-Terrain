#include "Perlin.h"
#include <random>
#include <algorithm>
#include <iostream>

Perlin::~Perlin()
{

}

Perlin::Perlin(int _seed)
{
	p.resize(256);
	for (int i = 0; i < p.size(); i++)
	{
		p[i] = i;
	}

	std::default_random_engine engine(_seed);

	std::shuffle(p.begin(), p.end(), engine);

	p.insert(p.end(), p.begin(), p.end());
}

//This algorithm uses Ken Perlin's own implementation:
//http://mrl.nyu.edu/~perlin/noise/
double Perlin::generateNoise(double x, double y, double z)
{
	int X = (int)floor(x) & 255,                  // FIND UNIT CUBE THAT
		Y = (int)floor(y) & 255,                  // CONTAINS POINT.
		Z = (int)floor(z) & 255;
	double oldZ = z;
	x -= floor(x);                                // FIND RELATIVE X,Y,Z
	y -= floor(y);                                // OF POINT IN CUBE.
	z -= floor(z);
	double u = fade(x),                                // COMPUTE FADE CURVES
		v = fade(y),                                // FOR EACH OF X,Y,Z.
		w = fade(z);

	int A = p[X] + Y,
		AA = p[A] + Z,
		AB = p[A + 1] + Z,      // HASH COORDINATES OF
		B = p[X + 1] + Y,
		BA = p[B] + Z,
		BB = p[B + 1] + Z;      // THE 8 CUBE CORNERS,

	double x1 = lerp(grad(p[AA], x, y, z), grad(p[BA], x - 1, y, z), u);
	double x2 = lerp(grad(p[AB], x, y - 1, z), grad(p[BB], x - 1, y - 1, z), u);
	double r1 = lerp(x1, x2, v);

	x1 = lerp(grad(p[AA + 1], x, y, z - 1), grad(p[BA + 1], x - 1, y, z - 1), u);
	x2 = lerp(grad(p[AB + 1], x, y - 1, z - 1), grad(p[BB + 1], x - 1, y - 1, z - 1), u);
	double r2 = lerp(x1, x2, v);

	//result will range between -1 and 1
	double result = lerp(r1, r2, w);

	//normalise result to be between 0 and 1
	return (result + 1.0f) / 2;
}

double Perlin::fade(double t)
{
	return t * t * t * (t * (t * 6 - 15) + 10);	//Ken Perlin's improved function
}

//linearly interpolates the two points by a weight factor
double Perlin::lerp(double a, double b, double weight)
{
	return a + weight * (b - a);
}

double Perlin::FBM(double x, double y, double z, int octaves, double persistence)
{
	double total = 0;
	double frequency = 1;
	double amplitude = 1;
	double maxValue = 0;	//Used to normalise the result to between 0.0 and 1.0
	//generate perlin noise multiple times based on number of octaves
	for (int i = 0; i < octaves; i++)
	{
		total += generateNoise(x * frequency, y * frequency, z * frequency) * amplitude;
		maxValue += amplitude;

		amplitude *= persistence;
		frequency *= 2;

	}
	return total / maxValue;
}

double Perlin::grad(int hash, double x, double y, double z) {
	int h = hash & 15;                      // CONVERT LO 4 BITS OF HASH CODE
	double u = h<8 ? x : y,                 // INTO 12 GRADIENT DIRECTIONS.
		v = h<4 ? y : h == 12 || h == 14 ? x : z;
	return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}