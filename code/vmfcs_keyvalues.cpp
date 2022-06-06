
inline b32 KVIsControlCharacter(char character)
{
	return character == '{'
		|| character == '}'
		|| character == '"';
}

// TODO: move this out of here?
inline b32 KVIsWhiteSpace(char character)
{
	b32 result = character == ' '
		|| character == '\r'
		|| character == '\n'
		|| character == '	';
	return result;
}

// TODO: move this out of here?
inline b32 KVIsEndOfLine(char character)
{
	b32 result = (character == '\n'
				  || character == '\r');
	return result;
}

// TODO: move this out of here?
inline b32 KVIsComment(char *string)
{
	return string[0] == '/' && string[1] == '/';
}

internal void EatWhiteSpace(KVTokeniser *tokeniser)
{
	while (1)
	{
		if (tokeniser->at[0] == ' ' || tokeniser->at[0] == '	')
		{
			++tokeniser->at;
		}
		else if (tokeniser->at[0] == '/' && tokeniser->at[1] == '/')
		{
			tokeniser->at += 2;
			while (tokeniser->at[0] && !KVIsEndOfLine(tokeniser->at[0]))
			{
				++tokeniser->at;
			}
		}
		else
		{
			break;
		}
	}
}

internal KVToken KVGetToken(KVTokeniser *tokeniser)
{
	EatWhiteSpace(tokeniser);
	
	KVToken token = {};
	token.textLength = 1;
	token.text = tokeniser->at;
	
	char c = tokeniser->at[0];
	tokeniser->at++;
	switch (c)
	{
		case '\0': {token.type = TOKEN_ENDOFSTREAM;} break;
		case '{': {token.type = TOKEN_OPENBRACE;} break;
		case '}': {token.type = TOKEN_CLOSEBRACE;} break;
		
		case '\n':
		case '\r': {token.type = TOKEN_NEWLINE;} break;
		
		// TODO: replace escape sequences with corresponding characters?
		default:
		{
			token.type = TOKEN_IDENTIFIER;
			
			--tokeniser->at;
			b32 inQuote = (c == '"');
			if (inQuote)
			{
				++tokeniser->at;
				++token.text;
			}
			
			while (1)
			{
				if (!tokeniser->at[0])
				{
					break;
				}
				
				if (inQuote)
				{
					if (tokeniser->at[0] == '"')
					{
						token.textLength = tokeniser->at - token.text;
						tokeniser->at++;
						break;
					}
				}
				else
				{
					if (KVIsWhiteSpace(tokeniser->at[0])
						|| KVIsControlCharacter(tokeniser->at[0])
						|| KVIsComment(tokeniser->at))
					{
						token.textLength = tokeniser->at - token.text;
						break;
					}
				}
				
				// skip over escape sequences
				if (tokeniser->at[0] == '\\')
				{
					++tokeniser->at;
				}
				
				++tokeniser->at;
			}
			
		} break;
	}
	
	return token;
}

internal char *KeyValuesToString_(KeyValues *kv, String *string, s32 recursionDepth)
{
	if (!kv)
	{
		return NULL;
	}
	
	if (string == NULL)
	{
		string = NewString(512);
	}
	
	String *buffer = NewString(512);
	for (s32 i = 0; i < recursionDepth; i++)
	{
		STRCONCATENATE(string, "\t");
	}
	
	if (kv->key[0] && kv->value[0])
	{
		STRFORMAT(buffer, "\"%s\" \"%s\"\n", kv->key, kv->value);
		STRCONCATENATE(string, buffer);
	}
	else if (kv->key[0])
	{
		if (kv->childCount == 0)
		{
			STRFORMAT(buffer, "%s\n{\n}\n", kv->key);
			STRCONCATENATE(string, buffer);
		}
		else
		{
			STRFORMAT(buffer, "%s\n", kv->key);
			STRCONCATENATE(string, buffer);
		}
	}
	else
	{
		recursionDepth -= 1;
	}
	
	if (kv->children)
	{
		if (kv->key[0])
		{
			for (s32 i = 0; i < recursionDepth; i++)
			{
				STRCONCATENATE(string, "\t");
			}
			STRCONCATENATE(string, "{\n");
		}
		
		for (u32 i = 0; i < kv->childCount; i++)
		{
			string = KeyValuesToString_(&kv->children[i], string, recursionDepth + 1);
		}
		
		if (kv->key[0])
		{
			for (s32 i = 0; i < recursionDepth; i++)
			{
				STRCONCATENATE(string, "\t");
			}
			STRCONCATENATE(string, "}\n");
		}
	}
	
	if (buffer != NULL)
	{
		STRFREE(buffer);
	}
	
	return string;
}

