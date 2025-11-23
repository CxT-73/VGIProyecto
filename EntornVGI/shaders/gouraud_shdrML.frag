//******** PRACTICA VISUALITZACIÓ GRÀFICA INTERACTIVA (EE-UAB)
//******** Entorn bàsic VS2022 amb interfície MFC/GLFW i Status Bar
//******** Enric Marti (Setembre 2023)
// gouraud_shdrML.frag: Fragment Program en GLSL en versió OpenGL 3.3 o 4.30 per a implementar:
//     a) Iluminació de Gouraud
//     b) Fonts de llum puntuals o direccionals
//     c) Fonts de llum restringides
//     d) Atenuació de fonts de llum
//     e) Modus d'assignació de textura MODULATE o DECAL

//#version 330 core
#version 460 core

// --- L14- Variables in
in vec4 VertexColor;	// Color del Vèrtex
in vec2 VertexTexCoord;	// Coordenades textura del vèrtex
in vec4 FragPosLightSpace; //AFEGIT

// --- L18- Variables uniform
uniform sampler2D texture0;	// Imatge textura
uniform sampler2D shadowMap;	//AFEGIT
uniform bool textur;		// Booleana d’activació (TRUE) de textures o no (FALSE).
uniform bool modulate;		// Booleana d'activació de barreja color textura- color intensitat llum (TRUE) o només color textura (FALSE)

// --- L23- Variables out
out vec4 FragColor;		// Color fragment (r,g,b,a)

// --- AFEGIT: FUNCIÓ DE CÀLCUL D'OMBRA ---
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
    
    // 5. Comparació amb bias (per evitar ratlles negres "acne")
    float bias = 0.005; 
    float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;

    return shadow;
}

void main ()
{
float shadow = ShadowCalculation(FragPosLightSpace); //AFEGIT
vec4 finalColorVertex = VertexColor;
if(shadow > 0.0)
    {
        finalColorVertex.rgb = finalColorVertex.rgb * 0.5; //AFEGIT (Factor de foscor de l'ombra)
    }
if (textur) {	// Intensitat amb textura
		vec4 colorT = texture(texture0,VertexTexCoord);
		// Textura modulada amb intensitat llum
	    	if (modulate) FragColor = colorT * finalColorVertex; // <--- MODIFICAT (usem finalColorVertex enlloc de VertexColor);
       			else FragColor=colorT; // textura sense modular intensitat llum
    	    }
    else { // Intensitat sense textura
           FragColor = finalColorVertex; // <--- MODIFICAT 
         }
}
