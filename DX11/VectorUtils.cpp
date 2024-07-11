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

Vec3 Clamp(const Vec3& value, const Vec3& min, const Vec3& max)
{
    return Vec3(
        max(min.x, min(value.x, max.x)), 
        max(min.y, min(value.y, max.y)), 
        max(min.z, min(value.z, max.z)) 
    );
}

Quaternion RotateByScaledVector(const Quaternion& quat, const Vector3& vec, float scale)
{
    // 벡터를 스케일링한 값으로 쿼터니언 생성 
    Quaternion scaledVecQuat(0.0f, vec.x * scale, vec.y * scale, vec.z * scale); 

    // 입력 쿼터니언과 곱셈하여 새로운 쿼터니언 반환
    return quat * scaledVecQuat; 
}

Vec3 Vec3XMatrix(const Matrix& matrix, const Vector3& vec)
{
    Vector4 vec4(vec.x, vec.y, vec.z, 1.0f); // vec를 4D 벡터로 확장
    Vector4 result = Vector4::Transform(vec4, matrix);

    return Vector3(result.x / result.w, result.y / result.w, result.z / result.w);
}
