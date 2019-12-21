#version 440 core

in vec2 tex_coord;

uniform sampler2D stars;
uniform int screen_width;
uniform int screen_height;
uniform int table_size;

uniform vec2 gradients[256];
uniform uint permutations[512];

uint Hash(uint x, uint y)
{
    return permutations[permutations[x] + y];
}

vec2 GetPoint(float x, float y) // fragcoord.x, fragcoord.y
{
    uint unit_x_length = uint(screen_width / table_size); // 1024x1024, table_size = 256, then width of one grid is 4.
    uint unit_y_length = uint(screen_height / table_size);

    return vec2(float(x / float(unit_x_length)), float(y / float(unit_y_length)));
}

float lerp(float a, float b, float t)
{
    return a + t * (b - a);
}

float GetPerlinNoise(vec2 p)
{
    int table_size_mask = table_size - 1;

    //Four corners
    int xi0 = (int(floor(p.x))) & table_size_mask;
    int yi0 = (int(floor(p.y))) & table_size_mask;
    int xi1 = (xi0 + 1) & table_size_mask;
    int yi1 = (yi0 + 1) & table_size_mask;

    //Fractional part in local grid
    float tx = p.x - (int(floor(p.x)));
    float ty = p.y - (int(floor(p.y)));

    float t1 = smoothstep(0.f, 1.f, tx);
    float t2 = smoothstep(0.f, 1.f, ty);

    //Getting gradient of each corner
    vec2 g_bot_left = gradients[Hash(xi0, yi0)];
    vec2 g_bot_right = gradients[Hash(xi1, yi0)];
    vec2 g_top_right= gradients[Hash(xi1, yi1)];
    vec2 g_top_left = gradients[Hash(xi0, yi1)];

    //Generate vectors going from each corner to p
    float x0 = tx;
    float x1 = tx - 1;
    float y0 = ty;
    float y1 = ty - 1;

    vec2 bot_left_to_p = vec2(x0, y0);
    vec2 bot_right_to_p = vec2(x1, y0);
    vec2 top_right_to_p = vec2(x1, y1);
    vec2 top_left_to_p = vec2(x0, y1);

    //Dot product
    float bot_left_val = dot(g_bot_left, bot_left_to_p);
    bot_left_val = (bot_left_val + 1) / 2.f;
    float bot_right_val = dot(g_bot_right, bot_right_to_p);
    bot_right_val = (bot_right_val + 1) / 2.f;
    float top_right_val = dot(g_top_right, top_right_to_p);
    top_right_val = (top_right_val + 1) / 2.f;
    float top_left_val = dot(g_top_left, top_left_to_p);
    top_left_val = (top_left_val + 1) / 2.f;

    //Bi-linear interpolation
    float bottom_edge_x = lerp(bot_left_val, bot_right_val, t1);
    float top_edge_x = lerp(top_left_val, top_right_val, t1);
    float final_result = lerp(bottom_edge_x, top_edge_x, t2);

    return final_result;
}

float FractalSum(vec2 point, int layer_num)
{
	float n = GetPerlinNoise(point);
	for(int i = 0; i < layer_num; ++i)
	{
		point /= 2.f;
		n += GetPerlinNoise(point);
	}
	return n / layer_num;
}

void main()
{
    vec3 s = texture2D(stars, tex_coord).rgb;
	vec2 point = GetPoint(gl_FragCoord.x, gl_FragCoord.y);

	float n = FractalSum(point, 8);

	n = pow(n + 0.25f, 8);


	//gl_FragColor = vec4(vec3(n), 1.f);
	gl_FragColor = vec4(mix(s, vec3(0.4f, 0.4f + n, 0.4f), n), 1.f);
}