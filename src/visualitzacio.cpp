//******** PRACTICA VISUALITZACIÓ GRÀFICA INTERACTIVA (Escola Enginyeria - UAB)
//******** Entorn bàsic VS2022 MONOFINESTRA amb OpenGL 4.6, interfície GLFW 3.4, ImGui i llibreries GLM
//******** Ferran Poveda, Marc Vivet, Carme Julià, Débora Gil, Enric Martí (Setembre 2025)
// visualitzacio.cpp : Funcions de visualització i pantalla
// FUNCIONS:	- Iluminació (Iluminacio)
//				- Visualització Ortogràfica (ProjeccioOrto i Ortografica)
//				- Visualització Perspectiva amb coord. esfèriques (ProjeccioPerspectiva(),Vista_Esferica())
//              - Visualització Perspectiva amb navegació per tecles cursor (ProjeccioPerspectiva(),Vista_Navega())
//				- Dibuixar els eixos (deixos)
//				- Incorporar plans de Grid2D (XY, XZ, YZ) i un Grid 3D (grid*())
//				- Dibuixar fons: blanc (FonsW) i negre (Fons)
//				- Carregar imatges de formats compatibles amb DevIL/OpenIL com a textura (loadIMA())
//				- Inicialitzar imatges textura per l'objecte Truck (Init_Textures())
//

#include "stdafx.h"

// Entorn V3D. QUATERNIONS: Include per a la definició del tipus GL_QUAT i crida a les funcions de quatern.
#include "quatern.h"

#include "visualitzacio.h"
#include "escena.h"

// Iluminació: Configurar iluminació de l'escena

