#ifndef CHUNK_H
#define CHUNK_H
#include"../BufferObjects/Mesh.h"
#include"../Collision/Collision.h"
#include"BlockBuilder.h"
#include"WorldProcessor.h"
#include<algorithm>
#include<thread>
//#include"../BufferObjects/Material.h"
class StructureGenerator;
typedef struct {
	__int16 x;
	__int16 y;
} ChunkID;
class World;
static ChunkID chunkDirections[] = {
	{1,0},
	{-1,0},
	{0,1},
	{0,-1}
};
static bool operator == (ChunkID& a, ChunkID& b) {
	return a.x == b.x && a.y == b.y;
}
static bool operator != (ChunkID& a, ChunkID& b) {
	return a.x != b.x && a.y != b.y;
}
class Chunk {
	// Neighbour of this chunk
	typedef struct {
		ChunkID id;
		Chunk* c;
	} ChunkNeighbour;
public:
	// Constructor for chunk (Assigning variables)
	Chunk(ChunkID id, WorldProcessor* proc, Material* mat, World* world);
	void Generate(WorldProcessor proc, StructureGenerator* gen);
	void Regenerate();
	void SolidRender();
	void WaterRender();
	void Unload();
	void SpawnStructures(StructureGenerator* gen, WorldProcessor proc);
	void Modify(int id, int block);
	int GetBlock(int id) {
		if (id >= 0 && id < 256 * 16 * 16) {
			return map[id];
		}
		return -1;
	}
	ChunkID GetID() {
		return id;
	}
	//Get collision boxes for the current chunk
	std::vector<BoundingBox> GetBoxes() {
		return boundingBox;
	}
	bool IsLoaded() {
		return loaded;
	}
	void UpdateNeighbours();
	Chunk* GetNeighbour(ChunkID targetId) {
		for (int i = 0; i < neighbours.size(); i++) {
			if (neighbours[i].id.x == targetId.x && neighbours[i].id.y == targetId.y)
				return neighbours[i].c;
		}
		return nullptr;
	}
private:
	std::vector<int> changed;
	bool loaded = false;
	bool generated = false;
	void Populate(WorldProcessor proc, StructureGenerator* gen);
	void Decorate();
	void Build();
	MeshComponent GetBatch(std::vector<VertexComponent> batch);
	std::vector<glm::vec3> CalculateBlock(BlockPos pos);
	void GenMesh();
	std::vector<VertexComponent> solidbatch;
	std::vector<VertexComponent> waterbatch;
	std::vector<BoundingBox> boundingBox;
	//The neighbouring chunks
	std::vector<ChunkNeighbour> neighbours;
	ChunkID id;
	Mesh mesh;
	Mesh waterMesh;
	// Block map
	unsigned __int8 map[16 * 16 * 256] = { 0 };
	WorldProcessor* worldProcessor;
	Material* mat;
	World* world;
};
#endif