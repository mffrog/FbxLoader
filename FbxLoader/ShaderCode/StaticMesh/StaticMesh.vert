#version 410
layout(location=0) in vec3 vPosition;
layout(location=1) in vec4 vColor;

layout(location=0) out vec4 outColor;

layout(std140) uniform VertexData{
    mat4x4 matMVP;
};

void main(){
    outColor = vColor;
    gl_Position = matMVP * vec4(vPosition,1.0);
}
