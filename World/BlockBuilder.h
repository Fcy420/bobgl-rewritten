#ifndef BLOCKBUILDER_H
#define BLOCKBUILDER_H
#include<vector>
#include<iostream>
#include<glm/vec3.hpp>
#include"Blockmap.h"

static glm::vec3 UP = { 0,1,0 };
static glm::vec3 DOWN = { 0,-1,0 };
static glm::vec3 RIGHT = { 1,0,0 };
static glm::vec3 LEFT = { -1,0,0 };
static glm::vec3 FORWARD = { 0,0,1 };
static glm::vec3 BACKWARD = { 0,0,-1 };
static glm::vec3 DIRECTIONS[] = {
	LEFT,
	BACKWARD,
	FORWARD,
	RIGHT,
	UP,
	DOWN
};
typedef struct {
	int x;
	int y;
	int z;
} BlockPos;
// L stands for lightvalue
typedef struct {
	unsigned char x;
	unsigned char y;
	unsigned char z;
	unsigned char l;
	unsigned char u;
	unsigned char v;
	bool reversed;
} vertex;
typedef struct {
	std::vector<vertex> vertices;
	std::vector<unsigned int> triangles;
} VertexComponent;

static vertex operator +(vertex& a, BlockPos& b) {
	vertex result;
	result.x = a.x + b.x;
	result.y = a.y + b.y;
	result.z = a.z + b.z;
	result.u = a.u;
	result.v = a.v;
	result.reversed = a.reversed;
	result.l = a.l;
	return result;
}
// Builds blocks
class BlockBuilder {
public:
	// Create a block with given directions and compare the block id to uv list
	static VertexComponent CreateBlock(std::vector<glm::vec3> directions, BlockPos pos, int blockType) {
		std::vector<vertex> vertices;
		std::vector<unsigned int> triangles;
		for (int sideIndex = 0; sideIndex < directions.size(); sideIndex++) {

			//Set block to a 0 -> amount of unique blocks so that air counts as 0
			int block = blockType - 1;
			std::vector<vertex> verts = GetDirectionVertices(directions[sideIndex], block);
			vertices.push_back(verts[0]+pos);
			vertices.push_back(verts[1]+pos);
			vertices.push_back(verts[2]+pos);
			vertices.push_back(verts[3]+pos);
			// Quad triangles
			//Combine the vertices in a quad order
			if (!verts[0].reversed) {
				triangles.push_back(sideIndex * 4);
				triangles.push_back(sideIndex * 4 + 1);
				triangles.push_back(sideIndex * 4 + 2);
				triangles.push_back(sideIndex * 4 + 3);
				triangles.push_back(sideIndex * 4 + 2);
				triangles.push_back(sideIndex * 4 + 1);
			} else {
				triangles.push_back(sideIndex * 4 + 2);
				triangles.push_back(sideIndex * 4 + 1);
				triangles.push_back(sideIndex * 4);
				triangles.push_back(sideIndex * 4 + 1);
				triangles.push_back(sideIndex * 4 + 2);
				triangles.push_back(sideIndex * 4 + 3);
			}
		}
		return { vertices, triangles };
	}

private:
	// not used
	static std::vector<char> VertexToChar(std::vector<vertex> vertexes) {
		std::vector<char> vertices;
		for (int i = 0; i < vertexes.size(); i++) {
			vertices.push_back(vertexes[i].x);
			vertices.push_back(vertexes[i].y);
			vertices.push_back(vertexes[i].z);
			vertices.push_back(vertexes[i].u);
			vertices.push_back(vertexes[i].v);
		}
		return vertices;
	}
	// Build a quad in given direction
	static std::vector<vertex> GetDirectionVertices(glm::vec3 side, int b) {
		Block faces = BLOCKMAP::blockMap[b];
		std::vector<vertex> vertices;
		if (side == glm::vec3(1, 0, 0)) {
			//Get texture coordinates from the block map
			uv uv = BLOCKMAP::faceMap[faces.side];
			vertices = {
				{1,0,0, 2, uv.u1[0],uv.u1[1], true},
				{1,0,1, 2, uv.v1[0],uv.v1[1], false },
				{1,1,0, 2, uv.u2[0],uv.u2[1], false },
				{1,1,1, 2, uv.v2[0],uv.v2[1], false },
			};
		}
		else if (side == glm::vec3(-1, 0, 0)) {
			uv uv = BLOCKMAP::faceMap[faces.side];
			vertices = {
				{0,0,0, 3, uv.u1[0],uv.u1[1], false },
				{0,0,1, 3, uv.v1[0],uv.v1[1], false },
				{0,1,0, 3, uv.u2[0],uv.u2[1], false },
				{0,1,1, 3, uv.v2[0],uv.v2[1], false },
			};
		}
		else if (side == glm::vec3(0, 0, -1)) {
			uv uv = BLOCKMAP::faceMap[faces.side];
			vertices = {
				{0,0,0, 0, uv.u1[0],uv.u1[1], true },
				{1,0,0, 0, uv.v1[0],uv.v1[1], false },
				{0,1,0, 0, uv.u2[0],uv.u2[1], false },
				{1,1,0, 0, uv.v2[0],uv.v2[1], false },
			};
		}
		else if (side == glm::vec3(0, 0, 1)) {
			uv uv = BLOCKMAP::faceMap[faces.side];
			vertices = {
				{0,0,1, 1, uv.u1[0],uv.u1[1], false },
				{1,0,1, 1, uv.v1[0],uv.v1[1], false },
				{0,1,1, 1, uv.u2[0],uv.u2[1], false },
				{1,1,1, 1, uv.v2[0],uv.v2[1], false },
			};
		}
		else if (side == glm::vec3(0, 1, 0)) {
			uv uv = BLOCKMAP::faceMap[faces.up];
			vertices = {
				{0,1,0, 4, uv.u1[0],uv.u1[1], true },
				{1,1,0, 4, uv.v1[0],uv.v1[1], false },
				{0,1,1, 4, uv.u2[0],uv.u2[1], false },
				{1,1,1, 4, uv.v2[0],uv.v2[1], false },
			};
		}
		else if (side == glm::vec3(0, -1, 0)) {
			uv uv = BLOCKMAP::faceMap[faces.down];
			vertices = {
				{0,0,0, 5, uv.u1[0],uv.u1[1], false },
				{1,0,0, 5, uv.v1[0],uv.v1[1], false },
				{0,0,1, 5, uv.u2[0],uv.u2[1], false },
				{1,0,1, 5, uv.v2[0],uv.v2[1], false },
			};
		}
		return vertices;
	}
};

#endif