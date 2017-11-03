
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

void Main(in VSIn input, out PSIn output)
{
    output.Position = mul(gProjectionMatrix, float4(input.Position, 0.0f, 1.0f));
    output.UV = input.UV;
    output.Color = input.Color;
}