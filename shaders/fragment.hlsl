cbuffer cb : register(b0, space0){
    column_major float4x4 view;
    column_major float4x4 projection;
};

struct i{
    [[vk::location(0)]] float3 vposition : TEXCOORD0;
    [[vk::location(1)]] nointerpolation float4 sphere : TEXCOORD1;
    [[vk::location(2)]] nointerpolation float4 colour : COLOR0;
};

struct o{
    [[vk::location(0)]] float4 colour : SV_Target;
    float depth : SV_Depth;
};

o main(i input){
    float3 ray = normalize(input.vposition);
    float centeray = dot(ray, input.sphere.xyz);
    float centerdsquared = dot(input.sphere.xyz, input.sphere.xyz);
    float rsquared = input.sphere.w * input.sphere.w;
    float discriminant = centeray * centeray - (centerdsquared - rsquared);
    clip(discriminant);

    float distanceAlongRay = centeray - sqrt(max(discriminant, 0.0f));
    clip(distanceAlongRay);

    float3 surface = ray * distanceAlongRay;
    float3 normal = normalize(surface - input.sphere.xyz);
    float3 lightDirection = normalize(float3(-0.35f, 0.45f, -1.0f));
    float diffuse = saturate(dot(normal, lightDirection));
    float3 reflectedLight = reflect(-lightDirection, normal);
    float specular = pow(saturate(dot(reflectedLight, -ray)), 24.0f) * 0.25f;
    float4 psurface = mul(projection, float4(surface, 1.0f));

    o output;
    output.colour = float4(input.colour.rgb * (0.25f + 0.75f * diffuse) + specular, input.colour.a);
    output.depth = psurface.z / psurface.w;
    return output;
}