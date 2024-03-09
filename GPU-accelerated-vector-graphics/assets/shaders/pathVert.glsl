#version 460

struct Vertex {
	vec4 position;
	vec4 color;
};

layout(std430, binding = 0) buffer TVertex
{
   Vertex vertex[]; 
};

uniform mat4  u_mvp;
uniform vec2  u_resolution;
uniform float u_thickness;

out vec2 texCoord;
out vec4 color;

void main()
{
    // borrowed from https://stackoverflow.com/questions/3484260/opengl-line-width
    int line_i = gl_VertexID / 6;
    int tri_i  = gl_VertexID % 6;

    

    vec4 va[4];
    for (int i=0; i<4; ++i)
    {
        va[i] = u_mvp * vertex[line_i+i].position;
        va[i].xyz /= abs(va[i].w);
    }

    vec2 v_line  = normalize(va[2].xy - va[1].xy);
    vec2 nv_line = vec2(-v_line.y, v_line.x);

    vec4 pos;
    if (tri_i == 0 || tri_i == 1 || tri_i == 3)
    {
        color = vertex[line_i+1].color;

        vec2 v_pred  = normalize(va[1].xy - va[0].xy);
        vec2 v_miter = normalize(nv_line + vec2(-v_pred.y, v_pred.x));

        pos = va[1];

        float angle = dot(v_miter, nv_line);

        pos.xy += 0.01*v_miter * u_thickness * (tri_i == 1 ? -0.5 : 0.5) / max(angle, 0.2); // temp miter fix 
    }
    else
    {
        color = vertex[line_i+2].color;

        vec2 v_succ  = normalize(va[3].xy - va[2].xy);
        vec2 v_miter = normalize(nv_line + vec2(-v_succ.y, v_succ.x));

        pos = va[2];

        float angle = dot(v_miter, nv_line);

        pos.xy += 0.01*v_miter * u_thickness * (tri_i == 5 ? 0.5 : -0.5) / max(angle, 0.2);
    }

    

    pos.xyz *= abs(pos.w);
    gl_Position = pos;

    texCoord = pos.xy;
}