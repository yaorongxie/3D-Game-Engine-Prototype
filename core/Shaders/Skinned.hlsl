#include "Constants.hlsl"

struct VIn
{
    float3 position : POSITION0;
    float3 normal : NORMAL0; 
    uint4 boneIndex: BONE0;
    float4 weight : WEIGHT0;
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

    // Skin the vertex position  
    float4 Vpx = vIn.weight.x * (mul(float4(vIn.position, 1.0f), c_skinMatrix[vIn.boneIndex.x]));
    float4 Vpy = vIn.weight.y * (mul(float4(vIn.position, 1.0f), c_skinMatrix[vIn.boneIndex.y]));
    float4 Vpz = vIn.weight.z * (mul(float4(vIn.position, 1.0f), c_skinMatrix[vIn.boneIndex.z]));
    float4 Vpw = vIn.weight.w * (mul(float4(vIn.position, 1.0f), c_skinMatrix[vIn.boneIndex.w]));
    float4 Vpf = Vpx + Vpy + Vpz + Vpw;
    // Transform position from model space to world space
    output.worldPos = mul(Vpf, c_modelToWorld);
    // Transform position from world space to view space
    output.viewPos = mul(output.worldPos, c_viewProj);

    // Skin the normal
    float4 Vnx = vIn.weight.x * (mul(float4(vIn.normal, 0.0f), c_skinMatrix[vIn.boneIndex.x]));
    float4 Vny = vIn.weight.y * (mul(float4(vIn.normal, 0.0f), c_skinMatrix[vIn.boneIndex.y]));
    float4 Vnz = vIn.weight.z * (mul(float4(vIn.normal, 0.0f), c_skinMatrix[vIn.boneIndex.z]));
    float4 Vnw = vIn.weight.w * (mul(float4(vIn.normal, 0.0f), c_skinMatrix[vIn.boneIndex.w]));
    float4 Vnf = Vnx + Vny + Vnz + Vnw;
    // Transform normal from model space to world space
    output.normal = normalize(mul(Vnf, c_modelToWorld)).xyz;

    // Get the texture coordinate from vertex data
    output.textCord = vIn.textCord;

    return output;
}

float4 PS(VOut pIn) : SV_TARGET
{
    float3 lightColor = c_ambient;

    // Pixel position in the world space
    float3 pixelPos = pIn.worldPos.xyz;
    // The camera position in the world space
    float3 cameraPos = c_cameraPosition;

    // Re-normalize the surface normal
    float3 n = normalize(pIn.normal);

    // Loop through every point light
    for (int i = 0; i < MAX_POINT_LIGHTS; i++)
    {   
        if (c_pointLight[i].isEnabled == true)
        {
            // Light position in the world space 
            float3 lightPos = c_pointLight[i].position;

            // Direction Vector to light #i
            float3 l = lightPos - pixelPos;

            // Distance from the light source to pixel
            float distance = length(l);

            if (distance > 0.0f)
            {
                float falloff = smoothstep(c_pointLight[i].outerRadius, c_pointLight[i].innerRadius, distance);

                // Diffuse Lighting Contribution
                l = normalize(l);
                float diffuseDot = max(dot(n, l), 0.0f);
                float3 diffuseContribution = c_diffuseColor * diffuseDot;
                diffuseContribution *= falloff;

                // Reflection Vector
                float3 r = normalize(-reflect(l, n));
                // Direction vector to camera
                float3 v = normalize(cameraPos - pixelPos);
                // Specular Lighting Contribution
                float specularDot = max(dot(r, v), 0.0f);
                float3 specularContribution = c_specularColor * pow(specularDot, c_specularPower);
                specularContribution *= falloff;

                // Combine diffuse and specular
                lightColor += (c_pointLight[i].lightColor * (diffuseContribution + specularContribution));
            }
        }
    }

    // Texture color
    float4 textureColor = DiffuseTexture.Sample(DefaultSampler, pIn.textCord);

    // Final color
    float4 finalColor = textureColor * float4(lightColor, 1.0f);

    return finalColor;
}
