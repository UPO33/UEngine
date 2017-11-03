struct VSOut
{
	float4 Pos : SV_Position;
	float4 Color : COLOROUT;
};

cbuffer ConstantBuffer : register(b0)
{
	float4 colorMultiplier;
};

float4 PS(in VSOut input) : SV_Target0
{
	return input.Color * colorMultiplier;
};