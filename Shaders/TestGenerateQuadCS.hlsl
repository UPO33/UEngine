struct QuadData
{
    float2 position;
    float2 size;
};


StructuredBuffer<QuadData> gInput : register(t0); //SRV
RWStructuredBuffer<float2> gOutTris : register(u0); // UAV


[numthreads(1, 1, 1)]
void CS(uint3 groupID : SV_GroupID, uint3 threadID : SV_GroupThreadID, uint3 dispatchID : SV_DispatchThreadID, uint gindex : SV_GroupIndex)
{
    uint index = gindex;
    
    float2 inPos = gInput[index].position;
    float2 inSize = gInput[index].size;

    float2 tl = float2(inPos.x - inSize.x, inPos.y + inSize.y); //top left
    float2 tr = float2(inPos.x + inSize.x, inPos.y + inSize.y); //top right
    float2 br = float2(inPos.x + inSize.x, inPos.y - inSize.y); //bottom right
    float2 bl = float2(inPos.x - inSize.x, inPos.y - inSize.y); //bottom left

    index *= 6;

    gOutTris[index + 0] = tl;
    gOutTris[index + 1] = tr;
    gOutTris[index + 2] = bl;

    gOutTris[index + 3] = tl;
    gOutTris[index + 4] = br;
    gOutTris[index + 5] = bl;
}