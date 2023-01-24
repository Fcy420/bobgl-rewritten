#include "Chunk.h"
#include "World.h"
#include "Structures/StructureManager.h"

Chunk::Chunk(ChunkID id, WorldProcessor* proc, Material* mat, World* world) {
	this->id = id;
	this->proc = proc;
	this->mat = mat;
	this->world = world;
}
// Generate noise and structures and the renderable mesh
void Chunk::Generate(WorldProcessor proc, StructureGenerator* gen) {
	if (!generated) {
		UpdateNeighbours();
		Populate(proc, gen);
		Decorate();
		SpawnStructures(gen, proc);
		Build();
		GenMesh();
		generated = true;
	}
	else {
		Regenerate();
	}
	loaded = true;
}
//Just rebuild the chunk
void Chunk::Regenerate() {
	Build();
	GenMesh();
	loaded = true;
}
//Populate the blockmap with noise
void Chunk::Populate(WorldProcessor proc, StructureGenerator* gen) {
	int hIndex = 0;
	int rX = rand() % 15;
	int rY = rand() % 15;
	bool hasStructure = rand() % 1000 < 300;
	for (int x = 0; x < 16; x++) {
		for (int y = 0; y < 256; y++) {
			for (int z = 0; z < 16; z++) {
				int type = proc.Process({ x + id.x * 16.0f,y + 0.0f,z + id.y * 16.0f });
				bool isChanged = false;
				for (int i = 0; i < changed.size(); i++) {
					if (i >= changed.size()) break;
					if (changed[i] == hIndex) {
						isChanged = true;
					}
				}
				if (!isChanged) {
					map[hIndex] = type;
				}
				hIndex++;
			}
		}
	}
}
//Try to spawn a random structure
void Chunk::SpawnStructures(StructureGenerator* gen, WorldProcessor proc) {
	bool hasStructure = rand() % 1000 < 10;
	#pragma region Ship
	if (hasStructure) {
		int rx = 0;
		int rz = 0;
		int ry = proc.water - 1;
		bool res = gen->EvaluatePosition({ rx + id.x * 16,ry,rz + id.y * 16 }, SHIP);
		//if (res) return;
		ry = proc.iso;
		bool canSpawn = true;
		while (proc.Process({ rx + id.x * 16, ry, rz + id.y * 16 }) != 4) {
			if (proc.Process({ rx + id.x * 16, ry, rz + id.y * 16 }) == 0) {
				canSpawn = false;
				break;
			}
			ry++;
		}
		if (canSpawn) res = gen->EvaluatePosition({ rx + id.x * 16, ry, rz + id.y * 16 }, ATLANTIS);
		if (res) return;
		ry = world->GetTopHeight(rx + id.x * 16, rz + id.y * 16);
		res = gen->EvaluatePosition({ rx + id.x * 16, ry, rz + id.y * 16 }, ATLANTIS);

	}
	#pragma endregion
}
void Chunk::Modify(int id, int block) {
	if (id >= 0 && id < 256*16*16) {
		changed.push_back(id);
		map[id] = block;
	}
}
//Build the collision and visible mesh
void Chunk::Build() {
	int hIndex = 0;
	boundingBox.clear();
	for (int x = 0; x < 16; x++) {
		for (int y = 0; y < 256; y++) {
			for (int z = 0; z < 16; z++) {
				if (map[hIndex] != 0) {
					std::vector<glm::vec3> directions;
					directions = CalculateBlock({ x,y,z });
					VertexComponent block = BlockBuilder::CreateBlock(directions, { x,y,z }, map[hIndex]);
					if(map[hIndex] == 4) 
						waterbatch.push_back(block);
					else {
						if (directions.size() > 0) {
							BoundingBox box;
							box.min = { x+id.x*16,y,z+id.y*16 };
							box.max = { x + 1+id.x*16,y + 1,z+1+id.y*16 };
							boundingBox.push_back(box);
						}
						solidbatch.push_back(block);
					}
				}
				hIndex++;
			}
		}
	}
}
//Decorate terrain with trees and other vegetables
void Chunk::Decorate() {
	int hIndex = 0;
	int dirs[] = {
		256 * 16,256 * -16,1,-1,16
	};
	std::vector<BlockPos> trees;
	for (int x = 0; x < 16; x++) {
		for (int y = 0; y < 256; y++) {
			for (int z = 0; z < 16; z++) {
				bool isChanged = false;
				for (int i = 0; i < changed.size(); i++) {
					if (changed[i] == hIndex) {
						isChanged = true;
					}
				}
				if (!isChanged) {
					if (map[hIndex] != 0 && map[hIndex] != 4) {
						if (map[hIndex] == 1) {
							if (rand() % 1000 < 3)
								trees.push_back({ x,y,z });
							continue;
						}
						for (int i = 0; i < 5; i++) {
							if (map[hIndex] == 5) break;
							if (hIndex + dirs[i] >= 256 * 16 * 16 || hIndex+dirs[i] < 0) continue;
							if (map[hIndex + dirs[i]] == 4)
								map[hIndex] = 5;
						}
						if (map[hIndex + 16] == 0) {
							if (map[hIndex] != 5) map[hIndex] = 1;
						}
 					}
				}
				hIndex++;
			}
		}
	}
	for (auto b : trees) {
		int id = b.x * 256 * 16 + b.y * 16 + b.z;
		for (int i = 1; i <= 5; i++) {
			Modify(id, 6);
		}
	}
}
//Calculate a blocks final directions (If block should have a direction or not and which ones)
std::vector<glm::vec3> Chunk::CalculateBlock(BlockPos pos) {
	std::vector<glm::vec3> directions;
	int posId = pos.x * 256 * 16 + pos.y * 16 + pos.z;
	if (map[posId] == 4) {
		glm::vec3 dir = UP;
		glm::vec3 dirPos = { pos.x + dir.x,pos.y + dir.y,pos.z + dir.z };
		int id = dirPos.x * 256 * 16 + dirPos.y * 16 + dirPos.z;
		if (map[id] != 4) {
			directions.push_back(dir);
		}
		return directions;
	}
	for (int i = 0; i < 6; i++) {
		glm::vec3 dir = DIRECTIONS[i];
		glm::vec3 dirPos = { pos.x + dir.x,pos.y + dir.y,pos.z + dir.z };
		int id = dirPos.x * 256 * 16 + dirPos.y * 16 + dirPos.z;
		if (dirPos.y >= 0 && dirPos.y < 256) {
			if (dirPos.x >= 0 && dirPos.x < 16 && dirPos.z >= 0 && dirPos.z < 16) {
				if (map[id] == 0) {
					directions.push_back(dir);
				}
				if (map[id] == 4 && map[posId] != 4) {
					directions.push_back(dir);
				}
			}
			else {
				bool isChanged = false;
				for (int i = 0; i < changed.size(); i++) {
					if (changed[i] == posId) {
						directions.push_back(dir);
						isChanged = true;
					}
				}
				if (map[posId] != 4 && !isChanged) {
					BlockPos pos = { this->id.x * 16 + dirPos.x, dirPos.y, this->id.y * 16 + dirPos.z };
					int type = proc->Process(glm::vec3(pos.x, pos.y, pos.z));
					if (type <= 0 || type == 4) {
						directions.push_back(dir);
					}
				}
			}
		}
	}
	return directions;
}
//Combine all the blocks into one draw call
void Chunk::GenMesh() {
	MeshComponent comp = GetBatch(solidbatch);
	solidbatch.clear();
	solidbatch.shrink_to_fit();
	mesh.Initialize(comp.vertices, comp.triangles, comp.mat);
	comp = GetBatch(waterbatch);
	waterbatch.clear();
	waterbatch.shrink_to_fit();
	waterMesh.Initialize(comp.vertices, comp.triangles, comp.mat);
}
//Combine a batch into one meshcomponent
MeshComponent Chunk::GetBatch(std::vector<VertexComponent> batch) {
	unsigned int bIndex = 0;
	std::vector<unsigned char> vertices;
	std::vector<unsigned int> triangles;
	for (int i = 0; i < batch.size(); i++) {
		for (int k = 0; k < batch[i].vertices.size(); k++) {

			vertices.push_back(batch[i].vertices[k].x);
			vertices.push_back(batch[i].vertices[k].y);
			vertices.push_back(batch[i].vertices[k].z);
			vertices.push_back(batch[i].vertices[k].u);
			vertices.push_back(batch[i].vertices[k].v);
			vertices.push_back(batch[i].vertices[k].l);
		}
		for (int k = 0; k < batch[i].triangles.size(); k++) {
			unsigned int index = batch[i].triangles[k] + bIndex;
			triangles.push_back(index);
		}
		bIndex += batch[i].vertices.size();
	}
	return { vertices, triangles, mat };
}
//update the chunk neighbours íf they are all updated dont do anything
void Chunk::UpdateNeighbours() {
	if (neighbours.size() < 4) {
		for (int i = 0; i < 4; i++) {
			ChunkID targetId = { chunkDirections[i].x + id.x, chunkDirections[i].y + id.y };
			bool exists = false;
			for (int k = 0; k < neighbours.size(); k++) {
				if (neighbours[k].id.x == targetId.x && neighbours[k].id.y == targetId.y) {
					exists = true;
					break;
				}
			}
			if (!exists) {
				Chunk* target = world->GetChunkAt(targetId);
				if (target != nullptr) {
					neighbours.push_back({ targetId, target });
				}
			}
		}
	}
}
//Render the solid part of the chunk
void Chunk::SolidRender() {
	if (loaded) {
		UpdateNeighbours();
		glm::vec4 color = { 1,1,1,1 };
		mesh.Draw({id.x*16.0f, 0.0f, id.y*16.0f}, color);
	}
}
//Render the water
void Chunk::WaterRender() {
	if (loaded) {
		glEnable(GL_BLEND);
		waterMesh.Draw({ id.x * 16.0f, -0.4f, id.y * 16.0f }, { 1,1,1,0.8f });
		glDisable(GL_BLEND);
	}
}
//Unload the chunk from memory (Dunno why this is not working)
void Chunk::Unload() {
	mesh.Unload();
	waterMesh.Unload();
	loaded = false;
}