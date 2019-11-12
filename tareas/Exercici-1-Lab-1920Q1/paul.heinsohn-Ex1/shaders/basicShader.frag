#version 330 core

in vec3 fcolor;

out vec4 FragColor;

void main() {
    FragColor = vec4(fcolor, 1);
    if (gl_FragCoord.y > 480.0f && int(gl_FragCoord.y) % 4 != 0) discard;
}