void Iluminacio(GLint sh_programID, char ilumin, bool ifix[], bool ilu2sides, bool ll_amb, LLUM* lumin, char obj, bool frnt_fcs,
	bool bc_lin, int step)
{

	glUseProgram(sh_programID);



	// Variables per a configurar paràmetres de les fonts de llum
	GLfloat especular[] = { 0.0,0.0,0.0,1.0 };
	GLfloat ambientg[] = { .5,.5,.5,1.0 };


// Definició de llum ambient segons booleana ll_amb
//	if (ll_amb) glLightModelfv(GL_LIGHT_MODEL_AMBIENT,ambientg);	// Codi OpenGL 1.1
//		else glLightModelfv(GL_LIGHT_MODEL_AMBIENT,especular);		// Codi OpenGL 1.1
	if (ll_amb) glUniform4f(glGetUniformLocation(sh_programID, "LightModelAmbient"), ambientg[0], ambientg[1], ambientg[2], ambientg[3]);
	else glUniform4f(glGetUniformLocation(sh_programID, "LightModelAmbient"), especular[0], especular[1], especular[2], especular[3]);
	//ifix[8] = 1;
	//ifix[9] = 1;
	//ifix[10] = 1;
	//ifix[11] = 1;
	//ifix[12] = 1;
	//ifix[13] = 1;
	//ifix[14] = 1;
	//ifix[15] = 1;
	//ifix[16] = 1;
	//ifix[17] = 1;
	for (int i = 0; i < NUM_MAX_LLUMS; i++) {
		// Si la teva llum global ifixe és bool, la convertim a int (1 o 0)
		printf("%d ", ifix[i] ? 1 : 0);
		
	}

	GLint fixedLightStatus[NUM_MAX_LLUMS];

	// 2. Omplir l'array amb els valors bool de ifixe (convertint bool a int: true=1, false=0)
	for (int i = 0; i < NUM_MAX_LLUMS; i++) {
		// Si la teva llum global ifixe és bool, la convertim a int (1 o 0)
		fixedLightStatus[i] = (GLint)ifix[i];
	}

	// 3. Enviar l'array al shader. Assegura't que el tipus del uniform GLSL és 'int' o 'bool'.
	GLint loc = glGetUniformLocation(sh_programID, "fixedLight");

	// imprimir location devuelta
	printf("Iluminacio: loc for 'fixedLight' = %d\n", loc);
	glUniform1iv(loc, NUM_MAX_LLUMS, fixedLightStatus);
// ---------------- LLUM #0 - (+Z)
// Posició de la font de llum:
	//	glLightfv(GL_LIGHT0, GL_POSITION, position);	// Codi OpenGL 1.1
	glUniform4f(glGetUniformLocation(sh_programID, "LightSource[0].position"), lumin[0].posicio.x, lumin[0].posicio.y, lumin[0].posicio.z, lumin[0].posicio.w);

// Intensitats difusa i especular de la font de llum:
	//	glLightfv(GL_LIGHT0, GL_DIFFUSE, lumin[0].difusa);	// Codi OpenGL 1.1
	glUniform4f(glGetUniformLocation(sh_programID, "LightSource[0].diffuse"), lumin[0].difusa.r, lumin[0].difusa.g, lumin[0].difusa.b, lumin[0].difusa.a);
	//	glLightfv(GL_LIGHT0, GL_SPECULAR, lumin[0].especular);	// Codi OpenGL 1.1
	glUniform4f(glGetUniformLocation(sh_programID, "LightSource[0].specular"), lumin[0].especular.r, lumin[0].especular.g, lumin[0].especular.b, lumin[0].especular.a);

// Coeficients factor atenuació f_att=1/(ad2+bd+c):
	//	glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, lumin[0].atenuacio.c);		// Codi OpenGL 1.1
	//	glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, lumin[0].atenuacio.b);		// Codi OpenGL 1.1
	//	glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, lumin[0].atenuacio.a);	// Codi OpenGL 1.1
	glUniform3f(glGetUniformLocation(sh_programID, "LightSource[0].attenuation"), lumin[0].atenuacio.a, lumin[0].atenuacio.b, lumin[0].atenuacio.c);

// Paràmetres font de llum restringida:
	//	if (lumin[0].restringida) {	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, lumin[0].spotdirection);	// Codi OpenGL 1.1
	//								glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, lumin[0].spotcutoff);			// Codi OpenGL 1.1
	//								glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, lumin[0].spotexponent);		// Codi OpenGL 1.1
	//							}
	glUniform1i(glGetUniformLocation(sh_programID, "LightSource[0].restricted"), lumin[0].restringida);
	glUniform3f(glGetUniformLocation(sh_programID, "LightSource[0].spotDirection"), lumin[0].spotdirection.x, lumin[0].spotdirection.y, lumin[0].spotdirection.z);
	glUniform1f(glGetUniformLocation(sh_programID, "LightSource[0].spotCosCutoff"), lumin[0].spotcoscutoff);
	glUniform1f(glGetUniformLocation(sh_programID, "LightSource[0].spotExponent"), lumin[0].spotexponent);

// Activació - Desactivació font de llum:
	//	glEnable(GL_LIGHT0);	//	else glDisable(GL_LIGHT0);	// Codi OpenGL 1.1
	glUniform1i(glGetUniformLocation(sh_programID, "LightSource[0].sw_light"), lumin[0].encesa);

// ---------------- LLUM #1 (+X)
// Posició de la font de llum:
	//glLightfv(GL_LIGHT1, GL_POSITION, position);	// Codi OpenGL 1.1
	glUniform4f(glGetUniformLocation(sh_programID, "LightSource[1].position"), lumin[1].posicio.x, lumin[1].posicio.y, lumin[1].posicio.z, lumin[1].posicio.w);

// Intensitats difusa i especular de la font de llum:
	//	glLightfv(GL_LIGHT1, GL_DIFFUSE, lumin[1].difusa);	// Codi OpenGL 1.1
	glUniform4f(glGetUniformLocation(sh_programID, "LightSource[1].diffuse"), lumin[0].difusa.r, lumin[1].difusa.g, lumin[1].difusa.b, lumin[1].difusa.a);
	//	glLightfv(GL_LIGHT1, GL_SPECULAR, lumin[1].especular);	// Codi OpenGL 1.1
	glUniform4f(glGetUniformLocation(sh_programID, "LightSource[1].specular"), lumin[1].especular.r, lumin[1].especular.g, lumin[1].especular.b, lumin[1].especular.a);

// Coeficients factor atenuació f_att=1/(ad2+bd+c):
	//	glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, lumin[1].atenuacio.c);		// Codi OpenGL 1.1
	//	glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, lumin[1].atenuacio.b);		// Codi OpenGL 1.1
	//	glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, lumin[1].atenuacio.a);	// Codi OpenGL 1.1
	glUniform3f(glGetUniformLocation(sh_programID, "LightSource[1].attenuation"), lumin[1].atenuacio.a, lumin[1].atenuacio.b, lumin[1].atenuacio.c);

// Paràmetres font de llum restringida:
	//	if (lumin[1].restringida) {	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, lumin[1].spotdirection);	// Codi OpenGL 1.1
	//								glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, lumin[1].spotcutoff);			// Codi OpenGL 1.1
	//								glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, lumin[1].spotexponent);		// Codi OpenGL 1.1
	//							}
	glUniform1i(glGetUniformLocation(sh_programID, "LightSource[1].restricted"), lumin[1].restringida);
	glUniform3f(glGetUniformLocation(sh_programID, "LightSource[1].spotDirection"), lumin[1].spotdirection.x, lumin[1].spotdirection.y, lumin[1].spotdirection.z);
	glUniform1f(glGetUniformLocation(sh_programID, "LightSource[1].spotCosCutoff"), lumin[1].spotcoscutoff);
	glUniform1f(glGetUniformLocation(sh_programID, "LightSource[1].SpotExponent"), lumin[1].spotexponent);

// Activació - Desactivació font de llum:
	//	glEnable(GL_LIGHT1);	//	glDisable(GL_LIGHT1);	// Codi OpenGL 1.1
	glUniform1i(glGetUniformLocation(sh_programID, "LightSource[1].sw_light"), lumin[1].encesa);

// ---------------- LLUM #2 (+Y)
// Posició de la font de llum:
	//	glLightfv(GL_LIGHT2, GL_POSITION, position);
	glUniform4f(glGetUniformLocation(sh_programID, "LightSource[2].position"), lumin[2].posicio.x, lumin[2].posicio.y, lumin[2].posicio.z, lumin[2].posicio.w);

// Intensitats difusa i especular de la font de llum:
	//	glLightfv(GL_LIGHT2, GL_DIFFUSE, lumin[2].difusa);	// Codi OpenGL 1.1
	glUniform4f(glGetUniformLocation(sh_programID, "LightSource[2].diffuse"), lumin[2].difusa.r, lumin[2].difusa.g, lumin[2].difusa.b, lumin[2].difusa.a);
	//	glLightfv(GL_LIGHT2, GL_SPECULAR, lumin[2].especular);	// Codi OpenGL 1.1
	glUniform4f(glGetUniformLocation(sh_programID, "LightSource[2].specular"), lumin[2].especular.r, lumin[2].especular.g, lumin[2].especular.b, lumin[2].especular.a);

// Coeficients factor atenuació f_att=1/(ad2+bd+c):
	//	glLightf(GL_LIGHT2, GL_CONSTANT_ATTENUATION, lumin[2].atenuacio.c);		// Codi OpenGL 1.1
	//	glLightf(GL_LIGHT2, GL_LINEAR_ATTENUATION, lumin[2].atenuacio.b);		// Codi OpenGL 1.1
	//	glLightf(GL_LIGHT2, GL_QUADRATIC_ATTENUATION, lumin[2].atenuacio.a);	// Codi OpenGL 1.1
	glUniform3f(glGetUniformLocation(sh_programID, "LightSource[2].attenuation"), lumin[2].atenuacio.a, lumin[2].atenuacio.b, lumin[2].atenuacio.c);

// Paràmetres font de llum restringida:
	//	if (lumin[2].restringida) {	glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, lumin[2].spotdirection);	// Codi OpenGL 1.1
	//								glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, lumin[2].spotcutoff);			// Codi OpenGL 1.1
	//								glLightf(GL_LIGHT2, GL_SPOT_EXPONENT, lumin[2].spotexponent);		// Codi OpenGL 1.1
	//							}
	glUniform1i(glGetUniformLocation(sh_programID, "LightSource[2].restricted"), lumin[2].restringida);
	glUniform3f(glGetUniformLocation(sh_programID, "LightSource[2].spotDirection"), lumin[2].spotdirection.x, lumin[2].spotdirection.y, lumin[2].spotdirection.z);
	glUniform1f(glGetUniformLocation(sh_programID, "LightSource[2].spotCosCutoff"), lumin[2].spotcoscutoff);
	glUniform1f(glGetUniformLocation(sh_programID, "LightSource[2].spotExponent"), lumin[2].spotexponent);

// Activació - Desactivació font de llum:
	//	glEnable(GL_LIGHT2);	//	glDisable(GL_LIGHT2);	// Codi OpenGL 1.1
	glUniform1i(glGetUniformLocation(sh_programID, "LightSource[2].sw_light"), lumin[2].encesa);

// ---------------- Llum #3 (Z=Y=X)
// Posició de la font de llum:
	//glLightfv(GL_LIGHT3, GL_POSITION, position);	// Codi OpenGL 1.1
	glUniform4f(glGetUniformLocation(sh_programID, "LightSource[3].position"), lumin[3].posicio.x, lumin[3].posicio.y, lumin[3].posicio.z, lumin[3].posicio.w);

// Intensitats difusa i especular de la font de llum:
	//	glLightfv(GL_LIGHT3, GL_DIFFUSE, lumin[3].difusa);		// Codi OpenGL 1.1
	glUniform4f(glGetUniformLocation(sh_programID, "LightSource[3].diffuse"), lumin[3].difusa.r, lumin[3].difusa.g, lumin[3].difusa.b, lumin[3].difusa.a);
	//	glLightfv(GL_LIGHT3, GL_SPECULAR, lumin[3].especular);	// Codi OpenGL 1.1
	glUniform4f(glGetUniformLocation(sh_programID, "LightSource[3].specular"), lumin[3].especular.r, lumin[3].especular.g, lumin[3].especular.b, lumin[3].especular.a);

// Coeficients factor atenuació f_att=1/(ad2+bd+c):
	//	glLightf(GL_LIGHT3, GL_CONSTANT_ATTENUATION, lumin[3].atenuacio.c);		// Codi OpenGL 1.1
	//	glLightf(GL_LIGHT3, GL_LINEAR_ATTENUATION, lumin[3].atenuacio.b);		// Codi OpenGL 1.1	
	//	glLightf(GL_LIGHT3, GL_QUADRATIC_ATTENUATION, lumin[3].atenuacio.a);	// Codi OpenGL 1.1
	glUniform3f(glGetUniformLocation(sh_programID, "LightSource[3].attenuation"), lumin[3].atenuacio.a, lumin[3].atenuacio.b, lumin[3].atenuacio.c);

// Paràmetres font de llum restringida:
	//	if (lumin[3].restringida) {	glLightfv(GL_LIGHT3, GL_SPOT_DIRECTION, lumin[3].spotdirection);	// Codi OpenGL 1.1
	//								glLightf(GL_LIGHT3, GL_SPOT_CUTOFF, lumin[3].spotcutoff);			// Codi OpenGL 1.1
	//								glLightf(GL_LIGHT3, GL_SPOT_EXPONENT, lumin[3].spotexponent);		// Codi OpenGL 1.1
	//							}
	glUniform1i(glGetUniformLocation(sh_programID, "LightSource[3].restricted"), lumin[3].restringida);
	glUniform3f(glGetUniformLocation(sh_programID, "LightSource[3].spotDirection"), lumin[3].spotdirection.x, lumin[3].spotdirection.y, lumin[3].spotdirection.z);
	glUniform1f(glGetUniformLocation(sh_programID, "LightSource[3].spotCosCutoff"), lumin[3].spotcoscutoff);
	glUniform1f(glGetUniformLocation(sh_programID, "LightSource[3].spotExponent"), lumin[3].spotexponent);

// Activació - Desactivació font de llum:
	//	glEnable(GL_LIGHT3);	// glDisable(GL_LIGHT3);	// Codi OpenGL 1.1
	glUniform1i(glGetUniformLocation(sh_programID, "LightSource[3].sw_light"), lumin[3].encesa);

// ---------------- LLUM #4 (-Z)
// Posició de la font de llum:
	//glLightfv(GL_LIGHT4, GL_POSITION, position);	// Codi OpenGL 1.1
	glUniform4f(glGetUniformLocation(sh_programID, "LightSource[4].position"), lumin[4].posicio.x, lumin[4].posicio.y, lumin[4].posicio.z, lumin[4].posicio.w);

// Intensitats difusa i especular de la font de llum:
	//	glLightfv(GL_LIGHT4, GL_DIFFUSE, lumin[4].difusa);	// Codi OpenGL 1.1
	glUniform4f(glGetUniformLocation(sh_programID, "LightSource[4].diffuse"), lumin[4].difusa.r, lumin[4].difusa.g, lumin[4].difusa.b, lumin[4].difusa.a);
	//	glLightfv(GL_LIGHT4, GL_SPECULAR, lumin[4].especular);	// Codi OpenGL 1.1
	glUniform4f(glGetUniformLocation(sh_programID, "LightSource[4].specular"), lumin[4].especular.r, lumin[4].especular.g, lumin[4].especular.b, lumin[4].especular.a);

// Coeficients factor atenuació f_att=1/(ad2+bd+c):
	//	glLightf(GL_LIGHT4, GL_CONSTANT_ATTENUATION, lumin[4].atenuacio.c);		// Codi OpenGL 1.1
	//	glLightf(GL_LIGHT4, GL_LINEAR_ATTENUATION, lumin[4].atenuacio.b);		// Codi OpenGL 1.1
	//	glLightf(GL_LIGHT4, GL_QUADRATIC_ATTENUATION, lumin[4].atenuacio.a);	// Codi OpenGL 1.1
	glUniform3f(glGetUniformLocation(sh_programID, "LightSource[4].attenuation"), lumin[4].atenuacio.a, lumin[4].atenuacio.b, lumin[4].atenuacio.c);

// Paràmetres font de llum restringida:
	//	if (lumin[4].restringida) {	glLightfv(GL_LIGHT4, GL_SPOT_DIRECTION, lumin[4].spotdirection);	// Codi OpenGL 1.1
	//								glLightf(GL_LIGHT4, GL_SPOT_CUTOFF, lumin[4].spotcutoff);			// Codi OpenGL 1.1
	//								glLightf(GL_LIGHT4, GL_SPOT_EXPONENT, lumin[4].spotexponent);		// Codi OpenGL 1.1
	//							}
	glUniform1i(glGetUniformLocation(sh_programID, "LightSource[4].restricted"), lumin[4].restringida);
	glUniform3f(glGetUniformLocation(sh_programID, "LightSource[4].spotDirection"), lumin[4].spotdirection.x, lumin[4].spotdirection.y, lumin[4].spotdirection.z);
	glUniform1f(glGetUniformLocation(sh_programID, "LightSource[4].spotCosCutoff"), lumin[4].spotcoscutoff);
	glUniform1f(glGetUniformLocation(sh_programID, "LightSource[4].spotExponent"), lumin[4].spotexponent);

// Activació - Desactivació font de llum:
	//	glEnable(GL_LIGHT4);	//	else glDisable(GL_LIGHT4);	// Codi OpenGL 1.1
	glUniform1i(glGetUniformLocation(sh_programID, "LightSource[4].sw_light"), lumin[4].encesa);

// ---------------- LLUM #5
// Posició de la font de llum :
	//glLightfv(GL_LIGHT5, GL_POSITION, position);	// Codi OpenGL 1.1
	glUniform4f(glGetUniformLocation(sh_programID, "LightSource[5].position"), lumin[5].posicio.x, lumin[5].posicio.y, lumin[5].posicio.z, lumin[5].posicio.w);

// Intensitats difusa i especular de la font de llum:
	//	glLightfv(GL_LIGHT5, GL_DIFFUSE, lumin[5].difusa);	// Codi OpenGL 1.1
	glUniform4f(glGetUniformLocation(sh_programID, "LightSource[5].diffuse"), lumin[5].difusa.r, lumin[5].difusa.g, lumin[5].difusa.b, lumin[5].difusa.a);
	//	glLightfv(GL_LIGHT5, GL_SPECULAR, lumin[5].especular);	// Codi OpenGL 1.1
	glUniform4f(glGetUniformLocation(sh_programID, "LightSource[5].specular"), lumin[5].especular.r, lumin[5].especular.g, lumin[5].especular.b, lumin[5].especular.a);

// Coeficients factor atenuació f_att=1/(ad2+bd+c)
	//	glLightf(GL_LIGHT5, GL_CONSTANT_ATTENUATION, lumin[5].atenuacio.c);		// Codi OpenGL 1.1
	//	glLightf(GL_LIGHT5, GL_LINEAR_ATTENUATION, lumin[5].atenuacio.b);		// Codi OpenGL 1.1
	//	glLightf(GL_LIGHT5, GL_QUADRATIC_ATTENUATION, lumin[5].atenuacio.a);	// Codi OpenGL 1.1
	glUniform3f(glGetUniformLocation(sh_programID, "LightSource[5].attenuation"), lumin[5].atenuacio.a, lumin[5].atenuacio.b, lumin[5].atenuacio.c);

// Paràmetres font de llum restringida:
	//	if (lumin[5].restringida) {	glLightfv(GL_LIGHT5, GL_SPOT_DIRECTION, lumin[5].spotdirection);	// Codi OpenGL 1.1
	//								glLightf(GL_LIGHT5, GL_SPOT_CUTOFF, lumin[5].spotcoscutoff);		// Codi OpenGL 1.1
	//								glLightf(GL_LIGHT5, GL_SPOT_EXPONENT, lumin[5].spotexponent);		// Codi OpenGL 1.1
	//							}
	glUniform1i(glGetUniformLocation(sh_programID, "LightSource[5].restricted"), lumin[5].restringida);
	glUniform3f(glGetUniformLocation(sh_programID, "LightSource[5].spotDirection"), lumin[5].spotdirection.x, lumin[5].spotdirection.y, lumin[5].spotdirection.z);
	glUniform1f(glGetUniformLocation(sh_programID, "LightSource[5].spotCosCutoff"), lumin[5].spotcoscutoff);
	glUniform1f(glGetUniformLocation(sh_programID, "LightSource[5].spotExponent"), lumin[5].spotexponent);

// Activació - Desactivació font de llum:
	//	glEnable(GL_LIGHT5);	//	else glDisable(GL_LIGHT5);	// Codi OpenGL 1.1
	glUniform1i(glGetUniformLocation(sh_programID, "LightSource[5].sw_light"), lumin[5].encesa);

// ---------------- LLUM #6 
// Posició de la font de llum:
	//	glLightfv(GL_LIGHT6, GL_POSITION, position);	// Codi OpenGL 1.1
	glUniform4f(glGetUniformLocation(sh_programID, "LightSource[6].position"), lumin[6].posicio.x, lumin[6].posicio.y, lumin[6].posicio.z, lumin[6].posicio.w);

// Intensitats difusa i especular de la font de llum:
	//	glLightfv(GL_LIGHT6, GL_DIFFUSE, lumin[6].difusa);	// Codi OpenGL 1.1
	glUniform4f(glGetUniformLocation(sh_programID, "LightSource[6].diffuse"), lumin[6].difusa.r, lumin[6].difusa.g, lumin[6].difusa.b, lumin[6].difusa.a);
	//	glLightfv(GL_LIGHT6, GL_SPECULAR, lumin[6].especular);	// Codi OpenGL 1.1
	glUniform4f(glGetUniformLocation(sh_programID, "LightSource[6].specular"), lumin[6].especular.r, lumin[6].especular.g, lumin[6].especular.b, lumin[6].especular.a);

// Coeficients factor atenuació f_att=1/(ad2+bd+c):
	//	glLightf(GL_LIGHT6, GL_CONSTANT_ATTENUATION, lumin[6].atenuacio.c);		// Codi OpenGL 1.1
	//	glLightf(GL_LIGHT6, GL_LINEAR_ATTENUATION, lumin[6].atenuacio.b);		// Codi OpenGL 1.1
	//	glLightf(GL_LIGHT6, GL_QUADRATIC_ATTENUATION, lumin[6].atenuacio.a);	// Codi OpenGL 1.1
	glUniform3f(glGetUniformLocation(sh_programID, "LightSource[6].attenuation"), lumin[6].atenuacio.a, lumin[6].atenuacio.b, lumin[6].atenuacio.c);

// Paràmetres font de llum restringida:
	//	if (lumin[6].restringida) {	glLightfv(GL_LIGHT6, GL_SPOT_DIRECTION, lumin[6].spotdirection);	// Codi OpenGL 1.1
	//								glLightf(GL_LIGHT6, GL_SPOT_CUTOFF, lumin[6].cutoff);				// Codi OpenGL 1.1
	//								glLightf(GL_LIGHT6, GL_SPOT_EXPONENT, lumin[6].exponent);			// Codi OpenGL 1.1
	//							}
	glUniform1i(glGetUniformLocation(sh_programID, "LightSource[6].restricted"), lumin[6].restringida);
	glUniform3f(glGetUniformLocation(sh_programID, "LightSource[6].spotDirection"), lumin[6].spotdirection.x, lumin[6].spotdirection.y, lumin[6].spotdirection.z);
	glUniform1f(glGetUniformLocation(sh_programID, "LightSource[6].spotCosCutoff"), lumin[6].spotcoscutoff);
	glUniform1f(glGetUniformLocation(sh_programID, "LightSource[6].spotExponent"), lumin[6].spotexponent);

// Activació - Desactivació font de llum:
	// glEnable(GL_LIGHT6)	//	glDisable(GL_LIGHT6);
	glUniform1i(glGetUniformLocation(sh_programID, "LightSource[6].sw_light"), lumin[6].encesa);

// ---------------- LLUM #7 
// Posició de la font de llum:
	//glLightfv(GL_LIGHT7, GL_POSITION, position);	// Codi OpenGL 1.1
	glUniform4f(glGetUniformLocation(sh_programID, "LightSource[7].position"), lumin[7].posicio.x, lumin[7].posicio.y, lumin[7].posicio.z, lumin[7].posicio.w);

// Intensitats difusa i especular de la font de llum:
	//	glLightfv(GL_LIGHT7, GL_DIFFUSE, lumin[7].difusa);	// Codi OpenGL 1.1
	glUniform4f(glGetUniformLocation(sh_programID, "LightSource[7].diffuse"), lumin[7].difusa.r, lumin[7].difusa.g, lumin[7].difusa.b, lumin[7].difusa.a);
	//	glLightfv(GL_LIGHT7, GL_SPECULAR, lumin[7].especular);	// Codi OpenGL 1.1
	glUniform4f(glGetUniformLocation(sh_programID, "LightSource[7].specular"), lumin[7].especular.r, lumin[7].especular.g, lumin[7].especular.b, lumin[7].especular.a);

// Coeficients factor atenuació f_att=1/(ad2+bd+c):
	//	glLightf(GL_LIGHT7, GL_CONSTANT_ATTENUATION, lumin[7].atenuacio.c);		// Codi OpenGL 1.1
	//	glLightf(GL_LIGHT7, GL_LINEAR_ATTENUATION, lumin[7].atenuacio.b);		// Codi OpenGL 1.1
	//	glLightf(GL_LIGHT7, GL_QUADRATIC_ATTENUATION, lumin[7].atenuacio.a);	// Codi OpenGL 1.1
	glUniform3f(glGetUniformLocation(sh_programID, "LightSource[7].attenuation"), lumin[7].atenuacio.a, lumin[7].atenuacio.b, lumin[7].atenuacio.c);

// Paràmetres font de llum restringida:
	//	if (lumin[7].restringida) {	glLightfv(GL_LIGHT7, GL_SPOT_DIRECTION, lumin[7].spotdirection);	// Codi OpenGL 1.1
	//								glLightf(GL_LIGHT7, GL_SPOT_CUTOFF, lumin[7].cutoff);				// Codi OpenGL 1.1
	//								glLightf(GL_LIGHt7, GL_SPOT_EXPONENT, lumin[7].exponent);			// Codi OpenGL 1.1
	//							}
	glUniform1i(glGetUniformLocation(sh_programID, "LightSource[7].restricted"), lumin[7].restringida);
	glUniform3f(glGetUniformLocation(sh_programID, "LightSource[7].spotDirection"), lumin[7].spotdirection.x, lumin[7].spotdirection.y, lumin[7].spotdirection.z);
	glUniform1f(glGetUniformLocation(sh_programID, "LightSource[7].spotCosCutoff"), lumin[7].spotcoscutoff);
	glUniform1f(glGetUniformLocation(sh_programID, "LightSource[7].spotExponent"), lumin[7].spotexponent);

// Activació - Desactivació font de llum:
	//	glEnable(GL_LIGHT7);	//	glDisable(GL_LIGHT7);	// Codi OpenGL 1.1
	glUniform1i(glGetUniformLocation(sh_programID, "LightSource[7].sw_light"), lumin[7].encesa);

	// =================================================================
	// BUCLE PER LLUMS COTXE
	// =================================================================

	for (int i = 8; i < NUM_MAX_LLUMS; i++)
	{
		std::string base = "LightSource[" + std::to_string(i) + "]"; // Crea "LightSource[0]", "LightSource[1]", etc.

		// 1. Posición
		glUniform4f(glGetUniformLocation(sh_programID, (base + ".position").c_str()),
			lumin[i].posicio.x, lumin[i].posicio.y, lumin[i].posicio.z, lumin[i].posicio.w);

		// 2. Colores (Difusa / Especular)
		glUniform4f(glGetUniformLocation(sh_programID, (base + ".diffuse").c_str()),
			lumin[i].difusa.r, lumin[i].difusa.g, lumin[i].difusa.b, lumin[i].difusa.a);

		glUniform4f(glGetUniformLocation(sh_programID, (base + ".specular").c_str()),
			lumin[i].especular.r, lumin[i].especular.g, lumin[i].especular.b, lumin[i].especular.a);

		// 3. Atenuación
		glUniform3f(glGetUniformLocation(sh_programID, (base + ".attenuation").c_str()),
			lumin[i].atenuacio.a, lumin[i].atenuacio.b, lumin[i].atenuacio.c);

		// 4. Focos Restringidos (Spotlights)
		glUniform1i(glGetUniformLocation(sh_programID, (base + ".restricted").c_str()), lumin[i].restringida);

		glUniform3f(glGetUniformLocation(sh_programID, (base + ".spotDirection").c_str()),
			lumin[i].spotdirection.x, lumin[i].spotdirection.y, lumin[i].spotdirection.z);

		glUniform1f(glGetUniformLocation(sh_programID, (base + ".spotCosCutoff").c_str()), lumin[i].spotcoscutoff);

		glUniform1f(glGetUniformLocation(sh_programID, (base + ".spotExponent").c_str()), lumin[i].spotexponent);

		// 5. Interruptor (Encendida/Apagada)
		glUniform1i(glGetUniformLocation(sh_programID, (base + ".sw_light").c_str()), lumin[i].encesa);
	}

	// =================================================================
	// FIN DEL BUCLE
	// =================================================================

	// Selecció del model d'iluminació segons variable ilumin
	switch(ilumin)
	{
	case PUNTS:
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
		break;

	case FILFERROS:
		glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
		break;

    case PLANA:
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		// PRÀCTICA FRACTAL: Crida a la funció pel càlcul dels vectors normals normals a les cares si l'objecte és un fractal
   
		// Il.luminació per cares planes
		glShadeModel(GL_FLAT); 
		break;

	case SUAU:
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		// PRÀCTICA FRACTAL: Crida a la funció pel càlcul dels vectors normals normals a les cares si l'objecte és un fractal

		// Il.luminació suau 
		glShadeModel(GL_SMOOTH);
		break;
	}
}


