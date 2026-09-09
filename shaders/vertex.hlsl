cbuffer cb : register(b0, space0){
    column_major float4x4 cb;
};

struct i{
    [[vk::location(0)]] float4 position : POSITION;
    [[vk::location(1)]] float4 colour : COLOR0;
};

struct o{
    [[vk::location(0)]] float4 varying : COLOR0;
    float4 position : SV_Position;
};

o main (i input){
    o output;
    output.varying = input.colour;
    output.position = mul(cb, input.position);
    return output;
}