SamplerState gSampler : register(s0);
Texture2D gTexture : register(t0);

struct PSIn
{
    float2 uv : UV;
    float4 color : COLOR;
};

float4 Main(in PSIn input) : SV_Target0
{
    return gTexture.Sample(gSampler, input.uv) * input.color;
}