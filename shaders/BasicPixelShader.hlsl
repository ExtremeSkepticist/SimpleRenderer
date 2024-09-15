struct PS_INPUT
{
    float4 mPosition : SV_POSITION;
    float2 mTexCoord : TEXCOORD0;
    float mDiffuse : TEXCOORD1;
};

struct PS_OUTPUT
{
    float4 mColor : SV_TARGET;
};

Texture2D g_Albedo : register(t0);
SamplerState g_Sampler : register(s0);

PS_OUTPUT main(PS_INPUT input)
{
    PS_OUTPUT output;
    output.mColor = g_Albedo.Sample(g_Sampler, input.mTexCoord);
   // output.mColor = float4(1.0f, 0.0f, 0.0f, 1.0f);
    return output;
}

