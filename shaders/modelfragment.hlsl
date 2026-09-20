struct input_data {
    [[vk::location(0)]] float3 normal : TEXCOORD0;
    [[vk::location(1)]] float4 colour : COLOR0;
};
float4 main(input_data input) : SV_Target {
    float3 normal = normalize(input.normal);
    float3 light = normalize(float3(-0.35f, 0.45f, -1.0f));
    float diffuse = abs(dot(normal, light));
    return float4(input.colour.rgb * (0.35f + 0.65f * diffuse), input.colour.a);
}