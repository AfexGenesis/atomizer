cbuffer cb : register(b0, space0){
    column_major float4x4 view;
    column_major float4x4 projection;
};

struct input_data {
    [[vk::location(0)]] float4 position : POSITION;
    [[vk::location(1)]] float4 normal : NORMAL;
    [[vk::location(2)]] float4 colour : COLOR0;
};
struct output_data {
    [[vk::location(0)]] float4 normal : TEXCOORD0;
    [[vk::location(1)]] float4 colour : COLOR0;
    [[vk::location(2)]] float4 viewpos : TEXCOORD1;
    float4 position : SV_Position;
};
output_data main(input_data input){
    output_data output;
    float4 viewpos = mul(view, input.position);
    output.position = mul(projection, viewpos);
    output.normal = normalize(mul(view, input.normal));
    output.colour = input.colour;
    output.viewpos = viewpos;
    return output;
}