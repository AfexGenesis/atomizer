cbuffer cb : register(b0, space0){
    column_major float4x4 view;
    column_major float4x4 projection;
};

struct i{
    [[vk::location(0)]] float3 vposition : TEXCOORD0;
    [[vk::location(1)]] nointerpolation float4 sphere : TEXCOORD1;
    [[vk::location(2)]] nointerpolation float4 colour : COLOR0;
    [[vk::location(3)]] nointerpolation float4 end : TEXCOORD3;
};

struct o{
    [[vk::location(0)]] float4 colour : SV_Target;
    float depth : SV_DepthGreaterEqual;
};

o main(i input){
    float3 ray = normalize(input.vposition);
    float distanceAlongRay = 1e30f;
    float3 normal = float3(0, 0, 1);
    float3 start = input.sphere.xyz;
    float3 finish = input.end.xyz;
    float radius = input.sphere.w;
    if (input.end.w > 0.5f){
        float3 axis = finish - start;
        float len2 = dot(axis, axis);
        float axial = dot(axis, ray);
        float origin = -dot(axis, start);
        float a = len2 - axial * axial;
        float b = len2 * (-dot(start, ray)) - origin * axial;
        float c = len2 * (dot(start, start) - radius * radius) - origin * origin;
        float det = b * b - a * c;
        if (a > 1e-6f && det >= 0.0f){
            float t = (-b - sqrt(det)) / a;
            float along = origin + t * axial;
            if (t > 0.0f && along >= 0.0f && along <= len2){
                distanceAlongRay = t;
                float3 hit = ray * t;
                normal = normalize(hit - (start + axis * (along / len2)));
            }
        }
    }
    // Spherical ends also render the ordinary atom when both endpoints coincide.
    for (int cap = 0; cap < 2; ++cap){
        if (cap == 1 && input.end.w < 0.5f) break;
        float3 center = cap == 0 ? start : finish;
        float projection = dot(ray, center);
        float det = projection * projection - dot(center, center) + radius * radius;
        if (det < 0.0f) continue;
        float t = projection - sqrt(det);
        if (t > 0.0f && t < distanceAlongRay){
            distanceAlongRay = t;
            normal = normalize(ray * t - center);
        }
    }
    clip(1e29f - distanceAlongRay);
    float3 surface = ray * distanceAlongRay;
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