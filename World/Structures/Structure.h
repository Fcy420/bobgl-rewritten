#ifndef STRUCTURE_H
#define STRUCTURE_H
#include "../World.h"
typedef enum {
	BOULDER,
	SHIP,
	ATLANTIS
} StructureType;
// Base structure class
class Structure {
public:
	// SDF function checks if the given position is within the distance to the structure position
	virtual float DistanceFunction(BlockPos p, BlockPos spawnedPosition) { return 0.0f; }
	// Defined position which if the structure has a model will just align the blocks to the given position
	virtual std::vector<ModifyBlock> DefinedFunction(BlockPos p) { return {}; }
	// Returns true if the conditions are met with the structure
	virtual bool SpawnConditions(World* world, BlockPos triedPosition) { return false; }
	// checks if the structure is a sdf model
	bool IsDist() {
		return isDistanceDependendent;
	}
	StructureType GetType() {
		return structType;
	}
	float GetSize() {
		return size;
	}
protected:
	float size = 0.0f;
	void SetValues(StructureType type, float size) {
		structType = type;
		this->size = size;
	}
	StructureType structType;
	bool isDistanceDependendent = true;
};
#endif