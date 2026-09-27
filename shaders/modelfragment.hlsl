struct input_data {
    [[vk::location(0)]] float4 normal : TEXCOORD0;
    [[vk::location(1)]] float4 colour : COLOR0;
    [[vk::location(2)]] float4 viewpos : TEXCOORD1;
};
float4 main(input_data input) : SV_Target {
    float4 normal = normalize(input.normal);
    float4 eyedir = normalize(float4(-input.viewpos.xyz,0.0f));
    normal *= dot(normal, eyedir) < 0.0f ? -1.0f : 1.0f;
    float4 keylight = normalize(float4(-0.35f, 0.45f, -1.0f, 0.0f));
    float4 filllight = normalize(float4(0.65f, -0.25f, -0.7f, 0.0f));

    float keyshade = saturate(dot(normal, keylight));
    float fillshade = saturate(dot(normal, filllight));
    float rim = 1.0f - saturate(dot(normal, eyedir));

    rim *= rim;
    float shade = 0.27f + 0.51f * keyshade + 0.22f * fillshade + 0.09f * rim;
    return float4(saturate(input.colour.rgb * shade), input.colour.a);
}