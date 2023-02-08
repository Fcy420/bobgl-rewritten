#ifndef WORLDPROC_H
#define WORLDPROC_H
#include"../libs/FastNoiseLite.h"
#include"BlockBuilder.h"
#include<algorithm>

class WorldProcessor {
public:
	//Initialize all settings of the noise 
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
	// Process a position given with noise and get the terrain block at point (Reason not used while checking direction is that the block could've been modified from the original state
	int Noise2D(glm::vec3 pos) {
		float noiseValue = noise.GetNoise(pos.x, pos.z);
		float falloffValue = FalloffValue(pos, 1200.0f);
		// Subtract the falloff value to create "islands"
		noiseValue -= falloffValue;
		//Clamp value between -1 and 1
		if (noiseValue < -1.0f) {
			noiseValue = -1.0f;
		}
		if (noiseValue > 1.0f) {
			noiseValue = 1.0f;
		}
		//Amplify the noise so its more visible
		noiseValue *= 64;
		// Make it "3D"
		float finalNoiseValue = pos.y - noiseValue;
		int blockType = 0;
		//Check conditions for different types of terrain blockss
		if (finalNoiseValue < isoSurface) {
			blockType = 2;
		}
		if (pos.y < water && blockType == 0) {
			blockType = 4;
		}
		// Apply the biome filters last so it doesn't get modified by height
		if (GetBiomeAmplifiers(pos.x, pos.z) >= 0.8f && GetBiomeAmplifiers(pos.x, pos.z) < 1.0f) {
			if (blockType > 0 && blockType != 4) blockType = 5;
		}
		//Set bottom of the world to stone
		if (pos.y == 0) blockType = 3;
		return blockType;
	}
	void SetSeed(long seed) {
		noise.SetSeed(seed);
	}
	//Return a noise value to a biome
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