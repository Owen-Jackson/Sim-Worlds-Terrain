#ifndef _QUADTREE_H_
#define _QUADTREE_H_

const int MAX_TRIANGLES = 10000;

#include "VBTerrain.h"

class QuadTree
{
public:
	QuadTree();
	QuadTree(const QuadTree&);
	~QuadTree();

	bool init(VBTerrain* _terrain, ID3D11Device* _GD);
};

#endif