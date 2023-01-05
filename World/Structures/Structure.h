#ifndef STRUCTURE_H
#define STRUCTURE_H
#include "../World.h"
typedef enum {
	BOULDER,
	SHIP,
	ATLANTIS
} StructureType;
class Structure {
public:
	virtual float DistanceFunction(BlockPos p, BlockPos spawnedPosition) { return 0.0f; }
	virtual std::vector<ModifyBlock> DefinedFunction(BlockPos p) { return {}; }
	virtual bool SpawnConditions(World* world, BlockPos triedPosition) { return false; }
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