struct PS_INPUT
{
    float4 mPosition : SV_POSITION;
};

struct PS_OUTPUT
{
    float4 mColor : SV_TARGET;
};

PS_OUTPUT main(PS_INPUT input)
{
    PS_OUTPUT output;
    output.mColor = float4(1, 0, 0, 1);

    return output;
}

