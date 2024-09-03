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
    float3 mTangent : NORMAL0;
    float3 mBiNormal : NORMAL0;
};

struct VS_OUTPUT
{
    float4 mPosition : SV_POSITION;
    float mDiffuse : TEXCOORD1;
};

// TODO: fixing calculating color
VS_OUTPUT main(VS_INPUT input)
{
    float4 vertex = float4(input.mPosition, 1);

    VS_OUTPUT output;
    output.mPosition = mul(vertex, mWorldMatrix);
    
    float3 lightPosition = float3(0, 5, -1); // TODO: update light position in main.cpp
    float3 lightDir = normalize(float3(lightPosition - output.mPosition.xyz));
    
    output.mPosition = mul(output.mPosition, mViewMatrix);
    output.mPosition = mul(output.mPosition, mProjectionMatrix);

    float3x3 Tbn = float3x3(input.mTangent, input.mBiNormal, input.mNormal);
    Tbn = transpose(Tbn);
    float3 worldNormal = mul(Tbn, input.mNormal);

    output.mDiffuse = dot(lightDir, worldNormal);
    
    return output;
}