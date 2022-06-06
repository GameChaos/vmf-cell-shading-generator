/* date = November 17th 2020 4:18 pm */

#ifndef HAMER_MATHS_H
#define HAMER_MATHS_H

#include "math.h"
#include "float.h"

struct v2
{
	f32 x, y, z;
};

struct v3
{
	f32 x, y, z;
};


//Euler angles in degrees
typedef v3 Angles;

inline v3 V3(f32 x, f32 y, f32 z)
{
	v3 result;
	
	result.x = x;
	result.y = y;
	result.z = z;
	
	return result;
}

inline v2 V2(f32 x, f32 y)
{
	v2 result;
	
	result.x = x;
	result.y = y;
	
	return result;
}

inline v3 operator*(f32 a, v3 b)
{
	v3 result;
	
	result.x = b.x * a;
	result.y = b.y * a;
	result.z = b.z * a;
	
	return result;
}

inline v3 operator*(v3 a, f32 b)
{
	v3 result = b * a;
	
	return result;
}

inline v3 & operator*=(v3 &a, f32 b)
{
	a = b * a;
	
	return a;
}

inline v3 operator-(v3 a)
{
	v3 result;
	
	result.x = -a.x;
	result.y = -a.y;
	result.z = -a.z;
	
	return result;
}

inline v3 operator+(v3 a, v3 b)
{
	v3 result;
	
	result.x = a.x + b.x;
	result.y = a.y + b.y;
	result.z = a.z + b.z;
	
	return result;
}

inline v3 operator+=(v3 &a, v3 b)
{
	a = a + b;
	
	return a;
}

inline v3 operator-(v3 a, v3 b)
{
	v3 result;
	
	result.x = a.x - b.x;
	result.y = a.y - b.y;
	result.z = a.z - b.z;
	
	return result;
}

inline v3 operator-=(v3 &a, v3 b)
{
	a = a - b;
	
	return a;
}

// </v3>

// <v2>

inline v2 operator*(f32 a, v2 b)
{
	v2 result;
	
	result.x = b.x * a;
	result.y = b.y * a;
	
	return result;
}

inline v2 operator*(v2 a, f32 b)
{
	v2 result = b * a;
	
	return result;
}

inline v2 & operator*=(v2 &a, f32 b)
{
	a = b * a;
	
	return a;
}

inline v2 operator-(v2 a)
{
	v2 result;
	
	result.x = -a.x;
	result.y = -a.y;
	
	return result;
}

inline v2 operator+(v2 a, v2 b)
{
	v2 result;
	
	result.x = a.x + b.x;
	result.y = a.y + b.y;
	
	return result;
}

inline v2 operator+=(v2 &a, v2 b)
{
	a = a + b;
	
	return a;
}

inline v2 operator-(v2 a, v2 b)
{
	v2 result;
	
	result.x = a.x - b.x;
	result.y = a.y - b.y;
	
	return result;
}

inline v2 operator-=(v2 &a, v2 b)
{
	a = a - b;
	
	return a;
}

// </v2>

inline f32 LinearInterpolate(f32 f1, f32 f2, f32 fraction)
{
	f32 result = f2 - f1;
	result = result * fraction + f1;
	
	return result;
}

inline s32 RoundF32ToS32(f32 value)
{
	s32 result = (s32)roundf(value);
	return result;
}

inline s32 FloorF32ToS32(f32 value)
{
	s32 result = (s32)floorf(value);
	return result;
}

inline s32 TruncateF32ToS32(f32 value)
{
	s32 result = (s32)value;
	return result;
}

inline f32 Sine(f32 angle)
{
	f32 result = sinf(angle);
	return result;
}

inline f32 Cosine(f32 angle)
{
	f32 result = cosf(angle);
	return result;
}

inline f32 DegToRad(f32 angle)
{
	f32 result = angle * F32_PI / 180.0f;
	return result;
}

inline f32 RadToDeg(f32 angle)
{
	f32 result = angle * 180.0f / F32_PI;
	return result;
}

inline f32 ArcTangent2(f32 y, f32 x)
{
	f32 result = atan2f(y, x);
	return result;
}

inline f32 Abs(f32 value)
{
	return value < 0.0f ? -value : value;
}



inline b32 IsZeroVector(v3 vec)
{
	b32 result = vec.x == 0.0 && vec.y == 0.0 && vec.z == 0.0;
	return result;
}

inline v3 LinearInterpolate(v3 vec1, v3 vec2, f32 fraction)
{
	v3 result = vec2 - vec1;
	result = result * fraction + vec1;
	
	return result;
}

inline b32 VectorsEqual(v3 vec1, v3 vec2)
{
	b32 result = (vec1.x == vec2.x) && (vec1.y == vec2.y) && (vec1.z == vec2.z);
	return result;
}

