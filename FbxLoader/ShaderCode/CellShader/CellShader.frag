#version 410
layout(location=0) in vec4 inColor;
layout(location=1) in vec2 inTexCoord;
layout(location=2) in vec3 inWorldPosition;
uniform sampler2D colorSampler;
//ライトデータ(点光源)
struct PointLight{
    vec4 position;  //座標（ワールド座標）
    vec4 color;     //明るさ
};

const int maxLightCount = 4;    //ライトの数

//ライティングパラメータ
layout(std140) uniform LightData{
    vec4 ambientColor;                  //環境光
    PointLight light[maxLightCount];    //ライトリスト
}lightData;

out vec4 fragColor;
void main(){
    vec3 lightColor = lightData.ambientColor.rgb;
    for( int i = 0; i < maxLightCount; ++i ){
        vec3 lightVector = lightData.light[i].position.xyz - inWorldPosition;
        float lightPower = 1.0f / dot(lightVector, lightVector);
        float cosTheta = clamp(dot(vec3(0,0,1), normalize(lightVector)), 0, 1);
        lightColor += lightData.light[i].color.rgb * cosTheta * lightPower;
    }
    fragColor = inColor * texture(colorSampler, inTexCoord);
    
    const float numShades = 3;  //影の段数
    lightColor = ceil(lightColor * numShades) * ( 1 / numShades);
    
    fragColor.rgb *= lightColor;
}