// -------- Entorn VGI: ORTOGRÀFICA (Funcions Projeccio_Orto i Vista_Ortografica)




// Projeccio_Orto: Definició Viewport i glOrtho 
// ---- Entorn VGI: ATENCIÓ!!. CAL DEFINIR PARÀMETRES DE LA FUNCIÓ
glm::mat4 Projeccio_Orto()
{   
	glm::dmat4 MatriuProjeccio(1.0);
// ---- Entorn VGI: ATENCIÓ!!. ESPECIFICACIO FUNCIONS PER A DEFINIR VIEWPORT i PROJECCIO ORTOGRÀFICA
//			        AMB ELS SEUS PARÀMETRES. CONTINGUT SEMBLANT A FUNCIÓ Projeccio_Perspectiva().
	return MatriuProjeccio;
}

//// Vista_Ortogràfica: Ilumina i dibuixa l'escena. Crida a la funció gluLookAt segons la variable prj 
////				(planta, alçat, perfil o axonometrica).
//glm::mat4 Vista_Ortografica(GLuint sh_programID, int prj,GLdouble Raux,CColor col_fons,CColor col_object,char objecte,GLdouble mida,int step,
//				bool frnt_fcs, bool oculta, bool testv, bool bck_ln, 
//				char iluminacio, bool llum_amb, LLUM* lumi, bool ifix, bool il2sides, 
//				bool eix, CMask3D reixa, CPunt3D hreixa)
//{
//	glm::mat4 MatriuVista(1.0);
//
//// Iluminacio movent-se amb la camara (abans gluLookAt)
//	if (!ifix) Iluminacio(sh_programID, iluminacio, ifix, il2sides, llum_amb, lumi, objecte, frnt_fcs, bck_ln, step);
//
//// Implementació de planta,alçat,perfil i isomètrica 
//// ---- Entorn VGI: ATENCIÓ!!. ESPECIFICACIO DE LA CÀMERA
////								Cal definir la càmera (glm::lookAt() ) en funció del
////								tipus de projecció definit a la variable prj.
//
//// Pas Matriu a shader
//	glUniformMatrix4fv(glGetUniformLocation(sh_programID, "viewMatrix"), 1, GL_FALSE, &MatriuVista[0][0]);
//
//// Neteja dels buffers de color i profunditat
//	Fons(col_fons);
//
//// Iluminacio fixe respecte la camara (després gluLookAt)
//	if (ifix) Iluminacio(sh_programID, iluminacio, ifix, il2sides, llum_amb, lumi, objecte, frnt_fcs, bck_ln, step);
//
//// Test de Visibilitat
//	if (testv) glEnable(GL_CULL_FACE);
//		else glDisable(GL_CULL_FACE);
//
//// Ocultacions (Z-buffer)
//	if (oculta) glEnable(GL_DEPTH_TEST);
//		else glDisable(GL_DEPTH_TEST);
//
//// Dibuix de les cares back com a línies en Il.luminacio PLANA i SUAU
//	if (bck_ln) glPolygonMode(GL_BACK, GL_LINE);
//	
//	return MatriuVista;
//}

// -------- Entorn VGI: PERSPECTIVA (Funcions Projeccio_Perspectiva i Vista_Esferica)

// Projeccio_Perspectiva: Definició Viewport i gluPerspective
glm::mat4 Projeccio_Perspectiva(GLuint sh_programID, int minx,int miny,GLsizei w,GLsizei h,double zoom)
{
	glm::mat4 MatriuProjeccio(1.0);

// Definició Viewport
	glViewport(minx,miny,w,h);
	if (h==0) h=1;

// PROJECCIO PERSPECTIVA.Definim volum de visualització adaptant-lo 
//							a les mides actuals de la finestra windows. Amb glm::perspective()
	if (w >= h) MatriuProjeccio = glm::perspective(glm::radians(60.0), 1.0 * w / h, p_near, p_far);
		else MatriuProjeccio = glm::perspective(glm::radians(60.0), 1.0 * w / h, p_near, p_far);

// Pas Matriu a shader
	glUniformMatrix4fv(glGetUniformLocation(sh_programID, "projectionMatrix"), 1, GL_FALSE, &MatriuProjeccio[0][0]);

	return MatriuProjeccio;
}

glm::mat4 Projeccio_Perspectiva(GLsizei w, GLsizei h, double fov_grados)
{
	if (h == 0) h = 1;

	double aspect_ratio = (double)w / (double)h;

	glm::mat4 MatriuProjeccio = glm::perspective(glm::radians(fov_grados), aspect_ratio, p_near, p_far);

	return MatriuProjeccio;
}

