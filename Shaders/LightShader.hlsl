cbuffer CameraConstantBuffer : register(b0)
{
    row_major matrix viewProjection;
    row_major matrix model;
    float3 cameraPosition;
};

struct RemLight
{
    float3 direction;
    float3 ambient;
    float3 diffuse;
    float3 specular;
};
cbuffer LightConstantBuffer : register(b1)
{
	RemLight remLight;
};

cbuffer CameraConstantBuffer : register(b2)
{
    row_major matrix lightViewProjection;
};

struct VS_IN
{
    float3 pos : POSITION0;
    float2 tex : TEXCOORD0;
    float4 normal : NORMAL0;
};

struct PS_IN
{
    float4 pos : SV_POSITION;
    float4 lightViewPosition : TEXCOORD1;
    float2 tex : TEXCOORD;
    float4 normal : NORMAL;
    float3 modelPos : POSITION;
};

PS_IN VSMain(VS_IN input)
{
    PS_IN output = (PS_IN) 0;

    float4 pos = float4(input.pos, 1.0f);
    float4 modelPos = mul(pos, model);
    output.pos = mul(modelPos, viewProjection);
    output.tex = input.tex;
    output.normal = mul(transpose(model), input.normal);
    output.modelPos = modelPos.xyz;
    
    //
    float4 modelLightPos = mul(modelPos, lightViewProjection);
    output.lightViewPosition = modelLightPos;
    //
    
    return output;
}

Texture2D DiffuseMap                    : register(t0);
SamplerState Sampler                    : register(s0);

Texture2D ShadowMap                     : register(t1);
SamplerComparisonState ShadowMapSampler : register(s1);

float3 CalcDirLight(RemLight remLight, float3 normal, float3 viewDir, float2 tex, float4 lightViewPosition);
float IsLighted(float4 lightViewPosition, float3 lightDir, float3 normal);

float4 PSMain(PS_IN input) : SV_Target
{
    float3 norm    = normalize(input.normal);
    float3 viewDir = normalize(cameraPosition - input.modelPos);

    float3 result = CalcDirLight(remLight, norm, viewDir, input.tex, input.lightViewPosition);

    return float4(result, 1.0f);
}

float3 CalcDirLight(RemLight remLight, float3 normal, float3 viewDir, float2 tex, float4 lightViewPosition)
{
    float3 diffValue = DiffuseMap.Sample(Sampler, tex).rgb;
    
    float3 lightDir = normalize( - remLight.direction);

    float diff = max(dot(normal, lightDir), 0.0);

    float3 reflectDir = reflect( - lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 128);

    float3 ambient  = remLight.ambient         * diffValue;
    float3 diffuse  = remLight.diffuse  * diff * diffValue;
    float3 specular = remLight.specular * spec * diffValue;
    
    float1 isLighted = 1;
    
    isLighted = IsLighted(lightViewPosition, lightDir, normal);
    
    return (ambient + (diffuse + specular) * isLighted);
}

float IsLighted(float4 lightViewPosition, float3 lightDir, float3 normal)
{
    float ndotl = dot(normal, lightDir);
    float bias = clamp(0.005f * (1.0f - ndotl), 0.0f, 0.0005f);
    
    float isVisibleForLight = 0;
    float3 projectTexCoord;

    projectTexCoord.x = lightViewPosition.x / lightViewPosition.w;
    projectTexCoord.y = lightViewPosition.y / lightViewPosition.w;
    projectTexCoord.z = lightViewPosition.z / lightViewPosition.w;

    projectTexCoord.x = projectTexCoord.x * 0.5 + 0.5f;
    projectTexCoord.y = projectTexCoord.y * -0.5 + 0.5f;

    float max_depth = ShadowMap.SampleCmpLevelZero(ShadowMapSampler, projectTexCoord.xy, projectTexCoord.z);

    float currentDepth = (lightViewPosition.z / lightViewPosition.w);

    currentDepth = currentDepth - bias;
    
    if (max_depth <= currentDepth)
    {
        return 0;
    }
    return max_depth;
}