


#shader_type vertex_shader
#version 330 core
layout(location = 0) in vec2 position;
layout(location = 1) in vec4 input_color_filter;
layout(location = 2) in vec2 input_texture_coordinates;

out vec4 color_filter;
out vec2 texture_coordinates;
void main() {
    color_filter = input_color_filter;
    texture_coordinates = input_texture_coordinates;
    gl_Position = vec4(position.xy, 0, 1);
}

#shader_type fragment_shader
#version 330 core
in vec4 color_filter;
in vec2 texture_coordinates;
uniform sampler2D texture_sampler;
out vec4 result_color;


void main() {
    result_color = texture(texture_sampler, texture_coordinates) * color_filter;
}