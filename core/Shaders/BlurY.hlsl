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
    float offset[5] = { 0.0, 1.0, 2.0, 3.0, 4.0 };
    float weight[5] = { 0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162 };

    // Vertical Pass
    float4 finalColor = OffScreenTexture.Sample(DefaultSampler, pIn.textCoord) * weight[0];

    // Up
    for (int i = 1; i < 5; i++)
    {
        finalColor += OffScreenTexture.Sample(DefaultSampler, (pIn.textCoord + (float2(0.0, offset[i]) / 150.0))) * weight[i];
    }
    // Down
    for (int i = 1; i < 5; i++)
    {
        finalColor += OffScreenTexture.Sample(DefaultSampler, (pIn.textCoord - (float2(0.0, offset[i]) / 150.0))) * weight[i];
    }

    return finalColor;
}