internal char *KeyValuesToString(KeyValues *kv)
{
	char *result = KeyValuesToString_(kv, NULL, 0);
	
	return result;
}

internal void PrintKeyValues(KeyValues *kv, s32 recursionDepth)
{
	if (!kv)
	{
		return;
	}
	
	for (s32 i = 0; i < recursionDepth; i++)
	{
		printf("\t");
	}
	
	if (kv->key[0] && kv->value[0])
	{
		printf("\"%s\" \"%s\"\n", kv->key, kv->value);
	}
	else if (kv->key[0])
	{
		printf("%s\n", kv->key);
	}
	else
	{
		recursionDepth -= 1;
	}
	
	if (kv->children)
	{
		if (kv->key[0])
		{
			for (s32 i = 0; i < recursionDepth; i++)
			{
				printf("\t");
			}
			printf("{\n");
		}
		
		for (u32 i = 0; i < kv->childCount; i++)
		{
			PrintKeyValues(&kv->children[i], recursionDepth + 1);
		}
		
		if (kv->key[0])
		{
			for (s32 i = 0; i < recursionDepth; i++)
			{
				printf("\t");
			}
			printf("}\n");
		}
	}
}

internal b32 KeyValuesGetChild(KeyValues *kv, KeyValues **out, char *keyName)
{
	b32 result = false;
	
	if (kv && out)
	{
		if (kv->children)
		{
			for (u32 i = 0; i < kv->childCount; i++)
			{
				if (StringEquals(kv->children[i].key, keyName))
				{
					*out = &kv->children[i];
					result = true;
					break;
				}
			}
		}
	}
	
	return result;
}

internal b32 KeyValuesGetNextChild(KeyValues *kv, KeyValues **out, char *keyName)
{
	b32 result = false;
	if (kv)
	{
		if (kv->children)
		{
			if (keyName == NULL)
			{
				if (kv->childIterIndex < kv->childCount)
				{
					*out = &kv->children[kv->childIterIndex];
					kv->childIterIndex++;
					result = true;
				}
			}
			else
			{				
				while (kv->childIterIndex < kv->childCount)
				{
					//u32 i = kv->childIterIndex;
					if (StringEquals(kv->children[kv->childIterIndex].key, keyName))
					{
						*out = &kv->children[kv->childIterIndex];
						result = true;
						kv->childIterIndex++;
						break;
					}
					kv->childIterIndex++;
				}
			}
		}
	}
	
	// NOTE(GameChaos): automatically reset iteration
	if (!result)
	{
		KeyValuesResetIteration(kv);
	}
	
	return result;
}

internal void KeyValuesResetIteration(KeyValues *kv)
{
	if (kv)
	{
		kv->childIterIndex = 0;
	}
}

internal KeyValues CopyKeyValues(KeyValues kv)
{
	KeyValues result = kv;
	if (kv.children)
	{
		size_t childBytes = kv.childCount * sizeof(kv);
		result.children = (KeyValues *)MemoryAllocate(childBytes);
		DuplicateMemory(kv.children, result.children, childBytes, childBytes);
		for (u32 childIndex = 0; childIndex < result.childCount; childIndex++)
		{
			result.children[childIndex] = CopyKeyValues(kv.children[childIndex]);
		}
	}
	return result;
}

