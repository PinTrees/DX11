#include "pch.h"
#include "VectorUtils.h"

Vec3 Normalize(Vec3 vec)
{
    vec.Normalize();
    return vec;
}

float Dot(Vec3 vec1, Vec3 vec2)
{
    return vec1.Dot(vec2); 
}

Vec3 Cross(Vec3 vec1, Vec3 vec2)
{
    return vec1.Cross(vec2);
}
