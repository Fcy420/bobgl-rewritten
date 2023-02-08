#ifndef WORLDPROC_H
#define WORLDPROC_H
#include"../libs/FastNoiseLite.h"
#include"BlockBuilder.h"
#include<algorithm>

class WorldProcessor {
public:
	WorldProcessor() {
		noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
		noise.SetFrequency(0.005f);
		noise.SetFractalLacunarity(4.0f);
		noise.SetFractalGain(0.02f);
		noise.SetFractalOctaves(4);
		noise.SetFractalType(FastNoiseLite::FractalType_FBm);
		biomeNoise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
		biomeNoise.SetFrequency(0.001f);
		biomeNoise.SetFractalLacunarity(1.0f);
		biomeNoise.SetFractalGain(0.01f);
		biomeNoise.SetFractalOctaves(1);
		biomeNoise.SetFractalType(FastNoiseLite::FractalType_FBm);
	}
	// Get the terrain block at a position 
	int Process(glm::vec3 pos) {
		return Noise2D(pos);
	}

	int Noise2D(glm::vec3 pos) {
		float noiseValue = noise.GetNoise(pos.x, pos.z);
		float falloffValue = FalloffValue(pos, 1200.0f);
		noiseValue -= falloffValue;
		if (noiseValue < -1.0f) {
			noiseValue = -1.0f;
		}
		if (noiseValue > 1.0f) {
			noiseValue = 1.0f;
		}
		noiseValue *= 64;
		float finalNoiseValue = pos.y - noiseValue;
		int blockType = 0;
		if (finalNoiseValue < isoSurface) {
			blockType = 2;
		}
		if (pos.y < water && blockType == 0) {
			blockType = 4;
		}
		if (GetBiomeAmplifiers(pos.x, pos.z) >= 0.8f && GetBiomeAmplifiers(pos.x, pos.z) < 1.0f) {
			if (blockType > 0 && blockType != 4) blockType = 5;
		}
		if (pos.y == 0) blockType = 3;
		return blockType;
	}
	void SetSeed(long seed) {
		noise.SetSeed(seed);
	}
	float GetBiomeAmplifiers(float x, float z) {
		return biomeNoise.GetNoise(x, z);
	}
	int isoSurface = 80;
	int water = 80;
private:
	float FalloffValue(glm::vec3 pos, float size) {
		float x = pos.x / size * 2;
		float y = pos.z / size * 2;
		return std::max(std::abs(x), std::abs(y));
	}
	FastNoiseLite noise;
	FastNoiseLite biomeNoise;
};

#endif