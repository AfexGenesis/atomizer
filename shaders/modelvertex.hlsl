cbuffer cb : register(b0, space0){
    column_major float4x4 view;
    column_major float4x4 projection;
};

struct input_data {
    [[vk::location(0)]] float3 position : POSITION;
    [[vk::location(1)]] float3 normal : NORMAL;
    [[vk::location(2)]] float4 colour : COLOR0;
};
struct output_data {
    [[vk::location(0)]] float3 normal : TEXCOORD0;
    [[vk::location(1)]] float4 colour : COLOR0;
    float4 position : SV_Position;
};
output_data main(input_data input){
    output_data output;
    output.position = mul(projection, mul(view, float4(input.position, 1.0f)));
    output.normal = normalize(mul((float3x3)view, input.normal));
    output.colour = input.colour;
    return output;
}