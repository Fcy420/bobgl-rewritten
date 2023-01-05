#ifndef VOX_LOADER
#define VOX_LOADER
#pragma once
//#define OGT_VOX_IMPLEMENTATION
#include "ogt_vox.h"
#include"../World/World.h"
#include<vector>
typedef struct {
    int index;
    int blockType;
} VoxBlock;
class vox_loader {
public:
	static std::vector<ModifyBlock> GetVoxModel(const char* filePath, std::vector<VoxBlock> predefinedBlocks) {
        const ogt_vox_scene* vox_scene = load_vox_scene(filePath, 0);
        std::vector<ModifyBlock> blocks;
        const ogt_vox_model* model = vox_scene->models[0];
        int voxel_index = 0;
        for (int x = 0; x < model->size_x; x++) {
            for (int y = 0; y < model->size_y; y++) {
                for (int z = 0; z < model->size_z; z++) {
                    uint8_t colorIndex = model->voxel_data[voxel_index];
                    if (colorIndex != 0) {
                        int blockType = 3;
                        for (auto type : predefinedBlocks) {
                            if (type.index == colorIndex) {
                                blockType = type.blockType;
                                break;
                            }
                        }
                        BlockPos pos = { y, x, z};
                        ModifyBlock b = { blockType, pos };
                        blocks.push_back(b);
                    }
                    voxel_index++;
                }
            }
        }
        ogt_vox_destroy_scene(vox_scene);
        return blocks;
	}
    static BlockPos GetVoxModelSize(const char* filePath) {
        const ogt_vox_scene* vox_scene = load_vox_scene(filePath, 0);
        BlockPos pos;
        pos.x = vox_scene->models[0]->size_x;
        pos.y = vox_scene->models[0]->size_y;
        pos.z = vox_scene->models[0]->size_z;
        ogt_vox_destroy_scene(vox_scene);
        return pos;
    }

private:
   static const ogt_vox_scene* load_vox_scene(const char* filename, uint32_t scene_read_flags = 0)
    {
        // open the file
#if defined(_MSC_VER) && _MSC_VER >= 1400
        FILE* fp;
        if (0 != fopen_s(&fp, filename, "rb"))
            fp = 0;
#else
        FILE* fp = fopen(filename, "rb");
#endif
        if (!fp)
            return NULL;

        // get the buffer size which matches the size of the file
        fseek(fp, 0, SEEK_END);
        uint32_t buffer_size = ftell(fp);
        fseek(fp, 0, SEEK_SET);

        // load the file into a memory buffer
        uint8_t* buffer = new uint8_t[buffer_size];
        fread(buffer, buffer_size, 1, fp);
        fclose(fp);

        // construct the scene from the buffer
        const ogt_vox_scene* scene = ogt_vox_read_scene_with_flags(buffer, buffer_size, scene_read_flags);

        // the buffer can be safely deleted once the scene is instantiated.
        delete[] buffer;

        return scene;
    }
};




#endif