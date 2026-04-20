#version 460 core
uniform vec2 resolution;
uniform sampler2D texture1;
uniform sampler2D texture2;
in vec2 tex_coord;
out vec4 FragColor;
void main() {
    //FragColor = vec4(tex_coord, 1.f, 1.f);
    FragColor = vec4(mix(texture(texture1, tex_coord).rgb, texture(texture2, tex_coord).rgb, 0.9999), 1.f);
    //FragColor = vec4(1,1,1,1);
}