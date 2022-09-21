#pragma once
#include "engineMath.h"
#include "Graphics.h"
#include <stdint.h>

struct MeshLayout
{
    Vector3 pos;
    Graphics::Color4 color;
};

struct BasicMeshLayout
{
    Vector3 position;
    Vector3 normal; 
    Graphics::Color4 color;
    Vector2 textCoord;
};

struct PhongUnlitLayout
{
    Vector3 position;
    Vector3 normal;
    Vector2 textCoord;
};

struct SkinnedLayout
{
    Vector3 position;
    Vector3 normal;
    uint8_t boneIndices[4];
    uint8_t weights[4];
    Vector2 textCoord;
};

struct NormalLayout
{
    Vector3 position;
    Vector3 normal;
    Vector3 tangent;
    Vector2 textCoord;
};

struct CopyLayout
{
    Vector3 position;
    Vector2 textCoord;
};