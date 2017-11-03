#include "DeferredCommon.hlsl"

struct PerDrawData
{
    float4x4 worldMatrix;
#ifdef USE_HITECTION
    uint    hitID;
#endif
};

cbuffer CBPerDraw : register(b2)
{
    PerDrawData gPerDraw;
};

Texture2D gDiffuseTexture : register(MaterialTextureSlot);
SamplerState gDiffuseSampler : register(MaterialSamplerSlot);

struct PerMaterialData
{
    float4 mColor;
    float2 mUVOffset;
    float2 mUVScale;
	/////add your material parameters here
};

cbuffer CBPerMaterial : register(MaterialCBufferSlot)
{
    PerMaterialData gMaterial;
};


struct VSInData
{
    float3 position : POSITION;
    float3 normal : NORMAL;

#ifdef NEED_VERTEX_TANGENT
    float3 tangent : TANGENT;                  
#endif
#ifdef NEED_VERTEX_COLOR
    float4  color;
#endif
    float2 uv : UV;
#ifdef SKINNED_MESH
	uin4		mBoneIDS : BONEID;
	float4		mBoneWeights : BONEWIGHT;
#endif
};

struct PSInData
{
    float4 position : SV_Position;
    float3 positionWS : WORLDPOS;
    float3 normal : NORMAL;
    float2 uv : UV;
    
};

struct PSOutData
{
    float4 gbufferA : SV_Target0;
    float4 gbufferB : SV_Target1;
    float4 gbufferC : SV_Target2;
#ifdef USE_HITECTION
    uin4 hit : SV_Target3
#endif 
};


void VSMain(in VSInData input, out PSInData output)
{
    float4 worldPos = mul(gPerDraw.worldMatrix, float4(input.position, 1));
    output.positionWS = worldPos;
    output.normal = mul((float3x3) gPerDraw.worldMatrix, input.normal);
    //float4x4 mat = mul(mul(gCamera.mProjection, gCamera.mView), gObject.mWorldMatrix);
    //output.position = mul(mat, float4(input.position, 1));
    output.position = mul(gCamera.mWorldToCilp, worldPos);
#ifdef APPLY_UV_TRANSFORMATION
    output.uv = input.uv * gMaterial.mUVScale + gMaterial.mUVOffset;
#else
    output.uv = input.uv;
#endif
}

void PSMain(in PSInData input
    ,out float4 gbufferA : SV_Target0
    ,out float4 gbufferB : SV_Target1
    ,out float4 gbufferC : SV_Target2
#ifdef USE_HITECTION
    ,out float4 outHitID : SV_Target3
#endif 
)
{
    GBufferData gbuffer;

    gbuffer.WorldPos = input.positionWS;
    gbuffer.Normal = normalize(input.normal);
    gbuffer.DiffuseColor = gDiffuseTexture.Sample(gDiffuseSampler, input.uv).rgb;

    EncodeGBuffer(gbuffer, gbufferA, gbufferB, gbufferC);
#ifdef USE_HITECTION
    outHitID = gPerDraw.hitID;
#endif
};