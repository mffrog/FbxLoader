#version 410
layout(location=0) in vec4 outColor;
out vec4 fragColor;
void main(){
    fragColor = outColor;
}
