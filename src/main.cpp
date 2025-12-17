// Entorn VGI.ImGui: Includes llibreria ImGui
#include "ImGui\imgui.h"
#include "ImGui\imgui_impl_glfw.h"
#include "ImGui\imgui_impl_opengl3.h"
#include "ImGui\nfd.h" // Native File Dialog
#include <btBulletDynamicsCommon.h> //BULLET

#include "stdafx.h"
#include "shader.h"
#include "visualitzacio.h"
#include "escena.h"
#include "main.h"
#include "zones.h"
#include "colocarObjeto.h"
#include "MainMenu.h"
#include "NewGame.h"
#include "Playing.h"
#include "PauseMenu.h"
#include "EndGame.h"

ShadowMapData shadowData;            // VARIABLE OMBRES
GLuint simpleDepthShaderID = 0; // VARIABLE OMBRES

void InitGL()
{
	//------ Entorn VGI: Inicialitzaci� de les variables globals de CEntornVGIView
	int i;

	// Entorn VGI: Variable de control per a Status Bar (consola) 
	statusB = false;

	// Entorn VGI: Variables de control per Men� C�mera: Esf�rica, Navega, M�bil, Zoom, Satelit, Polars... 
	camera = CAM_FOLLOW;
	mobil = false;	zzoom = true;		zzoomO = false;		satelit = false;



	OPV.R = 25.0f;
	OPV.alfa = 20.0f;
	OPV.beta = 90.0f;


	g_isOrbitingLeft = false;
	g_isOrbitingRight = false;


	controlLlumsCotxe.frenando = false;
	controlLlumsCotxe.intermitenteIzquierdo = false;
	controlLlumsCotxe.intermitenteDerecho = false;
	controlLlumsCotxe.modoFaros = 0;
	controlLlumsCotxe.tiempoTotal = 0.0f;

	// Entorn VGI: Variables de control Skybox Cube
	SkyBoxCube = true;		skC_programID = 0;
	skC_VAOID.vaoId = 0;	skC_VAOID.vboId = 0;	skC_VAOID.nVertexs = 0;
	cubemapTexture = 0;

	// Entorn VGI: Variables de control del men� Transforma
	transf = false;		trasl = false;		rota = false;		escal = false;
	fact_Tras = 1;		fact_Rota = 90;
	TG.VTras.x = 0.0;	TG.VTras.y = 0.0;	TG.VTras.z = 0;	TGF.VTras.x = 0.0;	TGF.VTras.y = 0.0;	TGF.VTras.z = 0;
	TG.VRota.x = 0;		TG.VRota.y = 0;		TG.VRota.z = 0;	TGF.VRota.x = 0;	TGF.VRota.y = 0;	TGF.VRota.z = 0;
	TG.VScal.x = 1;		TG.VScal.y = 1;		TG.VScal.z = 1;	TGF.VScal.x = 1;	TGF.VScal.y = 1;	TGF.VScal.z = 1;

	transX = false;		transY = false;		transZ = false;
	GTMatrix = glm::mat4(1.0);		// Inicialitzar a identitat

	// Entorn VGI: Variables de control per les opcions de men� Ocultacions
	front_faces = true;	test_vis = false;	oculta = true;		back_line = false;

	// Entorn VGI: Variables de control del men� Iluminaci�		
	ilumina = SUAU;			//ifixe = true;					
	ilum2sides = false;
	bool ifixe[NUM_MAX_LLUMS] = { false };  // TODOS A 0


	printf("MAIN ifixe[] = ");
	for (int i = 0; i < NUM_MAX_LLUMS; i++) printf("%d ", ifixe[i]);
	printf("\n");
	// Reflexions actives: Ambient [1], Difusa [2] i Especular [3]. No actives: Emission [0]. 
	sw_material[0] = false;			sw_material[1] = true;			sw_material[2] = true;			sw_material[3] = true;	sw_material[4] = true;
	sw_material_old[0] = false;		sw_material_old[1] = true;		sw_material_old[2] = true;		sw_material_old[3] = true;	sw_material_old[4] = true;
	textura = true;				t_textura = CAP;				textura_map = true;
	for (i = 0; i < NUM_MAX_TEXTURES; i++) texturesID[i] = 0;
	tFlag_invert_Y = false;

	// Entorn VGI: Variables de control del men� Llums
	// Entorn VGI: Inicialitzaci� variables Llums
	llum_ambient = true;		llumGL[0].encesa = true;
	for (i = 1; i < NUM_MAX_LLUMS; i++) llumGL[i].encesa = false;
	for (i = 0; i < NUM_MAX_LLUMS; i++) {
		llumGL[i].posicio.x = 0.0;		llumGL[i].posicio.y = 0.0;		llumGL[i].posicio.z = 0.0;		llumGL[i].posicio.w = 1.0;
		llumGL[i].difusa.r = 1.0f;		llumGL[i].difusa.g = 1.0f;		llumGL[i].difusa.b = 1.0f;		llumGL[i].difusa.a = 1.0f;
		llumGL[i].especular.r = 1.0f;	llumGL[i].especular.g = 1.0f;	llumGL[i].especular.b = 1.0f;	llumGL[i].especular.a = 1.0f;
		llumGL[i].atenuacio.a = 0.0;	llumGL[i].atenuacio.b = 0.0;	llumGL[i].atenuacio.c = 1.0;
		llumGL[i].restringida = false;
		llumGL[i].spotdirection.x = 0.0;	llumGL[i].spotdirection.y = 0.0;	llumGL[i].spotdirection.z = 0.0; llumGL[i].spotdirection.w = 0.0;
		llumGL[i].spotcoscutoff = 0.0;		llumGL[i].spotexponent = 0.0;
	}

	// ---------------- LLUM #0 - (+Z) no restringida, amb atenuaci� constant (a,b,c) = (0,0,1)
	// Posici� de la font de llum (x,y,z)=(0,200,0):
	llumGL[0].posicio.x = 0.0;			llumGL[0].posicio.y = 0.0;			llumGL[0].posicio.z = 200.0;	llumGL[0].posicio.w = 1.0;

	// Intensitats difusa i especular de la font de llum (r,g,b) = (1,1,1):
	llumGL[0].difusa.r = 1.0f;			llumGL[0].difusa.g = 1.0f;			llumGL[0].difusa.b = 1.0f;		llumGL[0].difusa.a = 1.0f;
	llumGL[0].especular.r = 1.0f;		llumGL[0].especular.g = 1.0f;		llumGL[0].especular.b = 1.0f;	llumGL[0].especular.a = 1.0f;

	// Coeficients factor atenuaci� f_att=1/(ad2+bd+c). Llum sense atenuaci� per dist�ncia (a,b,c)=(0,0,1):
	llumGL[0].atenuacio.a = 0.0;		llumGL[0].atenuacio.b = 0.0;		llumGL[0].atenuacio.c = 1.0;

	// Par�metres font de llum restringida:
	llumGL[0].restringida = false;
	llumGL[0].spotdirection.x = 0.0;	llumGL[0].spotdirection.y = 0.0;	llumGL[0].spotdirection.z = -1.0;
	llumGL[0].spotcoscutoff = cos(25.0 * PI / 180);		llumGL[0].spotexponent = 1.0;		// llumGL[0].spotexponent = 45.0; Model de Warn (10, 500)

	// Activaci� font de llum: ENCESA
	llumGL[0].encesa = true;

	// ---------------- LLUM #1 - (+X) no restringida, amb atenuaci� constant (a,b,c) = (0,0,1)
	// Posici� de la font de llum (x,y,z)=(75,0,0):
	llumGL[1].posicio.x = 75.0;			llumGL[1].posicio.y = 0.0;			llumGL[1].posicio.z = 0.0;		llumGL[1].posicio.w = 1.0;

	// Intensitats difusa i especular de la font de llum (r,g,b) = (1,1,1):
	llumGL[1].difusa.r = 1.0f;			llumGL[1].difusa.g = 1.0f;			llumGL[1].difusa.b = 1.0f;		llumGL[1].difusa.a = 1.0f;
	llumGL[1].especular.r = 1.0f;		llumGL[1].especular.g = 1.0f;		llumGL[1].especular.b = 1.0f;	llumGL[1].especular.a = 1.0f;

	// Coeficients factor atenuaci� f_att=1/(ad2+bd+c). Llum sense atenuaci� per dist�ncia (a,b,c)=(0,0,1):
	llumGL[1].atenuacio.a = 0.0;		llumGL[1].atenuacio.b = 0.0;		llumGL[1].atenuacio.c = 1.0;

	// Par�metres font de llum restringida:
	llumGL[1].restringida = false;
	llumGL[1].spotdirection.x = 0.0;	llumGL[1].spotdirection.y = 0.0;	llumGL[1].spotdirection.z = 0.0;
	llumGL[1].spotcoscutoff = 0.0;		llumGL[1].spotexponent = 0.0;

	// Activaci� font de llum: APAGADA
	llumGL[1].encesa = false;

	// ---------------- LLUM #2 - (+Y) no restringida, amb atenuaci� constant (a,b,c) = (0,0,1)
	// Posici� de la font de llum (x,y,z)=(0,75,0):
	llumGL[2].posicio.x = 0.0;			llumGL[2].posicio.y = 75.0;			llumGL[2].posicio.z = 0.0;		llumGL[2].posicio.w = 1.0;

	// Intensitats difusa i especular de la font de llum (r,g,b) = (1,1,1):
	llumGL[2].difusa.r = 1.0f;			llumGL[2].difusa.g = 1.0f;			llumGL[2].difusa.b = 1.0f;		llumGL[2].difusa.a = 1.0f;
	llumGL[2].especular.r = 1.0f;		llumGL[2].especular.b = 1.0f;		llumGL[2].especular.b = 1.0f;	llumGL[2].especular.a = 1.0f;

	// Coeficients factor atenuaci� f_att=1/(ad2+bd+c). Llum amb atenuaci� per dist�ncia (a,b,c)=(0,0.025,1):
	llumGL[2].atenuacio.a = 0.0;		llumGL[2].atenuacio.b = 0.0;		llumGL[2].atenuacio.c = 1.0;

	// Par�metres font de llum restringida:
	llumGL[2].restringida = false;
	llumGL[2].spotdirection.x = 0.0;	llumGL[2].spotdirection.y = -1.0;	llumGL[2].spotdirection.z = 0.0;
	llumGL[2].spotcoscutoff = cos(2.5 * PI / 180);							llumGL[2].spotexponent = 1.0;

	// Activaci� font de llum: APAGADA
	llumGL[2].encesa = false;

	// ---------------- LLUM #3 - (Y=X), restringida amb 25 graus obertura i exponent 45, amb atenuaci� constant (a,b,c) = (0,0,1)
	// Posici� de la font de llum (x,y,z)=(75,75,75):
	llumGL[3].posicio.x = 75.0;			llumGL[3].posicio.y = 75.0;			llumGL[3].posicio.z = 75.0;		llumGL[3].posicio.w = 1.0;

	// Intensitats difusa i especular de la font de llum (r,g,b) = (0,1,0):
	llumGL[3].difusa.r = 0.0f;			llumGL[2].difusa.g = 1.0f;			llumGL[3].difusa.b = 0.0f;		llumGL[3].difusa.a = 1.0f;
	llumGL[3].especular.r = 0.0f;		llumGL[2].especular.g = 1.0f;		llumGL[3].especular.b = 0.0f;	llumGL[3].especular.a = 1.0f;

	// Coeficients factor atenuaci� f_att=1/(ad2+bd+c). Llum sense atenuaci� per dist�ncia (a,b,c)=(0,0,1):
	llumGL[3].atenuacio.a = 0.0;		llumGL[3].atenuacio.b = 0.0;		llumGL[3].atenuacio.c = 1.0;

	// Par�metres font de llum restringida:
	llumGL[3].restringida = true;
	llumGL[3].spotdirection.x = -1.0;	llumGL[3].spotdirection.y = -1.0;	llumGL[3].spotdirection.z = -1.0;
	llumGL[3].spotcoscutoff = cos(25.0 * PI / 180);							llumGL[3].spotexponent = 45.0;

	// Activaci� font de llum: APAGADA
	llumGL[3].encesa = false;

	// ---------------- LLUM #4 - (-Z), no restringida, amb atenuaci� constant (a,b,c) = (0,0,1)
	// Posici� de la font de llum (x,y,z)=(0,0,-75):
	llumGL[4].posicio.x = 0.0;			llumGL[4].posicio.y = 0.0;			llumGL[4].posicio.z = -75.0;	llumGL[4].posicio.w = 1.0;

	// Intensitats difusa i especular de la font de llum (r,g,b) = (1,1,1):
	llumGL[4].difusa.r = 1.0f;			llumGL[4].difusa.g = 1.0f;			llumGL[4].difusa.b = 1.0f;		llumGL[4].difusa.a = 1.0f;
	llumGL[4].especular.r = 1.0f;		llumGL[4].especular.g = 1.0f;		llumGL[4].especular.b = 1.0f;	llumGL[4].especular.a = 1.0f;

	// Coeficients factor atenuaci� f_att=1/(ad2+bd+c). Llum sense atenuaci� per dist�ncia (a,b,c)=(0,0,1):
	llumGL[4].atenuacio.a = 0.0;		llumGL[4].atenuacio.b = 0.0;		llumGL[4].atenuacio.c = 1.0;

	// Par�metres font de llum restringida:
	llumGL[4].restringida = false;
	llumGL[4].spotdirection.x = 0.0;	llumGL[4].spotdirection.y = 0.0;	llumGL[4].spotdirection.z = -1.0;
	llumGL[4].spotcoscutoff = cos(5 * PI / 180);							llumGL[4].spotexponent = 30.0;

	// Activaci� font de llum: APAGADA
	llumGL[4].encesa = false;

	// ---------------- LLUM #5 - (-Z), direccional, no restringida, amb atenuaci� constant (a,b,c) = (0,0,1)
	// Vector de la font de llum direccional (x,y,z)=(-1,-1,-1):
	llumGL[5].posicio.x = -1.0;			llumGL[5].posicio.y = -1.0;			llumGL[5].posicio.z = -1.0;		llumGL[5].posicio.w = 0.0;

	// Intensitats difusa i especular de la font de llum (r,g,b) = (1,0,0):
	llumGL[5].difusa.r = 1.0f;			llumGL[5].difusa.g = 0.0f;			llumGL[5].difusa.b = 0.0f;		llumGL[5].difusa.a = 1.0f;
	llumGL[5].especular.r = 1.0f;		llumGL[5].especular.g = 0.0f;		llumGL[5].especular.b = 0.0f;	llumGL[5].especular.a = 1.0f;

	// Coeficients factor atenuaci� f_att=1/(ad2+bd+c). Llum sense atenuaci� per dist�ncia (a,b,c)=(0,0,1):
	llumGL[5].atenuacio.a = 0.0;		llumGL[5].atenuacio.b = 0.0;		llumGL[5].atenuacio.c = 1.0;

	// Par�metres font de llum restringida:
	llumGL[5].restringida = false;
	llumGL[5].spotdirection.x = 0.0;	llumGL[5].spotdirection.y = 0.0;	llumGL[5].spotdirection.z = 0.0;
	llumGL[5].spotcoscutoff = 0.0;		llumGL[5].spotexponent = 0.0;

	// Activaci� font de llum: APAGADA
	llumGL[5].encesa = false;

	// ---------------- LLUM #6 - Llum Vaixell, configurada a la funci� vaixell() en escena.cpp.
	// Posici� de la font de llum (x,y,z)=(-75,75,75):
	llumGL[6].posicio.x = -75.0;		llumGL[6].posicio.y = 75.0;			llumGL[6].posicio.z = 75.0;		llumGL[6].posicio.w = 1.0;

	// Intensitats difusa i especular de la font de llum (r,g,b) = (1,1,1):
	llumGL[6].difusa.r = 1.0f;			llumGL[6].difusa.g = 1.0f;			llumGL[6].difusa.b = 1.0f;		llumGL[6].difusa.a = 1.0f;
	llumGL[6].especular.r = 1.0f;		llumGL[6].especular.g = 1.0f;		llumGL[6].especular.b = 1.0f;	llumGL[6].especular.a = 1;

	// Coeficients factor atenuaci� f_att=1/(ad2+bd+c). Llum sense atenuaci� per dist�ncia (a,b,c)=(0,0,1):
	llumGL[6].atenuacio.a = 0.0;		llumGL[6].atenuacio.b = 0.0;		llumGL[6].atenuacio.c = 1.0;

	// Par�metres font de llum restringida:
	llumGL[6].restringida = false;
	llumGL[6].spotdirection.x = 0.0;	llumGL[6].spotdirection.y = 0.0;	llumGL[6].spotdirection.z = 0.0;
	llumGL[6].spotcoscutoff = 0.0;		llumGL[6].spotexponent = 0.0;

	// Activaci� font de llum: APAGADA
	llumGL[6].encesa = false;

	// ---------------- LLUM #7 - Llum Far, configurada a la funci� faro() en escena.cpp.
	// Posici� de la font de llum (x,y,z)=(75,75,-75). Cap posici� definida, definida en funci� faro() en escena.cpp:
	llumGL[7].posicio.x = 75.0;			llumGL[7].posicio.y = 75.0;			llumGL[7].posicio.z = -75.0;	llumGL[7].posicio.w = 1.0;

	// Intensitats difusa i especular de la font de llum (r,g,b) = (1,1,1):
	llumGL[7].difusa.r = 1.0f;			llumGL[7].difusa.g = 1.0f;			llumGL[7].difusa.b = 1.0f;		llumGL[7].difusa.a = 1.0f;
	llumGL[7].especular.r = 1.0f;		llumGL[7].especular.g = 1.0f;		llumGL[7].especular.b = 1.0f;	llumGL[7].especular.a = 1;

	// Coeficients factor atenuaci� f_att=1/(ad2+bd+c). Llum sense atenuaci� per dist�ncia (a,b,c)=(0,0,1):
	llumGL[7].atenuacio.a = 0.0;		llumGL[7].atenuacio.b = 0.0;		llumGL[7].atenuacio.c = 1.0;

	// Par�metres font de llum restringida:
	llumGL[7].restringida = false;
	llumGL[7].spotdirection.x = 0.0;	llumGL[7].spotdirection.y = 0.0;	llumGL[7].spotdirection.z = 0.0;
	llumGL[7].spotcoscutoff = 0.0;		llumGL[7].spotexponent = 0.0;

	// Activaci� font de llum: APAGADA
	llumGL[7].encesa = false;
	// ---------------- FI DEFINICI� LLUMS

	// Entorn VGI: Variables de control del men� Shaders
	shader = CAP_SHADER;	shader_programID = 0;
	shaderLighting.releaseAllShaders();
	// C�rrega Shader de Gouraud
	shader_programID = 0;
	fprintf(stderr, "Phong_shdrML: \n");

	if (!shader_programID) shader_programID = shaderLighting.loadFileShaders(".\\shaders\\phong_shdrML.vert", ".\\shaders\\phong_shdrML.frag");
	shader = PHONG_SHADER;

	// C�rrega SHADERS
	// C�rrega Shader Eixos
	fprintf(stderr, "Eixos: \n");
	if (!eixos_programID) eixos_programID = shaderEixos.loadFileShaders(".\\shaders\\eixos.VERT", ".\\shaders\\eixos.FRAG");

	// C�rrega Shader Skybox
	fprintf(stderr, "SkyBox: \n");
	if (!skC_programID) skC_programID = shader_SkyBoxC.loadFileShaders(".\\shaders\\skybox.VERT", ".\\shaders\\skybox.FRAG");

	// C�rrega VAO Skybox Cube
	if (skC_VAOID.vaoId == 0) skC_VAOID = loadCubeSkybox_VAO();
	Set_VAOList(CUBE_SKYBOX, skC_VAOID);

	if (!cubemapTexture)
	{	// load Skybox textures
		// -------------
		std::vector<std::string> faces =
		{ ".\\textures\\skybox\\right.jpg",
			".\\textures\\skybox\\left.jpg",
			".\\textures\\skybox\\top.jpg",
			".\\textures\\skybox\\bottom.jpg",
			".\\textures\\skybox\\front.jpg",
			".\\textures\\skybox\\back.jpg"
		};
		cubemapTexture = loadCubemap(faces);
	}

	// Entorn VGI: Variables de control dels botons de mouse
	m_PosEAvall.x = 0;			m_PosEAvall.y = 0;			m_PosDAvall.x = 0;			m_PosDAvall.y = 0;
	m_ButoEAvall = false;		m_ButoDAvall = false;
	m_EsfeEAvall.R = 0.0;		m_EsfeEAvall.alfa = 0.0;	m_EsfeEAvall.beta = 0.0;
	m_EsfeIncEAvall.R = 0.0;	m_EsfeIncEAvall.alfa = 0.0;	m_EsfeIncEAvall.beta = 0.0;

	// Entorn VGI: Variables que controlen par�metres visualitzaci�: Mides finestra Windows i PV
	w = 640;			h = 480;			// Mides de la finestra Windows (w-amplada,h-al�ada)
	width_old = 640;	height_old = 480;	// Mides de la resoluci� actual de la pantalla (finestra Windows)
	w_old = 640;		h_old = 480;		// Mides de la finestra Windows (w-amplada,h-al�ada) per restaurar Finestra des de fullscreen
	//OPV.R = cam_Esferica[0];	OPV.alfa = cam_Esferica[1];		OPV.beta = cam_Esferica[2];		// Origen PV en esf�riques
	//OPV.R = 15.0;		OPV.alfa = 0.0;		OPV.beta = 0.0;										// Origen PV en esf�riques
	Vis_Polar = POLARZ;	oPolars = -1;

	// Entorn VGI: Color de fons i de l'objecte
	fonsR = true;		fonsG = true;		fonsB = true;
	c_fons.r = clear_colorB.x;		c_fons.g = clear_colorB.y;		c_fons.b = clear_colorB.z;			c_fons.b = clear_colorB.w;
	sw_color = false;
	col_obj.r = clear_colorO.x;	col_obj.g = clear_colorO.y;	col_obj.b = clear_colorO.z;		col_obj.a = clear_colorO.w;

	// Entorn VGI: Objecte OBJ
	ObOBJ = NULL;		vao_OBJ.vaoId = 0;		vao_OBJ.vboId = 0;		vao_OBJ.nVertexs = 0;

	// Entorn VGI: Variables del Timer
	t = 0;			anima = false;

	// Entorn VGI: Variables de l'objecte FRACTAL
	t_fractal = CAP;	soroll = 'C';
	pas = 64;			pas_ini = 64;
	sw_il = true;		palcolFractal = false;

	// Entorn VGI: Altres variables
	mida = 1.0;			nom = "";		buffer = "";
	initVAOList();	// Inicialtzar llista de VAO'S.

	// CREACI� DE COTXE
	if (miCoche == nullptr) {
		miCoche = new Coche();

	}

	if (cono == nullptr) {
		cono = new OBJ("cono");
	}
	if (cono_estatic == nullptr) {
		cono_estatic = new OBJ("cono_estatic");
	}
	if (circuit == nullptr) {
		circuit = new OBJ("circuit");
	}
	if (muro == nullptr) {
		muro = new OBJ("muro");
	}
	if (barrera == nullptr) {
		barrera = new OBJ("barrera");
	}
	if (bloc == nullptr) {
		bloc = new OBJ("bloc");
	}
	if (barril == nullptr) {
		barril = new OBJ("barril");
	}
	if (zonas == nullptr) {
		zonas = new Zones("punt");
	}

	if (senyal1 == nullptr) {
		senyal1 = new OBJ("senyal1");
	}

	if (senyal2 == nullptr) {
		senyal2 = new OBJ("senyal2");
	}
}

