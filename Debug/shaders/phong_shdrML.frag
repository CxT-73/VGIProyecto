//******** PRACTICA VISUALITZACIÓ GRÀFICA INTERACTIVA (EE-UAB)
//******** Entorn bàsic VS2022 amb interfície MFC/GLFW i Status Bar
//******** Enric Marti (Setembre 2023)
// phong_shdrML.frag: Fragment Program en GLSL en versió OpenGL 3.3 o 4.30 per a implementar:
//     a) Iluminació de Gouraud
//     b) Fonts de llum puntuals o direccionals
//     c) Fonts de llum restringides
//     d) Atenuació de fonts de llum
//     e) Modus d'assignació de textura MODULATE o DECAL

//#version 330 core
#version 460 core

#define MaxLights 20

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

// Variables IN (Vienen del Vert)
in vec3 vertexPV;
in vec3 vertexNormalPV;
in vec2 vertexTexCoord;
in vec4 vertexColor;
in vec4 FragPosLightSpace;

// Uniforms
uniform mat4 viewMatrix;            // Necesaria para transformar luces fijas
uniform bool fixedLight[MaxLights]; // Array para saber si es luz de Mundo o Cámara
uniform sampler2D texture0;
uniform bool textur;
uniform bool modulate;
uniform bool sw_material;
uniform bvec4 sw_intensity;
uniform vec4 LightModelAmbient;
uniform Light LightSource[MaxLights];
uniform Material material;
uniform sampler2D shadowMap;

// Salida
out vec4 FragColor;

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // 1. Divisió perspectiva
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // 2. Transformar al rang [0,1]
    projCoords = projCoords * 0.5 + 0.5;
    
    // Si està fora del rang del mapa, no fem ombra
    if(projCoords.z > 1.0)
        return 0.0;

    // 3. Llegir profunditat del mapa d'ombres
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // 4. Profunditat actual
    float currentDepth = projCoords.z;
    
    // 5. Comparació amb bias
    float bias = 0.005; 
    float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;

    return shadow;
}

void main()
{
    // Normalizar vectores interpolados
    vec3 N = normalize(vertexNormalPV);
    vec3 V = normalize(-vertexPV); 

    vec3 Iemissive = vec3(0.0);
    vec3 Iambient = vec3(0.0);
    vec3 ILlums = vec3(0.0);
    
    // Alpha
    float alpha = vertexColor.a;
    int numMat = 0;
    float aValue = 0.0;

    // Emissive
    if (sw_intensity[0]) {
        if (sw_material) { 
            Iemissive = material.emission.rgb;
            numMat++; aValue += material.emission.a;
        } else Iemissive = vertexColor.rgb;
    }

    // Ambient
    if (sw_intensity[1]) {
        if (sw_material) { 
            Iambient = material.ambient.rgb * LightModelAmbient.rgb;
            numMat++; aValue += material.ambient.a;
        } else Iambient = vertexColor.rgb * LightModelAmbient.rgb;
    }

    // Bucle de Luces
    for (int i = 0; i < MaxLights; i++) {
        if (LightSource[i].sw_light) {
            
            vec4 lightPosEye;
            vec3 spotDirEye;

            // --- Lógica FIXED LIGHT ---
            if (fixedLight[i]) { 
                // Luz de Mundo (Coche/Farolas) -> Aplicar ViewMatrix
                lightPosEye = viewMatrix * LightSource[i].position;
                spotDirEye = vec3(viewMatrix * vec4(LightSource[i].spotDirection, 0.0));
            } else {
                // Luz de Cámara (Casco) -> Usar tal cual
                lightPosEye = LightSource[i].position; 
                spotDirEye = LightSource[i].spotDirection;
            }
            spotDirEye = normalize(spotDirEye);

            // Vector L
            vec3 L;
            float fatt = 1.0;
            
            if (LightSource[i].position.w == 1.0) { // Posicional
                vec3 distVec = lightPosEye.xyz - vertexPV;
                float d = length(distVec);
                L = normalize(distVec);
                fatt = 1.0 / (LightSource[i].attenuation.x * d * d + 
                              LightSource[i].attenuation.y * d + 
                              LightSource[i].attenuation.z);
            } else { // Direccional
                L = normalize(lightPosEye.xyz);
            }

            // Spotlight
            if (LightSource[i].restricted) {
                float spotDot = dot(-L, spotDirEye);
                if (spotDot > LightSource[i].spotCosCutoff) {
                    float spotFactor = pow(spotDot, LightSource[i].spotExponent);
                    fatt = fatt * spotFactor;
                } else {
                    fatt = 0.0;
                }
            }

            // Difusa
            if (sw_intensity[2]) {
                float diffuseFactor = max(dot(L, N), 0.0);
                if (sw_material) {
                    ILlums += material.diffuse.rgb * LightSource[i].diffuse.rgb * diffuseFactor * fatt;
                    numMat++; aValue += material.diffuse.a;
                } else {
                    ILlums += vertexColor.rgb * LightSource[i].diffuse.rgb * diffuseFactor * fatt;
                }
            }

            // Especular (Phong)
            if (sw_intensity[3]) {
                vec3 R = reflect(-L, N);
                float specularFactor = pow(max(dot(R, V), 0.0), material.shininess);
                if (sw_material) {
                    ILlums += material.specular.rgb * LightSource[i].specular.rgb * specularFactor * fatt;
                    numMat++; aValue += material.specular.a;
                } else {
                    ILlums += vertexColor.rgb * LightSource[i].specular.rgb * specularFactor * fatt;
                }
            }
        }
    }

    vec3 finalRGB = Iemissive + Iambient + ILlums;
    float shadow = ShadowCalculation(FragPosLightSpace);
    if(shadow > 0.0)
    {
        
        finalRGB = finalRGB * 0.5; 
    }
    if (sw_material && numMat > 0) alpha = aValue / float(numMat);
    
    vec4 finalColor = vec4(finalRGB, alpha);

    if (textur) {
        vec4 texColor = texture(texture0, vertexTexCoord);
        if (modulate) FragColor = finalColor * texColor;
        else FragColor = texColor;
    } else {
        FragColor = finalColor;
    }
}