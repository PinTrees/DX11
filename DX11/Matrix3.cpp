#include "pch.h"
#include "Matrix3.h"


Matrix3::Matrix3()
{
    setDiagonal(1.0f);
}

Matrix3::Matrix3(float value)
{
    setDiagonal(value);
}

Matrix3::Matrix3(float v1, float v2, float v3)
{
    entries[0] = v1;
    entries[1] = 0.0f;
    entries[2] = 0.0f;

    entries[3] = 0.0f;
    entries[4] = v2;
    entries[5] = 0.0f;

    entries[6] = 0.0f;
    entries[7] = 0.0f;
    entries[8] = v3;
}

void Matrix3::setDiagonal(float value)
{
    entries[0] = value;
    entries[1] = 0.0f;
    entries[2] = 0.0f;

    entries[3] = 0.0f;
    entries[4] = value;
    entries[5] = 0.0f;

    entries[6] = 0.0f;
    entries[7] = 0.0f;
    entries[8] = value;
}

Matrix3 Matrix3::transpose() const
{
    Matrix3 result(entries[0], entries[4], entries[8]);

    result.entries[1] = entries[3];
    result.entries[2] = entries[6];
    result.entries[3] = entries[1];
    result.entries[5] = entries[7];
    result.entries[6] = entries[2];
    result.entries[7] = entries[5];

    return result;
}

Matrix3 Matrix3::inverse() const
{
    float determinant =
        entries[0] * (entries[4] * entries[8] - entries[5] * entries[7])
        - entries[1] * (entries[3] * entries[8] - entries[5] * entries[6])
        + entries[2] * (entries[3] * entries[7] - entries[4] * entries[6]);

    /* 행렬식이 0 이면 역행렬이 존재하지 않는다 */
    if (determinant == 0.0f)
    {
        std::cout << "MATRIX3::This matrix's inverse does not exist." << std::endl;
        return *this;
    }

    determinant = 1.0f / determinant;
    Matrix3 result;

    result.entries[0] = (entries[4] * entries[8] - entries[5] * entries[7]) * determinant;
    result.entries[1] = -(entries[1] * entries[8] - entries[2] * entries[7]) * determinant;
    result.entries[2] = (entries[1] * entries[5] - entries[2] * entries[4]) * determinant;
    result.entries[3] = -(entries[3] * entries[8] - entries[5] * entries[6]) * determinant;
    result.entries[4] = (entries[0] * entries[8] - entries[2] * entries[6]) * determinant;
    result.entries[5] = -(entries[0] * entries[5] - entries[2] * entries[3]) * determinant;
    result.entries[6] = (entries[3] * entries[7] - entries[4] * entries[6]) * determinant;
    result.entries[7] = -(entries[0] * entries[7] - entries[1] * entries[6]) * determinant;
    result.entries[8] = (entries[0] * entries[4] - entries[1] * entries[3]) * determinant;

    return result;
}

Matrix3 Matrix3::operator+(const Matrix3& other) const
{
    Matrix3 result;

    for (int i = 0; i < 9; ++i)
    {
        result.entries[i] = entries[i] + other.entries[i];
    }

    return result;
}

void Matrix3::operator+=(const Matrix3& other)
{
    for (int i = 0; i < 9; ++i)
    {
        entries[i] += other.entries[i];
    }
}

Matrix3 Matrix3::operator-(const Matrix3& other) const
{
    Matrix3 result;

    for (int i = 0; i < 9; ++i)
    {
        result.entries[i] = entries[i] - other.entries[i];
    }

    return result;
}

void Matrix3::operator-=(const Matrix3& other)
{
    for (int i = 0; i < 9; ++i)
    {
        entries[i] -= other.entries[i];
    }
}

Matrix3 Matrix3::operator*(const Matrix3& other) const
{
    Matrix3 result;

    result.entries[0] = entries[0] * other.entries[0] +
        entries[1] * other.entries[3] + entries[2] * other.entries[6];
    result.entries[1] = entries[0] * other.entries[1] +
        entries[1] * other.entries[4] + entries[2] * other.entries[7];
    result.entries[2] = entries[0] * other.entries[2] +
        entries[1] * other.entries[5] + entries[2] * other.entries[8];

    result.entries[3] = entries[3] * other.entries[0] +
        entries[4] * other.entries[3] + entries[5] * other.entries[6];
    result.entries[4] = entries[3] * other.entries[1] +
        entries[4] * other.entries[4] + entries[5] * other.entries[7];
    result.entries[5] = entries[3] * other.entries[2] +
        entries[4] * other.entries[5] + entries[5] * other.entries[8];

    result.entries[6] = entries[6] * other.entries[0] +
        entries[7] * other.entries[3] + entries[8] * other.entries[6];
    result.entries[7] = entries[6] * other.entries[1] +
        entries[7] * other.entries[4] + entries[8] * other.entries[7];
    result.entries[8] = entries[6] * other.entries[2] +
        entries[7] * other.entries[5] + entries[8] * other.entries[8];

    return result;
}

void Matrix3::operator*=(const Matrix3& other)
{
    Matrix3 result;

    result.entries[0] = entries[0] * other.entries[0] +
        entries[1] * other.entries[3] + entries[2] * other.entries[6];
    result.entries[1] = entries[0] * other.entries[1] +
        entries[1] * other.entries[4] + entries[2] * other.entries[7];
    result.entries[2] = entries[0] * other.entries[2] +
        entries[1] * other.entries[5] + entries[2] * other.entries[8];

    result.entries[3] = entries[3] * other.entries[0] +
        entries[4] * other.entries[3] + entries[5] * other.entries[6];
    result.entries[4] = entries[3] * other.entries[1] +
        entries[4] * other.entries[4] + entries[5] * other.entries[7];
    result.entries[5] = entries[3] * other.entries[2] +
        entries[4] * other.entries[5] + entries[5] * other.entries[8];

    result.entries[6] = entries[6] * other.entries[0] +
        entries[7] * other.entries[3] + entries[8] * other.entries[6];
    result.entries[7] = entries[6] * other.entries[1] +
        entries[7] * other.entries[4] + entries[8] * other.entries[7];
    result.entries[8] = entries[6] * other.entries[2] +
        entries[7] * other.entries[5] + entries[8] * other.entries[8];

    *this = result;
}

SimpleMath::Vector3 Matrix3::operator*(SimpleMath::Vector3 vec) const 
{ 
    return SimpleMath::Vector3(
        entries[0] * vec.x + entries[1] * vec.y + entries[2] * vec.z, 
        entries[3] * vec.x + entries[4] * vec.y + entries[5] * vec.z,
        entries[6] * vec.x + entries[7] * vec.y + entries[8] * vec.z
    ); 
}

Matrix3 Matrix3::operator*(const float value) const
{
    Matrix3 result;

    for (int i = 0; i < 9; ++i)
    {
        result.entries[i] *= value;
    }

    return result;
}

void Matrix3::operator*=(const float value)
{
    for (int i = 0; i < 9; ++i)
    {
        entries[i] *= value;
    }
}

Matrix3& Matrix3::operator=(const Matrix3& other)
{
    for (int i = 0; i < 9; ++i)
        entries[i] = other.entries[i];
    return *this;
}
