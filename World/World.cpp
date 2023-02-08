#include "World.h"
#include "Structures/StructureManager.h"
//Constructor
World::World(WorldProcessor proc, Material* mat) {
	this->worldProcessor = proc;
	this->mat = mat;
}
//Create a world with given start distance and update chunks which had blocks put in them (Blockqueue)
void World::Create(int dist, StructureGenerator* gen) {
	for (int x = -dist; x < dist; x++) {
		for (int y = -dist; y < dist; y++) {
			ChunkID id = { x,y };
			Chunk chunk(id, &worldProcessor, mat, this);
			for (int i = 0; i < blockQueue.size(); i++) {
				if (blockQueue[i].id == id) {
					int blockId = blockQueue[i].pos.x * 256 * 16 + blockQueue[i].pos.y * 16 + blockQueue[i].pos.z;
					chunk.Modify(blockId, blockQueue[i].block);
					blockQueue.erase(blockQueue.begin() + i);
				}
			}
			chunksLoaded.push_back(chunk);
			loadQueue.push_back(chunksLoaded.size() - 1);
		}
	}
	blockQueue.shrink_to_fit();
}
//Update the render distance of chunk around the center
void World::Update(ChunkID center, StructureGenerator* gen) {
	if (loadQueue.size() > 0) {
		chunksLoaded[loadQueue[0]].Generate(worldProcessor, gen);
		loadQueue.erase(loadQueue.begin());
		loadQueue.shrink_to_fit();
	}
	if (unloadQueue.size() > 0) {
		chunksLoaded[unloadQueue[0]].Unload();
		unloadQueue.erase(unloadQueue.begin());
		unloadQueue.shrink_to_fit();
	}
	//Don't update if the loadqueue has contents
	if (loadQueue.size() != 0 || unloadQueue.size() != 0) return;
	//Unload the chunks which are too far away
	for (int i = 0; i < chunksLoaded.size(); i++) {
		if (chunksLoaded[i].IsLoaded()) {
			ChunkID chunkId = chunksLoaded[i].GetID();
			float xDiff = std::abs(chunkId.x - center.x);
			float yDiff = std::abs(chunkId.y - center.y);
			//Max value of the differens in x and y planes
			float diff = std::max(xDiff, yDiff);
			if (diff > renderDistance) {
				unloadQueue.push_back(i);
			}
		}
	}
	// Add chunks in a renderdistance*2, renderdistance*2 formation
	for (int x = -renderDistance; x < renderDistance; x++) {
		for (int y = -renderDistance; y < renderDistance; y++) {
			ChunkID id = { center.x + x,center.y + y };
			bool exists = false, loaded = false;;
			int index = 0;
			for (int i = 0; i < chunksLoaded.size(); i++) {
				if (chunksLoaded[i].GetID().x == id.x && chunksLoaded[i].GetID().y == id.y) {
					exists = true;
					loaded = chunksLoaded[i].IsLoaded();
					index = i;
				}
			}
			if (!exists) {
				Chunk chunk(id, &worldProcessor, mat, this);
				for (int i = 0; i < blockQueue.size(); i++) {
					if (blockQueue[i].id == id) {
						int blockId = blockQueue[i].pos.x * 256 * 16 + blockQueue[i].pos.y * 16 + blockQueue[i].pos.z;
						chunk.Modify(blockId, blockQueue[i].block);
						blockQueue.erase(blockQueue.begin() + i);
					}
				}
				chunksLoaded.push_back(chunk);
				loadQueue.push_back(chunksLoaded.size() - 1);
			}
			else {
				if (!chunksLoaded[index].IsLoaded()) {
					for (int i = 0; i < loadQueue.size(); i++) {
						if (loadQueue[i] == index) return;
					}
					loadQueue.push_back(index);
				}
			}
		}
	}
	blockQueue.shrink_to_fit();
}
Chunk* World::GetChunkAt(ChunkID id) {
	for (int i = 0; i < chunksLoaded.size(); i++) {
		if (chunksLoaded[i].GetID().x == id.x && chunksLoaded[i].GetID().y == id.y) {
			return &chunksLoaded[i];
		}
	}
	return nullptr;
}
std::vector<Chunk*> World::GetChunksNear(ChunkID id, int range) {
	std::vector<Chunk*> product;
	for (int i = 0; i < chunksLoaded.size(); i++) {
		ChunkID currentId = chunksLoaded[i].GetID();
		int diffX = std::abs(currentId.x - id.x);
		int diffY = std::abs(currentId.y - id.y);
		int finalDiff = std::max(diffX, diffY);
		if (finalDiff < range) {
			product.push_back(&chunksLoaded[i]);
		}
	}
	return product;
} 
// Modify a block globally
void World::Modify(std::vector<ModifyBlock> blocks) {
	//Dont update the chunk that the block is in directly wait until the loop is done and summarize all the chunks that should be updated so you dont update more than once
	std::vector<int> changedChunks;
	for (auto block : blocks) {
		if (block.globalPos.y == 255) continue;
		//Localize the block
		BlockPos localPosition = TranslatePos(block.globalPos);
		//Get chunk that the blocks resides in
		ChunkID id = { floorf(block.globalPos.x / 16.0f), floorf(block.globalPos.z / 16.0f) };
		bool chunkExists = false;
		for (int i = 0; i < chunksLoaded.size(); i++) {
			if (chunksLoaded[i].GetID().x == id.x && chunksLoaded[i].GetID().y == id.y) {
				chunkExists = true;
				int localId = localPosition.x * 256 * 16 + localPosition.y * 16 + localPosition.z;
				if (chunksLoaded[i].IsLoaded()) {
					chunksLoaded[i].Modify(localId, block.block);
					bool alreadyChanged = false;
					for (auto chunks : changedChunks) {
						if (chunks == i) {
							alreadyChanged = true;
							break;
						}
					}
					if (!alreadyChanged) {
						changedChunks.push_back(i);
					}
				}
				else {
					chunksLoaded[i].Modify(localId, block.block);
					break;
				}
			}
		}
		//If the chunk currently doesn't exist add it to a queue
		if (!chunkExists) {
			QueueBlock queueBlock;
			queueBlock.block = block.block;
			queueBlock.id = id;
			queueBlock.pos = localPosition;
			blockQueue.push_back(queueBlock);
		}
 	}
	for (auto i : changedChunks) {
		chunksLoaded[i].Regenerate();
	}
}
int World::GetBlockAt(BlockPos position) {
	ChunkID id = { floorf(position.x / 16.0f), floorf(position.z / 16.0f) };
	BlockPos localPosition = TranslatePos(position);
	for (int i = 0; i < chunksLoaded.size(); i++) {
		ChunkID chunkId = chunksLoaded[i].GetID();
		if (id == chunkId) {
			int blockId = localPosition.x * 256 * 16 + localPosition.y * 16 + localPosition.z;
			return chunksLoaded[i].GetBlock(blockId);
		}
	}
	return -1;
}
// Get highest surface at 2d point
int World::GetHighestBlock(int x, int y) {
	ChunkID id = { floorf(x / 16.0f), floorf(y / 16.0f) };
	Chunk* c = GetChunkAt(id);
	if (c == nullptr) return 0;
	for (int i = 0; i < 256; i++) {
		BlockPos translated = TranslatePos({ x,i,y });
		int curId = translated.x * 256 * 16 + translated.y * 16 + translated.z;
		if (c->GetBlock(curId) <= 0) {
			return i;
		}
	}
	return 127;
}
// Render the world and update the water offset
void World::Render() {
	mat->shader->Bind();
	mat->tex.Bind();
	double currentFrame = glfwGetTime();
	float deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
	glUniform3fv(glGetUniformLocation(mat->shader->id, "sunPos"), 1, glm::value_ptr(sunPosition));
	for (int i = 0; i < chunksLoaded.size(); i++) {
		chunksLoaded[i].SolidRender();
	}
	glUniform2fv(glGetUniformLocation(mat->shader->id, "waterPos"), 1, glm::value_ptr(waterPosition));
	for (int i = 0; i < chunksLoaded.size(); i++) {
		chunksLoaded[i].WaterRender();
	}
	// Move the center water position
	waterPosition.x += 3.0f*deltaTime;
	waterPosition.y += 3.0f*deltaTime;
}
//Delete the world from memory
void World::Delete() {
	for (auto& c : chunksLoaded) {
		c.Unload();
	}
	chunksLoaded.clear();
	chunksLoaded.shrink_to_fit();
}
