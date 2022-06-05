/* date = November 21st 2020 5:14 pm */

#ifndef HAMER_KEYVALUES_H
#define HAMER_KEYVALUES_H

#define KV_MAX_KEY_LEN     32
#define KV_MAX_VALUE_LEN   1024

enum KVTokenType
{
	TOKEN_UNKNOWN,
	TOKEN_OPENBRACE,
	TOKEN_CLOSEBRACE,
	TOKEN_IDENTIFIER,
	TOKEN_NEWLINE,
	TOKEN_ENDOFSTREAM,
};

struct KVToken
{
	KVTokenType type;
	size_t textLength;
	char *text;
};

struct KVTokeniser
{
	char *at;
};

struct KeyValues
{
	char key[KV_MAX_KEY_LEN];
	char value[KV_MAX_VALUE_LEN];
	// b32 isSection;?
	KeyValues *children;
	u32 childCount; // if it has children, then it's an opening of a section.
	u32 childBytes;
	u32 childIterIndex; // index of the last child + 1 that was got with KeyValuesGetNextChild. 0 by default
};

internal char *KeyValuesToString(KeyValues *kv, String *string = NULL, s32 recursionDepth = 0);
internal void PrintKeyValues(KeyValues *kv, s32 recursionDepth = 0);

internal KeyValues *KeyValuesGetChild(KeyValues *kv, char *keyName);
internal b32 KeyValuesGetNextChild(KeyValues *kv, KeyValues **out, char *keyName);
internal void KeyValuesResetIteration(KeyValues *kv);

internal void KeyValuesAppend(KeyValues *kv, KeyValues newKeyValues);
internal void KeyValuesAddChild(KeyValues *kv, char *key, char *value);

internal void KeyValuesFree(KeyValues *kv);
internal b32 ImportKeyValues(KeyValues *kv, char *path);

#endif //HAMER_KEYVALUES_H
