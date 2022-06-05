/* date = June 4th 2022 7:25 pm */

#ifndef VMFCS_H
#define VMFCS_H

#define MemoryAllocate(bytes) malloc(bytes)
#define MemoryAllocateZeroed(elements, elementsize) calloc(elements, elementsize)
#define FreeMemory(memory) free(memory)
#define ReallocateMemory(memory, newsize) realloc(memory, newsize)

#define MAP_MAX_SIZE 16384.0f

struct ReadFileResult
{
	s64 bytes;
	void *memory;
};

typedef struct Mesh {
    s32 vertexCount;        // Number of vertices stored in arrays
    s32 triangleCount;      // Number of triangles stored (indexed or not)
	
    // Default vertex data
    float *vertices;        // Vertex position (XYZ - 3 components per vertex) (shader-location = 0)
    float *texcoords;       // Vertex texture coordinates (UV - 2 components per vertex) (shader-location = 1)
    float *texcoords2;      // Vertex second texture coordinates (useful for lightmaps) (shader-location = 5)
    float *normals;         // Vertex normals (XYZ - 3 components per vertex) (shader-location = 2)
    float *tangents;        // Vertex tangents (XYZW - 4 components per vertex) (shader-location = 4)
	u8 *colors;  // Vertex colors (RGBA - 4 components per vertex) (shader-location = 3)
	u16 *indices;// Vertex indices (in case vertex data comes indexed)
	
    // Animation vertex data
    float *animVertices;    // Animated vertex positions (after bones transformations)
    float *animNormals;     // Animated normals (after bones transformations)
    s32 *boneIds;           // Vertex bone ids, up to 4 bones influence by vertex (skinning)
    float *boneWeights;     // Vertex bone weight, up to 4 bones influence by vertex (skinning)
	
    // OpenGL identifiers
    u32 vaoId;     // OpenGL Vertex Array Object id
	u32 *vboId;    // OpenGL Vertex Buffer Objects id (default vertex data)
} Mesh;

internal void DuplicateMemory(void *source, void *destination, size_t bytes, size_t destSize);
internal void CopyString(char *source, char *dest, size_t destSize);
internal void CopyString(char *source, size_t copyCount, char *dest, size_t destSize);
internal b32 StringEquals(char *str1, char *str2);
internal b32 StringToU32(char *str, u32 *out);
internal b32 StringToS32(char *str, s32 *out);
internal f32 StringToF32(char *str);
internal ReadFileResult ReadEntireFile(char *filePath);
internal s32 ScanStringFormat(char *string, char *format, ...);
internal s32 Format(char *buffer, size_t maxlen, char *format, ...);

#endif //VMFCS_H
