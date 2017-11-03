
cbuffer TestBuffer : register(b0)
{
    float4 gColor;
};
cbuffer TestBuffer2 : register(b1)
{
    float4 gColor2;
    float gOffset;
};

struct Data
{
    float mul;
};


float4 Main(
in float4 position : SV_Position,
in float3 inColor : COLOR) : SV_Target
{
    return float4(inColor, 1) * gColor2;
}