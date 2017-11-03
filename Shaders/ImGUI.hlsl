
struct VSIn
{
    float2 Position : POSITION;
    float2 UV : UV;
    float4 Color : COLOR;
};

struct PSIn
{
    float4 Position : SV_Position;
    float2 UV : UV;
    float4 Color : COLOR;
};

cbuffer CB : register(b0)
{
    float4x4 gProjectionMatrix;
};

float2 Screen2NDC(float2 screen, float2 size)
{
    float2 uv = screen / size;
    return uv * float2(2, -2) - float2(1, -1);
}
void VSMain(in VSIn input, out PSIn output)
{
    output.Position = mul(gProjectionMatrix, float4(input.Position, 0.0f, 1.0f));
    //output.Position = mul(gProjectionMatrix, float4(Screen2NDC(input.Position, float2(800, 600)), 0.0f, 1.0f));
    output.UV = input.UV;
    output.Color = input.Color;
}

SamplerState gSampler : register(s4);
Texture2D gTexture : register(t0);



float4 PSMain(in PSIn input) : SV_Target0
{
    return gTexture.Sample(gSampler, input.UV) * input.Color;
}