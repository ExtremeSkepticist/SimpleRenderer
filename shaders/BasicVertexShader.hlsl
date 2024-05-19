struct VS_INPUT
{
    float mPosition : POSITION;
};

struct VS_OUTPUT
{
    float4 mPosition : SV_POSITION;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    output.mPosition = saturate(0);
    
    return output;
}