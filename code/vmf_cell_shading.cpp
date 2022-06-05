
#include "gc_common.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#define STB_DS_IMPLEMENTATION
#pragma warning(push)
#pragma warning(disable: 4456)
#include "stb/stb_ds.h"
#pragma warning(pop)

#include "vmfcs.h"

#include "vmfcs_maths.h"
#include "vmfcs_string.h"

#include "vmfcs_keyvalues.h"
#include "vmfcs_mapdata.h"

#include "vmfcs_keyvalues.cpp"
#include "vmfcs_mapdata.cpp"

internal void DuplicateMemory(void *source, void *destination, size_t bytes, size_t destSize)
{
	memcpy(destination, source, (bytes < destSize) ? bytes : destSize);
}

internal void CopyString(char *source, char *dest, size_t destSize)
{
	strncpy_s(dest, destSize, source, _TRUNCATE);
	//ASSERT(dest[destSize - 1] == '\0');
	dest[destSize - 1] = '\0';
}

internal void CopyString(char *source, size_t copyCount, char *dest, size_t destSize)
{
	strncpy_s(dest, destSize, source, copyCount);
	//ASSERT(dest[destSize - 1] == '\0');
	dest[destSize - 1] = '\0';
}

internal ReadFileResult ReadEntireFile(char *filePath)
{
	ReadFileResult result = {};
	
	FILE *file = fopen(filePath, "rb");
	if (file)
	{
		fseek(file, 0, SEEK_END);
		result.bytes = ftell(file);
		fseek(file, 0, SEEK_SET);
		
		result.memory = MemoryAllocate(result.bytes);
		fread(result.memory, 1, result.bytes, file);
		fclose(file);
	}
	
	return result;
}

internal b32 WriteEntireFile(void *data, s64 bytes, char *path)
{
	b32 result = false;
	
	FILE *file = fopen(path, "wb");
	if (file)
	{
		s64 elementsWritten = fwrite(data, 1, bytes, file);
		if (elementsWritten == bytes)
		{
			result = true;
		}
		fclose(file);
	}
	
	return result;
}

internal b32 StringEquals(char *str1, char *str2)
{
	b32 result = strcmp(str1, str2) == 0;
	return result;
}

// out gets set to 0 if it doesn't succeed
internal b32 StringToU32(char *str, u32 *out)
{
	b32 result = false;
	if (str)
	{
		b32 inNumber = false;
		b32 searching = true;
		*out = 0;
		char *start = str;
		while (searching)
		{
			if (start[0] >= '0' && start[0] <= '9')
			{
				inNumber = true;
			}
			else if (inNumber)
			{
				start = start - 1;
				break;
			}
			else if (start[0] == '\0')
			{
				result = false;
				start = 0;
				break;
			}
			++start;
		}
		
		u32 power = 1;
		if (start)
		{
			for (char *ch = start; ch >= str; ch--)
			{
				// TODO: check for overflow?
				if (ch[0] >= '0' && ch[0] <= '9')
				{
					*out += (s32)(ch[0] - '0') * power;
					power *= 10;
					result = true;
				}
				else
				{
					break;
				}
			}
		}
	}
	
	return result;
}

// out gets set to 0 if it doesn't succeed
internal b32 StringToS32(char *str, s32 *out)
{
	b32 result = false;
	if (str)
	{
		b32 inNumber = false;
		b32 searching = true;
		*out = 0;
		char *start = str;
		while (searching)
		{
			if (start[0] >= '0' && start[0] <= '9')
			{
				inNumber = true;
			}
			else if (inNumber)
			{
				start = start - 1;
				break;
			}
			else if (start[0] == '\0')
			{
				result = false;
				start = 0;
				break;
			}
			++start;
		}
		
		u32 power = 1;
		if (start)
		{
			for (char *ch = start; ch >= str; ch--)
			{
				// TODO: check for overflow?
				if (ch[0] >= '0' && ch[0] <= '9')
				{
					*out += (s32)(ch[0] - '0') * power;
					power *= 10;
					result = true;
				}
				else
				{
					break;
				}
			}
		}
	}
	
	return result;
}

