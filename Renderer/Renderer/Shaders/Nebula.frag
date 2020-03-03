//********************************
#version 440 core

#define S(a, b, t) smoothstep(a, b, t)

out vec4 FragColor;
uniform vec2 iResolution;
//uniform float seed;
//********************************

float N21(vec2 p) // -1 ~ 1
{
    return fract(sin(p.x * 123. + 1 + p.y * 6574) * 6375.) * 2 - 1;
}

float N11(float p) // -1 ~ 1
{
    return fract(sin(p) * 43758.5453123) * 2 - 1;
}

float PerlinNoise(vec2 p)
{
    vec2 id = floor(p);
    vec2 local_uv = S(0, 1, fract(p));

    //gradient vectors
    float random = N21(id);
    vec2 gbl  = normalize(vec2(random, N11(random))); // bottom left (id)
    random = N21(id + vec2(1, 0));
    vec2 gbr  = normalize(vec2(random, N11(random))); // bottom right
    random = N21(id + vec2(0, 1));
    vec2 gtl  = normalize(vec2(random, N11(random))); // top left
    random = N21(id + vec2(1, 1));
    vec2 gtr  = normalize(vec2(random, N11(random))); // top right

    //vector from corners to current uv
    float tx = local_uv.x;
    float ty = local_uv.y;

    vec2 vbl = (vec2(tx, ty));
    vec2 vbr = (vec2(1 - tx, ty));
    vec2 vtl = (vec2(tx, 1 - ty));
    vec2 vtr = (vec2(1 - tx, 1 - ty));

    //dot product
    float bl = dot(gbl, vbl);
    float br = dot(gbr, vbr);
    float tl = dot(gtl, vtl);
    float tr = dot(gtr, vtr);

    float bt = mix(bl, br,local_uv.x);
    float tt = mix(tl, tr,  local_uv.x);
    float t = mix(bt, tt, local_uv.y);
    return clamp((t + 1) / sqrt(2), 0, 1);
}

float SmoothNoise(vec2 p)
{
    vec2 id = floor(p);
    vec2 local_uv = S(0, 1, fract(p));

    //corner value
    float bl = N21(id);
    float br = N21(id + vec2(1, 0));
    float tl = N21(id + vec2(0, 1));
    float tr = N21(id + vec2(1, 1));

    float bt = mix(bl, br, local_uv.x);
    float tt = mix(tl, tr, local_uv.x);
    float t = mix(bt, tt, local_uv.y);

    return (t + 1) / 2.;
}

float FractalSum(vec2 p)
{
     float n = SmoothNoise(p);
     n += SmoothNoise(p * 4) * .5;
     n += SmoothNoise(p * 8) * .25;
     n += SmoothNoise(p * 16) * .125;
     n += SmoothNoise(p * 32) * .0625;
     n += SmoothNoise(p * 64) * .03125;
     n += SmoothNoise(p * 128) * .010625;
     n /= 2.;
     return n;
}

float FractalSum2(vec2 p)
{
     float n = SmoothNoise(p);
     n += SmoothNoise(p * 4);
     n += SmoothNoise(p * 8);
     n += SmoothNoise(p * 16);
     n += SmoothNoise(p * 32);
     n += SmoothNoise(p * 64);
     n += SmoothNoise(p * 128);
     n += SmoothNoise(p * 256);
     n += SmoothNoise(p * 512);
     n += SmoothNoise(p * 1024);
     n += SmoothNoise(p * 2048);
     n += SmoothNoise(p * 4096);
     n /= 12.;
     return n;
}

void main()
{
    vec2 uv = (gl_FragCoord.xy - .5 * iResolution.xy) / iResolution.y;
    vec3 col = vec3(0);

    //Star
    float star = (N21(uv) + 1) / 2.;
    star = pow(star, 1000);

    
    //nebula
    float noise = FractalSum2(uv);
    noise = pow(noise, 13) * 100;
    vec3 nebula = mix(vec3(0, 0, 0), vec3(.4, 1., 1.), noise);

    noise = FractalSum(uv);
    noise = pow(noise, 8) * 10;
    nebula = mix(nebula, vec3(.4, 1., 1.), noise);

    noise = FractalSum2(uv + 1);
    noise = pow(noise, 13) * 100;
    nebula = mix(nebula, vec3(1., .8, .8), noise);

    noise = FractalSum(uv + 1);
    noise = pow(noise, 8) * 10;
    nebula = mix(nebula, vec3(1., .8, .8), noise);

    noise = FractalSum(uv + 100);
    noise = pow(noise, 2) * 2;
    nebula = mix(nebula, vec3(.0, .0, .0), noise);

    /////////////////
    //sun
    float distance = length(uv - vec2(.2, .1));
    float m = .01 / distance;
    nebula += vec3(m);

    distance = length(uv + vec2(.1, .3));
    m = .005 / distance;
    nebula += vec3(m);

    distance = length(uv - vec2(-.3, .3));
    m = .03 / distance;
    nebula += vec3(m);

    nebula += star;


    FragColor = vec4(nebula, 1);
    //FragColor = vec4(vec3(noise), 1);
}