#include "Chunk.h"
#include "World.h"
#include "Structures/StructureManager.h"

Chunk::Chunk(ChunkID id, WorldProcessor* proc, Material* mat, World* world) {
	this->id = id;
	this->worldProcessor = proc;
	this->mat = mat;
	this->world = world;
}
// Generate noise and structures and the renderable mesh
void Chunk::Generate(WorldProcessor proc, StructureGenerator* gen) {
	//Safety measure to not "Create" a chunk more than once
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
	//Go through a 3d grid of 0's and give them meaning in life and dont change if it already has a value
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
		int randomXPosition = 0;
		int randomZPosition = 0;
		int randomYPosition = proc.water - 1;
		bool result = gen->EvaluatePosition({ randomXPosition + id.x * 16,randomYPosition,randomZPosition + id.y * 16 }, SHIP);
		//if (res) return;
		randomYPosition = proc.isoSurface;
		bool canSpawn = true;
		//Check the position for water for a ship to spawn
		while (proc.Process({ randomXPosition + id.x * 16, randomYPosition, randomZPosition + id.y * 16 }) != 4) {
			if (proc.Process({ randomXPosition + id.x * 16, randomYPosition, randomZPosition + id.y * 16 }) == 0) {
				canSpawn = false;
				break;
			}
			randomYPosition++;
		}
		if (canSpawn) result = gen->EvaluatePosition({ randomXPosition + id.x * 16, randomYPosition, randomZPosition + id.y * 16 }, ATLANTIS);
		if (result) return;
		randomYPosition = world->GetHighestBlock(randomXPosition + id.x * 16, randomZPosition + id.y * 16);
		result = gen->EvaluatePosition({ randomXPosition + id.x * 16, randomYPosition, randomZPosition + id.y * 16 }, ATLANTIS);

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
	int heightIndex = 0;
	boundingBox.clear();
	//Go through the same 3d grid of values that earlier didn't have meaning and give them a block with faces depending on the surrounding values
	for (int x = 0; x < 16; x++) {
		for (int y = 0; y < 256; y++) {
			for (int z = 0; z < 16; z++) {
				if (map[heightIndex] != 0) {
					std::vector<glm::vec3> directions;
					//Check the surrounding values to see if any of them are transparent then add face
					directions = CalculateBlock({ x,y,z });
					VertexComponent block = BlockBuilder::CreateBlock(directions, { x,y,z }, map[heightIndex]);
					if(map[heightIndex] == 4) 
						waterbatch.push_back(block);
					else {
						//Add a colliding box if the block has a face
						if (directions.size() > 0) {
							BoundingBox box;
							box.min = { x+id.x*16,y,z+id.y*16 };
							box.max = { x + 1+id.x*16,y + 1,z+1+id.y*16 };
							boundingBox.push_back(box);
						}
						solidbatch.push_back(block);
					}
				}
				heightIndex++;
			}
		}
	}
}
//Decorate terrain with trees and other vegetables
void Chunk::Decorate() {
	int heightIndex = 0;
	int blockDirections[] = {
		256 * 16,256 * -16,1,-1,16
	};
	std::vector<BlockPos> trees;
	for (int x = 0; x < 16; x++) {
		for (int y = 0; y < 256; y++) {
			for (int z = 0; z < 16; z++) {
				bool isChanged = false;
				for (int i = 0; i < changed.size(); i++) {
					if (changed[i] == heightIndex) {
						isChanged = true;
					}
				}
				//If the block isn't changed since before the check for a tree
				if (!isChanged) {
					if (map[heightIndex] != 0 && map[heightIndex] != 4) {
						//if the block isn't transparent and is grass add a tree depending on a 3 on 1000 chance (Dunno not working) <- Anders fel lovar
						if (map[heightIndex] == 1) {
							if (rand() % 1000 < 3)
								trees.push_back({ x,y,z });
							continue;
						}
						for (int i = 0; i < 5; i++) {
							if (map[heightIndex] == 5) break;
							if (heightIndex + blockDirections[i] >= 256 * 16 * 16 || heightIndex+blockDirections[i] < 0) continue;
							if (map[heightIndex + blockDirections[i]] == 4)
								map[heightIndex] = 5;
						}
						if (map[heightIndex + 16] == 0) {
							if (map[heightIndex] != 5) map[heightIndex] = 1;
						}
 					}
				}
				heightIndex++;
			}
		}
	}
	//Modify the trees as a root
	for (auto& block : trees) {
		int id = block.x * 256 * 16 + block.y * 16 + block.z;
		for (int i = 1; i <= 5; i++) {
			Modify(id, 6);
		}
	}
}
//Calculate a blocks final directions (If block should have a direction or not and which ones)
std::vector<glm::vec3> Chunk::CalculateBlock(BlockPos pos) {
	std::vector<glm::vec3> directions;
	int positionId = pos.x * 256 * 16 + pos.y * 16 + pos.z;
	// if the given block is water only add the top direction if it touches air or any other block
	if (map[positionId] == 4) {
		glm::vec3 direction = UP;
		glm::vec3 directionPosition = { pos.x + direction.x,pos.y + direction.y,pos.z + direction.z };
		int id = directionPosition.x * 256 * 16 + directionPosition.y * 16 + directionPosition.z;
		if (map[id] != 4) {
			directions.push_back(direction);
		}
		return directions;
	}
	//Check all cube directions for blocks and if the blocks are solid don't add the direction
	for (int i = 0; i < 6; i++) {
		glm::vec3 direction = DIRECTIONS[i];
		glm::vec3 directionPosition = { pos.x + direction.x,pos.y + direction.y,pos.z + direction.z };
		int id = directionPosition.x * 256 * 16 + directionPosition.y * 16 + directionPosition.z;
		if (directionPosition.y >= 0 && directionPosition.y < 256) {
			if (directionPosition.x >= 0 && directionPosition.x < 16 && directionPosition.z >= 0 && directionPosition.z < 16) {
				if (map[id] == 0) {
					directions.push_back(direction);
				}
				if (map[id] == 4 && map[positionId] != 4) {
					directions.push_back(direction);
				}
			}
			else {
				bool isChanged = false;
				for (int i = 0; i < changed.size(); i++) {
					if (changed[i] == positionId) {
						directions.push_back(direction);
						isChanged = true;
					}
				}
				if (map[positionId] != 4 && !isChanged) {
					BlockPos position = { this->id.x * 16 + directionPosition.x, directionPosition.y, this->id.y * 16 + directionPosition.z };
					int blockType = worldProcessor->Process(glm::vec3(position.x, position.y, position.z));
					if (blockType <= 0 || blockType == 4) {
						directions.push_back(direction);
					}
				}
			}
		}
	}
	return directions;
}
//Combine all the blocks into one draw call
void Chunk::GenMesh() {
	//Get a single mesh component from a batch of vertex component / blocks
	MeshComponent meshComponent = GetBatch(solidbatch);
	solidbatch.clear();
	solidbatch.shrink_to_fit();
	//Initialize the solid mesh
	mesh.Initialize(meshComponent.vertices, meshComponent.triangles, meshComponent.mat);
	//Repeat for water (Seperating for transparency issues)
	meshComponent = GetBatch(waterbatch);
	waterbatch.clear();
	waterbatch.shrink_to_fit();
	waterMesh.Initialize(meshComponent.vertices, meshComponent.triangles, meshComponent.mat);
}
//Combine a batch into one meshcomponent
MeshComponent Chunk::GetBatch(std::vector<VertexComponent> batch) {
	unsigned int blockIndex = 0;
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
			unsigned int index = batch[i].triangles[k] + blockIndex;
			triangles.push_back(index);
		}
		blockIndex += batch[i].vertices.size();
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
		//Drawing the chunk in the right position sending data to the shader
		mesh.Draw({id.x*16.0f, 0.0f, id.y*16.0f}, color);
	}
}
//Render the water
void Chunk::WaterRender() {
	if (loaded) {
		glEnable(GL_BLEND);
		//Render the water in the right by sending data to the shader
		waterMesh.Draw({ id.x * 16.0f, -0.4f, id.y * 16.0f }, { 1,1,1,0.8f });
		glDisable(GL_BLEND);
	}
}
//Unload the chunk from memory (Dunno why this is not working)
void Chunk::Unload() {
	mesh.Unload();
	waterMesh.Unload();
	//Setting loaded to false to not render in the future
	loaded = false;
}