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
static float blockDistance(BlockPos x, BlockPos y) {
	float dst = sqrt((x.x - y.x) * (x.x - y.x) + (x.y - y.y) * (x.y - y.y) + (x.z - y.z) * (x.z - y.z));
	return dst;
}
class World {
public:
	World(WorldProcessor proc, Material* mat);
	void Create(int dist, StructureGenerator* gen);
	void Modify(std::vector<ModifyBlock> modifyBlocks);
	void Update(ChunkID center, StructureGenerator* gen);
	void Render();
	void Delete();
	int GetBlockAt(BlockPos position);
	Chunk* GetChunkAt(ChunkID id);
	std::vector<Chunk*> GetChunksNear(ChunkID, int range);
	int renderDistance = 8;
	//Translate a position to local chunk coordinates ( for example -1 to 15 )
	BlockPos TranslatePos(BlockPos pos) {
		ChunkID id = { floorf(pos.x / 16.0f), floorf(pos.z / 16.0f) };
		int x = pos.x - id.x * 16.0f;
		int z = pos.z - id.y * 16.0f;
		return { x,pos.y,z };
	}
	int GetHighestBlock(int x, int z);
	WorldProcessor* GetWorldProcessor() {
		return &worldProcessor;
	}
private:
	double lastFrame = 0.0;
	std::vector<Chunk> chunksLoaded;
	std::vector<unsigned int> loadQueue;
	std::vector<unsigned int> unloadQueue;
	std::vector<QueueBlock> blockQueue;
	glm::vec2 waterPosition = { 0,0 };
	glm::vec3 sunPosition = { 0,2.0f,0 };
	WorldProcessor worldProcessor;
	Material* mat;
};
#endif