// Vista_Esferica: Definició gluLookAt amb possibilitat de moure el punt de vista interactivament amb el ratolí, 
//					ilumina i dibuixa l'escena
//glm::mat4 Vista_Esferica(GLuint sh_programID,CEsfe3D opv,char VPol,bool pant,CPunt3D tr,CPunt3D trF,
//				 CColor col_fons,CColor col_object,char objecte,double mida,int step, 
//				 bool frnt_fcs, bool oculta, bool testv, bool bck_ln, 
//				 char iluminacio, bool llum_amb, LLUM* lumi, bool ifix, bool il2sides,
//				 bool eix, CMask3D reixa, CPunt3D hreixa)
//{
//	GLdouble cam[3] = { 0.0,0.0,0.0 }, up[3] = { 0.0,0.0,0.0 };
//	glm::mat4 MatriuVista(1.0);
//	glm::mat4 TransMatrix(1.0);	// Matrius Traslació
//
//// Rectificar paràmetre R si negatiu.
//	if (opv.R < 1.0) opv.R = 1.0;
//
//// Conversió angles radians -> graus
//	opv.alfa=opv.alfa*PI/180;
//	opv.beta=opv.beta*PI/180;
//
//// Neteja dels buffers de color i profunditat
//	Fons(col_fons);
//
//// Posició càmera i vector cap amunt
//	if (VPol==POLARZ) { cam[0]=opv.R*cos(opv.beta)*cos(opv.alfa);
//						cam[1]=opv.R*sin(opv.beta)*cos(opv.alfa);
//						cam[2]=opv.R*sin(opv.alfa);		
//						up[0]=-cos(opv.beta)*sin(opv.alfa);
//						up[1]=-sin(opv.beta)*sin(opv.alfa);
//						up[2]=cos(opv.alfa);	}
//		else if (VPol==POLARY) {	cam[0]=opv.R*sin(opv.beta)*cos(opv.alfa);
//									cam[1]=opv.R*sin(opv.alfa);
//									cam[2]=opv.R*cos(opv.beta)*cos(opv.alfa);		
//									up[0]=-sin(opv.beta)*sin(opv.alfa);
//									up[1]=cos(opv.alfa);
//									up[2]=-cos(opv.beta)*sin(opv.alfa);		}
//			else {	cam[0]=opv.R*sin(opv.alfa);
//					cam[1]=opv.R*cos(opv.beta)*cos(opv.alfa);
//					cam[2]=opv.R*sin(opv.beta)*cos(opv.alfa);		
//					up[0]=cos(opv.alfa);
//					up[1]=-cos(opv.beta)*sin(opv.alfa);
//					up[2]=-sin(opv.beta)*sin(opv.alfa);		}
//
//// Iluminacio movent-se amb la camara (abans glLookAt)
//	if (!ifix) Iluminacio(sh_programID,iluminacio, ifix, il2sides, llum_amb, lumi, objecte, frnt_fcs, bck_ln, step);
//
//// Opció pan: desplaçament del Centre de l'esfera (pant=1)
//	if (pant) TransMatrix = glm::translate(TransMatrix, vec3(tr.x, tr.y, tr.z));	//glTranslatef(tr.x, tr.y, tr.z);
//	TransMatrix = glm::translate(TransMatrix, vec3(trF.x, trF.y, trF.z));	// Traslació fixada amb la INSERT dins l'opció pan
//
//// Especificació del punt de vista
//   //gluLookAt(cam[0],cam[1],cam[2],0.,0.,0.,up[0],up[1],up[2]);
//   MatriuVista = glm::lookAt(
//	   glm::vec3(cam[0], cam[1], cam[2]),	// Camera is here
//	   glm::vec3(0, 0, 0),					// and looks here
//	   glm::vec3(up[0], up[1], up[2])		// Head is up (set to 0,-1,0 to look upside-down)
//	   );
//
//// Concatenar matrius Traslació amb la de càmera per al pan
//	MatriuVista = TransMatrix * MatriuVista;
//
//// Pas Matriu Vista a shader
//   glUniformMatrix4fv(glGetUniformLocation(sh_programID, "viewMatrix"), 1, GL_FALSE, &MatriuVista[0][0]);
//
//// Iluminacio fixe respecte la camara (després glLookAt)
//   if (ifix) Iluminacio(sh_programID,iluminacio, ifix, il2sides, llum_amb, lumi, objecte, frnt_fcs, bck_ln, step);
//
//// Test de Visibilitat
//	if (testv) glEnable(GL_CULL_FACE);
//		else glDisable(GL_CULL_FACE);
//
//// Ocultacions (Z-buffer)
//	if (oculta) glEnable(GL_DEPTH_TEST);
//		else glDisable(GL_DEPTH_TEST);
//
//	return MatriuVista;
//}
//
//
////Vista_Navega: Definició gluLookAt directament per paràmetre, sense esfèriques.
////              amb possibilitat de moure el punt de vista interactivament amb les
////				tecles de cursor per poder navegar.
//glm::mat4 Vista_Navega(GLuint sh_programID, CPunt3D pv, GLdouble n[3],GLdouble v[3],bool pant,CPunt3D tr,CPunt3D trF,
//				  CColor col_fons,CColor col_object,char objecte,bool color, int step,
//				  bool frnt_fcs, bool oculta, bool testv, bool bck_ln, 
//				  char iluminacio, bool llum_amb, LLUM* lumi, bool ifix, bool il2sides,
//				  bool eix, CMask3D reixa, CPunt3D hreixa)
//{   double altfar=0;
//	glm::mat4 MatriuVista(1.0), TransMatrix(1.0);
//
//// Neteja dels buffers de color i profunditat
//	Fons(col_fons);
//
//// Iluminacio movent-se amb la camara (abans glLookAt)
//	if (!ifix) Iluminacio(sh_programID,iluminacio, ifix, il2sides, llum_amb, lumi, objecte, frnt_fcs, bck_ln, step);
//
//// Opció pan: desplaçament del Centre de l'esfera (pant=true)
//	if (pant) TransMatrix = glm::translate(glm::mat4(), vec3(tr.x, tr.y, tr.z)); // glTranslatef(tr.x, tr.y, tr.z);
//	TransMatrix = glm::translate(TransMatrix, vec3(trF.x, trF.y, trF.z)); //glTranslatef(trF.x,trF.y,trF.z);	// Traslació fixada amb la INSERT dins l'opció pan
//
//// Especificació del punt de vista (CÀMERA)
//	//gluLookAt(pv.x,pv.y,pv.z,n[0],n[1],n[2],v[0],v[1],v[2]);
//	MatriuVista = glm::lookAt(
//		glm::vec3(pv.x, pv.y, pv.z), // Camera is here
//		glm::vec3(n[0], n[1], n[2]), // and looks here
//		glm::vec3(v[0], v[1], v[2])  // Head is up (set to 0,-1,0 to look upside-down)
//		);
//
//// Concatenar matrius Traslació amb la de càmera per al pan
//	MatriuVista = TransMatrix * MatriuVista;
//
//// Pas Matriu a shader
//	glUniformMatrix4fv(glGetUniformLocation(sh_programID, "viewMatrix"), 1, GL_FALSE, &MatriuVista[0][0]);
//
//// Iluminacio fixe respecte la camara (després glLookAt)
//	if (ifix) Iluminacio(sh_programID,iluminacio, ifix, il2sides, llum_amb, lumi, objecte, frnt_fcs, bck_ln, step);
//
//// Test de Visibilitat
//	if (testv) glEnable(GL_CULL_FACE);
//		else glDisable(GL_CULL_FACE);
//
//// Ocultacions (Z-buffer)
//	if (oculta) glEnable(GL_DEPTH_TEST);
//		else glDisable(GL_DEPTH_TEST);
//
//	return MatriuVista;
//}
//
//
////Vista_Geode: Definició gluLookAt a partir de coordenades esfèriques, però situant la càmera a l'origen (0,0,0)
////              i colocant el punt on mira cap el punt (R,alfa,beta) amb la possibilitat de moure aquest darrer punt
////				interactivament amb les	tecles de cursor per a veure una cúpula.
//glm::mat4 Vista_Geode(GLuint sh_programID, CEsfe3D opv, char VPol, bool pant, CPunt3D tr, CPunt3D trF,
//	CColor col_fons, CColor col_object, char objecte, double mida, int step,
//	bool frnt_fcs, bool oculta, bool testv, bool bck_ln,
//	char iluminacio, bool llum_amb, LLUM* lumi, bool ifix, bool il2sides,
//	bool eix, CMask3D reixa, CPunt3D hreixa)
//{
//	GLdouble cam[3] = {0.0, 0.0, 0.0}, camN[3] = { 0.0, 0.0, 0.0 }, up[3]{ 0.0, 0.0, 0.0 };
//	glm::mat4 MatriuVista(1.0);
//	glm::mat4 TransMatrix(1.0);	// Matrius Traslació
//
//// Rectificar paràmetre R si negatiu.
//	if (opv.R < 1.0) opv.R = 1.0;
//
//// Conversió angles radians -> graus
//	opv.alfa = opv.alfa * PI / 180;
//	opv.beta = opv.beta * PI / 180;
//
//// Neteja dels buffers de color i profunditat
//	Fons(col_fons);
//
//	// Posició càmera i vector cap amunt en GEODE
//	if (VPol == POLARZ) {
//		cam[0] = opv.R * cos(opv.beta) * cos(opv.alfa);
//		cam[1] = opv.R * sin(opv.beta) * cos(opv.alfa);
//		cam[2] = opv.R * sin(opv.alfa);
//		// Vector camp on mira (cap a (R,alfa,beta)
//		camN[0] = cos(opv.beta) * cos(opv.alfa);
//		camN[1] = sin(opv.beta) * cos(opv.alfa);
//		camN[2] = sin(opv.alfa);
//		// Vector tangent (diferencial respecte alfa)
//		up[0] = -cos(opv.beta) * sin(opv.alfa);
//		up[1] = -sin(opv.beta) * sin(opv.alfa);
//		up[2] = cos(opv.alfa);
//		}
//	else if (VPol == POLARY) {
//		cam[0] = opv.R * sin(opv.beta) * cos(opv.alfa);
//		cam[1] = opv.R * sin(opv.alfa);
//		cam[2] = opv.R * cos(opv.beta) * cos(opv.alfa);
//		// Vector camp on mira (cap a (R,alfa,beta)
//		camN[0] = sin(opv.beta) * cos(opv.alfa);
//		camN[1] = sin(opv.alfa);
//		camN[2] = cos(opv.beta) * cos(opv.alfa);
//		// Vector tangent (diferencial respecte alfa)
//		up[0] = -sin(opv.beta) * sin(opv.alfa);
//		up[1] = cos(opv.alfa);
//		up[2] = -cos(opv.beta) * sin(opv.alfa);
//		}
//	else {
//		cam[0] = opv.R * sin(opv.alfa);
//		cam[1] = opv.R * cos(opv.beta) * cos(opv.alfa);
//		cam[2] = opv.R * sin(opv.beta) * cos(opv.alfa);
//		// Vector camp on mira (cap a (R,alfa,beta)
//		camN[0] = sin(opv.alfa);
//		camN[1] = cos(opv.beta) * cos(opv.alfa);
//		camN[2] = sin(opv.beta) * cos(opv.alfa);
//		// Vector tangent (diferencial respecte alfa)
//		up[0] = cos(opv.alfa);
//		up[1] = -cos(opv.beta) * sin(opv.alfa);
//		up[2] = -sin(opv.beta) * sin(opv.alfa);
//		}
//
//// Iluminacio movent-se amb la camara (abans gluLookAt)
//	if (!ifix) Iluminacio(sh_programID, iluminacio, ifix, il2sides, llum_amb, lumi, objecte, frnt_fcs, bck_ln, step);
//
//// Opció pan: desplaçament del Centre de l'esfera (pant=1)
//	if (pant) TransMatrix = glm::translate(TransMatrix, vec3(tr.x, tr.y, tr.z));	//glTranslatef(tr.x, tr.y, tr.z);
//	TransMatrix = glm::translate(TransMatrix, vec3(trF.x, trF.y, trF.z));	// Traslació fixada amb la INSERT dins l'opció pan
//
//// Especificació de la càmera
//	MatriuVista = glm::lookAt(
//		glm::vec3(cam[0], cam[1], cam[2]), // Camera is here
//		glm::vec3(cam[0] + camN[0], cam[1] + camN[1], cam[2] + camN[2]), // and looks here
//		glm::vec3(up[0], up[1], up[2])  // Head is up (set to 0,-1,0 to look upside-down)
//	);
//
//// Concatenar matrius Traslació amb la de càmera per al pan
//	MatriuVista = TransMatrix * MatriuVista;
//
//// Pas Matriu Vista a shader
//	glUniformMatrix4fv(glGetUniformLocation(sh_programID, "viewMatrix"), 1, GL_FALSE, &MatriuVista[0][0]);
//
//// Iluminacio fixe respecte la camara (després gluLookAt)
//	if (ifix) Iluminacio(sh_programID, iluminacio, ifix, il2sides, llum_amb, lumi, objecte, frnt_fcs, bck_ln, step);
//
//// Test de Visibilitat
//	if (testv) glEnable(GL_CULL_FACE);
//	else glDisable(GL_CULL_FACE);
//
//// Ocultacions (Z-buffer)
//	if (oculta) glEnable(GL_DEPTH_TEST);
//	else glDisable(GL_DEPTH_TEST);
//
//	return MatriuVista;
//}

// Pega esta función junto a Vista_Esferica, Vista_Navega, etc.
// AFEGEIX AQUESTA FUNCIÓ AL FINAL DE VISUALITZACIO.CPP

