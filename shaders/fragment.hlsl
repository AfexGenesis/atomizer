cbuffer cb : register(b0, space0){
    column_major float4x4 view;
    column_major float4x4 projection;
};

struct i{
    [[vk::location(0)]] float4 vposition : TEXCOORD0;
    [[vk::location(1)]] nointerpolation float4 sphere : TEXCOORD1;
    [[vk::location(2)]] nointerpolation float4 colour : COLOR0;
    [[vk::location(3)]] nointerpolation float4 end : TEXCOORD3;
};

struct o{
    [[vk::location(0)]] float4 colour : SV_Target;
    float depth : SV_DepthGreaterEqual;
};

o main(i input){
    float4 ray = normalize(float4(input.vposition.xyz,0.0f));
    float distanceAlongRay = 1e30f;
    float4 normal = float4(0, 0, 1, 0);
    float4 start = float4(input.sphere.xyz,1.0f);
    float4 finish = float4(input.end.xyz,1.0f);
    float radius = input.sphere.w;
    if (input.end.w > 0.5f){
        float4 axis = finish - start;
        float len2 = dot(axis, axis);
        float axial = dot(axis, ray);
        float origin = -dot(axis, start);
        float a = len2 - axial * axial;
        float b = len2 * (-dot(start, ray)) - origin * axial;
        float c = len2 * (dot(start.xyz, start.xyz) - radius * radius) - origin * origin;
        float det = b * b - a * c;
        if (a > 1e-6f && det >= 0.0f){
            float t = (-b - sqrt(det)) / a;
            float along = origin + t * axial;
            if (t > 0.0f && along >= 0.0f && along <= len2){
                distanceAlongRay = t;
                float4 hit = ray * t;
                normal = normalize(float4(hit.xyz - (start + axis * (along / len2)).xyz,0.0f));
            }
        }
    }
    // Spherical ends also render the ordinary atom when both endpoints coincide.
    for (int cap = 0; cap < 2; ++cap){
        if (cap == 1 && input.end.w < 0.5f) break;
        float4 center = cap == 0 ? start : finish;
        float projection = dot(ray, center);
        float det = projection * projection - dot(center.xyz, center.xyz) + radius * radius;
        if (det < 0.0f) continue;
        float t = projection - sqrt(det);
        if (t > 0.0f && t < distanceAlongRay){
            distanceAlongRay = t;
            normal = normalize(float4(ray.xyz * t - center.xyz,0.0f));
        }
    }
    clip(1e29f - distanceAlongRay);
    float4 surface = float4(ray.xyz * distanceAlongRay,1.0f);
    float4 keylight = normalize(float4(-0.35f, 0.45f, -1.0f,0.0f));
    float4 filllight = normalize(float4(0.65f, -0.25f, -0.7f,0.0f));
    float4 eyedir = -ray;

    float keyshade = saturate(dot(normal, keylight));
    float fillshade = saturate(dot(normal, filllight));
    float rim = pow(1.0f - saturate(dot(normal, eyedir)), 2.0f);
    float4 highlight = reflect(-keylight, normal);
    float specular = pow(saturate(dot(highlight, eyedir)), 32.0f) * 0.12f;
    float shade = 0.24f + 0.53f * keyshade + 0.22f * fillshade + 0.10f * rim;
    float4 psurface = mul(projection, surface);

    o output;
    output.colour = float4(saturate(input.colour.rgb * shade + specular), input.colour.a);
    output.depth = psurface.z / psurface.w;
    return output;
}