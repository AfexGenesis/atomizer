cbuffer cb : register(b0, space0){
    column_major float4x4 view;
    column_major float4x4 projection;
};

struct i{
    [[vk::location(0)]] float2 corner : POSITION;
    [[vk::location(1)]] float4 iposition : TEXCOORD0;
    [[vk::location(2)]] float4 icolour : TEXCOORD1;
};

struct o{
    [[vk::location(0)]] float3 vposition : TEXCOORD0;
    [[vk::location(1)]] nointerpolation float4 sphere : TEXCOORD1;
    [[vk::location(2)]] nointerpolation float4 colour : COLOR0;
    float4 position : SV_Position;
};

o main (i input){
    o output;
    float4 center = mul(view, float4(input.iposition.xyz, 1.0f));
    float radius = input.iposition.w;

    // Calculate the exact tangent bounds of the sphere's projected silhouette.
    // Perspective makes these bounds asymmetric away from the optical axis.
    float rsquared = radius * radius;
    float zsquared = center.z * center.z;
    float denominator = max(zsquared - rsquared, rsquared * 0.0001f);
    float hroot = radius * sqrt(max(center.x * center.x + zsquared - rsquared, 0.0f));
    float vroot = radius * sqrt(max(center.y * center.y + zsquared - rsquared, 0.0f));

    float2 horizontalBounds = float2(
        (center.x * center.z - hroot) / denominator,
        (center.x * center.z + hroot) / denominator
    );
    float2 verticalBounds = float2(
        (center.y * center.z - vroot) / denominator,
        (center.y * center.z + vroot) / denominator
    );

    float2 corner01 = input.corner * 0.5f + 0.5f;
    float2 raySlope = float2(
        lerp(horizontalBounds.x, horizontalBounds.y, corner01.x),
        lerp(verticalBounds.x, verticalBounds.y, corner01.y)
    );
    float4 vposition = float4(raySlope * center.z, center.z, 1.0f);

    output.vposition = vposition.xyz;
    output.sphere = float4(center.xyz, radius);
    output.colour = input.icolour;
    output.position = mul(projection, vposition);
    return output;
}