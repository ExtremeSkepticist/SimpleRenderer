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
    output.mColor = float4(cos(input.mPosition.x), 0, 0, cos(input.mPosition.z));

    return output;
}

