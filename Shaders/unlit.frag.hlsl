Texture2D<float4> u_texture : register(t0, space2);
SamplerState u_sampler : register(s0, space2);

cbuffer Uniforms : register(b0, space3)
{
    float4 color;
}

struct Input
{
    float2 texcoord : TEXCOORD0;
    float4 color : TEXCOORD1;
};

struct Output
{
    float4 color : SV_Target0;
};

Output main(Input input)
{
    Output output;
    u_texture.Sample(u_sampler, input.texcoord) * input.color;

        return output;
}
		
