#ifndef STRUCTUREGENERATOR_H
#define STRUCTUREGENERATOR_H
#include "../World.h"
#include "Structure.h"
#include<vector>
#include<algorithm>
#include<memory>
#include"StructureType/Boulder.h"
#include"StructureType/Ship.h"
#include"StructureType/Atlantis.h"
//StructureGenerator class
class StructureGenerator {
public:
	void Initialize(World* world) {
		this->world = world;
	}
	//Eval a position to a type
	bool EvaluatePosition(BlockPos position, StructureType type) {
		return SpawnStructure(type, position);
	}
	// Add structure type to the available spawn types
	void AddStructureType(StructureType structure) {
		if (structure == StructureType::BOULDER) {
			Boulder* boulder = new Boulder();
			structures.emplace_back(boulder);
		}
		if (structure == StructureType::SHIP) {
			Ship* ship = new Ship();
			structures.emplace_back(ship);
		}
		if (structure == StructureType::ATLANTIS) {
			Atlantis* atlantis = new Atlantis();
			structures.emplace_back(atlantis);
		}
	}
private:
	float Distance(BlockPos a, BlockPos b) {
		return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y) * (a.z - b.z) * (a.z - b.z));
	}
	//Spawn a structure at position
	bool SpawnStructure(StructureType type, BlockPos position) {
		// Blocks to modify
		std::vector<ModifyBlock> blocks;
		bool success = false;
		//loops through the available the structures
		for (int i = 0; i < structures.size(); i++) {
			if (structures[i]->GetType() == type) {
				float size = structures[i]->GetSize();
				if (structures[i]->SpawnConditions(world, position) && Distance(position, latestSpawn) > 0.0f) {
					if (structures[i]->IsDist()) {
						for (int x = -size; x < size; x++) {
							for (int y = -size; y < size; y++) {
								for (int z = -size; z < size; z++) {
									BlockPos pos = { x + position.x,y + position.y,z + position.z };
									if (structures[i]->DistanceFunction(pos, position)) {
										ModifyBlock block = ModifyBlock{ 2, pos };
										blocks.push_back(block);
									}
								}
							}
						}
					}
					else {
						blocks = structures[i]->DefinedFunction(position);
					}
					success = true;
				}
				break;
			}
		}
		// If the position was successful modify the blocks
		if (success) {
			world->Modify(blocks);
			spawnCount++;
			latestSpawn = position;
		}
		else {
			//std::cout << "Couldn't spawn structure!\n";
		}
		return success;
	}
	BlockPos latestSpawn;
	int spawnCount = 0;
	World* world;
	std::vector<std::unique_ptr<Structure>> structures;
};

#endif