void GetGLVersion(int* major, int* minor)
{
	// for all versions
	char* ver = (char*)glGetString(GL_VERSION); // ver = "3.2.0"

	*major = ver[0] - '0';
	if (*major >= 3)
	{
		// for GL 3.x
		glGetIntegerv(GL_MAJOR_VERSION, major);		// major = 3
		glGetIntegerv(GL_MINOR_VERSION, minor);		// minor = 2
	}
	else
	{
		*minor = ver[2] - '0';
	}

	// GLSL
	ver = (char*)glGetString(GL_SHADING_LANGUAGE_VERSION);	// 1.50 NVIDIA via Cg compiler
}

void OnSize(GLFWwindow* window, int width, int height)
{
	// TODO: Agregue aqu� su c�digo de controlador de mensajes

	// A resize event occured; cx and cy are the window's new width and height.
	// Find the OpenGL change size function given in the Lab 1 notes and call it here

	// Entorn VGI: MODIFICACI� ->Establim les mides de la finestra actual
	w = width;	h = height;

	// Crida a OnPaint per a redibuixar la pantalla
	//	OnPaint();
}

// OnPaint: Funci� de dibuix i visualitzaci� en frame buffer del frame
void OnPaint(GLFWwindow* window)
{
	 


	int fbW, fbH;
	glfwGetFramebufferSize(window, &fbW, &fbH);
	int currentAmbience = 0; 
	if (g_MenuController != nullptr && g_MenuController->GetContext() != nullptr) {
		currentAmbience = g_MenuController->GetContext()->selectedLLum;
	}
	glViewport(0, 0, fbW, fbH);

	// Si usas w/h en el resto del c�digo, sincron�zalos
	w = fbW;
	h = fbH;

	// TODO: Agregue aqu� su c�digo de controlador de mensajes
	GLdouble vpv[3] = { 0.0, 0.0, 1.0 };

	ifixe[8] = true;
	ifixe[9] = true;
	ifixe[10] = true;
	ifixe[11] = true;
	ifixe[12] = true;
	ifixe[13] = true;
	ifixe[14] = true;
	ifixe[15] = true;
	ifixe[16] = true;
	ifixe[17] = true;

	// Entorn VGI.ImGui: Men� ImGui condicionat al color de fons
	if ((c_fons.r < 0.5) || (c_fons.g < 0.5) || (c_fons.b < 0.5))
		ImGui::StyleColorsLight();
	else ImGui::StyleColorsDark();

	// Entorn VGI: PROJECCI� PERSPECTIVA
				//glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); // Set Perspective Calculations To Most Accurate
	glDisable(GL_SCISSOR_TEST);		// Desactivaci� del retall de pantalla

	// Entorn VGI: Activar shader Visualitzaci� Escena
	glUseProgram(shader_programID);

	//LLUMS COTXE
	controlLlumsCotxe.tiempoTotal += 1.0f / 90.0f;
	func_llumsCotxe(miCoche, controlLlumsCotxe, llumGL);


	//OMBRES
	glm::vec3 lightPos(llumGL[0].posicio.x, llumGL[0].posicio.y, llumGL[0].posicio.z);
	glm::mat4 lightProjection = glm::ortho(-50.0f, 50.0f, -50.0f, 50.0f, 1.0f, 300.0f);
	glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 lightSpaceMatrix = lightProjection * lightView;

	glViewport(0, 0, shadowData.width, shadowData.height);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowData.FBO);
	glClear(GL_DEPTH_BUFFER_BIT);

	glUseProgram(simpleDepthShaderID);
	glUniformMatrix4fv(glGetUniformLocation(simpleDepthShaderID, "lightSpaceMatrix"), 1, GL_FALSE, &lightSpaceMatrix[0][0]);

	configura_Escena();
	dibuixa_EscenaGL(simpleDepthShaderID, eixos, eixos_Id, grid, hgrid, objecte, col_obj, sw_material,
		textura, texturesID, textura_map, tFlag_invert_Y,
		npts_T, PC_t, pas_CS, sw_Punts_Control, false,
		ObOBJ, lightView, GTMatrix, false);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glViewport(0, 0, w, h);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if ((c_fons.r < 0.5) || (c_fons.g < 0.5) || (c_fons.b < 0.5)) ImGui::StyleColorsLight();
	else ImGui::StyleColorsDark();

	glUseProgram(shader_programID);
	glUniformMatrix4fv(glGetUniformLocation(shader_programID, "lightSpaceMatrix"), 1, GL_FALSE, &lightSpaceMatrix[0][0]);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, shadowData.texture);
	glUniform1i(glGetUniformLocation(shader_programID, "shadowMap"), 1);
	glActiveTexture(GL_TEXTURE0);
	//FI OMBRES

	// Entorn VGI: Definici� de Viewport, Projecci� i C�mara
	ProjectionMatrix = Projeccio_Perspectiva(shader_programID, 0, 0, w, h, OPV.R);

	// Entorn VGI: Definici� de la c�mera.
	//if (camera == CAM_ESFERICA) {
	//	n[0] = 0;		n[1] = 0;		n[2] = 0;
	//	ViewMatrix = Vista_Esferica(shader_programID, OPV, Vis_Polar, pan, tr_cpv, tr_cpvF, c_fons, col_obj, objecte, mida, pas,
	//		front_faces, oculta, test_vis, back_line,
	//		ilumina, llum_ambient, llumGL, ifixe, ilum2sides,
	//		eixos, grid, hgrid);
	//	configura_Escena();     // Aplicar Transformacions Geometriques segons persiana Transformacio i configurar objectes.
	//	dibuixa_Escena();		// Dibuix geometria de l'escena amb comandes GL.
	//}


	if (camera == CAM_FOLLOW) {
		printf("MAIN ifixe[] = ");
		for (int i = 0; i < NUM_MAX_LLUMS; i++) printf("%d ", ifixe[i]);
		ViewMatrix = Vista_Seguimiento(shader_programID, miCoche, OPV, mobil, c_fons,
			oculta, test_vis, back_line, ilumina, llum_ambient,
			llumGL, ifixe, ilum2sides, currentAmbience);
		configura_Escena();     // Aplicar Transformacions Geometriques segons persiana Transformacio i configurar objectes.
		dibuixa_Escena();		// Dibuix geometria de l'escena amb comandes GL.
	}
	else if (camera == CAM_PRIMERA_PERSONA) {

		double fov_principal = 60.0f;
		double fov_central = 15.0f;
		double fov_lateral = 30.0f;
		glViewport(0, 0, w, h);

		ProjectionMatrix = Projeccio_Perspectiva(w, h, fov_principal);
		glUniformMatrix4fv(glGetUniformLocation(shader_programID, "projectionMatrix"), 1, GL_FALSE, &ProjectionMatrix[0][0]);
		ViewMatrix = Vista_PrimeraPersona(shader_programID, miCoche, c_fons,
			oculta, test_vis, back_line, ilumina, llum_ambient,
			llumGL, ifixe, ilum2sides,currentAmbience);

		configura_Escena();
		if (SkyBoxCube) dibuixa_Skybox(skC_programID, cubemapTexture, Vis_Polar, ProjectionMatrix, ViewMatrix);
		dibuixa_EscenaGL(shader_programID, eixos, eixos_Id, grid, hgrid, objecte, col_obj, sw_material,
			textura, texturesID, textura_map, tFlag_invert_Y,
			npts_T, PC_t, pas_CS, sw_Punts_Control, dibuixa_TriedreFrenet,
			ObOBJ, ViewMatrix, GTMatrix, true);
		int mirrorWidth = w / 4;
		int mirrorHeight = h / 5;
		int x_center = (w - mirrorWidth) / 2;
		int y_top = h - mirrorHeight - 10;


		glViewport(x_center, y_top, mirrorWidth, mirrorHeight);


		//glClear(GL_DEPTH_BUFFER_BIT);

		ProjectionMatrix = Projeccio_Perspectiva(mirrorWidth, mirrorHeight, fov_central);
		glUniformMatrix4fv(glGetUniformLocation(shader_programID, "projectionMatrix"), 1, GL_FALSE, &ProjectionMatrix[0][0]);
		ViewMatrix = Vista_Espejo_Central(shader_programID, miCoche, c_fons, oculta, test_vis, back_line, ilumina, llum_ambient, llumGL, ifixe, ilum2sides, currentAmbience);


		configura_Escena();
		dibuixa_EscenaGL(shader_programID, eixos, eixos_Id, grid, hgrid, objecte, col_obj, sw_material,
			textura, texturesID, textura_map, tFlag_invert_Y,
			npts_T, PC_t, pas_CS, sw_Punts_Control, dibuixa_TriedreFrenet,
			ObOBJ, ViewMatrix, GTMatrix, false);

		int x_left = 10;
		int retrovWidth = w / 5;
		int retrovHeight = h / 4;
		int y_top_ret = h - retrovHeight - 10;
		glViewport(x_left, y_top_ret, retrovWidth, retrovHeight);
		//glClear(GL_DEPTH_BUFFER_BIT);


		ProjectionMatrix = Projeccio_Perspectiva(retrovWidth, retrovHeight, fov_lateral);
		glUniformMatrix4fv(glGetUniformLocation(shader_programID, "projectionMatrix"), 1, GL_FALSE, &ProjectionMatrix[0][0]);
		ViewMatrix = Vista_Retrovisor(shader_programID, miCoche, true, c_fons, oculta, test_vis, back_line, ilumina, llum_ambient, llumGL, ifixe, ilum2sides, currentAmbience);


		configura_Escena();
		dibuixa_EscenaGL(shader_programID, eixos, eixos_Id, grid, hgrid, objecte, col_obj, sw_material,
			textura, texturesID, textura_map, tFlag_invert_Y,
			npts_T, PC_t, pas_CS, sw_Punts_Control, dibuixa_TriedreFrenet,
			ObOBJ, ViewMatrix, GTMatrix, false);

		int x_right = w - retrovWidth - 10;
		glViewport(x_right, y_top_ret, retrovWidth, retrovHeight);
		//glClear(GL_DEPTH_BUFFER_BIT);
		ProjectionMatrix = Projeccio_Perspectiva(retrovWidth, retrovHeight, fov_lateral);
		glUniformMatrix4fv(glGetUniformLocation(shader_programID, "projectionMatrix"), 1, GL_FALSE, &ProjectionMatrix[0][0]);
		ViewMatrix = Vista_Retrovisor(shader_programID, miCoche, false, c_fons, oculta, test_vis, back_line, ilumina, llum_ambient, llumGL, ifixe, ilum2sides,currentAmbience);


		configura_Escena();
		dibuixa_EscenaGL(shader_programID, eixos, eixos_Id, grid, hgrid, objecte, col_obj, sw_material,
			textura, texturesID, textura_map, tFlag_invert_Y,
			npts_T, PC_t, pas_CS, sw_Punts_Control, dibuixa_TriedreFrenet,
			ObOBJ, ViewMatrix, GTMatrix, false);


	}
	else if (camera == CAM_LLIURE) {
		ViewMatrix = Vista_Lliure(shader_programID, c_fons, OPV, g_FreeCamPos,
			oculta, test_vis, back_line, ilumina, llum_ambient,
			llumGL, ifixe, ilum2sides,currentAmbience);
		configura_Escena();     // Aplicar Transformacions Geometriques segons persiana Transformacio i configurar objectes.
		dibuixa_Escena();		// Dibuix geometria de l'escena amb comandes GL.
	}
	else if (camera == CAM_PAUSA) {

		ViewMatrix = Vista_Pausa(shader_programID, miCoche, OPV, mobil, c_fons,
			oculta, test_vis, back_line, ilumina, llum_ambient,
			llumGL, ifixe, ilum2sides, currentAmbience);
		configura_Escena();     // Aplicar Transformacions Geometriques segons persiana Transformacio i configurar objectes.
		dibuixa_Escena();		// Dibuix geometria de l'escena amb comandes GL.
	}
	else if (camera == CAM_INICI) {

		ViewMatrix = Vista_menu_inici(shader_programID, miCoche, OPV, mobil, c_fons,
			oculta, test_vis, back_line, ilumina, llum_ambient,
			llumGL, ifixe, ilum2sides, currentAmbience);
		configura_Escena();     // Aplicar Transformacions Geometriques segons persiana Transformacio i configurar objectes.
		dibuixa_Escena();		// Dibuix geometria de l'escena amb comandes GL.
	}
	
		//  Actualitzar la barra d'estat de l'aplicació amb els valors R,A,B,PVx,PVy,PVz
		//if (true) Barra_Estat(); 

}

