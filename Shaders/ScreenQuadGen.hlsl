#pragma once

cbuffer CBTest0 : register(b0)
{
    float4x4 gMatrix;
};



struct VSIn
{
    float2 pos : POSITION;
    float2 size : SIZE;
};

struct VSOut
{
    float2 pos : POS;
    float2 size : SIZE;
};

void VS(
in VSIn input,
out VSOut output
)
{
    float4 tp = mul(gMatrix, float4(input.pos, 0, 1));
    output.pos = tp.xy;
    output.size = input.size;
};

struct GSOut
{
    float4 positon : SV_Position;
    float2 uv : UV;
    uint pid : PID;
};

//we expand each point to a quad so the maximum number of vertices we output per geometry shader is 4
[maxvertexcount(4)]
void GS(
//vertex shader outs point
point VSOut input[1],
//for line it would be: line VSOut input[2],
//for triangle it would be: traiangle VSOut input[3]
//
in uint pid : SV_PrimitiveID,
inout TriangleStream<GSOut> output
)
{
    float2 inPos = input[0].pos;
    float2 inSize = input[0].size * 0.5;

    float2 tl = float2(inPos.x - inSize.x, inPos.y + inSize.y); //top left
    float2 tr = float2(inPos.x + inSize.x, inPos.y + inSize.y); //top right
    float2 br = float2(inPos.x + inSize.x, inPos.y - inSize.y); //bottom right
    float2 bl = float2(inPos.x - inSize.x, inPos.y - inSize.y); //bottom left

    
    GSOut tmp;
    tmp.pid = pid;

    float4 offset = float4(0, 0, 0, 0);

    //generating first triangle
    {
        tmp.positon = float4(tr, 0, 1) + offset;
        tmp.uv = float2(1, 0);
        output.Append(tmp);

        tmp.positon = float4(br, 0, 1) + offset;
        tmp.uv = float2(1, 1);
        output.Append(tmp);


        tmp.positon = float4(tl, 0, 1) + offset;
        tmp.uv = float2(0, 0);
        output.Append(tmp);
    }
    //generat the second triangle
    //note that outputstream is stip, I can call output.RestartStrip() and then send 3 new vertices of a new triangle
    //this solution cant generate same winding order for triangles but is easier and consume low memory
    tmp.positon = float4(bl, 0, 1) + float4(0, 0, 0, 0);
    tmp.uv = float2(0, 1);
    output.Append(tmp);

    //

    
};

//SRV == tN, CBV  == cN UAV == uN, Sampler sN

SamplerState gSampler0 : register(s0);


Texture2D gTexture0 : register(t2);
Texture2D gTexture1 : register(t3);
Texture2D gTexture2 : register(t4);


StructuredBuffer<float4> gColorBuffer0 : register(t0);
StructuredBuffer<float4> gColorBuffer1 : register(t1);



float4 PS(in GSOut input) : SV_Target
{
   //return float4(input.uv, 0, 1);
    
    float4 tc;

    input.pid = input.pid % 3;

    if(input.pid == 0)
        tc = gTexture0.Sample(gSampler0, input.uv);
    else if(input.pid == 1)
        tc = gTexture1.Sample(gSampler0, input.uv);
    else
        tc = gTexture2.Sample(gSampler0, input.uv);

    return tc + gColorBuffer0[input.pid];

    //float4 tc = gTexture0.Load(int3(input.uv * 32, 0));
    //return float4(tc.rgb + gColorBuffer1[input.pid], 1);

    //return float4(gColorBuffer1[input.pid] + tc);
   //return gColor;
}
