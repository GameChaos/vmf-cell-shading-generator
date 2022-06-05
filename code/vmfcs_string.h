/* date = December 26th 2020 10:13 pm */

#ifndef HAMER_STRING_H
#define HAMER_STRING_H

// only use for these types of strings!
typedef char String;

// internal use only
// prefix I_ means internal!
struct I_StringHeader
{
	s64 length;
	s64 capacity;
};

#define I_STRHEADER(str) (((I_StringHeader *)(str)) - 1)
#define I_STRING_MIN_GROW_SIZE 32

internal b32 I_StringGrow(String **string)
{
	b32 result = false;
	
	if (*string != NULL)
	{
		I_StringHeader *header = I_STRHEADER(*string);
		s64 newCapacity = MAX(I_STRING_MIN_GROW_SIZE, header->capacity * 2);
		header = (I_StringHeader *)ReallocateMemory((void *)header, newCapacity + sizeof(I_StringHeader));
		if (header)
		{
			header->capacity = newCapacity;
			*string = (String *)(header + 1);
			result = true;
		}
		else
		{
			// TODO: error codes?
			ASSERT(0);
		}
	}
	else
	{
		ASSERT(0);
	}
	
	return result;
}

// external use:

// the api is quite similar to stb_ds
// number of bytes stored in the string in bytes (excluding null terminator)
#define STRLEN(str) ((str) == NULL ? 0 : I_STRHEADER(str)->length)

// maximum capacity that the string can store in bytes (including null terminator)
#define STRCAP(str) ((str) == NULL ? 0 : I_STRHEADER(str)->capacity)

#define STRFORMAT(buffer, format, ...) StringFormat(&(buffer), format, ##__VA_ARGS__)
#define STRCONCATENATE(dest, source) StringConcatenate(&(dest), source)

#define STRFREE(str) FreeMemory(I_STRHEADER(str))

internal String *NewString(s64 size);
internal s32 StringConcatenate(String **dest, char *source);
internal s32 StringFormat(String **buffer, char *format, ...);


// internal use only
// TODO: you can't tell the difference between the string not growing because it doesn't need to
// and the string not growing because I_StringGrow failed. FIX!
inline b32 I_StringTryGrowing(String **string, s64 addedLength)
{
	if (*string == NULL)
	{
		*string = NewString(512);
	}
	
	b32 result = false;
	s64 bufferLen = STRLEN(*string);
	s64 bufferCap = STRCAP(*string);
	while (addedLength + bufferLen >= bufferCap)
	{
		result = I_StringGrow(string);
		ASSERT(result);
		if (!result)
		{
			break;
		}
		bufferLen = STRLEN(*string);
		bufferCap = STRCAP(*string);
	}
	
	return result;
}

// external use again
internal String *NewString(s64 size)
{
	String *result = NULL;
	
	if (size > 0)
	{
		I_StringHeader *header = (I_StringHeader *)MemoryAllocate(size + sizeof(I_StringHeader));
		if (header != NULL)
		{
			header->capacity = size;
			header->length = 0;
			result = (String *)(header + 1);
		}
	}
	
	return result;
}

internal s32 StringConcatenate(String **dest, char *source)
{
	s32 sourceLen = (s32)strlen(source);
	I_StringTryGrowing(dest, sourceLen + 1);
	ASSERT(dest);
	
	char *destEnd = (*dest + STRLEN(*dest));
	memcpy(destEnd, source, sourceLen + 1);
	
	I_STRHEADER(*dest)->length += sourceLen;
	
	return sourceLen;
}

internal s32 StringFormat(String **buffer, char *format, ...)
{
	va_list args;
    va_start(args, format);
	
	if (*buffer == NULL)
	{
		*buffer = NewString(512);
	}
	
	s32 formatLen = _vscprintf(format, args);
	I_StringTryGrowing(buffer, formatLen);
#if 0
	s64 bufferLen = STRLEN(*buffer);
	s64 bufferCap = STRCAP(*buffer);
	while (formatLen + bufferLen >= bufferCap)
	{
		b32 couldGrow = I_StringGrow(buffer);
		ASSERT(couldGrow);
	}
#endif
	
	s32 result = _vsnprintf_s(*buffer, STRCAP(*buffer), _TRUNCATE, format, args);
	va_end(args);
	ASSERT(result != -1);
	I_STRHEADER(*buffer)->length = result;
	
	return result;
}

#endif //HAMER_STRING_H
