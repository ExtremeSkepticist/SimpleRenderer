struct PS_INPUT
{
    float4 mPosition : SV_POSITION;
    float2 mTexCoord : TEXCOORD0;
    float3 mLightDir : TEXCOORD1;
    float mLightDistance : TEXCOORD2;
    float3 mNormal : TEXCOORD3;
    float3 mTangent : TEXCOORD4;
    float3 mBiNormal : TEXCOORD5;
};

struct PS_OUTPUT
{
    float4 mColor : SV_TARGET;
};

Texture2D g_Albedo : register(t0);
Texture2D g_Normal : register(t1);
Texture2D g_metallicRoughness : register(t2);

SamplerState g_Sampler : register(s0);

PS_OUTPUT main(PS_INPUT input)
{
    PS_OUTPUT output;
    float4 lightColor = saturate(1.0f);
    float lightIntensity = 2.0f;
    lightIntensity *= rcp(input.mLightDistance * input.mLightDistance); // how to apply light intensity in rendering equation?
    
        
    float3 normal = normalize(g_Normal.Sample(g_Sampler, input.mTexCoord).xyz);
    float3x3 Tbn = float3x3(input.mTangent, input.mBiNormal, input.mNormal);
    Tbn = transpose(Tbn);
    float3 worldNormal = mul(Tbn, normal);
    float lambert = dot(normal, input.mLightDir);
    float4 albedo = float4(g_metallicRoughness.Sample(g_Sampler, input.mTexCoord).xyz, 1.0f);
    
    albedo = albedo * lightColor * lightIntensity;
    albedo = albedo * lambert;
    output.mColor = albedo;

    return output;
}

