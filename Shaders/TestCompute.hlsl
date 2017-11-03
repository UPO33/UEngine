
Texture2D<float3> gInput : register(t0);

//this means we get 64 threads per group
[numThreads(8,8,1)]
void Main(uint3 groupID : SV_GroupID, uint3 threadID : SV_GroupThreadID, uint3 dispatchID : SV_DispatchThreadID )
{
    /*
    uint3 groupID : SV_GroupID
    - Index of the group within the dispatch for each dimension

    uint3 groupThreadID : SV_GroupThreadID
    - Index of the thread within the group for each dimension
   
    uint groupIndex : SV_GroupIndex
    - A sequential index within the group that starts from 0 top left back and goes on to bottom right front

    uint3 dispatchThreadID : SV_DispatchThreadID
    - Global thread index within the whole dispatch

    */


    //SV_GroupID میشه اندیس ک تابع دیسپچ کرده
    //SV_GroupThreadID اندیسه ترد داخل این تابع ینی اندازه ای ک تو numThreads(x,y,z) دادایم
    //dispatchID اندس کلی
}