// configura_Escena: Funcio que configura els parametres de Model i dibuixa les
//                   primitives OpenGL dins classe Model
void configura_Escena() {

	// Aplicar Transformacions Geometriques segons persiana Transformacio i Quaternions
	GTMatrix = instancia(transf, TG, TGF);
}

// dibuixa_Escena: Funcio que crida al dibuix dels diferents elements de l'escana
void dibuixa_Escena() {

	//glUseProgram(shader_programID);

//	Dibuix SkyBox C�bic.
	if (SkyBoxCube) dibuixa_Skybox(skC_programID, cubemapTexture, Vis_Polar, ProjectionMatrix, ViewMatrix);

	//	Dibuix Coordenades M�n i Reixes.
	dibuixa_Eixos(eixos_programID, eixos, eixos_Id, grid, hgrid, ProjectionMatrix, ViewMatrix);

	// Escalat d'objectes, per adequar-los a les vistes ortogr�fiques (Pr�ctica 2)
	//	GTMatrix = glm::scale();

	//	Dibuix geometria de l'escena amb comandes GL.
	dibuixa_EscenaGL(shader_programID, eixos, eixos_Id, grid, hgrid, objecte, col_obj, sw_material,
		textura, texturesID, textura_map, tFlag_invert_Y,
		npts_T, PC_t, pas_CS, sw_Punts_Control, dibuixa_TriedreFrenet,
		ObOBJ,				// Classe de l'objecte OBJ que cont� els VAO's
		ViewMatrix, GTMatrix);
}


