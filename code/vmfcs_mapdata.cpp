

// TODO: use %a (hex float) when saving float values? are those more accurate?

// 1st parameter is pointer to stb_ds dynamic array.
internal void ClipPolygon(v3 **polygon, BrushSide *side)
{
	u32 polygonLen = (u32)arrlen(*polygon);
	v3 *newPolygon = 0;
	
	if (side->id == 31)
	{
		newPolygon = 0;
	}
	
	for (u32 i = 0; i < arrlen(*polygon); i++)
	{
		u32 nextIndex = (i + 1) % arrlen(*polygon);
		v3 point1 = (*polygon)[i];
		v3 point2 = (*polygon)[nextIndex];
		f32 point1Distance = DotProduct(side->normal, point1) - side->distance;
		f32 point2Distance = DotProduct(side->normal, point2) - side->distance;
		b32 point1Behind = point1Distance < 0.0;
		b32 point2Behind = point2Distance < 0.0;
		
		if (point1Behind)
		{
			arrput(newPolygon, point1);
		}
		
		// intersect line with plane. only if the 2 points are on either side of the plane
		if (point1Behind && !point2Behind || !point1Behind && point2Behind)
		{
			f32 frac = point1Distance / (point1Distance - point2Distance);
			v3 newPoint = LinearInterpolate(point1, point2, frac);
			arrput(newPolygon, newPoint);
		}
	}
	// check if we managed to clip anything
	if (newPolygon)
	{
		arrfree(*polygon);
		*polygon = newPolygon;
	}
	ASSERT(*polygon);
}

internal v3 **GenerateBrushPolygons(Brush *brush)
{
	v3 **polygons = NULL;
	
	u32 sideCount = (u32)arrlen(brush->sides);
	
	for (u32 sideIndex = 0; sideIndex < sideCount; sideIndex++)
	{
		BrushSide *side = &brush->sides[sideIndex];
		
		f32 normalLen = GetVectorLength(side->normal);
		if (normalLen < 0.9 || normalLen > 1.1)
		{
			// TODO: error logging!
			ASSERT(0);
		}
		
		// make a vector that isn't parallel to the plane's normal
		v3 nonParallel = GetNonParallelVector(side->normal);
		
		f32 spread = MAP_MAX_SIZE * 2.0f;
		v3 tangentX = CrossProduct(side->normal, nonParallel);
		NormaliseVector(&tangentX);
		v3 tangentY = CrossProduct(side->normal, tangentX);
		NormaliseVector(&tangentY);
		
		v3 *polygon = NULL;
		v3 pointOnPlane = side->normal * side->distance;
		// generate an initial polygon that's bigger than the map and is in the same direction of the plane.
		arrput(polygon, ( tangentX + -tangentY) * spread + pointOnPlane);
		arrput(polygon, ( tangentX +  tangentY) * spread + pointOnPlane);
		arrput(polygon, (-tangentX +  tangentY) * spread + pointOnPlane);
		arrput(polygon, (-tangentX + -tangentY) * spread + pointOnPlane);
		ASSERT(polygon);
		
		for (u32 j = 0; j < sideCount; j++)
		{
			BrushSide *side2 = &brush->sides[j];
			if (side2 == side)
			{
				continue;
			}
			if (!VectorsEqual(side2->normal, -side->normal))
			{
				ClipPolygon(&polygon, side2);
				u32 test = (u32)arrlen(polygon);
				test += 0;
			}
			
			if (arrlen(polygon) < 3)
			{
				// TODO: error logging or something?
				ASSERT(!"polygon has less than 3 vertices!");
				break;
			}
		}
		ASSERT(polygon);
		arrput(polygons, polygon);
	}
	
	// Print("brush generation took %f seconds", GetTime() - startTime);
	
	return polygons;
}

internal b32 ParseVMFSolidSide(KeyValues *kv, BrushSide *outSide)
{
	b32 result = false;
	for (u32 child = 0; child < kv->childCount; child++)
	{
		char *key = kv->children[child].key;
		char *value = kv->children[child].value;
		if (StringEquals(key, "id"))
		{
			if (!StringToU32(value, &outSide->id))
			{
				// TODO: resort to some default int
				ASSERT(!"couldn't convert string to int!");
				result = false;
			}
		}
		else if (StringEquals(key, "plane"))
		{
			int numScanned = ScanStringFormat(value, "(%f %f %f) (%f %f %f) (%f %f %f)",
											  &outSide->plane[0].x, &outSide->plane[0].y, &outSide->plane[0].z,
											  &outSide->plane[1].x, &outSide->plane[1].y, &outSide->plane[1].z,
											  &outSide->plane[2].x, &outSide->plane[2].y, &outSide->plane[2].z);
			// TODO: print error/warning
			ASSERT(numScanned == 9);
			
			// generate plane normal and distance
			// TODO: check for collinearity?
			outSide->normal = CrossProduct(outSide->plane[0] - outSide->plane[1], (outSide->plane[2] - outSide->plane[1]));
			NormaliseVector(&outSide->normal);
			outSide->distance = DotProduct(outSide->normal, outSide->plane[0]);
		}
		else if (StringEquals(key, "material"))
		{
			CopyString(value, outSide->material, ARRAY_LENGTH(outSide->material));
		}
		else if (StringEquals(key, "uaxis"))
		{
			int numScanned = ScanStringFormat(value, "[%f %f %f %f] %f",
											  &outSide->uaxis.normal.x,
											  &outSide->uaxis.normal.y,
											  &outSide->uaxis.normal.z,
											  &outSide->uaxis.offset,
											  &outSide->uaxis.scale);
			// TODO: print error/warning
			ASSERT(numScanned == 5);
		}
		else if (StringEquals(key, "vaxis"))
		{
			int numScanned = ScanStringFormat(value, "[%f %f %f %f] %f",
											  &outSide->vaxis.normal.x,
											  &outSide->vaxis.normal.y,
											  &outSide->vaxis.normal.z,
											  &outSide->vaxis.offset,
											  &outSide->vaxis.scale);
			// TODO: print error/warning
			ASSERT(numScanned == 5);
		}
		else if (StringEquals(key, "rotation"))
		{
			StringToF32(value, &outSide->rotation);
		}
		else if (StringEquals(key, "lightmapscale"))
		{
			StringToU32(value, &outSide->lightmapScale);
		}
		else if (StringEquals(key, "smoothing_groups"))
		{
			StringToU32(value, &outSide->smoothingGroups);
		}
		else if (StringEquals(key, "dispinfo"))
		{
			// TODO: dispinfo
		}
		else
		{
			// TODO: preserve unknown keyvalues
		}
	}
	return result;
}