internal b32 StringToF32(char *str, f32 *out)
{
	b32 result = false;
	
	s64 len = strlen(str);
	char *strEnd = str + len + 1;
	f32 number = strtof(str, &strEnd);
	
	result = strEnd != str;
	if (result)
	{
		*out = number;
	}
	return result;
}

internal s32 ScanStringFormat(char *string, char *format, ...)
{
	va_list args;
	va_start(args, format);
	s32 result = vsscanf_s(string, format, args);
	va_end(args);
	return result;
}

internal s32 Format(char *buffer, size_t maxlen, char *format, ...)
{
	va_list args;
	va_start(args, format);
	s32 result = _vsnprintf_s(buffer, maxlen, _TRUNCATE, format, args);
	va_end(args);
	return result;
}

internal b32 ParseCmdArgs(CmdArgs *cmdArgs, s32 argCount, char *arguments[])
{
	b32 result = true;
	for (s32 i = 1; i < argCount; i++)
	{
		b32 found = false;
		for (s32 j = 0; j < ARRAY_LENGTH(cmdArgs->args); j++)
		{
			if (i >= argCount)
			{
				break;
			}
			
			if (StringEquals(arguments[i], cmdArgs->args[j].argName))
			{
				found = true;
				if (cmdArgs->args[j].isInCmdLine)
				{
					printf("ERROR: %s is used twice in the command line!\n", arguments[i]);
					result = false;
					break;
				}
				
				cmdArgs->args[j].isInCmdLine = true;
				if (cmdArgs->args[j].type != CMDARG_NONE)
				{
					if (i + 1 < argCount)
					{
						if (arguments[i + 1][0] != '-')
						{
							if (cmdArgs->args[j].type == CMDARG_STRING)
							{
								s64 argLen = strlen(arguments[i + 1]);
								if (argLen >= sizeof(cmdArgs->args[j].stringValue))
								{
									printf("ERROR: String is too long for argument %s! Maximum length is %lli characters.\n\n",
										   arguments[i], (s64)sizeof(MEMBER(CmdArg, stringValue)) - 1);
									result = false;
									break;
								}
								
								memcpy(cmdArgs->args[j].stringValue, arguments[i + 1], argLen);
								cmdArgs->args[j].stringValue[argLen] = '\0';
							}
							else if (cmdArgs->args[j].type == CMDARG_FLOAT)
							{
								if (!StringToF32(arguments[i + 1], &cmdArgs->args[j].floatValue))
								{
									printf("ERROR: Couldn't convert command \"%s\"'s argument \"%s\" to a number!\n\n",
										   arguments[i], arguments[i + 1]);
									result = false;
									break;
								}
							}
							else
							{
								printf("ERROR: Dumbass programmer configured the command line options incorrectly. Shame on them!\n\n");
							}
							
							i++;
						}
						else
						{
							// oh no sad sad :(
							printf("ERROR: Argument missing for command %s\n\n", arguments[i]);
							result = false;
							break;
						}
					}
					else
					{
						// oh no sad sad :(
						printf("ERROR: Argument missing for command %s\n\n", arguments[i]);
						result = false;
						break;
					}
				}
				continue;
			}
			
			if (found)
			{
				break;
			}
		}
		
		if (!found)
		{
			printf("Invalid command \"%s\"\n\n", arguments[i]);
			result = false;
			break;
		}
		
		if (!result)
		{
			break;
		}
	}
	
	return result;
}

internal void PrintCmdLineHelp(CmdArgs *cmdArgs)
{
	//printf("\n");
	printf("Available commands:\n");
	for (s32 i = 0; i < ARRAY_LENGTH(cmdArgs->args); i++)
	{
		if (cmdArgs->args[i].type == CMDARG_NONE)
		{
			printf("%s: %s \n",
				   cmdArgs->args[i].argName,
				   cmdArgs->args[i].description);
		}
		else
		{
			printf("%s [%s] : %s \n",
				   cmdArgs->args[i].argName,
				   g_cmdArgTypeStrings[cmdArgs->args[i].type],
				   cmdArgs->args[i].description);
		}
	}
	printf("\n");
}

internal void PrintCmdLine(s32 argCount, char *arguments[])
{
	printf("Command line: \"");
	for (s32 cmd = 1; cmd < argCount; cmd++)
	{
		printf("%s ", arguments[cmd]);
	}
	printf("\"\n\n");
}

