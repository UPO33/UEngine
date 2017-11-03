
struct AppData
{
    float3 position : POSITION;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
    float3 normal : NORMAL;
    float2 texCoord : TEXCOORD0;
};

cbuffer PerObject : register(b0)
{
    float4x4 ModelViewProjection;
    float4x4 ModelView;
}

struct VertexShaderOutput
{
    float3 positionVS : TEXCOORD0; // View space position.
    float2 texCoord : TEXCOORD1; // Texture coordinate
    float3 tangentVS : TANGENT; // View space tangent.
    float3 binormalVS : BINORMAL; // View space binormal.
    float3 normalVS : NORMAL; // View space normal.
    float4 position : SV_POSITION; // Clip space position.
};

VertexShaderOutput VS_main(AppData IN)
{
    VertexShaderOutput OUT;
 
    OUT.position = mul(ModelViewProjection, float4(IN.position, 1.0f));
 
    OUT.positionVS = mul(ModelView, float4(IN.position, 1.0f)).xyz;
    OUT.tangentVS = mul((float3x3) ModelView, IN.tangent);
    OUT.binormalVS = mul((float3x3) ModelView, IN.binormal);
    OUT.normalVS = mul((float3x3) ModelView, IN.normal);
 
    OUT.texCoord = IN.texCoord;
 
    return OUT;
};

struct Material
{
    float4 GlobalAmbient;
    //-------------------------- ( 16 bytes )
    float4 AmbientColor;
    //-------------------------- ( 16 bytes )
    float4 EmissiveColor;
    //-------------------------- ( 16 bytes )
    float4 DiffuseColor;
    //-------------------------- ( 16 bytes )
    float4 SpecularColor;
    //-------------------------- ( 16 bytes )
    // Reflective value.
    float4 Reflectance;
    //-------------------------- ( 16 bytes )
    float Opacity;
    float SpecularPower;
    // For transparent materials, IOR > 0.
    float IndexOfRefraction;
    bool HasAmbientTexture;
    //-------------------------- ( 16 bytes )
    bool HasEmissiveTexture;
    bool HasDiffuseTexture;
    bool HasSpecularTexture;
    bool HasSpecularPowerTexture;
    //-------------------------- ( 16 bytes )
    bool HasNormalTexture;
    bool HasBumpTexture;
    bool HasOpacityTexture;
    float BumpIntensity;
    //-------------------------- ( 16 bytes )
    float SpecularScale;
    float AlphaThreshold;
    float2 Padding;
    //--------------------------- ( 16 bytes )
}; //--------------------------- ( 16 * 10 = 160 bytes )

cbuffer Material : register(b2)
{
    Material Mat;
};

Texture2D AmbientTexture : register(t0);
Texture2D EmissiveTexture : register(t1);
Texture2D DiffuseTexture : register(t2);
Texture2D SpecularTexture : register(t3);
Texture2D SpecularPowerTexture : register(t4);
Texture2D NormalTexture : register(t5);
Texture2D BumpTexture : register(t6);
Texture2D OpacityTexture : register(t7);

struct Light
{
    /**
    * Position for point and spot lights (World space).
    */
    float4 PositionWS;
    //--------------------------------------------------------------( 16 bytes )
    /**
    * Direction for spot and directional lights (World space).
    */
    float4 DirectionWS;
    //--------------------------------------------------------------( 16 bytes )
    /**
    * Position for point and spot lights (View space).
    */
    float4 PositionVS;
    //--------------------------------------------------------------( 16 bytes )
    /**
    * Direction for spot and directional lights (View space).
    */
    float4 DirectionVS;
    //--------------------------------------------------------------( 16 bytes )
    /**
    * Color of the light. Diffuse and specular colors are not seperated.
    */
    float4 Color;
    //--------------------------------------------------------------( 16 bytes )
    /**
    * The half angle of the spotlight cone.
    */
    float SpotlightAngle;
    /**
    * The range of the light.
    */
    float Range;
 
    /**
     * The intensity of the light.
     */
    float Intensity;
 
    /**
    * Disable or enable the light.
    */
    bool Enabled;
    //--------------------------------------------------------------( 16 bytes )
 
    /**
     * Is the light selected in the editor?
     */
    bool Selected;
 
    /**
    * The type of the light.
    */
    uint Type;
    float2 Padding;
    //--------------------------------------------------------------( 16 bytes )
    //--------------------------------------------------------------( 16 * 7 = 112 bytes )
};

StructuredBuffer<Light> Lights : register(t8);
/*
Since all of the lighting computations will be performed in view space, the eye position (the position of the camera) is always (0, 0, 0).
This is a nice side effect of working in view space; The camera’s eye position does not need to be passed as an additional parameter to the shader.
*/
float3 ExpandNormal( float3 n )
{
    return n * 2.0f - 1.0f;
};
 
