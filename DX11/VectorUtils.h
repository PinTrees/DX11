#pragma once

Vec3 Normalize(Vec3 vec);

float Dot(Vec3 vec1, Vec3 vec2);

Vec3 Cross(Vec3 vec1, Vec3 vec2); 

Vec3 Clamp(const Vec3& value, const Vec3& minV, const Vec3& maxV);  

Quaternion RotateByScaledVector(const Quaternion& quat, const Vector3& vec, float scale); 

