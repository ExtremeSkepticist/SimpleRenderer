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
};

struct VS_OUTPUT
{
    float4 mPosition : SV_POSITION;
};

VS_OUTPUT main(VS_INPUT input)
{
    float4 vertex = float4(input.mPosition, 1);
    VS_OUTPUT output;
    output.mPosition = mul(vertex, mWorldMatrix);
    output.mPosition = mul(output.mPosition, mViewMatrix);
    output.mPosition = mul(output.mPosition, mProjectionMatrix);
    
    return output;
}