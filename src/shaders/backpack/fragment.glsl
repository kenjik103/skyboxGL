#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normals;

uniform vec3 viewerPos;
uniform samplerCube skybox;

void main()
{
  float ratio = 1.0 / 1.52;
  vec3 I = normalize(FragPos - viewerPos);
  vec3 R = refract(I, normalize(Normals), ratio);
  FragColor = vec4(texture(skybox, R).rgb, 1.0);
}

