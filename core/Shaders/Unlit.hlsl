#include "Constants.hlsl"

struct VIn
{
    float3 position : POSITION0;
    float3 normal : NORMAL0; 
    float2 textCord : TEXCOORD0;
};

struct VOut
{
    float4 viewPos : SV_POSITION;
    float4 worldPos : POSITION1; // Save the pixel's world position
    float3 normal : NORMAL0;
    float2 textCord : TEXCOORD0;
};

VOut VS(VIn vIn)
{
    VOut output;

    // Transform position (position vector, the fourth term should be 1.0f) 
    // from model space to world space
    output.worldPos = mul(float4(vIn.position, 1.0), c_modelToWorld);
    // Transform position from world space to view space
    output.viewPos = mul(output.worldPos, c_viewProj);

    // Transform normal (direction vector, the fourth term should be 0.0f) 
    // from model to world space
    float4 ret = normalize(mul(float4(vIn.normal, 0.0), c_modelToWorld));
    output.normal = ret.xyz;

    // Get the texture coordinate from vertex data
    output.textCord = vIn.textCord;

    return output;
}

float4 PS(VOut pIn) : SV_TARGET
{
    // Texture color
    float4 textureColor = DiffuseTexture.Sample(DefaultSampler, pIn.textCord);

    return textureColor;
}
