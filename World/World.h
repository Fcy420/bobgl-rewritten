#ifndef WORLD_H
#define WORLD_H
#include <vector>
#include "Chunk.h"
#include "WorldProcessor.h"
#include "../BufferObjects/Material.h"
#include<algorithm>
#include<chrono>
#include<vector>
#include<thread>
class StructureGenerator;
typedef struct {
	int block;
	BlockPos pos;
	ChunkID id;
} QueueBlock;
typedef struct {
	int block;
	BlockPos globalPos;
} ModifyBlock;
static BlockPos operator -(BlockPos& a, BlockPos& b) {
	return { a.x - b.x, a.y - b.y, a.z - b.z };
}
static float blength(BlockPos x, BlockPos y) {
	float dst = sqrt((x.x - y.x) * (x.x - y.x) + (x.y - y.y) * (x.y - y.y) + (x.z - y.z) * (x.z - y.z));
	return dst;
}
static BlockPos babs(BlockPos a) {
	return { std::abs(a.x), std::abs(a.y), std::abs(a.z) };
}
static BlockPos bmax(BlockPos a, BlockPos b) {
	return (a.x > b.x && a.y > b.y && a.z > b.z) ? a : b;
}
static BlockPos bmin(BlockPos a, BlockPos b) {
	return (a.x < b.x && a.y < b.y && a.z < b.z) ? a : b;
}
class World {
public:
	World(WorldProcessor proc, Material* mat);
	void Create(int dist, StructureGenerator* gen);
	void Modify(std::vector<ModifyBlock> modifyBlock);
	void Update(ChunkID center, StructureGenerator* gen);
	void Render();
	void Delete();
	int GetBlockAt(BlockPos position);
	Chunk* GetChunkAt(ChunkID id);
	std::vector<Chunk*> GetChunksNear(ChunkID, int range);
	int renderDistance = 8;
	//Translate a position to local chunk coordinates ( for example -1 to 63 )
	BlockPos TranslatePos(BlockPos pos) {
		ChunkID id = { floorf(pos.x / 16.0f), floorf(pos.z / 16.0f) };
		int x = pos.x - id.x * 16.0f;
		int z = pos.z - id.y * 16.0f;
		return { x,pos.y,z };
	}
	int GetTopHeight(int x, int z);
	WorldProcessor* GetProc() {
		return &proc;
	}
private:
	double lastFrame = 0.0;
	std::vector<Chunk> chunksLoaded;
	std::vector<unsigned int> loadQueue;
	std::vector<unsigned int> unloadQueue;
	std::vector<QueueBlock> blockQueue;
	glm::vec2 waterPosition = { 0,0 };
	glm::vec3 sunPosition = { 0,2.0f,0 };
	WorldProcessor proc;
	Material* mat;
};
#endif