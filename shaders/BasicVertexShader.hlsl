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
    float2 mTexCoords : TEXCOORD0;
    float3 mNormal : NORMAL;
    float3 mTangent : TANGENT;
    float3 mBiNormal : BINORMAL;
};

struct VS_OUTPUT
{
    float4 mPosition : SV_POSITION;
    float2 mTexCoord : TEXCOORD0;
    float3 mLightDir : TEXCOORD1;
    float mLightDistance : TEXCOORD2;
    float3 mNormal : TEXCOORD3;
    float3 mTangent : TEXCOORD4;
    float3 mBiNormal : TEXCOORD5;
};

// TODO: fixing calculating color
VS_OUTPUT main(VS_INPUT input)
{
    float4 vertex = float4(input.mPosition, 1);

    VS_OUTPUT output;
    output.mPosition = mul(vertex, mWorldMatrix);
    
    float3 pointLightPosition = float3(0, 6, 2); // TODO: update light position in main.cpp
    float3 surfaceToLightVector = float3(pointLightPosition - output.mPosition.xyz);
    output.mLightDir = normalize(surfaceToLightVector);
    output.mLightDistance = length(surfaceToLightVector);
    
    output.mPosition = mul(output.mPosition, mViewMatrix);
    output.mPosition = mul(output.mPosition, mProjectionMatrix);

    output.mTangent = input.mTangent;
    output.mNormal = input.mNormal;
    output.mBiNormal = input.mBiNormal;

    output.mTexCoord = input.mTexCoords;

    return output;
}