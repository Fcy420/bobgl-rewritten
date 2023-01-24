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
		float v = noise.GetNoise(pos.x, pos.z);
		float fv = FalloffValue(pos, 1200.0f);
		v -= fv;
		if (v < -1.0f) {
			v = -1.0f;
		}
		if (v > 1.0f) {
			v = 1.0f;
		}
		v *= 16;
		float finalV = pos.y - v;
		int bType = 0;
		if (finalV < iso) {
			bType = 2;
			/*int bTypeAbove = Noise2D(glm::vec3(pos.x, pos.y + 1, pos.z));
			if (bTypeAbove == 0) {
				bType = 1;
			}
			else if (bTypeAbove == 4) {
				bType = 5;
			}*/
		}
		if (pos.y < water && bType == 0) {
			bType = 4;
		}
		if (GetBiomeAmplifiers(pos.x, pos.z) >= 0.8f && GetBiomeAmplifiers(pos.x, pos.z) < 1.0f) {
			if (bType > 0 && bType != 4) bType = 5;
		}
		if (pos.y == 0) bType = 3;
		return bType;
	}
	int Noise3D(glm::vec3 pos) {
		float v = noise.GetNoise(pos.x, pos.y, pos.z);
		float d = pos.y / 255;
		int blockType = 0;
		if (v > 0) {
			blockType = 1;
		}
		return blockType;
	}
	void SetSeed(long seed) {
		noise.SetSeed(seed);
	}
	float GetBiomeAmplifiers(float x, float z) {
		return biomeNoise.GetNoise(x, z);
	}
	int iso = 80;
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