inline float GetVectorLength(v3 vec)
{
	return sqrtf(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
}

void AnglesToVector(Angles angles, v3 *vec)
{
	ASSERT(vec);
	float sinPitch = Sine(DegToRad(angles.x));
	float sinYaw = Sine(DegToRad(angles.y));
	float cosPitch = Cosine(DegToRad(angles.x));
	float cosYaw = Cosine(DegToRad(angles.y));
	
	vec->x = cosPitch * cosYaw;
	vec->y = cosPitch * sinYaw;
	vec->z = -sinPitch;
}

inline f32 NormaliseVector(v3 *vec)
{
	f32 len = GetVectorLength(*vec);
	
	if (len != 0.0f)
	{
		f32 invLen = 1.0f / len;
		vec->x *= invLen;
		vec->y *= invLen;
		vec->z *= invLen;
	}
	
	return len;
}

inline v3 NormaliseVector(v3 vec)
{
	f32 len = GetVectorLength(vec);
	v3 result = vec;
	
	if (len != 0.0f)
	{
		f32 invLen = 1.0f / len;
		result.x *= invLen;
		result.y *= invLen;
		result.z *= invLen;
	}
	
	return result;
}

inline void AnglesToVectors(Angles angles, v3 *forwards, v3 *right, v3 *up)
{
	float sinPitch = Sine(DegToRad(angles.x));
	float sinYaw = Sine(DegToRad(angles.y));
	float sinRoll = Sine(DegToRad(angles.z));
	float cosPitch = Cosine(DegToRad(angles.x));
	float cosYaw = Cosine(DegToRad(angles.y));
	float cosRoll = Cosine(DegToRad(angles.z));
	
	if (forwards)
	{
		forwards->x = cosPitch * cosYaw;
		forwards->y = cosPitch * sinYaw;
		forwards->z = -sinPitch;
	}
	
	if (right)
	{
		right->x = (-1.0f * sinRoll * sinPitch * cosYaw + -1.0f * cosRoll * -sinYaw);
		right->y = (-1.0f * sinRoll * sinPitch * sinYaw + -1.0f * cosRoll * cosYaw);
		right->z = -1.0f * sinRoll * cosPitch;
	}
	
	if (up)
	{
		up->x = (cosRoll * sinPitch * cosYaw + -sinRoll * -sinYaw);
		up->y = (cosRoll * sinPitch * sinYaw + -sinRoll * cosYaw);
		up->z = cosRoll * cosPitch;
	}
}

inline Angles VectorToAngles(v3 vec)
{
	Angles angles = {};
	
	if (vec.y == 0.0f && vec.x == 0.0f)
	{
		angles.y = 0.0f;
		if (vec.z > 0.0f)
		{
			angles.x = 270.0f;
		}
		else
		{
			angles.x = 90.0f;
		}
	}
	else
	{
		float tmp = sqrtf(vec.x * vec.x + vec.y * vec.y);
		angles.x = RadToDeg(ArcTangent2(-vec.z, tmp));
		angles.y = RadToDeg(ArcTangent2(vec.y, vec.x));
	}
	
	return angles;
}

inline v3 CrossProduct(v3 a, v3 b)
{
	v3 result;
	
	result.x = a.y * b.z - a.z * b.y;
	result.y = a.z * b.x - a.x * b.z;
	result.z = a.x * b.y - a.y * b.x;
	
	return result;
}

inline f32 DotProduct(v3 a, v3 b)
{
	f32 result = (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
	return result;
}

// v: a vector in 3D space
// k: a unit vector describing the axis of rotation
// theta: the angle (in radians) that v rotates around k
inline v3 RotateVector(v3 vec, v3 axis, f32 theta)
{
	f32 cosTheta = Cosine(theta);
	f32 sinTheta = Sine(theta);
	
	v3 cross = CrossProduct(axis, vec);
	f32 dot = DotProduct(axis, vec);
	
	v3 result = (vec * cosTheta) + (cross * sinTheta) + (axis * dot) * (1.0f - cosTheta);
	//glm::dvec3 rotated = (v * cosTheta) + (glm::cross(k, v) * sinTheta) + (k * glm::dot(k, v)) * (1 - cosTheta);
	return result;
}

inline v3 GetNonParallelVector(v3 vec)
{
	v3 result = {};
	
	if (!(vec.x == 0.0f && vec.y == 0.0f && vec.z == 0.0f))
	{
		v3 temp = {};
		
		// TODO: make faster?
		// find a vector that is pretty far from vec.
		f32 absX = Abs(vec.x);
		f32 absY = Abs(vec.y);
		f32 absZ = Abs(vec.z);
		
		if (absX < absY)
		{
			temp.x = 1.0;
		}
		else if (absY < absZ)
		{
			temp.y = 1.0;
		}
		else
		{
			temp.z = 1.0;
		}
		
		result = CrossProduct(vec, temp);
	}
	
	return result;
}

#endif //HAMER_MATHS_H
