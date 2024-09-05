#pragma once

struct Matrix3
{
    float entries[9];

    /* ������ */
    Matrix3();            // ������ķ� �ʱ�ȭ
    Matrix3(float value); // ����� �밢���� (diagonal entry) �� �־��� ������ �ʱ�ȭ
    Matrix3(float v1, float v2, float v3);
    Matrix3(Quaternion q);

    /* ����� �밢������ �־��� ������ �����Ѵ�
        ������ ������ 0���� �����Ѵ� */
    void setDiagonal(float value);
    void setDiagonal(Vec3 vec);
    void setIdentity();

    /* ����� ��ġ����� ��ȯ�Ѵ� */
    Matrix3 transpose() const;

    /* ����� ������� ��ȯ�Ѵ� */
    Matrix3 inverse() const;

    /*****************
     * ������ �����ε� *
     *****************/

     /* ��ĳ��� ���ϱ� */
    Matrix3 operator+(const Matrix3& other) const;
    void operator+=(const Matrix3& other);

    /* ��ĳ��� ���� */
    Matrix3 operator-(const Matrix3& other) const;
    void operator-=(const Matrix3& other);

    /* ��ĳ��� ���ϱ� */
    Matrix3 operator*(const Matrix3& other) const;
    void operator*=(const Matrix3& other);

    /* ���Ϳ� ���ϱ� */
    Vector3 operator*(Vector3 vec) const;

    /* �Ǽ��� ���ϱ� */
    Matrix3 operator*(const float value) const;
    void operator*=(const float value);

    Matrix3& operator=(const Matrix3& other);
};