float4 DoNormalMapping( float3x3 TBN, Texture2D tex, sampler s, float2 uv )
{
    float3 normal = tex.Sample( s, uv ).xyz;
    normal = ExpandNormal( normal );
 
    // Transform normal from tangent space to view space.
    normal = mul( normal, TBN );
    return normalize( float4( normal, 0 ) );
}
float4 DoBumpMapping( float3x3 TBN, Texture2D tex, sampler s, float2 uv, float bumpScale )
{
    // Sample the heightmap at the current texture coordinate.
    float height = tex.Sample( s, uv ).r * bumpScale;
    // Sample the heightmap in the U texture coordinate direction.
    float heightU = tex.Sample( s, uv, int2( 1, 0 ) ).r * bumpScale;
    // Sample the heightmap in the V texture coordinate direction.
    float heightV = tex.Sample( s, uv, int2( 0, 1 ) ).r * bumpScale;
 
    float3 p = { 0, 0, height };
    float3 pU = { 1, 0, heightU };
    float3 pV = { 0, 1, heightV };
 
    // normal = tangent x bitangent
    float3 normal = cross( normalize(pU - p), normalize(pV - p) );
 
    // Transform normal from tangent space to view space.
    normal = mul( normal, TBN );
 
    return float4( normal, 0 );
}
// This lighting result is returned by the 
// lighting functions for each light type.
struct LightingResult
{
    float4 Diffuse;
    float4 Specular;
};
 
LightingResult DoLighting( StructuredBuffer<Light> lights, Material mat, float4 eyePos, float4 P, float4 N )
{
    float4 V = normalize( eyePos - P );
 
    LightingResult totalResult = (LightingResult)0;
 
    for ( int i = 0; i < NUM_LIGHTS; ++i )
    {
        LightingResult result = (LightingResult)0;
 
        // Skip lights that are not enabled.
        if ( !lights[i].Enabled ) continue;
        // Skip point and spot lights that are out of range of the point being shaded.
        if ( lights[i].Type != DIRECTIONAL_LIGHT &&
             length( lights[i].PositionVS - P ) > lights[i].Range ) continue;
 
        switch ( lights[i].Type )
        {
        case DIRECTIONAL_LIGHT:
        {
            result = DoDirectionalLight( lights[i], mat, V, P, N );
        }
        break;
        case POINT_LIGHT:
        {
            result = DoPointLight( lights[i], mat, V, P, N );
        }
        break;
        case SPOT_LIGHT:
        {
            result = DoSpotLight( lights[i], mat, V, P, N );
        }
        break;
        }
        totalResult.Diffuse += result.Diffuse;
        totalResult.Specular += result.Specular;
    }
 
    return totalResult;
}

[earlydepthstencil]
float4 PS_main( VertexShaderOutput IN ) : SV_TARGET
{
    // Everything is in view space.
    float4 eyePos = { 0, 0, 0, 1 };
    Material mat = Mat;
    float4 diffuse = mat.DiffuseColor;
    if (mat.HasDiffuseTexture)
    {
        float4 diffuseTex = DiffuseTexture.Sample(LinearRepeatSampler, IN.texCoord);
        if (any(diffuse.rgb))
        {
            diffuse *= diffuseTex;
        }
        else
        {
            diffuse = diffuseTex;
        }
    }

    float alpha = diffuse.a;
    if (mat.HasOpacityTexture)
    {
    // If the material has an opacity texture, use that to override the diffuse alpha.
        alpha = OpacityTexture.Sample(LinearRepeatSampler, IN.texCoord).r;
    }

    float4 ambient = mat.AmbientColor;
    if (mat.HasAmbientTexture)
    {
        float4 ambientTex = AmbientTexture.Sample(LinearRepeatSampler, IN.texCoord);
        if (any(ambient.rgb))
        {
            ambient *= ambientTex;
        }
        else
        {
            ambient = ambientTex;
        }
    }
    // Combine the global ambient term.
    ambient *= mat.GlobalAmbient;
 
    float4 emissive = mat.EmissiveColor;
    if (mat.HasEmissiveTexture)
    {
        float4 emissiveTex = EmissiveTexture.Sample(LinearRepeatSampler, IN.texCoord);
        if (any(emissive.rgb))
        {
            emissive *= emissiveTex;
        }
        else
        {
            emissive = emissiveTex;
        }
    }

    if (mat.HasSpecularPowerTexture)
    {
        mat.SpecularPower = SpecularPowerTexture.Sample(LinearRepeatSampler, IN.texCoord).r * mat.SpecularScale;
    }



// Normal mapping
    if (mat.HasNormalTexture)
    {
    // For scenes with normal mapping, I don't have to invert the binormal.
        float3x3 TBN = float3x3(normalize(IN.tangentVS),
                             normalize(IN.binormalVS),
                             normalize(IN.normalVS));
 
        N = DoNormalMapping(TBN, NormalTexture, LinearRepeatSampler, IN.texCoord);
    }
// Bump mapping
    else if (mat.HasBumpTexture)
    {
    // For most scenes using bump mapping, I have to invert the binormal.
        float3x3 TBN = float3x3(normalize(IN.tangentVS),
                             normalize(-IN.binormalVS),
                             normalize(IN.normalVS));
 
        N = DoBumpMapping(TBN, BumpTexture, LinearRepeatSampler, IN.texCoord, mat.BumpIntensity);
    }
// Just use the normal from the model.
    else
    {
        N = normalize(float4(IN.normalVS, 0));
    }
}

float DoDiffuse(float3 N, float3 L)
{
	return max(dot(N, L));
}
float DoSpecular(R, V, Power)
{
	return  pow(max(dot(R, V), Power);
}

