cmake_minimum_required(VERSION 3.10)
project(DX11)

# vcpkg 경로 설정
set(VCPKG_ROOT "${CMAKE_SOURCE_DIR}/vcpkg")
set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")

# 소스 파일 및 헤더 파일 경로 설정
include_directories(${PROJECT_SOURCE_DIR}/DX11/*.h)
file(GLOB SOURCES "${PROJECT_SOURCE_DIR}/DX11/*.cpp")

# 실행 파일 생성
add_executable(DX11 DX11/Main.cpp)

# nlohmann/json 라이브러리 찾기
set(CMAKE_PREFIX_PATH "${VCPKG_ROOT}/installed/x64-windows")
find_package(nlohmann_json CONFIG REQUIRED)

# nlohmann/json 라이브러리 링크
target_link_libraries(DX11 PRIVATE nlohmann_json::nlohmann_json)