glm::mat4 Vista_Seguimiento(GLuint sh_programID, Coche* coche, CEsfe3D opv, bool mobil, CColor col_fons,
	bool oculta, bool testv, bool bck_ln, char iluminacio, bool llum_amb,
	LLUM* lumi, bool ifix[], bool il2sides)
{
	glm::mat4 MatriuVista = glm::mat4(1.0f);
	Fons(col_fons);
	Iluminacio(sh_programID, iluminacio, ifix, il2sides, llum_amb, lumi, ' ', false, bck_ln, 0);
	static glm::vec3 s_smoothedCameraPos = glm::vec3(0.0f);
	static bool s_firstCall = true;
	const float SMOOTHING_FACTOR = 0.8f;
	
	if (coche != nullptr)
	{
		
		glm::vec3 carPos = glm::vec3(coche->x, coche->y, coche->z);
		glm::vec3 modelOriginOffset = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 pivotPoint = carPos + modelOriginOffset; 
		//pillamos las variables globales que utilizan las otras camaras
		float distancia = opv.R;            
		float elevacion_grados = opv.alfa;    
		float azimut_grados = opv.beta;     

		float azimut_final_rad;
		if (!mobil) {
			
			azimut_final_rad = glm::radians(coche->psi); //si no movemos la camara
		}
		else {
			
			azimut_final_rad = glm::radians(azimut_grados); //si movemos la camara
		}

		//nos colocamos hacia atrás y arriba
		float elevacion_rad = glm::radians(elevacion_grados);
		float horizontalDist = distancia * cos(elevacion_rad);
		float verticalDist = distancia * sin(elevacion_rad);

		glm::vec3 cameraPos;

		cameraPos.x = pivotPoint.x - horizontalDist * sinf(azimut_final_rad);

		cameraPos.y = pivotPoint.y - horizontalDist * cosf(azimut_final_rad);

		cameraPos.z = pivotPoint.z + verticalDist;

		
		glm::vec3 cameraTarget = pivotPoint; 

		if (s_firstCall) {
			s_smoothedCameraPos = cameraPos;
			s_firstCall = false;
		}


		s_smoothedCameraPos = glm::mix(s_smoothedCameraPos, cameraPos, SMOOTHING_FACTOR);
	


		
		MatriuVista = glm::lookAt(
			s_smoothedCameraPos,
			cameraTarget,
			glm::vec3(0.0f, 0.0f, 1.0f) 
		);
	}
	else 
	{
		MatriuVista = glm::lookAt(glm::vec3(10, 10, 5), glm::vec3(0, 0, 0), glm::vec3(0, 0, 1));
		printf("No hay coche al que seguir!\n");
	}

	
	glUniformMatrix4fv(glGetUniformLocation(sh_programID, "viewMatrix"), 1, GL_FALSE, &MatriuVista[0][0]);
	//if (activarIluminacion) Iluminacio(sh_programID, iluminacio, ifix, il2sides, llum_amb, lumi, ' ', false, bck_ln, 0);
	if (testv) glEnable(GL_CULL_FACE); else glDisable(GL_CULL_FACE);
	if (oculta) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
	if (bck_ln) glPolygonMode(GL_BACK, GL_LINE);

	return MatriuVista;
}

glm::mat4 Vista_PrimeraPersona(GLuint sh_programID, Coche* coche, CColor col_fons, bool oculta, bool testv, bool bck_ln, char iluminacio, bool llum_amb, LLUM* lumi, bool ifix[], bool il2sides)
{
	glm::mat4 MatriuVista = glm::mat4(1.0);
	Fons(col_fons); 
	bool activarIluminacion = true;
	Iluminacio(sh_programID, iluminacio, ifix, il2sides, llum_amb, lumi, ' ', false, bck_ln, 0);

	if (coche != nullptr)
	{
		glm::mat4 ModelMatrixCar = coche->getModelMatrixCar();

		glm::mat3 Rot3 = glm::mat3(ModelMatrixCar);
		{
			glm::vec3 c0 = glm::normalize(glm::vec3(Rot3[0]));
			glm::vec3 c1 = glm::normalize(glm::vec3(Rot3[1]));
			glm::vec3 c2 = glm::normalize(glm::vec3(Rot3[2]));
			Rot3[0] = c0; Rot3[1] = c1; Rot3[2] = c2;
		}

		glm::vec3 carPos = glm::vec3(coche->x, coche->y, coche->z);
		glm::vec3 modelOriginOffset = glm::vec3(0.0f, 0.0f, 0.0f); 
		glm::vec3 pivotPoint = carPos + modelOriginOffset;
		glm::vec3 cameraUp = glm::normalize(Rot3 * glm::vec3(0.0f, 0.0f, 1.0f));
		float a_rad = glm::radians(coche->psi); 
		glm::vec3 worldForward = glm::normalize(Rot3 * glm::vec3(0.0f, 1.0f, 0.0f));


		float altura = 2.5f;     
		float posicio = -2.8f; 

		glm::vec3 cameraPos = pivotPoint - (worldForward * posicio); //ens posicionem on realment volem
		cameraPos.z = carPos.z + altura; 
		glm::vec3 cameraTarget = cameraPos + (worldForward * 10.0f); 

		
		MatriuVista = glm::lookAt(
			cameraPos,
			cameraTarget,
			cameraUp
		);
	}
	
	glUniformMatrix4fv(glGetUniformLocation(sh_programID, "viewMatrix"), 1, GL_FALSE, &MatriuVista[0][0]);
	//if (ifix) Iluminacio(sh_programID, iluminacio, ifix, il2sides, llum_amb, lumi, ' ', false, bck_ln, 0);
	if (testv) glEnable(GL_CULL_FACE); else glDisable(GL_CULL_FACE);
	if (oculta) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
	if (bck_ln) glPolygonMode(GL_BACK, GL_LINE);

	return MatriuVista;
}

glm::mat4 Vista_Espejo_Central(GLuint sh_programID, Coche* coche, CColor col_fons, bool oculta, bool testv, bool bck_ln, char iluminacio, bool llum_amb, LLUM* lumi, bool ifix[], bool il2sides)
{
	glm::mat4 MatriuVista = glm::mat4(1.0f);
	// ¡NO LIMPIAMOS FONDO (Fons(col_fons))! Dibujamos encima.
	Iluminacio(sh_programID, iluminacio, ifix, il2sides, llum_amb, lumi, ' ', false, bck_ln, 0);

	if (coche != nullptr)
	{
		glm::vec3 carPos = glm::vec3(coche->x, coche->y, coche->z);
		glm::vec3 modelOriginOffset = glm::vec3(0.0f,0.0f, 0.0f);
		glm::vec3 pivotPoint = carPos + modelOriginOffset;

		float a_rad = glm::radians(coche->psi);
		glm::vec3 worldForward = glm::normalize(glm::vec3(sinf(a_rad), cosf(a_rad), 0.0f));

		float altura = 1.0f; //altura que pillamos (aprox la del retrovisor)

		glm::vec3 cameraPos = pivotPoint + (worldForward * 1.1f); //valor que me posiciona más hacia delante o hacia atras e funncio del origen
		cameraPos.z = carPos.z + altura;

		
		glm::vec3 cameraTarget = cameraPos - (worldForward * 10.0f); 

		MatriuVista = glm::lookAt(cameraPos, cameraTarget, glm::vec3(0.0f, 0.0f, 1.0f)); // Z-up
	}

	glUniformMatrix4fv(glGetUniformLocation(sh_programID, "viewMatrix"), 1, GL_FALSE, &MatriuVista[0][0]);
	//if (ifix) Iluminacio(sh_programID, iluminacio, ifix, il2sides, llum_amb, lumi, ' ', false, bck_ln, 0);
	if (testv) glEnable(GL_CULL_FACE); else glDisable(GL_CULL_FACE);
	if (oculta) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
	if (bck_ln) glPolygonMode(GL_BACK, GL_LINE);

	return MatriuVista;
}

glm::mat4 Vista_Retrovisor(GLuint sh_programID, Coche* coche, bool esIzquierdo, CColor col_fons, bool oculta, bool testv, bool bck_ln, char iluminacio, bool llum_amb, LLUM* lumi, bool ifix[], bool il2sides)
{
	glm::mat4 MatriuVista = glm::mat4(1.0f);
	// ¡NO LIMPIAMOS FONDO (Fons(col_fons))! Dibujamos encima.
	Iluminacio(sh_programID, iluminacio, ifix, il2sides, llum_amb, lumi, ' ', false, bck_ln, 0);

	if (coche != nullptr)
	{
		glm::vec3 carPos = glm::vec3(coche->x, coche->y, coche->z);
		glm::vec3 modelOriginOffset = glm::vec3(0.0f, 0.5f, 0.0f);
		glm::vec3 pivotPoint = carPos + modelOriginOffset;

		float a_rad = glm::radians(coche->psi);
		glm::vec3 worldForward = glm::normalize(glm::vec3(sinf(a_rad), cosf(a_rad), 0.0f));
		// Vector "derecha" (perpendicular a "adelante" en Z-up)
		glm::vec3 worldRight = glm::normalize(glm::vec3(worldForward.y, worldForward.x, 0.0f));

		float mirrorHeight = 1.5f;
		float mirrorWidthOffset = 1.2f; 
		float side = esIzquierdo ? -2.5f : 2.5f; 

		glm::vec3 cameraPos = pivotPoint + (worldRight * mirrorWidthOffset * side);
		cameraPos.z = carPos.z + mirrorHeight;

		// Punto de mira: Hacia atrás y ligeramente hacia afuera
		glm::vec3 cameraTarget = cameraPos - (worldForward * 10.0f) + (worldRight * 2.0f * side);

		MatriuVista = glm::lookAt(cameraPos, cameraTarget, glm::vec3(0.0f, 0.0f, 1.0f)); 
	}

	glUniformMatrix4fv(glGetUniformLocation(sh_programID, "viewMatrix"), 1, GL_FALSE, &MatriuVista[0][0]);
	//if (ifix) Iluminacio(sh_programID, iluminacio, ifix, il2sides, llum_amb, lumi, ' ', false, bck_ln, 0);
	if (testv) glEnable(GL_CULL_FACE); else glDisable(GL_CULL_FACE);
	if (oculta) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
	if (bck_ln) glPolygonMode(GL_BACK, GL_LINE);

	return MatriuVista;
}

glm::mat4 Vista_Lliure(GLuint sh_programID, CColor col_fons, CEsfe3D opv, glm::vec3 g_FreeCamPos,
	bool oculta, bool testv, bool bck_ln, char iluminacio, bool llum_amb,
	LLUM* lumi, bool ifix[], bool il2sides)
{
	glm::mat4 MatriuVista = glm::mat4(1.0f);
	Fons(col_fons); // Limpia la pantalla
	Iluminacio(sh_programID, iluminacio, ifix, il2sides, llum_amb, lumi, ' ', false, bck_ln, 0);
	

	glm::vec3 front;
	float yaw_rad = glm::radians(opv.beta);    
	float pitch_rad = glm::radians(opv.alfa); 

	front.x = -sin(yaw_rad) * cos(pitch_rad);
	front.y = cos(yaw_rad) * cos(pitch_rad);
	front.z = sin(pitch_rad); 

	glm::vec3 cameraFront = glm::normalize(front);
	glm::vec3 worldUp = glm::vec3(0.0f, 0.0f, 1.0f);
	glm::vec3 cameraRight = glm::normalize(glm::cross(cameraFront, worldUp));

	glm::vec3 cameraUp = glm::normalize(glm::cross(cameraRight, cameraFront));

	glm::vec3 cameraTarget = g_FreeCamPos + cameraFront;

	MatriuVista = glm::lookAt(
		g_FreeCamPos,    
		cameraTarget,     
		cameraUp          
	);


	glUniformMatrix4fv(glGetUniformLocation(sh_programID, "viewMatrix"), 1, GL_FALSE, &MatriuVista[0][0]);
	//if (ifix) Iluminacio(sh_programID, iluminacio, ifix, il2sides, llum_amb, lumi, ' ', false, bck_ln, 0);
	if (testv) glEnable(GL_CULL_FACE); else glDisable(GL_CULL_FACE);
	if (oculta) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
	if (bck_ln) glPolygonMode(GL_BACK, GL_LINE);

	return MatriuVista;
}

