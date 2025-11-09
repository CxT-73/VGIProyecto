//******** PRACTICA VISUALITZACIÓ GRÀFICA INTERACTIVA (Escola Enginyeria - UAB)
//******** Entorn bàsic VS2022 MONOFINESTRA amb OpenGL 4.6, interfície GLFW 3.4, ImGui i llibreries GLM
//******** Ferran Poveda, Marc Vivet, Carme Julià, Débora Gil, Enric Martí (Setembre 2025)
// escena.cpp : Aqui es on ha d'anar el codi de les funcions que 
//              dibuixin les escenes.
//
//    Versió 2.0:	- Objectes Cub, Esfera, Tetera (primitives libreria GLUT)
//
//	  Versió 2.2:	- Objectes Cub, Esfera, Tetera definides en fitxer font glut_geometry amb altres primitives GLUT
//
//	  Versió 2.5:	- Objectes cubRGB i Tie (nau Star Wars fet per alumnes)
//

#include "stdafx.h"
#include "material.h"
#include "visualitzacio.h"
#include "escena.h"

Coche* miCoche = nullptr;
OBJ* cono = nullptr;
OBJ* circuit = nullptr;
OBJ* barrera = nullptr;
OBJ* bloc = nullptr;
OBJ* barril = nullptr;
// Dibuixa Eixos Coordenades Món i Reixes, activant un shader propi.
void dibuixa_Eixos(GLuint ax_programID, bool eix, GLuint axis_Id, CMask3D reixa, CPunt3D hreixa, 
	glm::mat4 MatriuProjeccio, glm::mat4 MatriuVista)
{
// Visualització Eixos Coordenades Mòn
	glUseProgram(ax_programID);

// Pas Matrius Projecció i Vista Vista a shader
	glUniformMatrix4fv(glGetUniformLocation(ax_programID, "projectionMatrix"), 1, GL_FALSE, &MatriuProjeccio[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(ax_programID, "viewMatrix"), 1, GL_FALSE, &MatriuVista[0][0]);

// Attribute Locations must be setup before calling glLinkProgram()
	glBindAttribLocation(ax_programID, 0, "in_Vertex"); // Vèrtexs
	glBindAttribLocation(ax_programID, 1, "in_Color");	// Color

//  Dibuix dels eixos
	if (eix) draw_Eixos(axis_Id);

// Dibuixa el grid actiu
	if ((reixa.x) || (reixa.y) || (reixa.z) || (reixa.w)) draw_Grid(reixa, hreixa);
}

// Dibuixa Skybox en forma de Cub, activant un shader propi.
void dibuixa_Skybox(GLuint sk_programID, GLuint cmTexture, char eix_Polar, glm::mat4 MatriuProjeccio, glm::mat4 MatriuVista)
{
	glm::mat4 ModelMatrix(1.0);

	glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content

// Activació shader per a cub skybox
	glUseProgram(sk_programID);

// Pas Matrius Projecció i Vista a shader
	glUniformMatrix4fv(glGetUniformLocation(sk_programID, "projectionMatrix"), 1, GL_FALSE, &MatriuProjeccio[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(sk_programID, "viewMatrix"), 1, GL_FALSE, &MatriuVista[0][0]);

// Rotar skyBox per a orientar sobre eix superior Z o X en Vista Esfèrica (POLARX, POLARY, POLARZ)
	if (eix_Polar == POLARZ) ModelMatrix = glm::rotate(ModelMatrix, radians(90.0f), vec3(1.0f, 0.0f, 0.0f));
	else if (eix_Polar == POLARX) ModelMatrix = glm::rotate(ModelMatrix, radians(-90.0f), vec3(0.0f, 0.0f, 1.0f));

// Escalar Cub Skybox a 5000 per encabir objectes escena a l'interior
	ModelMatrix = glm::scale(ModelMatrix, vec3(5000.0f, 5000.0f, 5000.0f));		//glScaled(5000.0, 5000.0, 5000.0);
	glUniformMatrix4fv(glGetUniformLocation(sk_programID, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);

// Activar textura cubemaps del Skybox per encabir objectes escena
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cmTexture);

// Attribute Locations must be setup before calling glLinkProgram()
	glBindAttribLocation(sk_programID, 0, "in_Vertex"); // Vèrtexs

//  Dibuix del Skybox
	drawCubeSkybox();

	glDepthFunc(GL_LESS); // set depth function back to default
}


// dibuixa_EscenaGL: Dibuix de l'escena amb comandes GL
void dibuixa_EscenaGL(GLuint sh_programID, bool eix, GLuint axis_Id, CMask3D reixa, CPunt3D hreixa, char objecte, 
			CColor col_object, bool sw_mat[5],
			bool textur, GLuint texturID[NUM_MAX_TEXTURES], bool textur_map, bool flagInvertY,
			int nptsU, CPunt3D PC_u[MAX_PATCH_CORBA], GLfloat pasCS, bool sw_PC, bool dib_TFrenet,
			COBJModel* objecteOBJ,
			glm::mat4 MatriuVista, glm::mat4 MatriuTG)
{
	float altfar = 0;
	GLint npunts = 0, nvertexs = 0;
	GLdouble tras[3] = { 0.0,0.0,0.0 }; //Sierpinski Sponge
	CColor color_vermell = { 0.0,0.0,0.0,1.0 }, color_Mar = { 0.0,0.0,0.0,0.0 };
	bool sw_material[5] = { 0.0,0.0,0.0,0.0,0.0 };
	
// Matrius de Transformació
	glm::mat4 NormalMatrix(1.0), ModelMatrix(1.0), TransMatrix(1.0), ScaleMatrix(1.0), RotMatrix(1.0);

// VAO
	CVAO objectVAO = { 0,0,0,0,0 };
	objectVAO.vaoId = 0;	objectVAO.vboId = 0;	objectVAO.eboId = 0;	 objectVAO.nVertexs = 0; objectVAO.nIndices = 0;

	tras[0] = 0.0;	tras[1] = 0.0; tras[2] = 0.0;
	color_vermell.r = 1.0;	color_vermell.g = 0.0; color_vermell.b = 0.0; color_vermell.a = 1.0;
	sw_material[0] = false;	sw_material[1] = true; sw_material[2] = true; sw_material[3] = false;	sw_material[4] = true;

// Shader Visualització Objectes
	glUseProgram(sh_programID);

// Parametrització i activació/desactivació de textures
	if (texturID[0] != 0) SetTextureParameters(0, texturID[0], true, true, textur_map, false);
	if (textur) {	glUniform1i(glGetUniformLocation(sh_programID, "textur"), GL_TRUE); //glEnable(GL_TEXTURE_2D);
					glUniform1i(glGetUniformLocation(sh_programID, "modulate"), GL_TRUE); //glEnable(GL_MODULATE);
				}
		else {	glUniform1i(glGetUniformLocation(sh_programID, "textur"), GL_FALSE); //glDisable(GL_TEXTURE_2D);
				glUniform1i(glGetUniformLocation(sh_programID, "modulate"), GL_FALSE); //glDisable(GL_MODULATE);
			}
	glUniform1i(glGetUniformLocation(sh_programID, "flag_invert_y"), flagInvertY);

// Attribute Locations must be setup before calling glLinkProgram()
	glBindAttribLocation(sh_programID, 0, "in_Vertex");		// Vèrtexs
	glBindAttribLocation(sh_programID, 1, "in_Color");		// Color
	glBindAttribLocation(sh_programID, 2, "in_Normal");		// Normals
	glBindAttribLocation(sh_programID, 3, "in_TexCoord");	// Textura

// Definició propietats de reflexió (emissió, ambient, difusa, especular) del material.
	SeleccionaColorMaterial(sh_programID, col_object, sw_mat);

	//Renderitzat del cotxe
	if (miCoche) {
		miCoche->render(sh_programID, MatriuVista);
	}
	if (cono)
		cono->render(sh_programID, MatriuVista, MatriuTG, col_object,sw_mat); 

	if (circuit) 
		circuit->render(sh_programID, MatriuVista, MatriuTG, col_object, sw_mat);

	if (barrera) 
		barrera->render(sh_programID, MatriuVista, MatriuTG, col_object, sw_mat);

	if (bloc) 
		bloc->render(sh_programID, MatriuVista, MatriuTG, col_object, sw_mat);

	if (barril) 
		barril->render(sh_programID, MatriuVista, MatriuTG, col_object, sw_mat);
	
// Enviar les comandes gràfiques a pantalla
//	glFlush();
}

// Mar amb ondulacions
CVAO loadSea_VAO(CColor colorM)
{
	int i, j, step, it1, it2;
	double angle, delay, Nx;
	glm::mat4 ModelMatrix(1.0), NormalMatrix(1.0);

// VAO
	GLuint vaoId = 0; GLuint vboId = 0;
	CVAO seaVAO = { 0,0,0,0,0 };
	seaVAO.vaoId = 0;	seaVAO.vboId = 0;	seaVAO.nVertexs = 0;	seaVAO.eboId = 0; seaVAO.nIndices = 0;
	std::vector <double> vertices, normals, colors, textures;	// Definició vectors dinàmics per a vertexs, normals i textures 
	vertices.resize(0);		normals.resize(0);		colors.resize(0);	textures.resize(0);			// Reinicialitzar vectors

// Aigua amb ondulacions simulades a partir de normals sinusoidals
	step = 10; //step = 5;
	delay = 0;
	it1 = 0;

//   SeleccionaMaterial(MAT_DEFAULT);	
	float h = 2 * PI*step / 500;
	for (j = -250; j<250 - step; j = j + step)
	{	delay = 1.0*h*it1;

		it2 = 0;
		//glColor3f(0.5f, 0.4f, 0.9f);
		for (i = -250; i<250 - step; i = i + step)
		{	//glBegin(GL_POLYGON);
			  angle = 1.0*it2*h * 15;
			  Nx = -cos(angle);
			  //glNormal3f(-cos(angle), 0, 1);
			  //glVertex3f(i, j, 0);			// Vèrtex P1
			  colors.push_back(colorM.r);		colors.push_back(colorM.g);		colors.push_back(colorM.b);		colors.push_back(colorM.a);  // Vector Colors
			  normals.push_back(Nx);			normals.push_back(0.0);			normals.push_back(1.0);			// Vector Normals
			  textures.push_back(0.0);			textures.push_back(0.0);										// Vector Textures
			  vertices.push_back(i);			vertices.push_back(j);			vertices.push_back(0.0);		// Vector Vertices

			  angle = 1.0*(it2 + 1.0)*h * 15;
			  Nx = -cos(angle);
			  //glNormal3f(-cos(angle), 0, 1);
			  //glVertex3f(i + step, j, 0);		// Vèrtex P2
			  colors.push_back(colorM.r);		colors.push_back(colorM.g);		colors.push_back(colorM.b);		colors.push_back(colorM.a);  // Vector Colors
			  normals.push_back(Nx);			normals.push_back(0.0);			normals.push_back(1.0);			// Vector Normals
			  textures.push_back(1.0);			textures.push_back(0.0);										// Vector Textures
			  vertices.push_back(i+step);		vertices.push_back(j);			vertices.push_back(0.0);		// Vector Vertices

			  angle = 1.0*(it2 + 1.0)*h * 15;
			  Nx = -cos(angle);
			  //glNormal3f(-cos(angle), 0, 1);
			  //glVertex3f(i + step, j + step, 0);// Vèrtex P3
			  colors.push_back(colorM.r);		colors.push_back(colorM.g);		colors.push_back(colorM.b);		colors.push_back(colorM.a);  // Vector Colors
			  normals.push_back(Nx);			normals.push_back(0.0);			normals.push_back(1.0);			// Vector Normals
			  textures.push_back(1.0);			textures.push_back(1.0);										// Vector Textures
			  vertices.push_back(i+step);		vertices.push_back(j+step);		vertices.push_back(0.0);		// Vector Vertices

			//glEnd();
// ----------------------- VAO
			//std::vector <int>::size_type nv = vertices.size();	// Tamany del vector vertices en elements.
			//draw_GL_TRIANGLES_VAO(vertices, normals, colors, textures);
			//vertices.resize(0);	normals.resize(0);	colors.resize(0);	textures.resize(0);	// Reinicialitzar vectors

			//glBegin(GL_POLYGON);
			  angle = 1.0*it2*h * 15;
			  //glNormal3f(-cos(angle), 0, 1);
			  //glVertex3f(i, j, 0);			// Vèrtex P1
			  colors.push_back(colorM.r);		colors.push_back(colorM.g);		colors.push_back(colorM.b);		colors.push_back(colorM.a);  // Vector Colors
			  normals.push_back(-cos(angle));	normals.push_back(0.0);			normals.push_back(1.0);			// Vector Normals
			  textures.push_back(1.0);			textures.push_back(1.0);										// Vector Textures
			  vertices.push_back(i);			vertices.push_back(j);			vertices.push_back(0.0);		// Vector Vertices

			  angle = 1.0*(it2 + 1)*h * 15;
			  //glNormal3f(-cos(angle), 0, 1);
			  //glVertex3f(i + step, j + step, 0);// Vèrtex P2
			  colors.push_back(colorM.r);		colors.push_back(colorM.g);		colors.push_back(colorM.b);		colors.push_back(colorM.a);  // Vector Colors
			  normals.push_back(-cos(angle));	normals.push_back(0.0);			normals.push_back(1.0);			// Vector Normals
			  textures.push_back(1.0);			textures.push_back(1.0);										// Vector Textures
			  vertices.push_back(i + step);		vertices.push_back(j + step);	vertices.push_back(0.0);		// Vector Vertices

			  angle = 1.0*it2*h * 15;
			  //glNormal3f(-cos(angle), 0, 1);
			  //glVertex3f(i, j + step, 0);		// Vèrtex P3
			  colors.push_back(colorM.r);		colors.push_back(colorM.g);		colors.push_back(colorM.b);		colors.push_back(colorM.a);  // Vector Colors
			  normals.push_back(-cos(angle));	normals.push_back(0.0);			normals.push_back(1.0);			// Vector Normals
			  textures.push_back(0.0);			textures.push_back(1.0);										// Vector Textures
			  vertices.push_back(i);			vertices.push_back(j + step);		vertices.push_back(0.0);	// Vector Vertices

			//glEnd();

			it2++;
		}
		it1++;
	}
// ----------------------- VAO
	std::vector <int>::size_type nv = vertices.size();	// Tamany del vector vertices en elements.
	//draw_GL_TRIANGLES_VAO(vertices, normals, colors, textures);
	// 
// Creació d'un VAO i un VBO i càrrega de la geometria. Guardar identificador VAO identificador VBO a struct CVAO.
	seaVAO = load_TRIANGLES_VAO(vertices, normals, colors, textures);
	//seaVAO.vaoId = vaoId;
	//seaVAO.vboId = vboId;

	seaVAO.nVertexs = (int)nv / 3;
	return seaVAO;
}

