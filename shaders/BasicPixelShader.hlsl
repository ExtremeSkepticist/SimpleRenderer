struct PS_INPUT
{
    float4 mPosition : SV_POSITION;
    float mDiffuse : TEXCOORD1;
};

struct PS_OUTPUT
{
    float4 mColor : SV_TARGET;
};

PS_OUTPUT main(PS_INPUT input)
{
    PS_OUTPUT output;
    float4 color = saturate(input.mDiffuse);
    output.mColor = color;

    return output;
}

