#include "World.h"
#include "Structures/StructureManager.h"

World::World(WorldProcessor proc, Material* mat) {
	this->proc = proc;
	this->mat = mat;
}
void World::Create(int dist, StructureGenerator* gen) {
	for (int x = -dist; x < dist; x++) {
		for (int y = -dist; y < dist; y++) {
			ChunkID id = { x,y };
			Chunk c(id, &proc, mat, this);
			for (int i = 0; i < blockQueue.size(); i++) {
				if (blockQueue[i].id == id) {
					int bId = blockQueue[i].pos.x * 256 * 16 + blockQueue[i].pos.y * 16 + blockQueue[i].pos.z;
					c.Modify(bId, blockQueue[i].block);
					blockQueue.erase(blockQueue.begin() + i);
				}
			}
			chunksLoaded.push_back(c);
			loadQueue.push_back(chunksLoaded.size() - 1);
		}
	}
	blockQueue.shrink_to_fit();
}
void World::Update(ChunkID center, StructureGenerator* gen) {
	if (loadQueue.size() > 0) {
		chunksLoaded[loadQueue[0]].Generate(proc, gen);
		loadQueue.erase(loadQueue.begin());
		loadQueue.shrink_to_fit();
	}
	if (unloadQueue.size() > 0) {
		chunksLoaded[unloadQueue[0]].Unload();
		unloadQueue.erase(unloadQueue.begin());
		unloadQueue.shrink_to_fit();
	}
	if (loadQueue.size() != 0 || unloadQueue.size() != 0) return;
	for (int i = 0; i < chunksLoaded.size(); i++) {
		if (chunksLoaded[i].IsLoaded()) {
			ChunkID cid = chunksLoaded[i].GetID();
			float xDiff = std::abs(cid.x - center.x);
			float yDiff = std::abs(cid.y - center.y);
			float diff = std::max(xDiff, yDiff);
			if (diff > renderDistance) {
				unloadQueue.push_back(i);
			}
		}
	}
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
				Chunk c(id, &proc, mat, this);
				for (int i = 0; i < blockQueue.size(); i++) {
					if (blockQueue[i].id == id) {
						int bId = blockQueue[i].pos.x * 256 * 16 + blockQueue[i].pos.y * 16 + blockQueue[i].pos.z;
						c.Modify(bId, blockQueue[i].block);
						blockQueue.erase(blockQueue.begin() + i);
					}
				}
				chunksLoaded.push_back(c);
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
void World::Modify(std::vector<ModifyBlock> blocks) {
	std::vector<int> changedChunks;
	for (auto block : blocks) {
		if (block.globalPos.y == 255) continue;
		BlockPos local = TranslatePos(block.globalPos);
		ChunkID id = { floorf(block.globalPos.x / 16.0f), floorf(block.globalPos.z / 16.0f) };
		bool chunkExists = false;
		for (int i = 0; i < chunksLoaded.size(); i++) {
			if (chunksLoaded[i].GetID().x == id.x && chunksLoaded[i].GetID().y == id.y) {
				chunkExists = true;
				int localId = local.x * 256 * 16 + local.y * 16 + local.z;
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
		if (!chunkExists) {
			QueueBlock queueBlock;
			queueBlock.block = block.block;
			queueBlock.id = id;
			queueBlock.pos = local;
			blockQueue.push_back(queueBlock);
		}
 	}
	for (auto i : changedChunks) {
		chunksLoaded[i].Regenerate();
	}
}
int World::GetBlockAt(BlockPos position) {
	ChunkID id = { floorf(position.x / 16.0f), floorf(position.z / 16.0f) };
	BlockPos local = TranslatePos(position);
	for (int i = 0; i < chunksLoaded.size(); i++) {
		ChunkID cid = chunksLoaded[i].GetID();
		if (id == cid) {
			int blockId = local.x * 256 * 16 + local.y * 16 + local.z;
			return chunksLoaded[i].GetBlock(blockId);
		}
	}
	return -1;
}
int World::GetTopHeight(int x, int y) {
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
	waterPosition.x += 3.0f*deltaTime;
	waterPosition.y += 3.0f*deltaTime;
}
void World::Delete() {
	for (auto c : chunksLoaded) {
		c.Unload();
	}
	chunksLoaded.clear();
	chunksLoaded.shrink_to_fit();
}
