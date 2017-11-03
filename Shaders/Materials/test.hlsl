struct VertexIn
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 uv : UV;
#ifdef HAS_TANGENT
    float3 tangent : TANGENT;
#endif
#ifdef HAS_VERTEX_COLOR
    float4 color : COLOR;
#endif
};

struct VertexOut
{
    float4 position : Sv_Position;

};

void VSDepthOnly(
in float3 vertexLocal : POSITION,
in float4x4 localToClip : LOCALTOCLIP,
out float4 vertexClip : Sv_Position
)
{
    vertexClip = mul(localToClip, float4(vertexLocal, 0));
}
float DepthOnlyPS(in float4 position : Sv_Position) : SV_Target0
{

}
void VSMain(in VertexIn input, out VertexOut output)
{
    
}

float4 PSDepthOnly() : SV_Target0
{

};