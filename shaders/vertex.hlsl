cbuffer cb : register(b0, space0){
    column_major float4x4 view;
    column_major float4x4 projection;
};

struct i{
    [[vk::location(0)]] float2 corner : POSITION;
    [[vk::location(1)]] float4 iposition : TEXCOORD0;
    [[vk::location(2)]] float4 icolour : TEXCOORD1;
    [[vk::location(3)]] float4 iend : TEXCOORD2;
};

struct o{
    [[vk::location(0)]] float3 vposition : TEXCOORD0;
    [[vk::location(1)]] nointerpolation float4 sphere : TEXCOORD1;
    [[vk::location(2)]] nointerpolation float4 colour : COLOR0;
    [[vk::location(3)]] nointerpolation float4 end : TEXCOORD3;
    float4 position : SV_Position;
};

o main (i input){
    o output;
    float4 start = mul(view, float4(input.iposition.xyz, 1.0f));
    float4 finish = mul(view, float4(input.iend.xyz, 1.0f));
    float4 center = input.iend.w > 0.5f ? (start + finish) * 0.5f : start;
    float radius = input.iposition.w + (input.iend.w > 0.5f ? length(start.xyz - finish.xyz) * 0.5f : 0.0f);

    output.sphere = float4(start.xyz, input.iposition.w);
    output.end = float4(finish.xyz, input.iend.w);
    output.colour = input.icolour;

    // Tangent bounds do not exist once the camera enters the bounding sphere.
    // Cull that very close instance instead of producing a screen-sized quad.
    float front = center.z - radius;
    if (front <= 0.01f){
        output.vposition = float3(0.0f, 0.0f, 1.0f);
        output.position = float4(2.0f, 2.0f, 0.0f, 1.0f);
        return output;
    }

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
    float4 frontposition = float4(raySlope * front, front, 1.0f);

    output.vposition = vposition.xyz;
    // Rasterize at the front of the bound. The fragment shader promises that
    // its exact depth is no closer, allowing early depth rejection.
    output.position = mul(projection, frontposition);
    return output;
}