/* date = June 4th 2022 7:25 pm */

#ifndef VMFCS_H
#define VMFCS_H

#define MemoryAllocate(bytes) malloc(bytes)
#define MemoryAllocateZeroed(elements, elementsize) calloc(elements, elementsize)
#define FreeMemory(memory) free(memory)
#define ReallocateMemory(memory, newsize) realloc(memory, newsize)

#define MAP_MAX_SIZE 16384.0f

#define VMFCS_VERSION "0.0.1"

struct ReadFileResult
{
	s64 bytes;
	void *memory;
};

enum CmdArgType
{
	CMDARG_NONE,
	CMDARG_STRING,
	CMDARG_FLOAT,
	
	CMDARGTYPE_COUNT,
};

struct CmdArg
{
	char argName[64];
	char description[1024];
	
	CmdArgType type;
	union
	{
		char stringValue[512];
		f32 floatValue;
	};
	
	b32 isInCmdLine; // whether this exists on the command line
};

union CmdArgs
{
	struct
	{
		CmdArg help;
		CmdArg printCmdLine;
		CmdArg debugExportObj;
		CmdArg input;
		CmdArg output;
		CmdArg outlineWidth;
		CmdArg outlineMaterial;
	};
	CmdArg args[7];
};

static_assert(MEMBER_SIZE(CmdArgs, args) == sizeof(CmdArgs), "CmdArgs size and args array length are mismatched!");

internal void DuplicateMemory(void *source, void *destination, size_t bytes, size_t destSize);
internal void CopyString(char *source, char *dest, size_t destSize);
internal void CopyString(char *source, size_t copyCount, char *dest, size_t destSize);
internal b32 StringEquals(char *str1, char *str2);
internal b32 StringToU32(char *str, u32 *out);
internal b32 StringToS32(char *str, s32 *out);
internal b32 StringToF32(char *str, f32 *out);
internal ReadFileResult ReadEntireFile(char *filePath);
internal s32 ScanStringFormat(char *string, char *format, ...);
internal s32 Format(char *buffer, size_t maxlen, char *format, ...);

global char *g_cmdArgTypeStrings[CMDARGTYPE_COUNT] = {
	"None",
	"String",
	"Number",
};

local_persist char *emptyVmf = R"(
versioninfo
{
	"editorversion" "400"
	"editorbuild" "8860"
	"mapversion" "0"
	"formatversion" "100"
	"prefab" "0"
}
viewsettings
{
	"bSnapToGrid" "1"
	"bShowGrid" "1"
	"bShowLogicalGrid" "0"
	"nGridSpacing" "64"
}
world
{
	"id" "1"
	"mapversion" "0"
	"classname" "worldspawn"
	"detailmaterial" "detail/detailsprites"
	"detailvbsp" "detail.vbsp"
	"maxpropscreenwidth" "-1"
	"skyname" "sky_dust"
}
cameras
{
	"activecamera" "-1"
}
cordons
{
	"active" "0"
}
)";

#endif //VMFCS_H
