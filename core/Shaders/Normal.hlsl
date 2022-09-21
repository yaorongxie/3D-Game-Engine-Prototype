#include "Constants.hlsl"

struct VIn
{
    float3 position : POSITION0;
    float3 normal : NORMAL0; 
    float3 tangent: TANGENT0;
    float2 textCord : TEXCOORD0;
};

struct VOut
{
    float4 viewPos : SV_POSITION;
    float4 worldPos : POSITION0; // Save the pixel's world position
    float4 normal : NORMAL0;
    float4 tangent: TANGENT0;
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
    output.normal = normalize(mul(float4(vIn.normal, 0.0), c_modelToWorld));

    // Transform tangent (direction vector, the fourth term should be 0.0f) 
    // from model to world space
    output.tangent = normalize(mul(float4(vIn.tangent, 0.0), c_modelToWorld));

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

    // Re-normalize the normal and tangent
    float3 n = normalize(pIn.normal).xyz;
    float3 t = normalize(pIn.tangent).xyz;
    // Calculate the bitangent and then normalize it
    float3 b = normalize(cross(n, t));

    // Create the TBN matrix
    float3x3 tbnMatrix =
    {
        t.x, t.y, t.z,
        b.x, b.y, b.z,
        n.x, n.y, n.z
    };

    // ************ Normal Mapping ************ //
    // Sample the normal map using the given UV coordinates
    float4 textureNormal = NormalTexture.Sample(DefaultSampler, pIn.textCord);
    // Un-bias the normal
    float3 unbiasedNormal = 2 * textureNormal.xyz - float3(1.0f, 1.0f, 1.0f);
    // Transform from tangent space into world space
    float3 worldNormal = normalize(mul(unbiasedNormal, tbnMatrix));

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
                float diffuseDot = max(dot(worldNormal, l), 0.0f);
                float3 diffuseContribution = c_diffuseColor * diffuseDot;
                diffuseContribution *= falloff;

                // Reflection Vector
                float3 r = normalize(-reflect(l, worldNormal));
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
