#include "pch.h"
#include "JsonUtility.h"

float SafeGetFloat(const json& j, const std::string& key, size_t index, float defaultValue)
{
    try
    {
        float value = j.at(key).at(index).get<float>();
        if (std::isnan(value) || value == 0.0f)
        {
            return defaultValue;
        }
        return value;
    }
    catch (const std::exception& e)
    {
        // Log the error if needed
        return defaultValue;
    }
}

Vector3 SafeGetVector3(const json& j, const std::string& key, const Vector3& defaultValue)
{
    return {
        SafeGetFloat(j, key, 0, defaultValue.x),
        SafeGetFloat(j, key, 1, defaultValue.y),
        SafeGetFloat(j, key, 2, defaultValue.z)
    };
}