glm::mat4 Vista_menu_inici(GLuint sh_programID, Coche* coche, CEsfe3D opv, bool mobil, CColor col_fons,
	bool oculta, bool testv, bool bck_ln, char iluminacio, bool llum_amb,
	LLUM* lumi, bool ifix[], bool il2sides)
{
	glm::mat4 MatriuVista = glm::mat4(1.0f);
	Fons(col_fons);
	Iluminacio(sh_programID, iluminacio, ifix, il2sides, llum_amb, lumi, ' ', false, bck_ln, 0);

	if (coche != nullptr)
	{

		glm::vec3 puntMig = glm::vec3(0.0f, 300.0f, 0.0f);

		//pillamos las variables globales que utilizan las otras camaras
		float distancia = opv.R;
		float elevacion_grados = opv.alfa;
		float azimut_grados = opv.beta;

		float azimut_final_rad;


		azimut_final_rad = glm::radians(azimut_grados); //si movemos la camara
		

		//nos colocamos hacia atrás y arriba
		float elevacion_rad = glm::radians(elevacion_grados);
		float horizontalDist = distancia * cos(elevacion_rad);
		float verticalDist = distancia * sin(elevacion_rad);

		glm::vec3 cameraPos;
		//aplicamos los calculos al punto del coche
		cameraPos.x = puntMig.x + horizontalDist * cosf(azimut_final_rad) * 30;
		cameraPos.y = puntMig.y - horizontalDist * sinf(azimut_final_rad) * 30;
		cameraPos.z = puntMig.z + verticalDist * 160;


		glm::vec3 cameraTarget = puntMig;


		MatriuVista = glm::lookAt(
			cameraPos,
			cameraTarget,
			glm::vec3(0.0f, 0.0f, 1.0f)
		);
	}
	else
	{
		MatriuVista = glm::lookAt(glm::vec3(10, 10, 5), glm::vec3(0, 0, 0), glm::vec3(0, 0, 1));
		printf("No hay coche al que seguir!\n");
	}


	glUniformMatrix4fv(glGetUniformLocation(sh_programID, "viewMatrix"), 1, GL_FALSE, &MatriuVista[0][0]);
	//if (ifix) Iluminacio(sh_programID, iluminacio, ifix, il2sides, llum_amb, lumi, ' ', false, bck_ln, 0);
	if (testv) glEnable(GL_CULL_FACE); else glDisable(GL_CULL_FACE);
	if (oculta) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
	if (bck_ln) glPolygonMode(GL_BACK, GL_LINE);

	return MatriuVista;
} 
glm::mat4 Vista_Pausa(GLuint sh_programID, Coche* coche, CEsfe3D opv, bool mobil, CColor col_fons,
	bool oculta, bool testv, bool bck_ln, char iluminacio, bool llum_amb,
	LLUM* lumi, bool ifix[], bool il2sides)
{
	glm::mat4 MatriuVista = glm::mat4(1.0f);

	Fons(col_fons);
	Iluminacio(sh_programID, iluminacio, ifix, il2sides, llum_amb, lumi, ' ', false, bck_ln, 0);

	if (coche != nullptr)
	{

		glm::vec3 carPos = glm::vec3(coche->x, coche->y, coche->z);
		glm::vec3 modelOriginOffset = glm::vec3(3.0f, 0.0f, 0.0f);
		glm::vec3 pivotPoint = carPos + modelOriginOffset;
		//pillamos las variables globales que utilizan las otras camaras
		float distancia = opv.R;
		float elevacion_grados = opv.alfa;
		float azimut_grados = opv.beta;

		float azimut_final_rad;


		azimut_final_rad = glm::radians(azimut_grados); //si movemos la camara


		//nos colocamos hacia atrás y arriba
		float elevacion_rad = glm::radians(elevacion_grados);
		float horizontalDist = distancia * cos(elevacion_rad);
		float verticalDist = distancia * sin(elevacion_rad);

		glm::vec3 cameraPos;
		//aplicamos los calculos al punto del coche
		cameraPos.x = pivotPoint.x + horizontalDist * cosf(azimut_final_rad);
		cameraPos.y = pivotPoint.y - horizontalDist * sinf(azimut_final_rad);
		cameraPos.z = pivotPoint.z + verticalDist * 3;


		glm::vec3 cameraTarget = pivotPoint;


		MatriuVista = glm::lookAt(
			cameraPos,
			cameraTarget,
			glm::vec3(0.0f, 0.0f, 1.0f)
		); 

	}
	else
	{
		MatriuVista = glm::lookAt(glm::vec3(10, 10, 5), glm::vec3(0, 0, 0), glm::vec3(0, 0, 1));
		printf("No hay coche al que seguir!\n");
	}


	glUniformMatrix4fv(glGetUniformLocation(sh_programID, "viewMatrix"), 1, GL_FALSE, &MatriuVista[0][0]);
	//if (ifix) Iluminacio(sh_programID, iluminacio, ifix, il2sides, llum_amb, lumi, ' ', false, bck_ln, 0);
	if (testv) glEnable(GL_CULL_FACE); else glDisable(GL_CULL_FACE);
	if (oculta) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
	if (bck_ln) glPolygonMode(GL_BACK, GL_LINE);

	return MatriuVista;
}


void func_llumsCotxe(Coche* coche, ControlLuces& control, LLUM* lumin)
{
	if (coche == nullptr) return;
	btRigidBody* body = coche->getChassisBody();
	btTransform trans;
	btVector3 velocity_bt = body->getLinearVelocity();
	glm::vec3 worldVelocity = glm::vec3(velocity_bt.getX(), velocity_bt.getY(), velocity_bt.getZ());
	if (body && body->getMotionState()) {
		body->getMotionState()->getWorldTransform(trans);
	}
	else {
		// fallback: construir transform desde x,y,z,psi si no hay física (poco probable)
		trans.setIdentity();
		trans.setOrigin(btVector3(coche->x, coche->y, coche->z));
		float r = glm::radians(coche->psi);
		btMatrix3x3 rot;
		// rotación alrededor de Z (ten en cuenta que en tu sistema forward=Y)
		rot.setEulerZYX(0.0f, 0.0f, r); // ZYX: solo Z
		trans.setBasis(rot);
	}

	// Basis: columnas -> 0 = right (X), 1 = forward (Y), 2 = up (Z)
	btMatrix3x3 basis = trans.getBasis();
	glm::vec3 right = glm::normalize(glm::vec3(basis.getColumn(0).getX(),
		basis.getColumn(0).getY(),
		basis.getColumn(0).getZ()));
	glm::vec3 forward = glm::normalize(glm::vec3(basis.getColumn(1).getX(),
		basis.getColumn(1).getY(),
		basis.getColumn(1).getZ()));
	glm::vec3 up = glm::normalize(glm::vec3(basis.getColumn(2).getX(),
		basis.getColumn(2).getY(),
		basis.getColumn(2).getZ()));

	glm::vec3 carPos = glm::vec3(trans.getOrigin().getX(),
		trans.getOrigin().getY(),
		trans.getOrigin().getZ());
	float velocidadAdelante = glm::dot(worldVelocity, forward);
	bool enMarchaAtras = (velocidadAdelante < -0.1f);
	bool frenando = control.frenando && !enMarchaAtras;
	bool parpadeoOn = (fmod(control.tiempoTotal, 1.0f) < 0.5f);


	float modeloEscala = 0.8f;
	// ---------------------------------------------------------
	// 1. FAROS DELANTEROS (Indices 8 y 9)
	// ---------------------------------------------------------
	if (control.modoFaros != 0)
	{
		for (int i = 8; i < 10; ++i)
		{
			lumin[i].encesa = true;

			float offX = (i == 8) ? -2.0f : 2.0f;
			float forwardDist = 5.0f;
			float height = 0.7f; 


			glm::vec3 posFaro_world = carPos + right * offX + forward * forwardDist + up * height;


			float tilt = (control.modoFaros == 1) ? -0.20f : -0.02f;

			float spread = (control.modoFaros == 1) ? ((i == 8) ? -0.1f : 0.1f) : 0.0f;

			glm::vec3 worldDir = glm::normalize(forward + (up * tilt) + (right * spread));

			lumin[i].posicio = { posFaro_world.x, posFaro_world.y, posFaro_world.z, 1.0f };
			lumin[i].spotdirection = { worldDir.x, worldDir.y, worldDir.z };
			lumin[i].restringida = true;

			if (control.modoFaros == 1) 
			{
				
				lumin[i].spotcoscutoff = cos(glm::radians(40.0f));
				lumin[i].spotexponent = 10.0f; 

				
				lumin[i].difusa = { 200.0f, 200.0f, 180.0f, 1.0f };
				lumin[i].especular = { 0.0f, 0.0f, 0.0f, 1.0f };


				lumin[i].atenuacio = { 1.0f, 0.09f, 0.032f };
			}
			else 
			{
				
				lumin[i].spotcoscutoff = cos(glm::radians(15.0f));
				lumin[i].spotexponent = 120.0f; 

				
				lumin[i].difusa = { 1500.0f, 1500.0f, 1400.0f, 1.0f };
				lumin[i].especular = { 10.0f, 10.0f, 10.0f, 1.0f }; 

				
				lumin[i].atenuacio = { 1.0f, 0.005f, 0.0001f };
			}
		}

	}
	else
	{
		
		lumin[8].encesa = false;
		lumin[9].encesa = false;
	}

	// ---------------------------------------------------------
	// 2. FRENOS (Índices 10 y 11)
	// ---------------------------------------------------------
	for (int i = 10; i < 12; i++)
	{
		float offX, offY;
		if (i == 10) { offX = -2.0f;  }
		else { offX = 2.0f; }
		offY = 5.0f;
		glm::vec3 posCola = carPos - (forward * offY) + (right * offX);
		posCola.z += 1.0f;

		lumin[i].posicio = { posCola.x, posCola.y, posCola.z, 1.0f };
		lumin[i].restringida = true;

		glm::vec3 dirFreno = -forward;
		dirFreno.z -= 0.3f;
		dirFreno = glm::normalize(dirFreno);

		lumin[i].spotdirection = { dirFreno.x, dirFreno.y, dirFreno.z };
		lumin[i].spotcoscutoff = cos(glm::radians(30.0f));
		lumin[i].spotexponent = 10.0f;
		lumin[i].especular = { 1.0f, 0.5f, 0.5f, 1.0f };

		if (frenando) {
			lumin[i].encesa = true;
			lumin[i].difusa = { 80.0f, 0.0f, 0.0f, 1.0f };
			lumin[i].atenuacio = { 0.5f, 0.5f, 0.5f };
		}
		else if (control.modoFaros != 0) {
			lumin[i].encesa = true;
			lumin[i].difusa = { 5.0f, 0.0f, 0.0f, 1.0f };
			lumin[i].atenuacio = { 0.5f, 0.5f, 0.5f };
		}
		else lumin[i].encesa = false;
	}

	// ---------------------------------------------------------
	// 3. MARCHA ATRÁS (Índices 12 y 13)
	// ---------------------------------------------------------
	for (int i = 12; i < 14; i++)
	{
		float offX, offY;
		if (i == 12) { offX = -2.0f;  }
		else { offX = 2.0f;  }
		offY = 5.0f;
		glm::vec3 posMarcha = carPos - (forward * offY) + (right * offX);
		posMarcha.z += 1.0f;

		lumin[i].posicio = { posMarcha.x, posMarcha.y, posMarcha.z, 1.0f };
		lumin[i].restringida = true;

		glm::vec3 dirMarcha = -forward;
		lumin[i].spotdirection = { dirMarcha.x, dirMarcha.y, dirMarcha.z };
		lumin[i].spotcoscutoff = cos(glm::radians(45.0f));
		lumin[i].spotexponent = 5.0f;
		lumin[i].especular = { 1.0f, 1.0f, 1.0f, 1.0f };

		if (enMarchaAtras) {
			lumin[i].encesa = true;
			lumin[i].difusa = { 30.0f, 30.0f, 30.0f, 1.0f };
			lumin[i].atenuacio = { 0.5f, 0.5f, 0.5f };
		}
		else lumin[i].encesa = false;
	}

	// ---------------------------------------------------------
	// 4. INTERMITENTES (Índices 14-17)
	// ---------------------------------------------------------
	float intRetro_X_Izq = -2.0f;   // más separado hacia la izquierda
	float intRetro_X_Der = 2.0f;    // más separado hacia la derecha
	float distDelante = 6.0f;       // distancia hacia delante
	float distDetras =6.0f;        // distancia hacia atrás
	float alturaRetro = 1.0f;       

	if (control.intermitenteIzquierdo && parpadeoOn)
	{
		// DELANTE IZQUIERDA (14)
		lumin[14].encesa = true;
		lumin[14].difusa = { 20.0f, 10.0f, 0.0f, 1.0f };   
		lumin[14].restringida = true;
		lumin[14].especular = { 1.0f, 0.8f, 0.0f, 1.0f };
		lumin[14].atenuacio = { 1.0f, 2.0f, 3.0f };

		glm::vec3 pos = carPos + (forward * distDelante) + (right * intRetro_X_Izq);
		pos.z += alturaRetro;
		lumin[14].posicio = { pos.x, pos.y, pos.z, 1.0f };

		glm::vec3 dir = -right; // ilumina hacia fuera
		lumin[14].spotdirection = { dir.x, dir.y, dir.z };
		lumin[14].spotcoscutoff = cos(glm::radians(80.0f));
		//lumin[14].spotexponent = 10.0f;

		// DETRÁS IZQUIERDA (16)
		lumin[16].encesa = true;
		lumin[16].difusa = { 20.0f, 10.0f, 0.0f, 1.0f };
		lumin[16].restringida = true;
		lumin[16].especular = { 1.0f, 0.8f, 0.0f, 1.0f };
		lumin[16].atenuacio = { 1.0f, 2.0f, 3.0f };

		pos = carPos - (forward * distDetras) + (right * intRetro_X_Izq);
		pos.z += alturaRetro;
		lumin[16].posicio = { pos.x, pos.y, pos.z, 1.0f };

		dir = -right;
		lumin[16].spotdirection = { dir.x, dir.y, dir.z };
		lumin[16].spotcoscutoff = cos(glm::radians(80.0f));
		//lumin[16].spotexponent = 10.0f;
	}
	else { lumin[14].encesa = false; lumin[16].encesa = false; }

	if (control.intermitenteDerecho && parpadeoOn)
	{
		// DELANTE DERECHA (15)
		lumin[15].encesa = true;
		lumin[15].difusa = { 20.0f, 10.0f, 0.0f, 1.0f };
		lumin[15].restringida = true;
		lumin[15].especular = { 1.0f, 0.8f, 0.0f, 1.0f };
		lumin[15].atenuacio = { 1.0f, 2.0f, 3.0f };

		glm::vec3 pos = carPos + (forward * distDelante) + (right * intRetro_X_Der);
		pos.z += alturaRetro;
		lumin[15].posicio = { pos.x, pos.y, pos.z, 1.0f };

		glm::vec3 dir = right; // ilumina hacia fuera
		lumin[15].spotdirection = { dir.x, dir.y, dir.z };
		lumin[15].spotcoscutoff = cos(glm::radians(80.0f));
		//lumin[15].spotexponent = 10.0f;

		// DETRÁS DERECHA (17)
		lumin[17].encesa = true;
		lumin[17].difusa = { 20.0f, 10.0f, 0.0f, 1.0f };
		lumin[17].restringida = true;
		lumin[17].especular = { 1.0f, 0.8f, 0.0f, 1.0f };
		lumin[17].atenuacio = { 1.0f, 2.0f, 3.0f };

		pos = carPos - (forward * distDetras) + (right * intRetro_X_Der);
		pos.z += alturaRetro;
		lumin[17].posicio = { pos.x, pos.y, pos.z, 1.0f };

		dir = right;
		lumin[17].spotdirection = { dir.x, dir.y, dir.z };
		lumin[17].spotcoscutoff = cos(glm::radians(80.0f));
		//lumin[17].spotexponent = 10.0f;
	}
	else { lumin[15].encesa = false; lumin[17].encesa = false; }
}