// Barra_Estat: Actualitza la barra d'estat (Status Bar) de l'aplicaci� en la consola amb els
//      valors R,A,B,PVx,PVy,PVz en Visualitzaci� Interactiva.
void Barra_Estat()
{
	std::string buffer, sss;
	CEsfe3D OPVAux = { 0.0, 0.0, 0.0 };
	double PVx, PVy, PVz;

	// Status Bar fitxer fractal
	if (nom != "") fprintf(stderr, "Fitxer: %s \n", nom.c_str());

	// C�lcul dels valors per l'opci� Vista->Navega
	if (projeccio != CAP && projeccio != ORTO) {
		if (camera == CAM_ESFERICA)
		{	// C�mera Esf�rica
			OPVAux.R = OPV.R; OPVAux.alfa = OPV.alfa; OPVAux.beta = OPV.beta;
		}
	}
	else {
		OPVAux.R = OPV.R; OPVAux.alfa = OPV.alfa; OPVAux.beta = OPV.beta;
	}

	// Status Bar R Origen Punt de Vista
	if (projeccio == CAP) buffer = "       ";
	else if (projeccio == ORTO) buffer = " ORTO   ";
	else buffer = std::to_string(OPVAux.R);
	// Refrescar posici� R Status Bar
	fprintf(stderr, "R=: %s", buffer.c_str());

	// Status Bar angle alfa Origen Punt de Vista
	if (projeccio == CAP) buffer = "       ";
	else if (projeccio == ORTO) buffer = "ORTO   ";
	else buffer = std::to_string(OPVAux.alfa);
	// Refrescar posici� angleh Status Bar
	fprintf(stderr, " a=: %s", buffer.c_str());

	// Status Bar angle beta Origen Punt de Vista
	if (projeccio == CAP) buffer = "       ";
	else if (projeccio == ORTO) buffer = "ORTO   ";
	else buffer = std::to_string(OPVAux.beta);
	// Refrescar posici� anglev Status Bar
	fprintf(stderr, " �=: %s  ", buffer.c_str());

	// Status Bar PVx
	if (projeccio == CAP) buffer = "       ";
	else if (pan) buffer = std::to_string(tr_cpv.x);
	//sss = _T("PVx=") + buffer;
// Refrescar posici� PVx Status Bar
	fprintf(stderr, "PVx= %s", buffer.c_str());

	// Status Bar PVy
	if (projeccio == CAP) buffer = "       ";
	else if (pan) buffer = std::to_string(tr_cpv.y);
	//sss = "PVy=" + buffer;
// Refrescar posici� PVy Status Bar
	fprintf(stderr, " PVy= %s", buffer.c_str());

	// Status Bar PVz
	if (projeccio == CAP) buffer = "       ";
	else if (pan) buffer = std::to_string(tr_cpv.z);
	//sss = "PVz=" + buffer;
// Refrescar posici� PVz Status Bar
	fprintf(stderr, " PVz= %s \n", buffer.c_str());

	// Status Bar per indicar el modus de canvi de color (FONS o OBJECTE)
	sss = " ";
	if (sw_grid) sss = "GRID ";
	else if (pan) sss = "PAN ";
	else if (sw_color) sss = "OBJ ";
	else sss = "FONS ";
	// Refrescar posici� Transformacions en Status Bar
	fprintf(stderr, "%s ", sss.c_str());

	// Status Bar per indicar tipus de Transformaci� (TRAS, ROT, ESC)
	sss = " ";
	if (transf) {
		if (rota) sss = "ROT";
		else if (trasl) sss = "TRA";
		else if (escal) sss = "ESC";
	}
	// Refrescar posici� Transformacions en Status Bar
	fprintf(stderr, "%s ", sss.c_str());

	// Status Bar dels par�metres de Transformaci�, Color i posicions de Robot i Cama
	sss = " ";
	if (transf)
	{
		if (rota)
		{
			buffer = std::to_string(TG.VRota.x);
			sss = " " + buffer + " ";

			buffer = std::to_string(TG.VRota.y);
			sss = sss + buffer + " ";

			buffer = std::to_string(TG.VRota.z);
			sss = sss + buffer;
		}
		else if (trasl)
		{
			buffer = std::to_string(TG.VTras.x);
			sss = " " + buffer + " ";

			buffer = std::to_string(TG.VTras.y);
			sss = sss + buffer + " ";

			buffer = std::to_string(TG.VTras.z);
			sss = sss + buffer;
		}
		else if (escal)
		{
			buffer = std::to_string(TG.VScal.x);
			sss = " " + buffer + " ";

			buffer = std::to_string(TG.VScal.y);
			sss = sss + buffer + " ";

			buffer = std::to_string(TG.VScal.x);
			sss = sss + buffer;
		}
	}

	// Refrescar posici� PVz Status Bar
	fprintf(stderr, "%s \n", sss.c_str());
	// -----------------------------------------------------
	// --- NOU CODI LLUMS: VISUALITZAR ESTAT COTXE ---------
	// -----------------------------------------------------
	std::string infoLlums = "COTXE: ";

	// Estat Faros
	if (controlLlumsCotxe.modoFaros == 0) infoLlums += "[OFF] ";
	else if (controlLlumsCotxe.modoFaros == 1) infoLlums += "[CURTES] ";
	else if (controlLlumsCotxe.modoFaros == 2) infoLlums += "[LLARGUES] ";

	// Estat Frens
	if (controlLlumsCotxe.frenando) infoLlums += " [!!!FRE!!!] ";
	else infoLlums += "           ";

	// Estat Intermitents
	if (controlLlumsCotxe.intermitenteIzquierdo) infoLlums += "<< ";
	else infoLlums += "   ";

	if (controlLlumsCotxe.intermitenteDerecho) infoLlums += ">>";
	else infoLlums += "  ";

	// Imprimir al final de tot
	fprintf(stderr, "%s \n--------------------------------\n", infoLlums.c_str());

	// Status Bar per indicar el pas del Fractal
	if (objecte == O_FRACTAL)
	{
		buffer = std::to_string(pas);
		//sss = "Pas=" + buffer;
		fprintf(stderr, "Pas= %s \n", buffer.c_str());
	}
	else {
		sss = "          ";
		fprintf(stderr, "%s \n", sss.c_str());
	}
}

/* -------------------------------------------------------------------------------- */
/*					5. VISTA: Pantalla Completa, Pan i Eixos	                    */
/* -------------------------------------------------------------------------------- */

// SKYBOX: Visualitzar Skybox en l'escena (opci� booleana)
void OnVistaSkyBox()
{
	// TODO: Agregue aqu� su c�digo de controlador de comandos
	//SkyBoxCube = !SkyBoxCube;

// C�rrega Shader Skybox
	if (!skC_programID) skC_programID = shader_SkyBoxC.loadFileShaders(".\\shaders\\skybox.VERT", ".\\shaders\\skybox.FRAG");

	// C�rrega VAO Skybox Cube
	if (skC_VAOID.vaoId == 0) skC_VAOID = loadCubeSkybox_VAO();
	Set_VAOList(CUBE_SKYBOX, skC_VAOID);

	if (!cubemapTexture)
	{	// load Skybox textures
		// -------------
		std::vector<std::string> faces =
		{ ".\\textures\\skybox\\right.jpg",
			".\\textures\\skybox\\left.jpg",
			".\\textures\\skybox\\top.jpg",
			".\\textures\\skybox\\bottom.jpg",
			".\\textures\\skybox\\front.jpg",
			".\\textures\\skybox\\back.jpg"
		};
		cubemapTexture = loadCubemap(faces);
	}
}


/* ------------------------------------------------------------------------- */
/*                           CONTROL DEL TECLAT                              */
/* ------------------------------------------------------------------------- */

