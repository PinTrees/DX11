#pragma once
#include <nlohmann/json.hpp>
using json = nlohmann::json;

float SafeGetFloat(const json& j, const std::string& key, size_t index, float defaultValue);

Vector3 SafeGetVector3(const json& j, const std::string& key, const Vector3& defaultValue);


json QuaternionToJSON(const Quaternion& q); 
Quaternion SafeGetQuaternion(const json& j, const std::string& key, const Quaternion& defaultValue);

#define SERIALIZE_QUATERNION(JSON, VALUE) JSON[#VALUE] = QuaternionToJSON(VALUE)   
#define DE_SERIALIZE_QUATERNION(JSON, VALUE) VALUE = SafeGetQuaternion(JSON, #VALUE, Quaternion::Identity)

#define SERIALIZE_VECTOR3(JSON, VALUE) JSON[#VALUE] = { VALUE.x, VALUE.y, VALUE.z };
#define DE_SERIALIZE_VECTOR3(JSON, VALUE) VALUE = SafeGetVector3(JSON, #VALUE, Vector3::Zero);
#define DE_SERIALIZE_VECTOR3_D(JSON, VALUE, Default) VALUE = SafeGetVector3(JSON, #VALUE, Default); 