// instancia: Aplica Transformacions Geometriques d'instanciació, segons els parametres 
//            definits a la persiana Transformacions
glm::mat4 instancia(bool TR, INSTANCIA tg, INSTANCIA tgF)
{
	glm::mat4 MatriuTG(1.0), MatriuT(1.0);

// Transformacions geometriques sobre objecte (Traslacio, Rotacions i Escalatge) amb conversió angles Euler.
	if (TR) MatriuTG = glm::translate(MatriuTG, glm::vec3(tg.VTras.x, tg.VTras.y, tg.VTras.z));
	MatriuTG = glm::translate(MatriuTG, glm::vec3(tgF.VTras.x, tgF.VTras.y, tgF.VTras.z));	// Traslacio fixada amb la INSERT dins l'opcio Transformacions

// Convertir angles Euler tg i tgF en Matrius
	if (TR) {	MatriuT = glm::eulerAngleX(tg.VRota.x * PI / 180.f); // Passar angle en radians
				MatriuTG = MatriuTG * MatriuT;
			}
	MatriuT = glm::eulerAngleX(tgF.VRota.x * PI / 180.f); // Passar angle en radians
	MatriuTG = MatriuTG * MatriuT;
	if (TR) {	MatriuT = glm::eulerAngleY(tg.VRota.y * PI / 180.f); // Passar angle en radians
				MatriuTG = MatriuTG * MatriuT;
			}
	MatriuT = glm::eulerAngleY(tgF.VRota.y * PI / 180.f); // Passar angle en radians
	MatriuTG = MatriuTG * MatriuT;
	if (TR) {	MatriuT = glm::eulerAngleZ(tg.VRota.z * PI / 180.f); // Passar angle en radians
				MatriuTG = MatriuTG * MatriuT;
			}
	MatriuT = glm::eulerAngleX(tgF.VRota.z * PI / 180.f); // Passar angle en radians
	MatriuTG = MatriuTG * MatriuT;

	if (TR) MatriuTG = glm::scale(MatriuTG, vec3(tg.VScal.x, tg.VScal.y, tg.VScal.z));
	MatriuTG = glm::scale(MatriuTG, vec3(tgF.VScal.x, tgF.VScal.y, tgF.VScal.z));	// Escalatge fixada amb la INSERT dins l'opcio Transformacions

// Transformacions geometriques sobre objecte (Traslacio, Rotacions i Escalatge) amb Quaternions
	if (TR) MatriuTG = glm::translate(MatriuTG, glm::vec3(tg.VTras.x, tg.VTras.y, tg.VTras.z));
	MatriuTG = glm::translate(MatriuTG, glm::vec3(tgF.VTras.x, tgF.VTras.y, tgF.VTras.z));	// Traslacio fixada amb la INSERT dins l'opcio Transformacions

	return MatriuTG;
}


// deixos: Creació del VBO per al dibuix dels eixos coordenats
GLuint deixos()
{
	GLuint vaoId = 0;	// VAO Index
	GLuint vboId = 0;	// VBO Index

// vertex coords array for glDrawArrays() =====================================
// The World Coordinates Axes (WCA) are 3 lines, one over X axis (red), one over Y axis (green), one over Z axis(blue),
// the WCA cube consists of 6 vertices (3 axes * 4 vertices = 6 vertices). And, each
// vertex is 3 components (x,y,z) of doubles, therefore, the size of vertex
// array is 18 doubles (6 * 3 = 18). The size of each double is 4 bytes (72 * 4 = 288 bytes)
	GLfloat vertices[] = { 0.0f, 0.0f, 0.0f,  300.0f,   0.0f,   0.0f,  // X-Axis
						   0.0f, 0.0f, 0.0f,    0.0f, 300.0f,   0.0f,	 // Y-Axis
						   0.0f, 0.0f, 0.0f,    0.0f,   0.0f, 300.0f}; // Z-Axis

	GLfloat colors[] = { 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,   // Red color (X-Axis)
						 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f,   // Green color (Y-Axis)
						 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f }; // Blue color (Z-Axis)

// Doblar el gruix de la linia dels eixos.
	//glLineWidth(2.0);

// Create Vertex Array Object (VAO) for 3D Model Cube
	glGenVertexArrays(1, &vaoId);

// Create vertex buffer objects for 3D Model attributes in the VAO
	glGenBuffers(1, &vboId);

// Bind our Vertex Array Object as the current used object
	glBindVertexArray(vaoId);

// Bind first VBO as being active buffer and storing vertex attributes (coordinates)
	glBindBuffer(GL_ARRAY_BUFFER, vboId);							// Activar VBO vèrtexs

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) + sizeof(colors), 0, GL_STATIC_DRAW);	// Allocate data to VBO starting from 0 offest

// Position Vertex attributes
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), &vertices);	// Copy geometry data to VBO starting from 0 offest
	glEnableVertexAttribArray(0);										// Enable attribute index 0 as being used (position)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);	// Specify that our coordinate data is going into attribute index 0 and contains 3 double

// Normal vertex Attributes
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices), sizeof(colors), &colors); // Copy color data to VBO starting from 0 offest
	glEnableVertexAttribArray(1);										// Enable attribute index 1 as being used (normals)
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)sizeof(vertices));	// Specify that our color data is going into attribute index 0 and contains 3 double

//Unbind the registered VBO as current VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);

// Unbind VAO, to prevent bugs
	glBindVertexArray(0);

/*
// Eix X (vermell)
	glColor3f(1.0,0.0,0.0);
	glBegin(GL_LINES);
	 glVertex3f(0.0,0.0,0.0);
	 glVertex3f(300.0,0.0,0.0);
	glEnd();

// Eix Y (verd)
    glColor3f(0.0,1.0,0.0);
	glBegin(GL_LINES);
	 glVertex3f(0.0,0.0,0.0);
	 glVertex3f(0.0,300.0,0.0);
	glEnd();

// Eix Z (blau) 
	glColor3f(0.0,1.0,1.0);
	glBegin(GL_LINES);
	 glVertex3f(0.0,0.0,0.0);
	 glVertex3f(0.0,0.0,300.0);
	glEnd();
*/
	return vaoId;
}

// draw_Eixos: Dibuix del VBO dels eixos coordenats
void draw_Eixos(GLuint vaoId)
{
// Doblar el gruix de la linia dels eixos.
//	glEnable(GL_LINE_WIDTH);
//	glLineWidth(2.0);

	glBindVertexArray(vaoId);
	glDrawArrays(GL_LINES, 0, 6);
	glBindVertexArray(0);

// Restaurar el gruix de la linia dels eixos
//	glLineWidth(1.0);
//	glDisable(GL_LINE_WIDTH);
}


// ------------------- GRID (gridXY, gridYZ, gridXZ, gridXYZ) -------------------------

void draw_Grid(CMask3D quadricula, CPunt3D hquadricula)
{
// Switch on transparency to draw the grid over the scene
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

// Grids drawing
	if (quadricula.w) gridXYZ(GRID_SIZE);
	else  {	if (quadricula.x) gridXY(GRID_SIZE, hquadricula.z);
			if (quadricula.y) gridXZ(GRID_SIZE, hquadricula.y);
			if (quadricula.z) gridYZ(GRID_SIZE, hquadricula.x);
			}

// Switch off transparency
	glDisable(GL_BLEND);
}


// gridXY: Construeix un grid XY de tamany size amb pas 1 situat una alçada h de l'eiz Z.
void gridXY(int size, GLdouble h)
{
	int i;
	//CColor col;

// VAO
	GLuint vaoId = 0, vboId = 0;
	std::vector <double> vertices, colors;		// Definició vectors dinàmics per a vertexs i colors 
	//vertices.resize(0);		colors.resize(0);	// Reinicialitzar vectors
	vertices.clear();		colors.clear();		// Reinicialitzar vectors

// Eix X
	//glColor4f(1.0f, 0.0f, 0.0f, 0.5f);
	for (i = -size; i < size + 1; i++)
		{	//glBegin(GL_LINES);
			  colors.push_back(1.0);		colors.push_back(0.0);			colors.push_back(0.0);	colors.push_back(1.0);
			  //glVertex3f(i, -size, h);	// glVertex3f(i, -0.5f, 0.0f);
			  vertices.push_back(-i);		vertices.push_back(-size);		vertices.push_back(h);
			  
			  colors.push_back(1.0);			colors.push_back(0.0);		colors.push_back(0.0);	colors.push_back(1.0);
			  //glVertex3f(i, +size, h);	// glVertex3f(i, +0.5f, 0.0f);
			  vertices.push_back(-i);		vertices.push_back(+size);		vertices.push_back(h);

			//glEnd();
		}

// Eix Y
	//glColor4f(0.0f, 1.0, 0.0f, 0.5f);
	for (i = -size; i < size + 1; i++)
		{	//glBegin(GL_LINES);
			  colors.push_back(0.0);		colors.push_back(1.0);		colors.push_back(0.0);	colors.push_back(1.0);
			  //glVertex3f(-size, i, h);	// glVertex3f(-0.5f, i, 0.0f);
			  vertices.push_back(-size);	vertices.push_back(i);		vertices.push_back(h);

			  colors.push_back(0.0);		colors.push_back(1.0);		colors.push_back(0.0);	colors.push_back(1.0);
			  //glVertex3f(+size, i, h);	// glVertex3f(+0.5f, i, 0.0f);
			  vertices.push_back(+size);	vertices.push_back(i);		vertices.push_back(h);
			//glEnd();
		}

// ----------------------- VBO
	std::vector <int>::size_type nv = vertices.size();	// Tamany del vector vertices en elements.
	std::vector <int>::size_type nc = colors.size();	// Tamany del vector colors en elements.

	draw_GL_LINES_VAO(vertices, colors);
}