// OnKeyDown: Funci� de tractament de teclat (funci� que es crida quan es prem una tecla)
//   PAR�METRES:
//    - key: Codi del caracter seleccionat
//    - scancode: Nombre de vegades que s'ha apretat la tecla (acceleraci�)
//    - action: Acci� de la tecla: GLFW_PRESS (si s'ha apretat), GLFW_REPEAT, si s'ha repetit pressi� i GL_RELEASE, si es deixa d'apretar.
//    - mods: Variable que identifica si la tecla s'ha pulsat directa (mods=0), juntament amb la tecla Shift (mods=1) o la tecla Ctrl (mods=2).
void OnKeyDown(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// TODO: Agregue aqu� su c�digo de controlador de mensajes o llame al valor predeterminado
	const double incr = 0.025f;
	double modul = 0;
	GLdouble vdir[3] = { 0, 0, 0 };

	// EntornVGI.ImGui: Test si events de mouse han sigut filtrats per ImGui (io.WantCaptureMouse)
// (1) ALWAYS forward mouse data to ImGui! This is automatic with default backends. With your own backend:
	ImGuiIO& io = ImGui::GetIO();
	//io.AddMouseButtonEvent(button, true);

	// (2) ONLY forward mouse data to your underlying app/game.
	if (!io.WantCaptureKeyboard) { //<Tractament mouse de l'aplicaci�>}
		// EntornVGI: Si tecla pulsada �s ESCAPE, tancar finestres i aplicaci�.
		if (mods == 0 && key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {

			std::string currentState = g_MenuController->getState();

			if (currentState == "Playing") {

				g_MenuController->SwitchState(new PauseMenuState());
				return; // Salimos de la función para no procesar otras teclas.
			}
			if (currentState == "Pause") {
				// Si estamos en PauseMenuState, volvemos a PlayingState

				g_MenuController->SwitchState(new PlayingState());
				return; // Salimos de la función para no procesar otras teclas.
			}
		}
		else if (key == GLFW_KEY_M && action == GLFW_PRESS) {
			g_MenuController->SwitchState(new EndGameState());
		}
		else if (key == GLFW_KEY_R && action == GLFW_PRESS) {
			if (miCoche) miCoche->reiniciarPosicion();
		}
		else if (camera == CAM_FOLLOW)
			if (mods == 0 && key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) glfwSetWindowShouldClose(window, GL_TRUE);
		// =====================================================
	// ZONA DE CONTROL DE LUCES DEL COCHE
	// =====================================================


		if (key == GLFW_KEY_SPACE) //FRE o MARCHA ENRERE
		{
			if (action == GLFW_PRESS) controlLlumsCotxe.frenando = true;
			else if (action == GLFW_RELEASE) controlLlumsCotxe.frenando = false;
		}

		if (action == GLFW_PRESS)
		{
			switch (key)
			{
			case GLFW_KEY_L: //LLUMS CURTES I LLARGUES
				controlLlumsCotxe.modoFaros = (controlLlumsCotxe.modoFaros + 1) % 3;
				break;
			case GLFW_KEY_J: // ` //INTERMITENT ESQUERRE
				controlLlumsCotxe.intermitenteIzquierdo = !controlLlumsCotxe.intermitenteIzquierdo;
				break;
			case GLFW_KEY_K: // + //INTERMITENT DRET
				controlLlumsCotxe.intermitenteDerecho = !controlLlumsCotxe.intermitenteDerecho;
				break;
			case GLFW_KEY_H: //DOBLE INTERMITENT
				bool estado = !(controlLlumsCotxe.intermitenteIzquierdo && controlLlumsCotxe.intermitenteDerecho);
				controlLlumsCotxe.intermitenteIzquierdo = estado;
				controlLlumsCotxe.intermitenteDerecho = estado;
				break;
			}
		}

		// FI ZONA DE CONTROL DE LUCES DEL COCHE

		//SISTEMA CAMARA
		if (camera == CAM_FOLLOW)
		{
			// Si se PRESIONA una tecla
			if (action == GLFW_PRESS)
			{
				if (key == GLFW_KEY_LEFT) {
					g_isOrbitingLeft = true; // Activa la bandera izquierda
					mobil = true; // Pone la c�mara en modo manual
				}
				if (key == GLFW_KEY_RIGHT) {
					g_isOrbitingRight = true; // Activa la bandera derecha
					mobil = true; // Pone la c�mara en modo manual
				}
			}

			// Si se SUELTA una tecla
			if (action == GLFW_RELEASE)
			{
				if (key == GLFW_KEY_LEFT) {
					g_isOrbitingLeft = false; // Desactiva la bandera izquierda
				}
				if (key == GLFW_KEY_RIGHT) {
					g_isOrbitingRight = false; // Desactiva la bandera derecha
				}
			}
			if (key == GLFW_KEY_UP) {
				OPV.R = 25.0f;
				OPV.alfa = 20.0f;
				OPV.beta = 0.0f;
				mobil = false;

			}
		}
		else if (camera == CAM_LLIURE) {

			if (action == GLFW_PRESS) {
				if (key == GLFW_KEY_UP)    g_isMovingForward = true;
				if (key == GLFW_KEY_DOWN)  g_isMovingBackward = true;
				if (key == GLFW_KEY_LEFT)  g_isMovingLeft = true;
				if (key == GLFW_KEY_RIGHT) g_isMovingRight = true;
			}
			if (action == GLFW_RELEASE) {
				if (key == GLFW_KEY_UP)    g_isMovingForward = false;
				if (key == GLFW_KEY_DOWN)  g_isMovingBackward = false;
				if (key == GLFW_KEY_LEFT)  g_isMovingLeft = false;
				if (key == GLFW_KEY_RIGHT) g_isMovingRight = false;
			}

		}
		if (key == GLFW_KEY_C && action == GLFW_PRESS)
		{
			if (camera == CAM_FOLLOW) {
				camera = CAM_PRIMERA_PERSONA;
			}
			else {
				OPV.R = 25.0f;
				OPV.alfa = 20.0f;
				OPV.beta = 0.0f;
				camera = CAM_FOLLOW;
			}
		}
		if (key == GLFW_KEY_X && action == GLFW_PRESS)
		{
			if (camera == CAM_LLIURE) {
				OPV.R = 25.0f;
				OPV.alfa = 20.0f;
				OPV.beta = 0.0f;
				camera = CAM_PAUSA;
			}
			else {
				if (camera == CAM_PAUSA) {
					OPV.R = 25.0f;
					OPV.alfa = 20.0f;
					OPV.beta = 0.0f;
					camera = CAM_INICI;
				}
				else {
					OPV.R = 25.0f;
					OPV.alfa = 20.0f;
					OPV.beta = 0.0f;
					mobil = true;
					camera = CAM_LLIURE;
				}
			}

		}
	}
}

void OnJoystick(GLFWwindow* window) {

	// Solo entramos si hay mando conectado
	if (glfwJoystickPresent(GLFW_JOYSTICK_1)) {

		int count;
		const unsigned char* buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &count);

		int axesCount;
		const float* axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &axesCount);

		static bool lastButtons[20] = { false };


		if (count > 9 && buttons[9] == GLFW_PRESS && !lastButtons[9]) {

			// Obtenemos el estado actual del juego
			std::string currentState = "";
			if (g_MenuController) currentState = g_MenuController->getState();

			if (currentState == "Playing") {
				// Pausar juego
				g_MenuController->SwitchState(new PauseMenuState());
			}
			else if (currentState == "Pause") {
				// Reanudar juego
				g_MenuController->SwitchState(new PlayingState());
			}
		}


		if (g_MenuController && g_MenuController->getState() != "Playing") {
			// Actualizamos la memoria de botones antes de salir para evitar "clics fantasma" al volver
			for (int i = 0; i < count && i < 20; i++) lastButtons[i] = (buttons[i] == GLFW_PRESS);
			return; // <--- �AQU� SE ACABA LA FUNCI�N SI EST�S EN MEN�!
		}

		// --- MAPA DE BOTONES PS5 (Indices GLFW est�ndar) ---
		// 1: Cruz (Freno mano - Dejamos que coche.cpp lo gestione o lo hacemos aqu� si prefieres)
		// 2: C�rculo (ABS)
		// 0: Cuadrado (Warning)
		// 3: Tri�ngulo (Luces)
		// 4: L1 (Intermitente Izq)
		// 5: R1 (Intermitente Der)

		// ------------------------------------------
		// 1. CONTROL DE LUCES (Btn 3 - Tri�ngulo)
		// ------------------------------------------
		if (buttons[3] == GLFW_PRESS && !lastButtons[3]) {
			controlLlumsCotxe.modoFaros = (controlLlumsCotxe.modoFaros + 1) % 3;
			// printf("Luces cambiadas: %d\n", controlLlumsCotxe.modoFaros);
		}

		// ------------------------------------------
		// 2. INTERMITENTE IZQUIERDO (Btn 4 - L1)
		// ------------------------------------------
		if (buttons[4] == GLFW_PRESS && !lastButtons[4]) {
			controlLlumsCotxe.intermitenteIzquierdo = !controlLlumsCotxe.intermitenteIzquierdo;
			if (controlLlumsCotxe.intermitenteIzquierdo) controlLlumsCotxe.intermitenteDerecho = false;
		}

		// ------------------------------------------
		// 3. INTERMITENTE DERECHO (Btn 5 - R1)
		// ------------------------------------------
		if (buttons[5] == GLFW_PRESS && !lastButtons[5]) {
			controlLlumsCotxe.intermitenteDerecho = !controlLlumsCotxe.intermitenteDerecho;
			if (controlLlumsCotxe.intermitenteDerecho) controlLlumsCotxe.intermitenteIzquierdo = false;
		}

		// ------------------------------------------
		// 4. WARNING / EMERGENCIA (Btn 2 - CIRCULO)
		// ------------------------------------------
		if (buttons[2] == GLFW_PRESS && !lastButtons[2]) {
			bool estado = !(controlLlumsCotxe.intermitenteIzquierdo && controlLlumsCotxe.intermitenteDerecho);
			controlLlumsCotxe.intermitenteIzquierdo = estado;
			controlLlumsCotxe.intermitenteDerecho = estado;
		}

		// ------------------------------------------
		// 5. ABS (Btn 15 - Flecha arriba)
		// ------------------------------------------
		// NOTA: Necesitamos acceder a la variable del coche. 
		// Aseg�rate de que 'miCoche' es accesible aqu� y 'activadoABS' es p�blico.
		if (buttons[15] == GLFW_PRESS && !lastButtons[15]) {
			miCoche->activadoABS = !miCoche->activadoABS;
			// printf("ABS: %d\n", miCoche->activadoABS);
		}



		//flecha de abajo en el mando para cambiar la camara.
		//flecha de abajo en el mando para cambiar la camara.

		static bool wasCameraPressed = false;
		bool isCameraPressed = (count > 10 && buttons[17] == GLFW_PRESS);

		if (isCameraPressed && !wasCameraPressed) {

			if (camera == CAM_FOLLOW) {
				camera = CAM_PRIMERA_PERSONA;
			}
			else {
				// Resetear posici�n al volver a externa
				OPV.R = 25.0f;
				OPV.alfa = 20.0f;
				OPV.beta = 90.0f;

				mobil = false;
				g_isOrbitingLeft = false;
				g_isOrbitingRight = false;


				camera = CAM_FOLLOW;
			}
		}



		// ACTUALIZAR MEMORIA C�MARA INMEDIATAMENTE
		wasCameraPressed = isCameraPressed;



		static bool lastFrenoState = false;

		// 1. CAMBIO DE �NDICE:
		// Seg�n tu manual: "Fletxa dreta" es el �ndice 14.
		// Aseg�rate de comprobar que 'count > 14' para evitar errores de memoria.
		bool FMact = (count > 10 && buttons[16] == GLFW_PRESS);

		// 2. L�GICA (Id�ntica a la c�mara):
		// Si est� pulsado AHORA y NO lo estaba ANTES -> Entra.
		if (FMact && !lastFrenoState) {
			if (miCoche) {
				miCoche->FrenoDeMano = !miCoche->FrenoDeMano;
				printf("Freno Mano: %s\n", miCoche->FrenoDeMano ? "PUESTO" : "QUITADO");
			}
		}

		// 3. ACTUALIZAR MEMORIA:
		lastFrenoState = FMact;


		//controlar camara con r3
		if (camera == CAM_FOLLOW) {
			float rightStickX = (axesCount > 2) ? axes[2] : 0.0f;
			float deadzone = 0.2f;

			// Orbitar Izquierda/Derecha (Igual que Flechas Left/Right)
			if (rightStickX < -deadzone) {
				g_isOrbitingLeft = true;
				g_isOrbitingRight = false;
				mobil = true; // Activar modo manual
			}
			else if (rightStickX > deadzone) {
				g_isOrbitingRight = true;
				g_isOrbitingLeft = false;
				mobil = true;
			}
			else {
				// Si el stick est� quieto, solo desactivamos si NO se est� usando el teclado
				if (glfwGetKey(window, GLFW_KEY_LEFT) != GLFW_PRESS) g_isOrbitingLeft = false;
				if (glfwGetKey(window, GLFW_KEY_RIGHT) != GLFW_PRESS) g_isOrbitingRight = false;
			}
		}

		if (count > 10 && buttons[13] == GLFW_PRESS && !lastButtons[13]) {
			if (camera == CAM_FOLLOW) {
				// RESET TOTAL
				OPV.R = 25.0f;
				OPV.alfa = 20.0f;

				// IMPORTANTE: Pon aqu� 90.0f (o el valor que te funcion� antes para que no salte)
				OPV.beta = 90.0f;

				mobil = false; // Desactiva modo manual -> La c�mara sigue al coche
				g_isOrbitingLeft = false;
				g_isOrbitingRight = false;
			}
		}



		// --- ACTUALIZAR ESTADO ANTERIOR ---
		for (int i = 0; i < count && i < 20; i++) {
			lastButtons[i] = (buttons[i] == GLFW_PRESS);
		}
	}

}



void OnKeyUp(GLFWwindow* window, int key, int scancode, int action, int mods)
{

}

void OnTextDown(GLFWwindow* window, unsigned int codepoint)
{
}

// Teclat_Shift: Shortcuts per Pop Ups Fitxer, Finestra, Vista, Projecci� i Objecte
void Teclat_Shift(int key, GLFWwindow* window)
{
	//const char* nomfitxer;
	bool testA = false;
	int error = 0;

	//	nfdchar_t* nomFitxer = NULL;
	//	nfdresult_t result; // = NFD_OpenDialog(NULL, NULL, &nomFitxer);

	CColor color_Mar = { 0.0,0.0,0.0,1.0 };
}


// Teclat_Ctrl: Shortcuts per Pop Ups Transforma, Iluminaci�, llums, Shaders
void Teclat_Ctrl(int key)
{
	std::string nomVert, nomFrag;	// Nom de fitxer.

}


/* ------------------------------------------------------------------------- */
/*                           CONTROL DEL RATOLI                              */
/* ------------------------------------------------------------------------- */

