
struct VSIn
{
	float3 Pos : POSITION;
	float4 Color : COLOR;
};
struct VSOut
{
	float4 Pos : SV_Position;
	float4 Color : COLOROUT;
};
void VS(in VSIn input, out VSOut output)
{
	output.Pos = float4(input.Pos, 1);
	output.Color = input.Color;
};