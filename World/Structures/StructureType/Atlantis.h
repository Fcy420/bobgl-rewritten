#ifndef ATLANTIS_H
#define ATLANTIS_H
#include"../Structure.h"
#include"../../World.h"
#include"../../../objhelpers/vox_loader.h"
// Derive from the structure class
class Atlantis : public Structure {
public:
	// Set the properties of Atlantis and use the Vox loader library to load in Vox files as arrays of block and disable the sdf function
	Atlantis() {
		SetValues(ATLANTIS, 0.0f); 
		isDistanceDependendent = false;
		std::vector<VoxBlock> voxBlocks = { {121, 13}, {122, 12}, {97, 11 } };
		predefinedBlocks = vox_loader::GetVoxModel("Models/atlantis.vox", voxBlocks);
		size = vox_loader::GetVoxModelSize("Models/atlantis.vox");

	}
	// Place all the predefined blocks in a vector that the world can read and align them to the given position
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
		for (auto block : predefinedBlocks) {
			block.globalPos.x += p.x;
			block.globalPos.y += p.y;
			block.globalPos.z += p.z;
			blocks.push_back(block);
		}
		return blocks;
	}
	// Spawn conditions for the structure to spawn
	virtual bool SpawnConditions(World* world, BlockPos triedPosition) {
		return true;
		for (int x = 0; x < size.x; x++) {
			for (int y = 0; y < size.y; y++) {
				for (int z = 0; z < size.z; z++) {
					BlockPos curPos = { triedPosition.x + x, triedPosition.y+y, triedPosition.z + y };
					int blockAt = world->GetBlockAt(curPos);
					if (blockAt > 0 && blockAt != 4) {
						return false;
					}
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