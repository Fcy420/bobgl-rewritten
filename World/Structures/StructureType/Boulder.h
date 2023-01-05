#ifndef BOULDER_H
#define BOULDER_H
#include"../Structure.h"
#include"../../World.h"
class Boulder : public Structure {
public:
	Boulder() {
		SetValues(BOULDER, 5.0f);
	}
	virtual float DistanceFunction(BlockPos p, BlockPos spawnedPosition) {
		return blength(p, spawnedPosition) < size;
	}
};
#endif