// gridXZ: Construeix un grid YZ de tamany size amb pas 1 situat una alçada h de l'eiz Z.
void gridXZ(int size, GLdouble h)
{
	int i;
	//CColor col;

// VAO
	//GLuint vaoId = 0, vboId = 0;
	std::vector <double> vertices, colors;		// Definició vectors dinàmics per a vertexs i colors 
	//vertices.resize(0);		colors.resize(0);	// Reinicialitzar vectors
	vertices.clear();		colors.clear();		// Reinicialitzar vectors


// Eix X
	//glColor4f(1.0f, 0.0, 0.0f, 0.5f);
	//col.r = 1.0f;	col.g = 0.0f;	col.b = 0.0f;	col.a = 0.5f;
	//SeleccionaColor(ref_mat, sw_mat, col);
	for (i = -size; i < size + 1; i++)
		{	//glBegin(GL_LINES);
			  colors.push_back(1.0);		colors.push_back(0.0);	colors.push_back(0.0);	colors.push_back(1.0);
			  // glVertex3f(i, h, -size);
			  vertices.push_back(i);		vertices.push_back(h);	vertices.push_back(-size);

			  colors.push_back(1.0);		colors.push_back(0.0);	colors.push_back(0.0);	colors.push_back(1.0);
			  //glVertex3f(i, h, +size); 
			  vertices.push_back(i);	vertices.push_back(h);		vertices.push_back(+size);
			//glEnd();
		}

// Eix Z
	//glColor4f(0.0f, 1.0, 1.0f, 0.5f);
	//col.r = 0.0f;	col.g = 1.0f;	col.b = 1.0f;	col.a = 0.5f;
	//SeleccionaColor(ref_mat, sw_mat, col);
	for (i = -size; i < size + 1; i++)
		{	//glBegin(GL_LINES);
			  colors.push_back(0.0);		colors.push_back(1.0);	colors.push_back(1.0); colors.push_back(1.0);
			  //glVertex3f(-size, h, i);
			  vertices.push_back(-size);	vertices.push_back(h);	vertices.push_back(i);

			  colors.push_back(0.0);		colors.push_back(1.0);	colors.push_back(1.0);	colors.push_back(1.0);
			  //glVertex3f(size, h, i);
			  vertices.push_back(size);	vertices.push_back(h);		vertices.push_back(i);
			//glEnd();
	}

// ----------------------- VBO
	std::vector <int>::size_type nv = vertices.size();	// Tamany del vector vertices en elements.
	std::vector <int>::size_type nc = colors.size();	// Tamany del vector colors en elements.

	draw_GL_LINES_VAO(vertices, colors);
}


// gridYZ: Construeix un grid YZ de tamany size amb pas 1 situat una alçada h de l'eiz Z.
void gridYZ(int size, GLdouble h)
{
	int i;
	//CColor col;
	//bool ref_mat;
	//bool sw_mat[4];

// VAO, VBO
	//GLuint vaoId = 0, vboId = 0;
	std::vector <double> vertices, colors;		// Definició vectors dinàmics per a vertexs i colors 
	//vertices.resize(0);		colors.resize(0);	// Reinicialitzar vectors
	vertices.clear();		colors.clear();		// Reinicialitzar vectors

	//ref_mat = false;
	//sw_mat[0] = false;	sw_mat[1] = true;	sw_mat[2] = true;	sw_mat[3] = false;

// Eix Y
	//glColor4f(0.0f, 1.0, 0.0f, 0.5f);
	//col.r = 0.0f;	col.g = 1.0f;	col.b = 0.0f;	col.a = 0.5f;
	//SeleccionaColor(ref_mat, sw_mat, col);
	for (i = -size; i < size + 1; i++)
		{	//glBegin(GL_LINES);
		      colors.push_back(0.0);		colors.push_back(1.0);			colors.push_back(0.0);	colors.push_back(1.0);
			  //glVertex3f(h, i, -size);	// glVertex3f(0.0f, i, -0.5f);
			  vertices.push_back(h);		vertices.push_back(i);			vertices.push_back(-size);
			  
			  colors.push_back(0.0);		colors.push_back(1.0);			colors.push_back(0.0);	colors.push_back(1.0);
			  //glVertex3f(h, i, +size);	// glVertex3f(0.0f, i, 0.5f);
			  vertices.push_back(h);		vertices.push_back(i);			vertices.push_back(+size);

			//glEnd();
		}
/*
// Eix Z
	//glColor4f(0.0f, 1.0, 1.0f, 0.5f);
	//col.r = 0.0f;	col.g = 1.0f;	col.b = 1.0f;	col.a = 0.5f;
	//SeleccionaColor(ref_mat, sw_mat, col);
	for (i = -size; i < size + 1; i++)
		{	//glBegin(GL_LINES);
			  colors.push_back(0.0);		colors.push_back(1.0);			colors.push_back(1.0);	colors.push_back(1.0);
			  //glVertex3f(h, -size, i);	// glVertex3f(0.0, -0.5f, i);
			  vertices.push_back(h);		vertices.push_back(-size);		vertices.push_back(i);
			  
			  colors.push_back(0.0);		colors.push_back(1.0);			colors.push_back(1.0);	colors.push_back(1.0);
			  //glVertex3f(h, +size, i);	// glVertex3f(0.0f, 0.5f, i);
			  vertices.push_back(h);		vertices.push_back(+size);		vertices.push_back(i);
			//glEnd();
	}
*/
// ----------------------- VBO
	std::vector <int>::size_type nv = vertices.size();	// Tamany del vector vertices en elements.
	std::vector <int>::size_type nc = colors.size();	// Tamany del vector colors en elements.

	draw_GL_LINES_VAO(vertices, colors);
}


// gridXYZ: Construeix un grid 3D de tamany size amb pas 1.
void gridXYZ(int size)
{
	int i, j;
	//CColor col;
	//bool ref_mat;
	//bool sw_mat[4];

// VAO, VBO
	GLuint vaoId = 0, vboId = 0;
	std::vector <double> vertices, colors;		// Definició vectors dinàmics per a vertexs i colors 
	//vertices.resize(0);		colors.resize(0);	// Reinicialitzar vectors
	vertices.clear();		colors.clear();		// Reinicialitzar vectors

	//ref_mat = false;
	//sw_mat[0] = false;	sw_mat[1] = true;	sw_mat[2] = true;	sw_mat[3] = false;

	for (j = -size; j < size; j++)
	{
// Eix X
		//glColor4f(1.0f, 0.0, 0.0f, 0.5f);
		//col.r = 1.0f;	col.g = 0.0f;	col.b = 0.0f;	col.a = 0.5f;
		//SeleccionaColor(ref_mat, sw_mat, col);
		for (i = -size; i <= size; i++)
			{	//glBegin(GL_LINES);
				  colors.push_back(1.0);		colors.push_back(0.0);			colors.push_back(0.0);	colors.push_back(1.0);
				  //glVertex3f(i, -size, j);	// glVertex3f(i, -0.5f, z);
				  vertices.push_back(i);		vertices.push_back(-size);		vertices.push_back(j);

				  colors.push_back(1.0);		colors.push_back(0.0);			colors.push_back(0.0);	colors.push_back(1.0);
				  //glVertex3f(i, +size, j);	// glVertex3f(i, +0.5f, z);
				  vertices.push_back(i);		vertices.push_back(+size);		vertices.push_back(j);
				//glEnd();
			}

// Eix Y
		//glColor4f(0.0f, 1.0, 0.0f, 0.5f);
		//col.r = 0.0f;	col.g = 1.0f;	col.b = 0.0f;	col.a = 0.5f;
		//SeleccionaColor(ref_mat, sw_mat, col);
		for (i = -size; i <= size; i++)
			{	//glBegin(GL_LINES);
				  colors.push_back(0.0);		colors.push_back(1.0);		colors.push_back(0.0);	colors.push_back(1.0);
				  //glVertex3f(-size, i, j);	// glVertex3f(-0.5f, i, z);
				  vertices.push_back(-size);	vertices.push_back(i);		vertices.push_back(j);

				  colors.push_back(0.0);		colors.push_back(1.0);		colors.push_back(0.0);	colors.push_back(1.0);
				  //glVertex3f(+size, i, j);	// glVertex3f(+0.5f, i, z);
				  vertices.push_back(+size);	vertices.push_back(i);		vertices.push_back(j);
				//glEnd();
			}
	}

	draw_GL_LINES_VAO(vertices, colors);
}


// Fons: Dibuixa el fons variable sefons el color int_fons
void Fons(CColor int_fons)
{
	glClearColor(int_fons.r,int_fons.g,int_fons.b,1.f);
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(0.f,0.f,0.f,1.f);
	glClear(GL_DEPTH_BUFFER_BIT);
   glFlush();
}

// FonsN: Dibuixa el fons negre
void FonsN()
{
	glClearColor(0.f,0.f,0.f,1.f);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
   glFlush();
}

// FonsB: Dibuixa el fons blanc
void FonsB()
{
	glClearColor(1.f,1.f,1.f,1.f);
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(0.f,0.f,0.f,1.f);
	glClear(GL_DEPTH_BUFFER_BIT);
   glFlush();
}

// TEXTURES------------------------------------------------------
// loadIMA_SOIL: This function receives as input the image filename and an 
// integer identifier (0 for the first texture) and creates an OpenGL 
// texture which is stored in the global array 'textures'
// It uses SOIL library functions to load the image
// Paràmetres:
//		- filename: Fitxer que conté la imatge de qualsevol format:
//					BMP, JPG, TIFF, TGA, GIF, i d'altres suportats per SOIL
//		- Retorna: Identificador dins la taula textures on volem
//                assignar la imatge
GLint loadIMA_SOIL(const char * filename)
{	FILE *file = NULL;
	int errno;
	GLuint textureID = 0;

// Open the image file for reading
//  file=fopen(nomf,"r");					// Funció Visual Studio 6.0
	errno = fopen_s(&file, filename, "rb");		// Funció Visual 2010

// If the file is empty (or non existent) print an error and return false
// if (file == NULL)
	if (errno != 0)
	{	//	printf("Could not open file '%s'.\n",filename) ;
		return false;
	}

// Close the image file
	fclose(file);

//Generate texture ID and load texture data 
	glGenTextures(1, &textureID);
	int width, height;
	//unsigned char* image = SOIL_load_image(filename, &width, &height, 0, SOIL_LOAD_RGB);	// Imatge 3 canals (RGB)
	unsigned char* image = SOIL_load_image(filename, &width, &height, 0, SOIL_LOAD_RGBA);	// Imatge 4 canals (RGBA)

// Assign texture to ID
	glBindTexture(GL_TEXTURE_2D, textureID);

	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);	// Imatge 3 canals (RGB)
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);	// Imatge 4 canals (RGBA)

	glGenerateMipmap(GL_TEXTURE_2D);

// Parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	SOIL_free_image_data(image);

// If execution arrives here it means that all went well. Return true
	return textureID;
}

// ----------------- CUBEMAP TEXTURE
// loads a cubemap texture from 6 individual texture faces
// order:
// +X (right)
// -X (left)
// +Y (top)
// -Y (bottom)
// +Z (front) 
// -Z (back)
// -------------------------------------------------------
unsigned int loadCubemap(std::vector<std::string> faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height; // nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		//unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		unsigned char* data = SOIL_load_image(faces[i].c_str(), &width, &height, 0, SOIL_LOAD_RGB);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			//stbi_image_free(data);
			SOIL_free_image_data(data);
		}
		else
		{	//std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			fprintf(stderr, "Cubemap texture failed to load at path: %s \n", faces[i].c_str());
			//stbi_image_free(data);
			SOIL_free_image_data(data);
			return 0;
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}


// SetTextureParameters: Funció que configura paràmetres de l'objecte textura IDText 
// Paràmetres:
//		- num: Número de bloc de textura en el shader
//		- IDText: Identificador de l'objecte textura a configurar
//		- extensT: Extén la imatge textura fora dels límits normalitzats (0,1) [TRUE] o no [FALSE]
//		- filterT: Aplica filtre linieal a la imatge textura [TRUE] o pren píxel més proper [FALSE]
//		- blendT: Aplica la funció de blending tenint en compte luminància [TRUE] o només la intensitat de la imatge textura [FALSE]
//		- genMipmpT: Implementa mipmaps sobre la imatge textura [TRUE] o no [FALSE]
void SetTextureParameters(GLint num, GLint IDText, bool extensT, bool filterT, bool blendT, bool genMipmpT)
{
	glActiveTexture(GL_TEXTURE0 + num);		// Activació del bloc de texture en el shader (texture<num>).
	glBindTexture(GL_TEXTURE_2D, IDText);

// Funció de Blending de textura
	//if (blendT) glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	//else glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

// Extensió de textures
	if (extensT) {	// Configuració textura per a que es repereixi més enllà de coordenades (0,0)-(1,1)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}
	else {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	}

// Filtratge de textures
	if (filterT) {	// Filtratge de textures (GL_LINEAR)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		if (genMipmpT) glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		else glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}
	else {	// Filtratge de textures (GL_NEAREST)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		if (genMipmpT) glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		else glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}

// Activació de MipMaps de la imatge textura
	if (genMipmpT) {	// Activació MIPMAP en textures
		glGenerateMipmap(GL_TEXTURE_2D);
	}
}

