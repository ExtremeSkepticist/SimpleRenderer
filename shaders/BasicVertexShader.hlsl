cbuffer ConstBuffer
{
    float4x4 mWorldMatrix;
    float4x4 mViewMatrix;
    float4x4 mProjectionMatrix;
    float4x4 mUnused;
};

struct VS_INPUT
{
    float3 mPosition : POSITION;
    float2 mTexCoords : TEXCOORDS0;
    float3 mNormal : NORMAL0;
};

struct VS_OUTPUT
{
    float4 mPosition : SV_POSITION;
    float3 mDiffuse : TEXCOORD1;
};

// TODO: fixing calculating color
VS_OUTPUT main(VS_INPUT input)
{
    float4 vertex = float4(input.mPosition, 1);

    VS_OUTPUT output;
    output.mPosition = mul(vertex, mWorldMatrix);
    output.mPosition = mul(output.mPosition, mViewMatrix);
    output.mPosition = mul(output.mPosition, mProjectionMatrix);

    float4 worldNormal = mul(normalize(float4(input.mNormal, 0)), mWorldMatrix);
    float4 lightPosition = normalize(float4(0, 5, 2, 0)); // TODO: update light position in main.cpp
    output.mDiffuse = dot(lightPosition, worldNormal);
    
    return output;
}