// OnMouseButton: Funci� que es crida quan s'apreta algun bot� (esquerra o dreta) del mouse.
//      PARAMETRES: - window: Finestra activa
//					- button: Bot� seleccionat (GLFW_MOUSE_BUTTON_LEFT o GLFW_MOUSE_BUTTON_RIGHT)
//					- action: Acci� de la tecla: GLFW_PRESS (si s'ha apretat), GLFW_REPEAT, si s'ha repetit pressi� i GL_RELEASE, si es deixa d'apretar.
void OnMouseButton(GLFWwindow* window, int button, int action, int mods)
{
	// TODO: Agregue aqu� su c�digo de controlador de mensajes o llame al valor predeterminado
	// Get the cursor position when the mouse key has been pressed or released.
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	// EntornVGI.ImGui: Test si events de mouse han sigut filtrats per ImGui (io.WantCaptureMouse)
	// (1) ALWAYS forward mouse data to ImGui! This is automatic with default backends. With your own backend:
	ImGuiIO& io = ImGui::GetIO();
	io.AddMouseButtonEvent(button, action);

	// (2) ONLY forward mouse data to your underlying app/game.
	if (!io.WantCaptureMouse) { //<Tractament mouse de l'aplicaci�>}
		// OnLButtonDown
		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
		{
			// Entorn VGI: Detectem en quina posici� s'ha apretat el bot� esquerra del
			//				mouse i ho guardem a la variable m_PosEAvall i activem flag m_ButoEAvall
			m_ButoEAvall = true;
			m_PosEAvall.x = xpos;	m_PosEAvall.y = ypos;
			m_EsfeEAvall = OPV;
		}
		// OnLButtonUp: Funci� que es crida quan deixem d'apretar el bot� esquerra del mouse.
		else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
		{	// Entorn VGI: Desactivem flag m_ButoEAvall quan deixem d'apretar bot� esquerra del mouse.
			m_ButoEAvall = false;

			// OPCI� VISTA-->SAT�LIT: C�lcul increment despla�ament del Punt de Vista
			if ((satelit) && (projeccio != ORTO))
			{	//m_EsfeIncEAvall.R = m_EsfeEAvall.R - OPV.R;
				m_EsfeIncEAvall.alfa = 0.01f * (OPV.alfa - m_EsfeEAvall.alfa); //if (abs(m_EsfeIncEAvall.alfa)<0.01) { if ((m_EsfeIncEAvall.alfa)>0.0) m_EsfeIncEAvall.alfa = 0.01 else m_EsfeIncEAvall.alfa=0.01}
				m_EsfeIncEAvall.beta = 0.01f * (OPV.beta - m_EsfeEAvall.beta);
				if (abs(m_EsfeIncEAvall.beta) < 0.01)
				{
					if ((m_EsfeIncEAvall.beta) > 0.0) m_EsfeIncEAvall.beta = 0.01;
					else m_EsfeIncEAvall.beta = 0.01;
				}
				//if ((m_EsfeIncEAvall.R == 0.0) && (m_EsfeIncEAvall.alfa == 0.0) && (m_EsfeIncEAvall.beta == 0.0)) KillTimer(WM_TIMER);
				//else SetTimer(WM_TIMER, 10, NULL);
			}
		}
		// OnRButtonDown
		else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
		{	// Entorn VGI: Detectem en quina posici� s'ha apretat el bot� esquerra del
			//				mouse i ho guardem a la variable m_PosEAvall i activem flag m_ButoEAvall
			m_ButoDAvall = true;
			//m_PosDAvall = point;
			m_PosDAvall.x = xpos;	m_PosDAvall.y = ypos;
		}
		// OnLButtonUp: Funci� que es crida quan deixem d'apretar el bot� esquerra del mouse.
		else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
		{	// Entorn VGI: Desactivem flag m_ButoEAvall quan deixem d'apretar bot� esquerra del mouse.
			m_ButoDAvall = false;
		}
	}
}

// OnMouseMove: Funci� que es crida quan es mou el mouse. La utilitzem per la 
//				  Visualitzaci� Interactiva amb les tecles del mouse apretades per 
//				  modificar els par�metres de P.V. (R,angleh,anglev) segons els 
//				  moviments del mouse.
//      PARAMETRES: - window: Finestra activa
//					- xpos: Posici� X del cursor del mouse (coord. pantalla) quan el bot� s'ha apretat.
//					- ypos: Posici� Y del cursor del mouse(coord.pantalla) quan el bot� s'ha apretat.
void OnMouseMove(GLFWwindow* window, double xpos, double ypos)
{
	// TODO: Agregue aqu� su c�digo de controlador de mensajes o llame al valor predeterminado
	double modul = 0;
	GLdouble vdir[3] = { 0, 0, 0 };
	CSize gir = { 0,0 }, girn = { 0,0 }, girT = { 0,0 }, zoomincr = { 0,0 };

	// TODO: Add your message handler code here and/or call default
	if (m_ButoEAvall && mobil && projeccio != CAP)
	{
		// Entorn VGI: Determinaci� dels angles (en graus) segons l'increment
		//				horitzontal i vertical de la posici� del mouse.
		gir.cx = m_PosEAvall.x - xpos;		gir.cy = m_PosEAvall.y - ypos;
		m_PosEAvall.x = xpos;				m_PosEAvall.y = ypos;
		if (camera == CAM_ESFERICA || camera == CAM_LLIURE)
		{	// C�mera Esf�rica
			OPV.beta = OPV.beta - gir.cx / 2.0;
			OPV.alfa = OPV.alfa + gir.cy / 2.0;

			// Entorn VGI: Control per evitar el creixement desmesurat dels angles.
			while (OPV.alfa >= 360)		OPV.alfa = OPV.alfa - 360.0;
			while (OPV.alfa < 0)		OPV.alfa = OPV.alfa + 360.0;
			while (OPV.beta >= 360)		OPV.beta = OPV.beta - 360.0;
			while (OPV.beta < 0)		OPV.beta = OPV.beta + 360.0;

		}
		else { // C�mera Geode
			OPV_G.beta = OPV_G.beta + gir.cx / 2.0;
			OPV_G.alfa = OPV_G.alfa + gir.cy / 2.0;
			// Entorn VGI: Control per evitar el creixement desmesurat dels angles
			while (OPV_G.alfa >= 360.0f)	OPV_G.alfa = OPV_G.alfa - 360.0;
			while (OPV_G.alfa < 0.0f)		OPV_G.alfa = OPV_G.alfa + 360.0;
			while (OPV_G.beta >= 360.f)		OPV_G.beta = OPV_G.beta - 360.0;
			while (OPV_G.beta < 0.0f)		OPV_G.beta = OPV_G.beta + 360.0;
		}
		// Crida a OnPaint() per redibuixar l'escena
		//OnPaint(window);
	}
	else if (m_ButoEAvall && (camera == CAM_NAVEGA) && (projeccio != CAP && projeccio != ORTO)) // Opci� Navegaci�
	{	// Entorn VGI: Canviar orientaci� en opci� de Navegaci�
		girn.cx = m_PosEAvall.x - xpos;		girn.cy = m_PosEAvall.y - ypos;
		angleZ = girn.cx / 2.0;
		// Entorn VGI: Control per evitar el creixement desmesurat dels angles.
		while (angleZ >= 360, 0) angleZ = angleZ - 360;
		while (angleZ < 0.0)	angleZ = angleZ + 360;

		// Entorn VGI: Segons orientaci� dels eixos Polars (Vis_Polar)
		if (Vis_Polar == POLARZ) { // (X,Y,Z)
			n[0] = n[0] - opvN.x;
			n[1] = n[1] - opvN.y;
			n[0] = n[0] * cos(angleZ * PI / 180.0) - n[1] * sin(angleZ * PI / 180.0);
			n[1] = n[0] * sin(angleZ * PI / 180.0) + n[1] * cos(angleZ * PI / 180.0);
			n[0] = n[0] + opvN.x;
			n[1] = n[1] + opvN.y;
		}
		else if (Vis_Polar == POLARY) { //(X,Y,Z) --> (Z,X,Y)
			n[2] = n[2] - opvN.z;
			n[0] = n[0] - opvN.x;
			n[2] = n[2] * cos(angleZ * PI / 180.0) - n[0] * sin(angleZ * PI / 180.0);
			n[0] = n[2] * sin(angleZ * PI / 180.0) + n[0] * cos(angleZ * PI / 180.0);
			n[2] = n[2] + opvN.z;
			n[0] = n[0] + opvN.x;
		}
		else if (Vis_Polar == POLARX) { //(X,Y,Z) --> (Y,Z,X)
			n[1] = n[1] - opvN.y;
			n[2] = n[2] - opvN.z;
			n[1] = n[1] * cos(angleZ * PI / 180.0) - n[2] * sin(angleZ * PI / 180.0);
			n[2] = n[1] * sin(angleZ * PI / 180.0) + n[2] * cos(angleZ * PI / 180.0);
			n[1] = n[1] + opvN.y;
			n[2] = n[2] + opvN.z;
		}

		m_PosEAvall.x = xpos;		m_PosEAvall.y = ypos;
		// Crida a OnPaint() per redibuixar l'escena
		//OnPaint(window);
	}

	// Entorn VGI: Transformaci� Geom�trica interactiva pels eixos X,Y boto esquerra del mouse.
	else {
		bool transE = transX || transY;
		if (m_ButoEAvall && transE && transf)
		{	// Calcular increment
			girT.cx = m_PosEAvall.x - xpos;		girT.cy = m_PosEAvall.y - ypos;
			if (transX)
			{
				long int incrT = girT.cx;
				if (trasl)
				{
					TG.VTras.x += incrT * fact_Tras;
					if (TG.VTras.x < -100000.0) TG.VTras.x = 100000.0;
					if (TG.VTras.x > 100000.0) TG.VTras.x = 100000.0;
				}
				else if (rota)
				{
					TG.VRota.x += incrT * fact_Rota;
					while (TG.VRota.x >= 360.0) TG.VRota.x -= 360.0;
					while (TG.VRota.x < 0.0) TG.VRota.x += 360.0;
				}
				else if (escal)
				{
					if (incrT < 0) incrT = -1 / incrT;
					TG.VScal.x = TG.VScal.x * incrT;
					if (TG.VScal.x < 0.25) TG.VScal.x = 0.25;
					if (TG.VScal.x > 8192.0) TG.VScal.x = 8192.0;
				}
			}
			if (transY)
			{
				long int incrT = girT.cy;
				if (trasl)
				{
					TG.VTras.y += incrT * fact_Tras;
					if (TG.VTras.y < -100000) TG.VTras.y = 100000;
					if (TG.VTras.y > 100000) TG.VTras.y = 100000;
				}
				else if (rota)
				{
					TG.VRota.y += incrT * fact_Rota;
					while (TG.VRota.y >= 360.0) TG.VRota.y -= 360.0;
					while (TG.VRota.y < 0.0) TG.VRota.y += 360.0;
				}
				else if (escal)
				{
					if (incrT <= 0.0) {
						if (incrT >= -2) incrT = -2;
						incrT = 1 / Log2(-incrT);
					}
					else incrT = Log2(incrT);
					TG.VScal.y = TG.VScal.y * incrT;
					if (TG.VScal.y < 0.25) TG.VScal.y = 0.25;
					if (TG.VScal.y > 8192.0) TG.VScal.y = 8192.0;
				}
			}
			m_PosEAvall.x = xpos;	m_PosEAvall.y = ypos;
			// Crida a OnPaint() per redibuixar l'escena
			//InvalidateRect(NULL, false);
			//OnPaint(windows);
		}
	}

	// Entorn VGI: Determinaci� del despla�ament del pan segons l'increment
	//				vertical de la posici� del mouse (tecla dreta apretada).
	if (m_ButoDAvall && pan && (projeccio != CAP && projeccio != ORTO))
	{
		//CSize zoomincr = m_PosDAvall - point;
		zoomincr.cx = m_PosDAvall.x - xpos;		zoomincr.cy = m_PosDAvall.y - ypos;
		long int incrx = zoomincr.cx;
		long int incry = zoomincr.cy;

		// Despla�ament pan vertical
		tr_cpv.y -= incry * fact_pan;
		if (tr_cpv.y > 100000.0) tr_cpv.y = 100000.0;
		else if (tr_cpv.y < -100000.0) tr_cpv.y = -100000.0;

		// Despla�ament pan horitzontal
		tr_cpv.x += incrx * fact_pan;
		if (tr_cpv.x > 100000.0) tr_cpv.x = 100000.0;
		else if (tr_cpv.x < -100000.0) tr_cpv.x = -100000.0;

		//m_PosDAvall = point;
		m_PosDAvall.x = xpos;	m_PosDAvall.y = ypos;
		// Crida a OnPaint() per redibuixar l'escena
		//OnPaint(window);
	}
	// Determinaci� del par�metre R segons l'increment
	//   vertical de la posici� del mouse (tecla dreta apretada)
		//else if (m_ButoDAvall && zzoom && (projeccio!=CAP && projeccio!=ORTO))
	else if (m_ButoDAvall && zzoom && (projeccio != CAP))
	{	//CSize zoomincr = m_PosDAvall - point;
		zoomincr.cx = m_PosDAvall.x - xpos;		zoomincr.cy = m_PosDAvall.y - ypos;
		long int incr = zoomincr.cy / 1.0;

		if (camera == CAM_ESFERICA || camera == CAM_LLIURE) {	// C�mera Esf�rica
			OPV.R = OPV.R + incr;
			//if (OPV.R < 0.25) OPV.R = 0.25;
			if (OPV.R < p_near) OPV.R = p_near;
			if (OPV.R > p_far) OPV.R = p_far;
		}
		else { // C�mera Geode
			OPV_G.R = OPV_G.R + incr;
			if (OPV_G.R < 0.0) OPV_G.R = 0.0;
		}

		//m_PosDAvall = point;
		m_PosDAvall.x = xpos;				m_PosDAvall.y = ypos;
		// Crida a OnPaint() per redibuixar l'escena
		//OnPaint(window);
	}

	// Entorn VGI: Transformaci� Geom�trica interactiva per l'eix Z amb boto dret del mouse.
	else if (m_ButoDAvall && transZ && transf)
	{	// Calcular increment
		girT.cx = m_PosDAvall.x - xpos;		girT.cy = m_PosDAvall.y - ypos;
		long int incrT = girT.cy;
		if (trasl)
		{
			TG.VTras.z += incrT * fact_Tras;
			if (TG.VTras.z < -100000.0) TG.VTras.z = 100000.0;
			if (TG.VTras.z > 100000.0) TG.VTras.z = 100000.0;
		}
		else if (rota)
		{
			incrT = girT.cx;
			TG.VRota.z += incrT * fact_Rota;
			while (TG.VRota.z >= 360.0) TG.VRota.z -= 360.0;
			while (TG.VRota.z < 0.0) TG.VRota.z += 360.0;
		}
		else if (escal)
		{
			if (incrT <= 0) {
				if (incrT >= -2.0) incrT = -2.0;
				incrT = 1.0 / Log2(-incrT);
			}
			else incrT = Log2(incrT);
			TG.VScal.z = TG.VScal.z * incrT;
			if (TG.VScal.z < 0.25) TG.VScal.z = 0.25;
			if (TG.VScal.z > 8192.0) TG.VScal.z = 8192.0;
		}
		//m_PosDAvall = point;
		m_PosDAvall.x = xpos;	m_PosDAvall.y = ypos;
		// Crida a OnPaint() per redibuixar l'escena
		//OnPaint(window);
	}
}

