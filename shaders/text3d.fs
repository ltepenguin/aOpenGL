#version 330 core
in vec2 fs_uv;
out vec4 color;

uniform sampler2D u_text;
uniform vec3 u_textColor;

void main()
{    
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(u_text, fs_uv).r);
    color = vec4(u_textColor, 1.0) * sampled;
}