// NOTE: this copies memory!
internal void KeyValuesAppend(KeyValues *kv, KeyValues newKeyValues)
{
	if (kv)
	{
		
		
		if (!kv->children)
		{
			kv->childBytes = sizeof(KeyValues) * 4;
			kv->children = (KeyValues *)MemoryAllocate(kv->childBytes);
			// return if alloc fails.
			if (!kv->children)
			{
				kv->childBytes = 0;
				return;
			}
		}
		else if (kv->childBytes < sizeof(KeyValues) * (kv->childCount + 1))
		{
			// integer overflow check
			if (kv->childBytes > kv->childBytes * 2)
			{
				kv->childBytes = U32_MAX;
			}
			kv->childBytes *= 2;
			kv->children = (KeyValues *)ReallocateMemory(kv->children, kv->childBytes);
			ASSERT(kv->children);
		}
		
		KeyValues copied = CopyKeyValues(newKeyValues);
		
		kv->children[kv->childCount] = copied;
		++kv->childCount;
	}
}

internal void KeyValuesAddChild(KeyValues *kv, char *key, char *value)
{
	if (kv)
	{
		KeyValues child = {};
		CopyString(key, child.key, sizeof(child.key));
		CopyString(value, child.value, sizeof(child.value));
		KeyValuesAppend(kv, child);
	}
}

internal void KeyValuesFree(KeyValues *kv)
{
	if (!kv)
	{
		return;
	}
	
	if (kv->children)
	{
		for (u32 i = 0; i < kv->childCount; i++)
		{
			KeyValuesFree(&kv->children[i]);
		}
		FreeMemory(kv->children);
	}
}

internal KeyValues KeyValuesFromString(KVTokeniser *tokeniser)
{
	KeyValues keyValues = {};
	
	KeyValues tempValues = {};
	b32 parsing = true;
	
	KVToken lastToken = {};
	while (parsing)
	{
		KVToken token = KVGetToken(tokeniser);
		
		switch (token.type)
		{
			case TOKEN_OPENBRACE:
			{
				// repetitive code
				if (tempValues.key[0])
				{
					KeyValuesAppend(&keyValues, tempValues);
					// clear tempValues
					tempValues = {};
				}
				
				if (keyValues.childCount)
				{
					KeyValues newKv = KeyValuesFromString(tokeniser);
					keyValues.children[keyValues.childCount - 1].children = newKv.children;
					keyValues.children[keyValues.childCount - 1].childCount = newKv.childCount;
					keyValues.children[keyValues.childCount - 1].childBytes = newKv.childBytes;
				}
			} break;
			
			case TOKEN_CLOSEBRACE:
			{
				// repetitive code
				if (tempValues.key[0])
				{
					KeyValuesAppend(&keyValues, tempValues);
					// clear tempValues
					tempValues = {};
				}
				
				parsing = false;
			} break;
			
			case TOKEN_IDENTIFIER:
			{
				if (tempValues.key[0] && tempValues.value[0])
				{
					// TODO: errors!
					printf("???????????");
					ASSERT(!"this shouldn't happen, there can only be 2 identifiers on 1 line!");
				}
				else if (tempValues.key[0])
				{
					CopyString(token.text, token.textLength, tempValues.value, ARRAY_LENGTH(tempValues.value));
				}
				else
				{
					CopyString(token.text, token.textLength, tempValues.key, ARRAY_LENGTH(tempValues.key));
				}
			} break;
			
			case TOKEN_NEWLINE:
			{
				// repetitive code
				if (tempValues.key[0])
				{
					KeyValuesAppend(&keyValues, tempValues);
					// clear tempValues
					tempValues = {};
				}
				// clear tempValues
				tempValues = {};
			} break;
			
			case TOKEN_ENDOFSTREAM:
			{
				parsing = false;
			} break;
			
			default:
			{
				printf("[KeyValues] Unknown token %i \"%.*s\"\n", token.type, (s32)token.textLength, token.text);
			} break;
		}
		
		lastToken = token;
	}
	
	return keyValues;
}

internal b32 ImportKeyValues(KeyValues *kv, char *path)
{
	ReadFileResult file = ReadEntireFile(path);
	b32 result = false;
	if (file.memory)
	{
		KVTokeniser tokeniser = {};
		tokeniser.at = (char *)file.memory;
		*kv = KeyValuesFromString(&tokeniser);
		
		FreeMemory(file.memory);
		result = true;
	}
	
	return result;
}
