#pragma once
#include <vector>
#include <memory>
class Perlin
{
public:
	Perlin() = default;
	Perlin(int seed);
	~Perlin();

	double generateNoise(double x, double y, double z);
	double fade(double t);
	double lerp(double a, double b, double x);
	double grad(int hash, double x, double y, double z);
	double FBM(double x, double y, double z, int octaves, double persistence);


private:
	//Perlin variables
	//used when seeding the random generator
	int m_seed;

	//permutations vector
	std::vector<int> p;
};