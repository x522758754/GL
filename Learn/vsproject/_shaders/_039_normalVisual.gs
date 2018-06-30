#version 330 core
layout(triangles) in;
layout(line_strip, max_vertices = 6) out;

in VS_OUT{
	vec2 texCoords;
	vec3 normal;
}gs_in[];

out vec2 TexCoords;
out vec3 fColor;
uniform float time;

const float MAGNITUDE = 0.4;

vec3 GetNormal()
{
    vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
    vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
    return normalize(cross(a, b));
}

vec4 explode(vec4 position, vec3 normal)
{
	float magnitude = 2.0;
	vec3 direction = normal * (sin(time) + 1.0) / 2.0 * magnitude;

	return position + vec4(direction, 0.0);
}

void GenerateLine(int index)
{
	gl_Position = gl_in[index].gl_Position;
	EmitVertex();
	gl_Position = gl_in[index].gl_Position + vec4(gs_in[index].normal, 0.0) * MAGNITUDE;
	EmitVertex();
	EndPrimitive();
}

void produceExplode()
{
	vec3 normal = GetNormal();

	normal = normalize(gs_in[0].normal);
	gl_Position = explode(gl_in[0].gl_Position, normal);
	TexCoords = gs_in[0].texCoords;
	fColor = vec3(1.0, 1.0, 0.1);
	EmitVertex();

	normal = normalize(gs_in[0].normal);
	gl_Position = explode(gl_in[1].gl_Position, normal);
	TexCoords = gs_in[1].texCoords;
	fColor = vec3(1.0, 1.0, 0.1);
	EmitVertex();

	normal = normalize(gs_in[0].normal);
	gl_Position = explode(gl_in[2].gl_Position, normal);
	TexCoords = gs_in[2].texCoords;
	fColor = vec3(1.0, 1.0, 0.1);
	EmitVertex();

	EndPrimitive();
}

void main()
{
	GenerateLine(0);
	GenerateLine(1);
	GenerateLine(2);
}