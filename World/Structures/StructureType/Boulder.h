#ifndef BOULDER_H
#define BOULDER_H
#include"../Structure.h"
#include"../../World.h"
// NOT USED
class Boulder : public Structure {
public:
	Boulder() {
		SetValues(BOULDER, 5.0f);
	}
	virtual float DistanceFunction(BlockPos p, BlockPos spawnedPosition) {
		return blockDistance(p, spawnedPosition) < size;
	}
};
#endif