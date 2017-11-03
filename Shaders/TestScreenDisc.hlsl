struct VSData
{
    float2 pos : POSITION;
    float radius : RADIUS;
};

static float2 gPositions[] =
{
  float2(0,0)
};
static float gRadius[] =
{
  float(0.99)
};

struct Data
{
    float2 pos;
    float radius;
};

StructuredBuffer<Data> gDataRO : register(t0);

void VS(in uint vid : SV_VertexID, out VSData output)
{
    output.pos = gDataRO[vid].pos;
    output.radius = gDataRO[vid].radius;
}

struct PSIn
{
    float4 pos : SV_Position;
};

#define CIRCLE_STRIP 64

[maxvertexcount(CIRCLE_STRIP+1)]
void GS(point VSData input[1], inout LineStream<PSIn> output)
{
    float stepAngle = 2 * 3.141592 / CIRCLE_STRIP;

    PSIn tmp;

    [unroll]
    for (int i = 0; i <= CIRCLE_STRIP; i++)
    {
        float rad = i * stepAngle;
        
        float2 pos = input[0].pos + float2(sin(rad), cos(rad)) * input[0].radius;
        tmp.pos = float4(pos, 0, 1);
        output.Append(tmp);
    }

}

float4 PS(in PSIn input) : SV_Target
{
    return float4(0, 1, 0, 1);
}