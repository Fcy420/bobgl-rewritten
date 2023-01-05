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

class BlockBuilder {
public:
	static VertexComponent CreateBlock(std::vector<glm::vec3> directions, BlockPos pos, int b) {
		std::vector<vertex> vertices;
		std::vector<unsigned int> triangles;
		for (int sideIndex = 0; sideIndex < directions.size(); sideIndex++) {
			int block = b - 1;
			std::vector<vertex> verts = GetDirectionVertices(directions[sideIndex], block);
			vertices.push_back(verts[0]+pos);
			vertices.push_back(verts[1]+pos);
			vertices.push_back(verts[2]+pos);
			vertices.push_back(verts[3]+pos);
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
	static std::vector<vertex> GetDirectionVertices(glm::vec3 side, int b) {
		Block faces = BLOCKMAP::blockMap[b];
		std::vector<vertex> vertices;
		if (side == glm::vec3(1, 0, 0)) {
			uv v = BLOCKMAP::faceMap[faces.side];
			vertices = {
				{1,0,0, 2, v.u1[0],v.u1[1], true},
				{1,0,1, 2, v.v1[0],v.v1[1], false },
				{1,1,0, 2, v.u2[0],v.u2[1], false },
				{1,1,1, 2, v.v2[0],v.v2[1], false },
			};
		}
		else if (side == glm::vec3(-1, 0, 0)) {
			uv v = BLOCKMAP::faceMap[faces.side];
			vertices = {
				{0,0,0, 3, v.u1[0],v.u1[1], false },
				{0,0,1, 3, v.v1[0],v.v1[1], false },
				{0,1,0, 3, v.u2[0],v.u2[1], false },
				{0,1,1, 3, v.v2[0],v.v2[1], false },
			};
		}
		else if (side == glm::vec3(0, 0, -1)) {
			uv v = BLOCKMAP::faceMap[faces.side];
			vertices = {
				{0,0,0, 0, v.u1[0],v.u1[1], true },
				{1,0,0, 0, v.v1[0],v.v1[1], false },
				{0,1,0, 0, v.u2[0],v.u2[1], false },
				{1,1,0, 0, v.v2[0],v.v2[1], false },
			};
		}
		else if (side == glm::vec3(0, 0, 1)) {
			uv v = BLOCKMAP::faceMap[faces.side];
			vertices = {
				{0,0,1, 1, v.u1[0],v.u1[1], false },
				{1,0,1, 1, v.v1[0],v.v1[1], false },
				{0,1,1, 1, v.u2[0],v.u2[1], false },
				{1,1,1, 1, v.v2[0],v.v2[1], false },
			};
		}
		else if (side == glm::vec3(0, 1, 0)) {
			uv v = BLOCKMAP::faceMap[faces.up];
			vertices = {
				{0,1,0, 4, v.u1[0],v.u1[1], true },
				{1,1,0, 4, v.v1[0],v.v1[1], false },
				{0,1,1, 4, v.u2[0],v.u2[1], false },
				{1,1,1, 4, v.v2[0],v.v2[1], false },
			};
		}
		else if (side == glm::vec3(0, -1, 0)) {
			uv v = BLOCKMAP::faceMap[faces.down];
			vertices = {
				{0,0,0, 5, v.u1[0],v.u1[1], false },
				{1,0,0, 5, v.v1[0],v.v1[1], false },
				{0,0,1, 5, v.u2[0],v.u2[1], false },
				{1,0,1, 5, v.v2[0],v.v2[1], false },
			};
		}
		return vertices;
		return vertices;
	}
};

#endif