struct PS_INPUT
{
    float4 mPosition : SV_POSITION;
    float2 mTexCoord : TEXCOORD0;
    float3 mNormal : TEXCOORD3;
    float3 mTangent : TEXCOORD4;
    float3 mBiNormal : TEXCOORD5;
    float3 mWorldPosition : TEXCOORD6;
};

struct PS_OUTPUT
{
    float4 mColor : SV_TARGET;
};

struct LightSource
{
    float3 Color;
    float Intensity;
    float3 Position;
};
Texture2D g_Albedo : register(t0); // sRGB texture
Texture2D g_Normal : register(t1);
Texture2D g_metallicRoughness : register(t2);

SamplerState g_Sampler : register(s0);

float SchlickGGX(const float NdotV, const float remappedRoughness)
{
    return NdotV / (NdotV * (1 - remappedRoughness) + remappedRoughness);
}

PS_OUTPUT main(PS_INPUT input)
{
    PS_OUTPUT output;
    
    // TODO: update light position in main.cpp
    // TODO: Figure out how to limit the boundary of light intensity
    // TODO: Update CameraWorldPosition via const buffer
    const float3 cameraWorldPosition = float3(0, 6, 0);
    const uint incomingLightCount = 2;
    
    // Assumes only point lights
    LightSource lightSources[incomingLightCount] =
    {
        {
            float3(1.0f, 1.0f, 1.0f), 2.0f, float3(0, 6, 2)
        },
        {
            float3(1.0f, 1.0f, 1.0f), 2.0f, float3(0, 10, 0)
        }
    };
    
    // Compute common components
    float3x3 Tbn = float3x3(input.mTangent, input.mBiNormal, input.mNormal);
    Tbn = transpose(Tbn);
    float3 tangentNormal = g_Normal.Sample(g_Sampler, input.mTexCoord).rgb;
    tangentNormal = normalize(tangentNormal * 2 - 1);
    
    const float3 N = mul(Tbn, tangentNormal);
    const float3 V = normalize(cameraWorldPosition - input.mWorldPosition);
    const float PI = radians(180);

    const float3 albedo = pow(g_Albedo.Sample(g_Sampler, input.mTexCoord).xyz, 2.2); // TODO: can I gamma correct this sRGB texture with DXGI?
    const float metalness = g_metallicRoughness.Sample(g_Sampler, input.mTexCoord).b;
    const float roughness = g_metallicRoughness.Sample(g_Sampler, input.mTexCoord).g;
        
    float3 finalColor = saturate(0);
    for (uint i = 0; i < incomingLightCount; ++i)
    {
        const float3 L = normalize(lightSources[i].Position - input.mWorldPosition);
        const float3 H = normalize(V + L);
    
        // Calulate F term with fresnel-schlick
        float3 F = saturate(0.0f);
        {
            const float3 nonMetalF0 = saturate(0.04);
            const float3 F0 = lerp(nonMetalF0, albedo, metalness); 
            F = F0 + (1.0 - F0) * pow(1.0 - saturate(dot(H, V)), 5);
        }
        // Calculate D term with Trowbridge-Reitz GGX
        float D = 0;
        {
            const float numerator = roughness * roughness;
            float denominator = pow(saturate(dot(N, H)), 2) * (numerator - 1) + 1;
            denominator = denominator * denominator * PI;

            D = numerator / denominator;
        }
        // Calculate G term with Schlick-GGX
        float G = 0;
        {
            const float remappedRoughness = pow(roughness + 1, 2) / 8; // For direct lighting
            const float ggxWithViewVector = SchlickGGX(saturate(dot(N, V)), remappedRoughness);
            const float ggxWithLightVector = SchlickGGX(saturate(dot(N, L)), remappedRoughness);

            G = ggxWithViewVector * ggxWithLightVector;
        }

        float3 reflection = saturate(0.0f);
        {
            float3 numerator = D * G * F;
            float denominator = 4 * saturate(dot(N, V)) * saturate(dot(N, L));
            denominator += 0.000001f; // Prevents divide-by-zero;
        
            reflection = numerator / denominator;
        }
        
        const float3 kS = F;
        const float3 kD = (float3(1, 1, 1) - kS) * 1.0 - metalness; // Metals don't refract light. Therefore, neturalize it if it's metallic with 1 - metallness

        const float intensityWithAttenuation = lightSources[i].Intensity * rcp(length(L) * length(L));
        const float3 incomingRadiance = lightSources[i].Color * intensityWithAttenuation;
        const float lambert = saturate(dot(N, L));

        finalColor += (kD * albedo + reflection) * incomingRadiance * lambert;
        
    }
    
    // TODO: proper gamma correction, tone mapping, ambient occlusion
    // finalColor += 0.01 * albedo * 100; // ambient light - TODO: read from ao map
    // finalColor = finalColor / (finalColor + float3(1, 1, 1)); // Reinhard tone mapping
    finalColor = pow(finalColor, 1 / 2.2); // Gamma correction

    output.mColor = float4(finalColor.xyz, 1.0f);

    return output;
}