// OnMouseWheel: Funci� que es crida quan es mou el rodet del mouse. La utilitzem per la 
//				  Visualitzaci� Interactiva per modificar el par�metre R de P.V. (R,angleh,anglev) 
//				  segons el moviment del rodet del mouse.
//      PARAMETRES: -  (xoffset,yoffset): Estructura (x,y) que d�na la posici� del mouse 
//							 (coord. pantalla) quan el bot� s'ha apretat.
void OnMouseWheel(GLFWwindow* window, double xoffset, double yoffset)
{
	// TODO: Agregue aqu� su c�digo de controlador de mensajes o llame al valor predeterminado
	double modul = 0;
	GLdouble vdir[3] = { 0, 0, 0 };

	// EntornVGI.ImGui: Test si events de mouse han sigut filtrats per ImGui (io.WantCaptureMouse)
	// (1) ALWAYS forward mouse data to ImGui! This is automatic with default backends. With your own backend:
	ImGuiIO& io = ImGui::GetIO();
	//io.AddMouseButtonEvent(button, true);

// (2) ONLY forward mouse data to your underlying app/game.
	if (!io.WantCaptureMouse) { // <Tractament mouse de l'aplicaci�>}
		// Funci� de zoom quan est� activada la funci� pan o les T. Geom�triques
		if ((zzoom || zzoomO) || (transX) || (transY) || (transZ))
		{
			if (camera == CAM_ESFERICA) {	// C�mera Esf�rica
				OPV.R = OPV.R + yoffset / 4.0;
				if (OPV.R < 0.25) OPV.R = 0.25;
				//InvalidateRect(NULL, false);
			}
		}
	}
}


/* ------------------------------------------------------------------------- */
/*					     TIMER (ANIMACI�)									 */
/* ------------------------------------------------------------------------- */
void OnTimer()
{
	// TODO: Agregue aqu� su c�digo de controlador de mensajes o llame al valor predeterminado
	if (anima) {
		// Codi de tractament de l'animaci� quan transcorren els ms. del crono.

		// Crida a OnPaint() per redibuixar l'escena
		//InvalidateRect(NULL, false);
	}
	else if (satelit) {	// OPCI� SAT�LIT: Increment OPV segons moviments mouse.
		//OPV.R = OPV.R + m_EsfeIncEAvall.R;
		OPV.alfa = OPV.alfa + m_EsfeIncEAvall.alfa;
		while (OPV.alfa > 360.0) OPV.alfa = OPV.alfa - 360.0;
		while (OPV.alfa < 0.0) OPV.alfa = OPV.alfa + 360.0;
		OPV.beta = OPV.beta + m_EsfeIncEAvall.beta;
		while (OPV.beta > 360.0) OPV.beta = OPV.beta - 360.0;
		while (OPV.beta < 0.0) OPV.beta = OPV.beta + 360.0;

		// Crida a OnPaint() per redibuixar l'escena
		//OnPaint();
	}
}

// ---------------- Entorn VGI: Funcions locals a main.cpp

// Log2: C�lcul del log base 2 de num
int Log2(int num)
{
	int tlog;

	if (num >= 8192) tlog = 13;
	else if (num >= 4096) tlog = 12;
	else if (num >= 2048) tlog = 11;
	else if (num >= 1024) tlog = 10;
	else if (num >= 512) tlog = 9;
	else if (num >= 256) tlog = 8;
	else if (num >= 128) tlog = 7;
	else if (num >= 64) tlog = 6;
	else if (num >= 32) tlog = 5;
	else if (num >= 16) tlog = 4;
	else if (num >= 8) tlog = 3;
	else if (num >= 4) tlog = 2;
	else if (num >= 2) tlog = 1;
	else tlog = 0;

	return tlog;
}

// -------------------- TRACTAMENT ERRORS
// error_callback: Displaia error que es pugui produir
void error_callback(int code, const char* description)
{
	//const char* descripcio;
	//int codi = glfwGetError(&descripcio);

 //   display_error_message(code, description);
	fprintf(stderr, "Codi Error: %i", code);
	fprintf(stderr, "Descripcio: %s\n", description);
}


GLenum glCheckError_(const char* file, int line)
{
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR)
	{
		std::string error;
		switch (errorCode)
		{
		case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
		case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
		case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
		case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
		case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
		case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
		}
		fprintf(stderr, "ERROR %s | File: %s | Line ( %3i ) \n", error.c_str(), file, line);
		//fprintf(stderr, "ERROR %s | ", error.c_str());
		//fprintf(stderr, "File: %s | ", file);
		//fprintf(stderr, "Line ( %3i ) \n", line);
	}
	return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

void APIENTRY glDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
	const GLchar* message, const void* userParam)
{
	if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return; // ignore these non-significant error codes

	fprintf(stderr, "---------------\n");
	fprintf(stderr, "Debug message ( %3i %s \n", id, message);

	switch (source)
	{
	case GL_DEBUG_SOURCE_API:             fprintf(stderr, "Source: API \n"); break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   fprintf(stderr, "Source: Window System \n"); break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER: fprintf(stderr, "Source: Shader Compiler \n"); break;
	case GL_DEBUG_SOURCE_THIRD_PARTY:     fprintf(stderr, "Source: Third Party \n"); break;
	case GL_DEBUG_SOURCE_APPLICATION:     fprintf(stderr, "Source: Application \n"); break;
	case GL_DEBUG_SOURCE_OTHER:           fprintf(stderr, "Source: Other \n"); break;
	} //std::cout << std::endl;

	switch (type)
	{
	case GL_DEBUG_TYPE_ERROR:               fprintf(stderr, "Type: Error\n"); break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: fprintf(stderr, "Type: Deprecated Behaviour\n"); break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  fprintf(stderr, "Type: Undefined Behaviour\n"); break;
	case GL_DEBUG_TYPE_PORTABILITY:         fprintf(stderr, "Type: Portability\n"); break;
	case GL_DEBUG_TYPE_PERFORMANCE:         fprintf(stderr, "Type: Performance\n"); break;
	case GL_DEBUG_TYPE_MARKER:              fprintf(stderr, "Type: Marker\n"); break;
	case GL_DEBUG_TYPE_PUSH_GROUP:          fprintf(stderr, "Type: Push Group\n"); break;
	case GL_DEBUG_TYPE_POP_GROUP:           fprintf(stderr, "Type: Pop Group\n"); break;
	case GL_DEBUG_TYPE_OTHER:               fprintf(stderr, "Type: Other\n"); break;
	} //std::cout << std::endl;

	switch (severity)
	{
	case GL_DEBUG_SEVERITY_HIGH:         fprintf(stderr, "Severity: high\n"); break;
	case GL_DEBUG_SEVERITY_MEDIUM:       fprintf(stderr, "Severity: medium\n"); break;
	case GL_DEBUG_SEVERITY_LOW:          fprintf(stderr, "Severity: low\n"); break;
	case GL_DEBUG_SEVERITY_NOTIFICATION: fprintf(stderr, "Severity: notification\n"); break;
	} //std::cout << std::endl;
	//std::cout << std::endl;
	fprintf(stderr, "\n");

}


