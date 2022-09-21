#include "Constants.hlsl"

struct VIn
{
    float3 position : POSITION0;
    float2 textCoord : TEXCOORD0;
};

struct VOut
{
    float4 position : SV_POSITION;
    float2 textCoord : TEXCOORD0;
};

VOut VS(VIn vIn)
{
    VOut output;

    // Just copy vertices 
    // The forth term of a position vector is 1!!!
    output.position = float4(vIn.position, 1.0f);
    output.textCoord = vIn.textCoord;

    return output;
}

float4 PS(VOut pIn) : SV_TARGET
{
    float4 finalColor = OffScreenTexture.Sample(DefaultSampler, pIn.textCoord);

    if (finalColor.x >= 0.8 || finalColor.y >= 0.8 || finalColor.z >= 0.8)
    {
        return finalColor;
    }
    else
    {
        return float4(0.0, 0.0, 0.0, 1.0);
    }

    return finalColor;
}