int main(s32 argCount, char *arguments[])
{
	CmdArgs cmdArgs = {};
	cmdArgs.help = {"-help", "Help!!!", CMDARG_NONE};
	cmdArgs.debugExportObj = {"-debugexportobj", "Export an obj file of brush faces for debugging.", CMDARG_STRING};
	cmdArgs.input = {"-input", "Input vmf file to be used for generating cell shading.", CMDARG_STRING};
	cmdArgs.output = {"-output", "Output instance vmf file of cell shading brushes.", CMDARG_STRING};
	cmdArgs.outlinewidth = {"-outlinewidth", "Cell shading outline width in hammer units. It's 2.0 by default.", CMDARG_FLOAT};
	cmdArgs.outlinewidth.floatValue = 2;
	
	
	printf("Running VMF Cell Shading V%s\n\n", VMFCS_VERSION);
	
	if (!ParseCmdArgs(&cmdArgs, argCount, arguments))
	{
		printf("Command line parsing failed!\n\n");
		PrintCmdLine(argCount, arguments);
		PrintCmdLineHelp(&cmdArgs);
		goto cleanup;
	}
	
	PrintCmdLine(argCount, arguments);
	
	if (!cmdArgs.input.isInCmdLine)
	{
		printf("ERROR: Please provide an input vmf file with -input.\n\n");
		PrintCmdLineHelp(&cmdArgs);
		goto cleanup;
	}
	
	if (!cmdArgs.output.isInCmdLine)
	{
		printf("ERROR: Please provide an output path with -output.\n\n");
		PrintCmdLineHelp(&cmdArgs);
		goto cleanup;
	}
	
	if (cmdArgs.help.isInCmdLine)
	{
		PrintCmdLineHelp(&cmdArgs);
	}
	
	
	KeyValues kv = {};
	if (!ImportKeyValues(&kv, cmdArgs.input.stringValue))
	{
		printf("ERROR: Couldn't import vmf file from path \"%s\".\n\n", cmdArgs.input.stringValue);
		goto cleanup;
	}
	
	printf("Generating cell shading for \"%s\" with outline width %.1f.\n\n", cmdArgs.input.stringValue, cmdArgs.outlinewidth.floatValue);
	
	s32 cellshadeVisgroupId = -1;
	s32 maxVisgroupId = -1;
	KeyValues *visgroups = KeyValuesGetChild(&kv, "visgroups");
	{
		if (visgroups)
		{
			KeyValues *visgroup = NULL;
			KeyValuesResetIteration(visgroups);
			while (KeyValuesGetNextChild(visgroups, &visgroup, "visgroup"))
			{
				KeyValues *nameKv = KeyValuesGetChild(visgroup, "name");
				KeyValues *visgroupIdKv = KeyValuesGetChild(visgroup, "visgroupid");
				s32 visgroupId = -1;
				if (nameKv && visgroupIdKv)
				{
					if (StringToS32(visgroupIdKv->value, &visgroupId))
					{
						if (StringEquals(nameKv->value, "Cellshade"))
						{
							cellshadeVisgroupId = visgroupId;
						}
						maxVisgroupId = MAX(maxVisgroupId, visgroupId);
					}
				}
			}
		}
	}
	
	if (cellshadeVisgroupId < 0 || maxVisgroupId < 0)
	{
		printf("ERROR: Couldn't find the \"Cellshade\" visgroup. To mark brush entities for cell shading, you have to create a visgroup named \"Cellshade\" and put the entities in it.\n\n");
		getchar();
		goto cleanup;
	}
	
	// NOTE(GameChaos): find biggest id to avoid conflicts
	s32 biggestId = -1;
	{
		KeyValues *entParams = NULL;
		KeyValuesResetIteration(&kv);
		while (KeyValuesGetNextChild(&kv, &entParams, NULL))
		{
			if (StringEquals(entParams->key, "entity") || StringEquals(entParams->key, "world"))
			{
				KeyValues *idKv = KeyValuesGetChild(entParams, "id");
				if (idKv)
				{
					s32 id = -1;
					StringToS32(idKv->value, &id);
					biggestId = MAX(biggestId, id);
					
					KeyValues *entChild = NULL;
					while (KeyValuesGetNextChild(entParams, &entChild, "solid"))
					{
						idKv = KeyValuesGetChild(entChild, "id");
						if (idKv)
						{
							id = -1;
							StringToS32(idKv->value, &id);
							biggestId = MAX(biggestId, id);
							
							KeyValues *sideParams = NULL;
							while (KeyValuesGetNextChild(entChild, &sideParams, "side"))
							{
								idKv = KeyValuesGetChild(sideParams, "id");
								if (idKv)
								{
									id = -1;
									StringToS32(idKv->value, &id);
									biggestId = MAX(biggestId, id);
								}
							}
						}
					}
				}
			}
		}
	}
	
	// NOTE(GameChaos): generate cell shading brushes
	Brush **arrNewBrushes = NULL;
	{
		KeyValues *entity = NULL;
		KeyValuesResetIteration(&kv);
		while (KeyValuesGetNextChild(&kv, &entity, "entity"))
		{
			KeyValues *editorSettings = KeyValuesGetChild(entity, "editor");
			s32 visgroupId = -1;
			if (editorSettings)
			{
				KeyValues *visgroupIdKv = KeyValuesGetChild(editorSettings, "visgroupid");
				if (visgroupIdKv)
				{
					if (!StringToS32(visgroupIdKv->value, &visgroupId))
					{
						visgroupId = -1;
					}
				}
			}
			
			if (visgroupId < 0 || visgroupId != cellshadeVisgroupId)
			{
				continue;
			}
			
			KeyValuesResetIteration(entity);
			KeyValues *solid = NULL;
			Brush *brushes = NULL;
			while (KeyValuesGetNextChild(entity, &solid, "solid"))
			{
				// NOTE(GameChaos): entity -> solid
				Brush brush = {};
				ParseVMFSolid(solid, &brush);
				
				Brush baseBrush = {};
				// NOTE(GameChaos): we don't store these in the vmf, so we don't need to set the id.
				for (s32 side = 0; side < arrlen(brush.sides); side++)
				{
					BrushSide brushSide = brush.sides[side];
					// NOTE(GameChaos): inflate brush by x units
					if (!StringEquals(brush.sides[side].material, "TOOLS/TOOLSNODRAW"))
					{
						brushSide.distance += cmdArgs.outlinewidth.floatValue;
					}
					arrput(baseBrush.sides, brushSide);
				}
				
				v3 **polygons = GenerateBrushPolygons(&baseBrush);
				for (s32 poly = 0; poly < arrlen(polygons); poly++)
				{
					if (StringEquals(baseBrush.sides[poly].material, "TOOLS/TOOLSNODRAW"))
					{
						continue;
					}
					
					Brush newBrush = {};
					newBrush.id = ++biggestId;
					
					BrushSide firstSide = baseBrush.sides[poly];
					{
						firstSide.id = ++biggestId;
						
						firstSide.plane[0] = polygons[poly][0];
						firstSide.plane[1] = polygons[poly][1];
						firstSide.plane[2] = polygons[poly][2];
						
						CopyString("TOOLS/TOOLSBLACK", firstSide.material, sizeof(firstSide.material));
						arrput(newBrush.sides, firstSide);
					}
					
					v3 avgPoint = {};
					for (s32 vert = 0; vert < arrlen(polygons[poly]); vert++)
					{
						avgPoint += polygons[poly][vert];
					}
					avgPoint *= 1.0f / (f32)arrlen(polygons[poly]);
					avgPoint += firstSide.normal * 10;
					
					// construct the other sides
					for (s32 vert = 0; vert < arrlen(polygons[poly]); vert++)
					{
						s32 nextVert = vert + 1;
						if (nextVert >= arrlen(polygons[poly]))
						{
							nextVert -= (s32)arrlen(polygons[poly]);
						}
						
						v3 vert1 = polygons[poly][vert];
						v3 vert2 = polygons[poly][nextVert];
						
						BrushSide newSide = baseBrush.sides[poly];
						newSide.id = ++biggestId;
						
						newSide.plane[0] = avgPoint;
						newSide.plane[1] = vert2;
						newSide.plane[2] = vert1;
						
						CopyString("TOOLS/TOOLSINVISIBLE", newSide.material, sizeof(newSide.material));
						arrput(newBrush.sides, newSide);
					}
					arrput(brushes, newBrush);
				}
			}
			arrput(arrNewBrushes, brushes);
		}
	}
	KeyValues newEntitiesKv = {};
	for (s32 i = 0; i < arrlen(arrNewBrushes); i++)
	{
		KeyValues entity = {"entity"};
		
		char id[32];
		Format(id, sizeof(id), "%i", ++biggestId);
		KeyValuesAddChild(&entity, "id", id);
		
		KeyValuesAddChild(&entity, "classname", "func_illusionary");
		KeyValuesAddChild(&entity, "disableflashlight", "1");
		KeyValuesAddChild(&entity, "disablereceiveshadows", "1");
		KeyValuesAddChild(&entity, "disableshadowdepth", "1");
		KeyValuesAddChild(&entity, "disableshadows", "1");
		KeyValuesAddChild(&entity, "fademaxdist", "0");
		KeyValuesAddChild(&entity, "fademindist", "-1");
		KeyValuesAddChild(&entity, "fadescale", "1");
		
		// TODO:
		KeyValuesAddChild(&entity, "origin", "0 0 0");
		
		KeyValuesAddChild(&entity, "renderamt", "255");
		KeyValuesAddChild(&entity, "rendercolor", "255 255 255");
		KeyValuesAddChild(&entity, "renderfx", "0");
		KeyValuesAddChild(&entity, "rendermode", "0");
		KeyValuesAddChild(&entity, "shadowdepthnocache", "0");
		
		for (s32 brush = 0; brush < arrlen(arrNewBrushes[i]); brush++)
		{
			KeyValues brushKv = {};
			BrushToKeyValues(&arrNewBrushes[i][brush], &brushKv, &biggestId);
			//PrintKeyValues(&brushKv);
			KeyValuesAppend(&entity, brushKv);
			KeyValuesFree(&brushKv);
		}
		
		KeyValuesAppend(&newEntitiesKv, entity);
		KeyValuesFree(&entity);
	}
	
	// NOTE(GameChaos): make an obj file for debugging
	if (cmdArgs.debugExportObj.isInCmdLine)
	{
		KeyValues *entity = NULL;
		String *string = NULL;
		s64 vertCount = 0;
		KeyValuesResetIteration(&kv);
		while (KeyValuesGetNextChild(&kv, &entity, "entity"))
		{
			KeyValues *solid = NULL;
			while (KeyValuesGetNextChild(entity, &solid, "solid"))
			{
				Brush brush = {};
				ParseVMFSolid(solid, &brush);
				for (s32 side = 0; side < arrlen(brush.sides); side++)
				{
					for (s32 i = 0; i < 3; i++)
					{
						char line[512];
						Format(line, sizeof(line), "v %f %f %f\n",
							   brush.sides[side].plane[i].x, brush.sides[side].plane[i].y, brush.sides[side].plane[i].z);
						vertCount++;
						STRCONCATENATE(string, line);
					}
					char face[128];
					Format(face, sizeof(face), "f %i %i %i\n", vertCount - 2, vertCount - 1, vertCount);
					STRCONCATENATE(string, face);
				}
			}
		}
		if (!WriteEntireFile(string, STRLEN(string), cmdArgs.debugExportObj.stringValue))
		{
			printf("ERROR: Failed to export debug obj to \"%s\".\n\n", cmdArgs.debugExportObj.stringValue);
			goto cleanup;
		}
	}
	
	char *editedOriginalVmf = KeyValuesToString(&kv);
	
	String *newVMF = NULL;
	STRCONCATENATE(newVMF, emptyVmf);
	char *newEntities = KeyValuesToString(&newEntitiesKv);
	STRCONCATENATE(newVMF, newEntities);
	
	printf("Writing vmf of cell shading brushes to \"%s\".\n\n", cmdArgs.output.stringValue);
	if (!WriteEntireFile(newVMF, strlen(newVMF), cmdArgs.output.stringValue))
	{
		printf("ERROR: Failed to export instance vmf to path \"%s\".\n\n", cmdArgs.output.stringValue);
		goto cleanup;
	}
	
	printf("Cell shading generation done!\n\n");
	
	cleanup:
#if GC_DEBUG
	getchar();
#endif
	return 0;
}
