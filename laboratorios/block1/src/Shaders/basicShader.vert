#version 330 core

// Proviene de OpenGL y el vertex se puede escalar multiplicándolo
in vec3 vertex;
uniform mat4 TG;
uniform float val;

void main() {
    // Para ejercicios 1 y 2
    gl_Position = TG * vec4(vertex * val, 1.0);
    // gl_Position = vec4(vertex, 1.0);
    // Para ejercicios 3 y 4
    // gl_Position = vec4 (vertex * 0.5, 1.0);
}
