// We want to use row major matrices
#pragma pack_matrix(row_major)

#define MAX_POINT_LIGHTS 8
#define MAX_SKELETON_BONES 80

Texture2D DiffuseTexture : register(t0);
Texture2D NormalTexture : register(t1);
Texture2D OffScreenTexture : register(t2);

SamplerState DefaultSampler : register(s0);

struct PointLightData
{
    float3 lightColor; // 12 bytes at 16-byte alignment
    float3 position;
    float innerRadius;
    float outerRadius;
    bool isEnabled;
};

cbuffer PerCameraConstants : register(b0)
{
    float4x4 c_viewProj;
    float3 c_cameraPosition;
};

cbuffer PerObjectConstants : register(b1)
{
    float4x4 c_modelToWorld;
};

cbuffer MaterialConstants : register(b2)
{
    float3 c_diffuseColor;
    float3 c_specularColor;
    float c_specularPower;
};

cbuffer LightingConstants : register(b3)
{
    float3 c_ambient;
    PointLightData c_pointLight[MAX_POINT_LIGHTS];
};

cbuffer SkinConstants : register(b4)
{
    float4x4 c_skinMatrix[MAX_SKELETON_BONES];
};