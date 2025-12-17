//******** PRACTICA VISUALITZACIÓ GRÀFICA INTERACTIVA (EE-UAB)
//******** Entorn bàsic VS2022 amb interfície MFC/GLFW i Status Bar
//******** Enric Marti (Setembre 2023)
// phong_shdrML.vert: Vertex Program en GLSL en versió OpenGL 3.3 o 4.30 per a implementar:
//     a) Iluminació de Gouraud
//     b) Fonts de llum puntuals o direccionals
//     c) Fonts de llum restringides
//     d) Atenuació de fonts de llum
//     e) Modus d'assignació de textura MODULATE o DECAL

//#version 330 core
#version 460 core

#define MaxLights 20

// Estructuras (Deben coincidir aunque no se usen todas aquí)
struct Light {
    bool sw_light;
    vec4 position;
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    vec3 attenuation;
    bool restricted;
    vec3 spotDirection;
    float spotCosCutoff;
    float spotExponent;
};

struct Material {
    vec4 emission;
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    float shininess;
};

// Variables IN
layout (location = 0) in vec3 in_Vertex;
layout (location = 1) in vec4 in_Color;
layout (location = 2) in vec3 in_Normal;
layout (location = 3) in vec2 in_TexCoord;

// Uniforms
uniform mat4 normalMatrix;
uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
uniform bool flag_invert_y;

uniform mat4 lightSpaceMatrix;

// Variables OUT (Se envían al Fragment Shader para ser interpoladas)
out vec3 vertexPV;       // Posición en View Space
out vec3 vertexNormalPV; // Normal en View Space
out vec2 vertexTexCoord;
out vec4 vertexColor;
out vec4 FragPosLightSpace;

void main()
{
    // 1. Calcular posición del vértice en View Space
    vertexPV = vec3(viewMatrix * modelMatrix * vec4(in_Vertex, 1.0));

    // 2. Calcular normal en View Space
    vertexNormalPV = normalize(vec3(normalMatrix * vec4(in_Normal, 1.0)));

    // 3. Texturas
    if (flag_invert_y) vertexTexCoord = vec2(in_TexCoord.x, 1.0 - in_TexCoord.y);
    else vertexTexCoord = vec2(in_TexCoord.x, in_TexCoord.y);

    // 4. Color base
    vertexColor = in_Color;

    // 5. Posición en pantalla
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(in_Vertex, 1.0);

    FragPosLightSpace = lightSpaceMatrix * modelMatrix * vec4(in_Vertex, 1.0);
}