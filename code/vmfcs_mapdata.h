/* date = November 23rd 2020 8:04 pm */

#ifndef VMFCS_MAPDATA_H
#define VMFCS_MAPDATA_H

#include "vmfcs_keyvalues.h"

// TODO: what is the actual max?
#define VMF_MAX_MATERIAL_LEN 256

struct TextureTransform
{
	v3 normal;
	f32 offset;
	f32 scale;
};

struct BrushSide
{
	u32 id;
	
	// plane info
	v3 plane[3];
	v3 normal;
	f32 distance;
	
	TextureTransform uaxis;
	TextureTransform vaxis;
	
	f32 rotation;
	u32 lightmapScale;
	u32 smoothingGroups;
	
	char material[VMF_MAX_MATERIAL_LEN];
};

struct Brush
{
	u32 id;
	BrushSide *arrSides; // stb_ds dynamic array
	KeyValues unknownKV;
};

internal v3 **GenerateBrushPolygons(Brush *brush);
internal b32 ParseVMFSolid(KeyValues *kv, Brush *outBrush);
internal b32 BrushToKeyValues(Brush *brush, KeyValues *out);

#endif //VMFCS_MAPDATA_H