internal b32 ParseVMFSolid(KeyValues *kv, Brush *outBrush)
{
	b32 result = true;
	
	for (u32 child = 0; child < kv->childCount; child++)
	{
		char *key = kv->children[child].key;
		char *value = kv->children[child].value;
		if (StringEquals(key, "id"))
		{
			if (!StringToU32(value, &outBrush->id))
			{
				// TODO: resort to some default int
				ASSERT(!"couldn't convert string to int!");
				result = false;
			}
		}
		else if (StringEquals(key, "side"))
		{
			BrushSide side = {};
			
			if (!ParseVMFSolidSide(&kv->children[child], &side))
			{
				result = false;
			}
			
			arrput(outBrush->sides, side);
		}
		else
		{
			// TODO: preserve unknown keyvalues
			KeyValuesAppend(&outBrush->unknownKV, kv->children[child]);
		}
	}
	return result;
}

internal b32 BrushToKeyValues(Brush *brush, KeyValues *out, s32 *id)
{
	b32 result = false;
	
	KeyValuesFree(out);
	
	if (out)
	{
		CopyString("solid", out->key, ARRAY_LENGTH(out->key));
		
		KeyValues idKv = {};
		
		CopyString("id", idKv.key, ARRAY_LENGTH(idKv.key));
		Format(idKv.value, ARRAY_LENGTH(idKv.value), "%i", (*id)++);
		KeyValuesAppend(out, idKv);
		
		for (s32 sideIndex = 0; sideIndex < arrlen(brush->sides); sideIndex++)
		{
			BrushSide *side = &brush->sides[sideIndex];
			
			KeyValues sideKv = {};
			CopyString("side", sideKv.key, ARRAY_LENGTH(sideKv.key));
			
			{
				CopyString("id", idKv.key, ARRAY_LENGTH(idKv.key));
				Format(idKv.value, ARRAY_LENGTH(idKv.value), "%i", (*id)++);
				KeyValuesAppend(&sideKv, idKv);
			}
			
			{
				KeyValues planeKv = {};
				CopyString("plane", planeKv.key, ARRAY_LENGTH(planeKv.key));
				v3 *planes = side->plane;
				Format(planeKv.value, ARRAY_LENGTH(planeKv.value), "(%f %f %f) (%f %f %f) (%f %f %f)",
					   planes[0].x, planes[0].y, planes[0].z,
					   planes[1].x, planes[1].y, planes[1].z,
					   planes[2].x, planes[2].y, planes[2].z);
				KeyValuesAppend(&sideKv, planeKv);
			}
			
			{
				KeyValues kv = {};
				CopyString("material", kv.key, ARRAY_LENGTH(kv.key));
				CopyString(side->material, kv.value, ARRAY_LENGTH(kv.value));
				KeyValuesAppend(&sideKv, kv);
			}
			
			{
				KeyValues kv = {};
				CopyString("uaxis", kv.key, ARRAY_LENGTH(kv.key));
				Format(kv.value, ARRAY_LENGTH(kv.value), "[%f %f %f %f] %f",
					   side->uaxis.normal.x, side->uaxis.normal.y, side->uaxis.normal.z, side->uaxis.offset, side->uaxis.scale);
				KeyValuesAppend(&sideKv, kv);
			}
			
			{
				KeyValues kv = {};
				CopyString("vaxis", kv.key, ARRAY_LENGTH(kv.key));
				Format(kv.value, ARRAY_LENGTH(kv.value), "[%f %f %f %f] %f",
					   side->vaxis.normal.x, side->vaxis.normal.y, side->vaxis.normal.z, side->vaxis.offset, side->vaxis.scale);
				KeyValuesAppend(&sideKv, kv);
			}
			
			{
				KeyValues kv = {};
				CopyString("rotation", kv.key, ARRAY_LENGTH(kv.key));
				Format(kv.value, ARRAY_LENGTH(kv.value), "%f", side->rotation);
				KeyValuesAppend(&sideKv, kv);
			}
			
			{
				KeyValues kv = {};
				CopyString("lightmapscale", kv.key, ARRAY_LENGTH(kv.key));
				Format(kv.value, ARRAY_LENGTH(kv.value), "%u", side->lightmapScale);
				KeyValuesAppend(&sideKv, kv);
			}
			
			{
				KeyValues kv = {};
				CopyString("smoothing_groups", kv.key, ARRAY_LENGTH(kv.key));
				Format(kv.value, ARRAY_LENGTH(kv.value), "%u", side->smoothingGroups);
				KeyValuesAppend(&sideKv, kv);
			}
			
			KeyValuesAppend(out, sideKv);
		}
		
		result = true;
	}
	
	return result;
}
