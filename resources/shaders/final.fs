#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D hdrBuffer;
uniform sampler2D bloomBlur;
uniform bool hdr;
uniform float exposure;
uniform bool bloom;

void main()
{

    vec3 hdrColor = texture(hdrBuffer, TexCoords).rgb;
    vec3 bloomColor = texture(bloomBlur, TexCoords).rgb;
    vec3 result;

    if(bloom){
        hdrColor += bloomColor;
    }
    if(hdr)
       result = vec3(1.0) - exp(-hdrColor * exposure);
    else
       result = hdrColor;



    FragColor = vec4(result, 1.0);
}