
struct PerObjectData
{
    float4x4 LocalToWorld;
};

struct PerCameraData
{
    float4x4 LocalToClip;
};

cbuffer CBPerCamera : register(b0)
{
    PerCameraData gCamera;
};

cbuffer CBPerObject : register(b1)
{
    PerObjectData gObject;
};

struct VSIn
{
    float3 position : POSITION;
    float3 normal : NORMAL;
};

struct VSOut
{
    float4 position : SV_Position;
    float3 normal : NORMAL;
};

void VS(
in VSIn input,
out VSOut output
)
{
#if 0
    float4 vertexWS = mul(gObject.LocalToWorld, float4(input.position, 1));
    float4 clip = mul(gCamera.WorldToClip, vertexWS);
#else
    float4 vclip = mul(gCamera.LocalToClip, float4(input.position, 1));
#endif

    output.position = vclip;
    output.normal = mul((float3x3)gCamera.LocalToClip, input.normal);
};

float4 PS(in VSOut input) : SV_Target
{
    float3 normal = normalize(input.normal);
    //return float4(normal + 1 * 0.5f, 1);
    return float4(input.position.w / 140.0f, 0, 0, 1);
    //return float4(1, 0, 0, 1);
}
