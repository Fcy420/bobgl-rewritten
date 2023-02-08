#ifndef SHIP_H
#define SHIP_H
#include "../Structure.h"
#include"../../World.h"
#include "../../../objhelpers/vox_loader.h"
#include<iostream>
// Derive from structure class
class Ship : public Structure {
public:
	//Load the ship vox model into the game and pair the different color values to blocks (Further explantion in Atlantis file)
	Ship() {
		SetValues(SHIP, 0.0f);
		isDistanceDependendent = false;
		std::vector<VoxBlock> voxBlocks = { {242, 6}, {241, 8}, {248, 9}, {79, 3} };
		predefinedBlocks = vox_loader::GetVoxModel("Models/ship.vox", voxBlocks);
		size = vox_loader::GetVoxModelSize("Models/ship.vox");
	}
	// Align the block position to a main position and a random rotation
	virtual std::vector<ModifyBlock> DefinedFunction(BlockPos p) {
		std::vector<ModifyBlock> blocks;
		/*for (int x = 0; x < 6; x++) {
			for (int y = 0; y < 1; y++) {
				for (int z = 0; z < 12; z++) {
					BlockPos bPos = BlockPos{ p.x + x, p.y + y, p.z + z };
					ModifyBlock block = ModifyBlock{ 6, bPos};
					if ((x == 0 || x == 5) && (z == 11 || z == 0)) {
						block.globalPos.y = y + 1+p.y;
					}
					if ((x != 0 && x != 5) && z == 11) {
						ModifyBlock addedBlock = { 6, BlockPos{bPos.x, bPos.y+1, bPos.z + 1} };
						blocks.push_back(addedBlock);
					}
					blocks.push_back(block);
				}
			}
		}*/
		int rotation = rand() % 3;
		//std::cout << "Ship with rotation: " << rotation << "\n";
		for (auto block : predefinedBlocks) {
			switch (rotation) {
			case 0:
				block.globalPos.z += p.z;
				block.globalPos.y += p.y;
				block.globalPos.x = size.x - block.globalPos.x + p.x;
				break;
			case 1:
				block.globalPos.x += p.x;
				block.globalPos.y += p.y;
				block.globalPos.z = p.z + size.z - block.globalPos.z;
				break;
			default:
				block.globalPos.x += p.x;
				block.globalPos.y += p.y;
				block.globalPos.z += p.z;
				break;
			}
			blocks.push_back(block);
		}
		return blocks;
	}
	virtual bool SpawnConditions(World* world, BlockPos triedPosition) {
		for (int x = 0; x < size.x; x++) {
			for (int y = 0; y < size.z; y++) {
				BlockPos curPos = { triedPosition.x, triedPosition.y, triedPosition.z };
				if (world->GetWorldProcessor()->Process({ curPos.x, curPos.y, curPos.z }) != 4) {
					//std::cout << world->GetProc()->Process({ curPos.x,curPos.y,curPos.z }) << "\n";
					return false;
				}
			}
		}
		return true;
	}

private:
	std::vector<ModifyBlock> predefinedBlocks;
	BlockPos size;
};


#endif