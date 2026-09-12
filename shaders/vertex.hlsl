cbuffer cb : register(b0, space0){
    column_major float4x4 cb;
};

struct i{
    [[vk::location(0)]] float4 position : POSITION;
    [[vk::location(1)]] float4 colour : COLOR0;
    [[vk::location(2)]] float4 iposition : TEXCOORD0;
    [[vk::location(3)]] float4 icolour : TEXCOORD1;
};

struct o{
    [[vk::location(0)]] float4 varying : COLOR0;
    float4 position : SV_Position;
};

o main (i input){
    o output;
    output.varying = input.colour * input.icolour;
    float4 worldpos = float4(input.position.xyz + input.iposition.xyz, 1.0f);
    output.position = mul(cb, worldpos);
    return output;
}