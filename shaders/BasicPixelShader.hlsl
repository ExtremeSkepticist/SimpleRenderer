struct PS_INPUT
{
    float4 mPosition : SV_POSITION;
    float3 mDiffuse : TEXCOORD1;
};

struct PS_OUTPUT
{
    float4 mColor : SV_TARGET;
};

PS_OUTPUT main(PS_INPUT input)
{
    PS_OUTPUT output;
    output.mColor = float4(input.mDiffuse, 1);

    return output;
}

