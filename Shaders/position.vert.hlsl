struct Input
{
    float3 position : TEXCOORD0;
};

struct Output
{
    float4 Position : SV_Position;
};

Output main(Input input)
{
    Output output;
    output.Position = float4(input.position, 1.0f);

    return output;
}