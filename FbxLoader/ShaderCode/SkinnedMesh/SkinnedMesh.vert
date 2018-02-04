#version 410
layout(location=0) in vec3 vPosition;
layout(location=1) in vec4 vColor;
layout(location=2) in uvec4 vBoneIndex;
layout(location=3) in vec4 weights;

layout(location=0) out vec4 outColor;

layout(std140) uniform VertexData{
    mat4x4 matMVP;
};

layout(std140) uniform BoneData{
    mat4x4 boneMat[2];
};
void main(){
    mat4x4 boneMatSum = boneMat[vBoneIndex[0]] * weights[0] +
    boneMat[vBoneIndex[1]] * weights[1] +
    boneMat[vBoneIndex[2]] * weights[2] +
    boneMat[vBoneIndex[3]] * weights[3];
    
    gl_Position = matMVP */* boneMatSum **/  vec4(vPosition, 1.0);
    outColor = vColor;
}
