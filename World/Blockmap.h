#ifndef BLOCKMAP_H
#define BLOCKMAP_H
typedef struct {
	unsigned char u1[2];
	unsigned char v1[2];
	unsigned char u2[2];
	unsigned char v2[2];
} uv;
typedef struct {
	int up;
	int side;
	int down;
} Block;
namespace BLOCKMAP {
	static uv faceMap[] = {
		uv{ {0,0},{1,0},{0,1},{1,1} }, // GRASS TOP
		uv{ {2,0},{3,0},{2,1},{3,1} }, // GRASS SIDE
		uv{ {1,0},{2,0},{1,1},{2,1} }, // DIRT
		uv{ {3,0},{4,0},{3,1},{4,1} }, // STONE
		uv{ {5,0},{6,0},{5,1},{6,1} }, // WATER
		uv{ {4,0},{5,0},{4,1},{5,1} }, // SAND
		uv{ {7,0},{8,0},{7,1},{8,1} }, // LOG TOP
		uv{ {6,0},{7,0},{6,1},{7,1} }, // LOG SIDE
		uv{ {8,0},{9,0},{8,1},{9,1} }, // LEAVES
		uv{ {9,0},{10,0},{9,1},{10,1} }, // PLANKS UNIVERSAL
		uv{ {0,1},{1,1},{0,2},{1,2} }, // WHITE WOOL UNIVERSAL
		uv{ {1,1},{2,1},{1,2},{2,2} }, // CACTUS UNIVERSAL
		uv{ {4,1},{5,1},{4,2},{5,2} }, // YELLOW WOOL UNIVERSAL
		uv{ {3,1},{4,1},{3,2},{4,2} }, // CLAY UNIVERSAL
		uv{ {2,1},{3,1},{2,2},{3,2} }, // SANDSTONE UNIVERSAL
	};
	static Block grass = { 0,1,2 };
	static Block dirt = { 2,2,2 };
	static Block stone = { 3,3,3 };
	static Block water = { 4,4,4 };
	static Block sand = { 5,5,5 };
	static Block log = { 6,7,6 };
	static Block leaves = { 8,8,8 };
	static Block planks = { 9,9,9 };
	static Block whitewool = { 10,10,10 };
	static Block cactus = { 11,11,11 };
	static Block yellowwool = { 12,12,12 };
	static Block clay = { 13,13,13 };
	static Block sandstone = { 14,14,14 };
	static Block blockMap[] = {
		grass,
		dirt,
		stone,
		water,
		sand,
		log,
		leaves,
		planks,
		whitewool,
		cactus,
		yellowwool,
		clay,
		sandstone
	};
}
#endif