void RenderLoadingScreenSimple()
{
	// ---------- Fondo base neutro ----------
	glClearColor(0.12f, 0.14f, 0.18f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGuiIO& io = ImGui::GetIO();
	ImVec2 size = io.DisplaySize;
	ImVec2 center(size.x * 0.5f, size.y * 0.5f);

	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(size);

	ImGui::Begin("##LoadingGimcanaBG", nullptr,
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoBackground |
		ImGuiWindowFlags_NoInputs);

	ImDrawList* draw = ImGui::GetWindowDrawList();
	const float grid = 60.0f;
	ImU32 gridColor = IM_COL32(255, 255, 255, 25);

	for (float x = 0; x < size.x; x += grid)
		draw->AddLine(ImVec2(x, 0), ImVec2(x, size.y), gridColor);

	for (float y = 0; y < size.y; y += grid)
		draw->AddLine(ImVec2(0, y), ImVec2(size.x, y), gridColor);


	// -------- TÍTULO PRINCIPAL --------
	ImGui::SetWindowFontScale(4.0f);
	const char* title = u8"GIMCANA AUTOMOBILÍSTICA";
	ImVec2 titleSize = ImGui::CalcTextSize(title);

	ImVec2 titlePos(center.x - titleSize.x * 0.5f, center.y - 170);

	// Sombra fuerte
	draw->AddText(
		ImVec2(titlePos.x + 4, titlePos.y + 4),
		IM_COL32(0, 0, 0, 160),
		title
	);

	// Texto blanco
	draw->AddText(
		titlePos,
		IM_COL32(255, 255, 255, 255),
		title
	);

	ImGui::SetWindowFontScale(1.0f);

	// -------- SUBT�TULO -------
	const char* loading = u8"Carregant...";
	ImVec2 loadSize = ImGui::CalcTextSize(loading);
	ImVec2 loadPos(center.x - loadSize.x * 0.5f, center.y - 40);

	// Sombra suave
	draw->AddText(
		ImVec2(loadPos.x + 2, loadPos.y + 2),
		IM_COL32(0, 0, 0, 100),
		loading
	);

	// Texto gris claro
	draw->AddText(
		loadPos,
		IM_COL32(220, 220, 220, 240),
		loading
	);

	// -------- CONSEJO --------
	const char* tip =
		u8"Consell: Controla l'accelerador,\n"
		u8"             no tot és velocitat.";

	ImVec2 tipSize = ImGui::CalcTextSize(tip);
	ImVec2 tipPos(center.x - tipSize.x * 0.5f, center.y + 45);

	// Sombra
	draw->AddText(
		ImVec2(tipPos.x + 2, tipPos.y + 2),
		IM_COL32(0, 0, 0, 120),
		tip
	);

	// Texto azul
	draw->AddText(
		tipPos,
		IM_COL32(30, 130, 200, 255),
		tip
	);


	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void RenderExitScreenSimple()
{
	// ---------- Fondo ----------
	glClearColor(0.12f, 0.14f, 0.18f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGuiIO& io = ImGui::GetIO();
	ImVec2 size = io.DisplaySize;
	ImVec2 center(size.x * 0.5f, size.y * 0.5f);

	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(size);

	ImGui::Begin("##ExitScreenBG", nullptr,
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoBackground |
		ImGuiWindowFlags_NoInputs);

	ImDrawList* draw = ImGui::GetWindowDrawList(); 
	const float grid = 60.0f;
	ImU32 gridColor = IM_COL32(255, 255, 255, 25);

	for (float x = 0; x < size.x; x += grid)
		draw->AddLine(ImVec2(x, 0), ImVec2(x, size.y), gridColor);

	for (float y = 0; y < size.y; y += grid)
		draw->AddLine(ImVec2(0, y), ImVec2(size.x, y), gridColor);

	// ---------- TÍTULO ----------
	ImGui::SetWindowFontScale(3.5f);
	const char* title = u8"GIMCANA AUTOMOBILÍSTICA";
	ImVec2 titleSize = ImGui::CalcTextSize(title);
	ImVec2 titlePos(center.x - titleSize.x * 0.5f, center.y - 160);

	draw->AddText(
		ImVec2(titlePos.x + 3, titlePos.y + 3),
		IM_COL32(0, 0, 0, 160),
		title);

	draw->AddText(
		titlePos,
		IM_COL32(255, 255, 255, 255),
		title);

	ImGui::SetWindowFontScale(1.0f);

	// ---------- MENSAJE FINAL ----------
	const char* msg = u8"Gràcies per jugar!";
	ImVec2 msgSize = ImGui::CalcTextSize(msg);
	ImVec2 msgPos(center.x - msgSize.x * 0.5f, center.y - 20);

	draw->AddText(
		ImVec2(msgPos.x + 2, msgPos.y + 2),
		IM_COL32(0, 0, 0, 120),
		msg);

	draw->AddText(
		msgPos,
		IM_COL32(220, 220, 220, 255),
		msg);
	 
	const char* credits =
		u8"                                 Projecte acadèmic · UAB · 2025\n"
		u8"                                Visualització Gràfica Interactiva\n" 
		u8"Jose Angel Boza Zapater · Alvaro Ruiz Hi · Joana Figueroa Zapata\n"
		u8" Oussama Berrouhou · Iker Bolancel Fernández · Pau Pérez Cardús";

	ImVec2 credSize = ImGui::CalcTextSize(credits);
	ImVec2 credPos(center.x - credSize.x * 0.5f, center.y + 50);

	draw->AddText(
		ImVec2(credPos.x + 2, credPos.y + 2),
		IM_COL32(0, 0, 0, 120),
		credits);

	draw->AddText(
		credPos,
		IM_COL32(150, 180, 220, 255),
		credits);

	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}


int main(void)
{
	//    GLFWwindow* window;
	// Entorn VGI. Timer: Variables
	float time = elapsedTime;
	float now;
	float delta;



	// glfw: initialize and configure
	// ------------------------------
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		getchar();
		return -1;
	}

	// Retrieving main monitor
	primary = glfwGetPrimaryMonitor();

	// To get current video mode of a monitor
	mode = glfwGetVideoMode(primary);
	//glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE); window = glfwCreateWindow(640, 480, "Titulo", NULL, NULL); 
	// window = glfwCreateWindow(640, 480, "Entorn Grafic VS2022 amb GLFW i OpenGL 4.6 (Visualitzacio Grafica Interactiva - Grau en Enginyeria Informatica - Escola Enginyeria - UAB)", NULL, NULL);
	// Retrieving monitors
	//    int countM;
	//   GLFWmonitor** monitors = glfwGetMonitors(&countM);

	// Retrieving video modes of the monitor
	int countVM;
	const GLFWvidmode* modes = glfwGetVideoModes(primary, &countVM);

	// Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(mode->width, mode->height, u8"Gimcana Automobil�stica", primary, NULL);
	if (!window)
	{
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 4.6 compatible. Try the 2.1 version of the tutorials.\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// Make the window's context current
	glfwMakeContextCurrent(window);

	// Llegir resoluci� actual de pantalla
	glfwGetWindowSize(window, &width_old, &height_old);

	// Initialize GLEW
	if (GLEW_VERSION_3_3) glewExperimental = GL_TRUE; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		glGetError();	// Esborrar codi error generat per bug a llibreria GLEW
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	const unsigned char* version = (unsigned char*)glGetString(GL_VERSION);

	int major, minor;
	GetGLVersion(&major, &minor);

	// ------------- Entorn VGI: Configure OpenGL context
	//	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, major);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minor); // GL 4.6

	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Si funcions deprecades s�n eliminades (no ARB_COMPATIBILITY)
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_FALSE);  // Si funcions deprecades NO s�n eliminades (Si ARB_COMPATIBILITY)

	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);	// Creaci� contexte CORE
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);	// Creaci� contexte ARB_COMPATIBILITY
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE); // comment this line in a release build! 


	// ------------ - Entorn VGI : Enable OpenGL debug context if context allows for DEBUG CONTEXT (GL4.6)
	if (GLEW_VERSION_4_6)
	{
		GLint flags; glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
		if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
		{
			glEnable(GL_DEBUG_OUTPUT);
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); // makes sure errors are displayed synchronously
			glDebugMessageCallback(glDebugOutput, nullptr);
			glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
		}
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);


	// Entorn VGI.ImGui: Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();


	// Initialize API
		//InitAPI();

	// Make the window's context current 

	ImGuiIO& io = ImGui::GetIO();

	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Habilita flechas del teclado (Arriba/Abajo + Enter)
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Habilita D-Pad/Stick del mando (Cruceta + Bot�n X/A)

	ImFontConfig cfg;
	cfg.OversampleH = 3;
	cfg.OversampleV = 3;
	cfg.PixelSnapH = false;

	static const ImWchar ranges[] =
	{
		0x0020, 0x017F,
		0,
	};


	ImFont* font = io.Fonts->AddFontFromFileTTF(
		"../include/RobotoSlab-VariableFont_wght.ttf",
		32.0f,
		&cfg,
		ranges
	);

	io.FontDefault = font;

	io.Fonts->Build();


	ImGui::StyleColorsLight();


	// ------------- Entorn VGI: Callbacks
	// Set GLFW event callbacks. I removed glfwSetWindowSizeCallback for conciseness
	glfwSetWindowSizeCallback(window, OnSize);										// - Windows Size in screen Coordinates
	glfwSetFramebufferSizeCallback(window, OnSize);									// - Windows Size in Pixel Coordinates
	glfwSetMouseButtonCallback(window, (GLFWmousebuttonfun)OnMouseButton);			// - Directly redirect GLFW mouse button events
	glfwSetCursorPosCallback(window, (GLFWcursorposfun)OnMouseMove);				// - Directly redirect GLFW mouse position events
	glfwSetScrollCallback(window, (GLFWscrollfun)OnMouseWheel);						// - Directly redirect GLFW mouse wheel events
	glfwSetKeyCallback(window, (GLFWkeyfun)OnKeyDown);								// - Directly redirect GLFW key events
	glfwSetErrorCallback(error_callback);											// Error callback
	glfwSetWindowRefreshCallback(window, (GLFWwindowrefreshfun)OnPaint);			// - Callback to refresh the screen

	// Entorn VGI.ImGui: Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 130");
	// Entorn VGI.ImGui: End Setup Dear ImGui context

	RenderLoadingScreenSimple();
	glfwSwapBuffers(window);
	glfwPollEvents();


	// Initialize Application control varibles
	InitGL();


	bool g_ShowMenu = true; // Control maestro

	g_GameContext.isGameRunning = false;
	g_MenuController = new MenuController(&g_GameContext, &camera);
	g_MenuController->SwitchState(new MainMenuState()); // Estado inicial

	OnVistaSkyBox();

	initFisicas();

	if (circuit != nullptr)
	{
		crearColisionadorEstatico(circuit);
		crearColisionadorEstatico(muro);
	}
	iniciarFisicasCoche();
	// Convertimos el circuito gr�fico en suelo f�sico
	if (circuit != nullptr) {
		crearColisionadorEstatico(circuit);
		crearColisionadorEstatico(muro);
	}
	else {
		printf("ALERTA: La variable circuit es nula. Revisa dond haces new OBJ.\n");
	}
	initEscenaDuplicados();

	// Entorn VGI; Timer: Lectura temps
	float previous = glfwGetTime();

	// Loop until the user closes the window
	while (!glfwWindowShouldClose(window))
	{

		// Entorn VGI. Timer: common part, do this only once
		now = glfwGetTime();
		delta = now - previous;
		previous = now;
		if (g_MenuController->GetContext()->isGameRunning) {
			stepFisicas(delta);
		}
		if (g_MenuController && g_MenuController->getState() == "Playing") {
			g_GameContext.gameTime += delta;

			// Actualizamos finalTime para que se guarde el �ltimo tiempo registrado
			g_GameContext.finalTime = g_GameContext.gameTime;
		}
		// Entorn VGI. Timer: for each timer do this
		time -= delta;
		if ((time <= 0.0) && (satelit || anima)) OnTimer();

		if (camera == CAM_FOLLOW && mobil)
		{

			float orbitSpeedPerSecond = 120.0f;

			if (g_isOrbitingLeft) {

				OPV.beta -= orbitSpeedPerSecond * delta;
			}
			if (g_isOrbitingRight) {
				OPV.beta += orbitSpeedPerSecond * delta;
			}
		}
		if (camera == CAM_LLIURE)
		{
			float velocity = 100.0f; // AUMETAR PARA QUE LA CAMARA VAYA MAS RAPIDO
			float moveSpeed = velocity * delta;


			glm::vec3 front;
			float yaw_rad = glm::radians(OPV.beta);
			float pitch_rad = glm::radians(OPV.alfa);
			front.x = -sin(yaw_rad) * cos(pitch_rad);
			front.y = cos(yaw_rad) * cos(pitch_rad);
			front.z = sin(pitch_rad);

			glm::vec3 cameraFront = glm::normalize(front);
			glm::vec3 worldUp = glm::vec3(0.0f, 0.0f, 1.0f);
			glm::vec3 cameraRight = glm::normalize(glm::cross(cameraFront, worldUp));

			// Mover la posici�n de la c�mara
			if (g_isMovingForward)  g_FreeCamPos += cameraFront * moveSpeed;
			if (g_isMovingBackward) g_FreeCamPos -= cameraFront * moveSpeed;
			if (g_isMovingLeft)     g_FreeCamPos -= cameraRight * moveSpeed;
			if (g_isMovingRight)    g_FreeCamPos += cameraRight * moveSpeed;
		}
		if (camera == CAM_PAUSA)
		{

			float orbitSpeedPerSecond = 20.0f;


			OPV.beta += orbitSpeedPerSecond * delta;

		}
		if (camera == CAM_INICI)
		{
			float orbitSpeedPerSecond = 10.0f;


			OPV.beta += orbitSpeedPerSecond * delta;
		}





		// Poll for and process events
		glfwPollEvents();
		OnJoystick(window);

		ImGuiIO& io = ImGui::GetIO();
		if (g_MenuController && g_MenuController->getState() == "Playing") {
			// ESTAMOS JUGANDO: Mentimos a ImGui diciendo que NO hay mando.
			// Esto evita que el bot�n Cuadrado active el men� de ventanas (NavWindowing)
			io.ConfigFlags &= ~ImGuiConfigFlags_NavEnableGamepad;
			io.BackendFlags &= ~ImGuiBackendFlags_HasGamepad;
		}
		else {
			// ESTAMOS EN MEN�: Volvemos a conectar el mando
			io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
			io.BackendFlags |= ImGuiBackendFlags_HasGamepad;
		}

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// 2. Comprobar si debemos cerrar app desde el men�
		if (g_GameContext.shouldCloseApp) {
			glfwSetWindowShouldClose(window, GL_TRUE);
		}

		if (g_MenuController->getCarHealth() == 0) {
			g_MenuController->calculateScore();
			g_MenuController->SwitchState(new EndGameState());
		}

		if (g_MenuController) {
			g_MenuController->Render();
		}



		if (GLFW_KEY_ESCAPE && GLFW_PRESS)

			// Crida a OnPaint() per redibuixar l'escena
			OnPaint(window);

		// Entorn VGI: Activa la finestra actual
		glfwMakeContextCurrent(window);
		glfwSwapInterval(1);

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
		glfwPollEvents();
		float velMPH = miCoche->getVelocidad();
		g_GameContext.velocity = velMPH / 3; //truco para que vaya rapido y el limite marque 35 (si no el coche va muy lento)
		static float damageAcumulado = 0.0f;

		if (velMPH > 105.0f) {

			float damagePerSecond = 0.5f;
			damageAcumulado += damagePerSecond * delta;
			if (damageAcumulado >= 1.0f) {
				g_GameContext.carHealth -= 1;
				damageAcumulado -= 1.0f;


				if (g_GameContext.carHealth < 0) g_GameContext.carHealth = 0;
			}
		}
	}


	// Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);


	// -------- PANTALLA DE CIERRE --------
	RenderExitScreenSimple();
	glfwSwapBuffers(window);
	glfwPollEvents();

	// Pequeña pausa para que se vea
	glfwWaitEventsTimeout(6);

	cleanFisicas();
	// Entorn VGI.ImGui: Cleanup ImGui
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);

	// Terminating GLFW: Destroy the windows, monitors and cursor objects
	glfwTerminate();

	if (shaderLighting.getProgramID() != -1) shaderLighting.DeleteProgram();
	if (shaderSkyBox.getProgramID() != -1) shaderSkyBox.DeleteProgram();
	return 0;
}