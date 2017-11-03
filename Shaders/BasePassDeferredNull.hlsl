#include "DeferredCommon.hlsl"



void VSMain(in VSIn input, out PSIn output)
{
    float4 worldPos = mul(gObject.mWorldMatrix, float4(input.position, 1));
    output.positionWS = worldPos;
    output.normal = mul((float3x3) gObject.mWorldMatrix, input.normal);
    output.position = mul(gCamera.mWorldToCilp, worldPos);
    output.uv = input.uv;
};

struct VSInData
{
    float3 position : POSITION;
#ifdef NEED_NORMAL
    float3 normal : NORMAL;
#endif
#ifdef NEED_TANGENT
    float3 tangent : TANGENT;
#endif
#ifdef NEED_VERTEXCOLOR
    float4  color;
#endif
    float2 uv : UV;
#ifdef IS_INSTANCES
	float4x4	instanceLocalToWorld;
#endif
};

struct PSOutData
{
    float4 gbufferA : SV_Target0;
    float4 gbufferB : SV_Target1;
    float4 gbufferC : SV_Target2;
#ifdef USE_HITSELECTION
    float4 hitSelection : SV_Target3
#endif 
};

void VSMain(in VSInData input, out PSInData output)
{
    
}

void PSMain(in PSInData input, out PSOutData)
{
    
}

void PSMain(in PSIn input,
    out float4 gbufferA : SV_Target0,
    out float4 gbufferB : SV_Target1,
    out float4 gbufferC : SV_Target2)
{
    GBufferData gbuffer;

    gbuffer.depth = 0;
    gbuffer.WorldPos = input.positionWS;
    gbuffer.Normal = normalize(input.normal);
    gbuffer.DiffuseColor = float3(1, 1, 1);

    EncodeGBuffer(gbuffer, gbufferA, gbufferB, gbufferC);
};