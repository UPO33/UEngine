

void Main(
#ifdef HAS_INPUT
in float2 inPos : POSITION,
in float3 inColor : COLOR,
#endif
in uint vid : SV_VertexID,
out float4 outPosition : SV_Position,
out float3 outColor : COLOR
)
{
#ifdef HAS_INPUT
    outPosition = float4(inPos, 0, 1);
    outColor = inColor;
#else
    float4 corners[] = { float4(-1, 1, 0.001, 1), float4(1, 1, 0001, 1), float4(-1, -1, 0001, 1) };
    outPosition = corners[vid % 3];
    outColor = float3(1, 0, 0);

#endif
}