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
    float3 mNormal : TEXCOORD3;
    float3 mTangent : TEXCOORD4;
    float3 mBiNormal : TEXCOORD5;
    float3 mWorldPosition : TEXCOORD6;
    // float3 mLightPosition : TEXCOORD7;
};

// TODO: fixing calculating color
VS_OUTPUT main(VS_INPUT input)
{
    float4 vertex = float4(input.mPosition, 1);

    VS_OUTPUT output;
    output.mPosition = mul(vertex, mWorldMatrix);
    output.mWorldPosition = output.mPosition.xyz;
    
    output.mPosition = mul(output.mPosition, mViewMatrix);
    output.mPosition = mul(output.mPosition, mProjectionMatrix);

    output.mTangent = mul(input.mTangent, (float3x3)mWorldMatrix);
    output.mNormal = mul(input.mNormal, (float3x3)mWorldMatrix);
    output.mBiNormal = mul(input.mBiNormal, (float3x3)mWorldMatrix);

    // output.mLightPosition = mLightPositions[0].xyz;

    output.mTexCoord = input.mTexCoords;

    return output;
}