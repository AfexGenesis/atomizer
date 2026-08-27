struct i{
    [[vk::location(0)]] float3 varying : COLOR0;
};

struct o{
    [[vk::location(0)]] float4 colour : SV_Target;
};

o main(i input){
    o output;
    output.colour = float4(input.varying, 1.0);
    return output;
}