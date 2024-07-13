#pragma once
#include <nlohmann/json.hpp>
using json = nlohmann::json;

float SafeGetFloat(const json& j, const std::string& key, size_t index, float defaultValue);

Vector3 SafeGetVector3(const json& j, const std::string& key, const Vector3& defaultValue);