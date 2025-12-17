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
#include "zones.h"
#include "escena.h"
#include "colocarObjeto.h"
#include <btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionShapes/btTriangleMesh.h>
#include <BulletCollision/CollisionShapes/btBvhTriangleMeshShape.h>
#include "MenuController.h"
#include "EndGame.h"

std::vector<ObjetoSeguidor*> seguidores;
Coche* miCoche = nullptr;
OBJ* cono = nullptr;
OBJ* cono_estatic = nullptr;
OBJ* circuit = nullptr;
OBJ* muro = nullptr;
OBJ* barrera = nullptr;
OBJ* bloc = nullptr;
OBJ* barril = nullptr;
OBJ* senyal1 = nullptr;
OBJ* senyal2 = nullptr;

Zones* zonas = nullptr;
// ===== BULLET: SISTEMA FÍSICO =====
btDiscreteDynamicsWorld* mundo = nullptr;
btBroadphaseInterface* broadphase = nullptr;
btDefaultCollisionConfiguration* collisionConfiguration = nullptr;
btCollisionDispatcher* dispatcher = nullptr;
btSequentialImpulseConstraintSolver* solver = nullptr;


float tiempoInvulnerabilidad = 5.0f;

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
	glm::mat4 MatriuVista, glm::mat4 MatriuTG, bool dibuixarCotxe)
{
	float altfar = 0;
	GLint npunts = 0, nvertexs = 0;
	int i, j, k;
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
	if (textur) {
		glUniform1i(glGetUniformLocation(sh_programID, "textur"), GL_TRUE); //glEnable(GL_TEXTURE_2D);
		glUniform1i(glGetUniformLocation(sh_programID, "modulate"), GL_TRUE); //glEnable(GL_MODULATE);
	}
	else {
		glUniform1i(glGetUniformLocation(sh_programID, "textur"), GL_FALSE); //glDisable(GL_TEXTURE_2D);
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

	switch (objecte)
	{

		// Arc
	case ARC:
		// Definició propietats de reflexió (emissió, ambient, difusa, especular) del material pel color de l'objecte.
		SeleccionaColorMaterial(sh_programID, col_object, sw_mat);
		arc(sh_programID, MatriuVista, MatriuTG, sw_mat);

		// Activar transparència
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// Dibuix geometria Mar
		color_Mar.r = 0.5;	color_Mar.g = 0.4; color_Mar.b = 0.9; color_Mar.a = 0.5;
		// Definició propietats de reflexió (emissió, ambient, difusa, especular) del material pel color de l'objecte.
		SeleccionaColorMaterial(sh_programID, color_Mar, sw_mat);
		// Pas ModelView Matrix a shader
		glUniformMatrix4fv(glGetUniformLocation(sh_programID, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
		NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
		// Pas NormalMatrix a shader
		glUniformMatrix4fv(glGetUniformLocation(sh_programID, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
		draw_TriVAO_Object(MAR_FRACTAL_VAO);

		// Desactivar transparència
		glDisable(GL_BLEND);
		break;

		// Dibuix de l'objecte TIE (Nau enemiga Star Wars)
	case TIE:
		tie(sh_programID, MatriuVista, MatriuTG, sw_mat);
		break;

		// Dibuix de l'objecte OBJ
	case OBJOBJ:
		ModelMatrix = MatriuTG;
		// Pas ModelView Matrix a shader
		glUniformMatrix4fv(glGetUniformLocation(sh_programID, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
		NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
		// Pas NormalMatrix a shader
		glUniformMatrix4fv(glGetUniformLocation(sh_programID, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);

		// Definir característiques material de cada punt
		SeleccionaColorMaterial(sh_programID, col_object, sw_mat);
		// Objecte OBJ: Dibuix de l'objecte OBJ amb textures amb varis VAO's, un per a cada material.
		objecteOBJ->draw_TriVAO_OBJ(sh_programID);	// Dibuixar VAO a pantalla
		break;

		// Corba Bezier
	case C_BEZIER:
		// Dibuixar Punts de Control
		if (sw_PC)
		{	// Definició propietats de reflexió (emissió, ambient, difusa, especular) del material pel color vermell.
			SeleccionaColorMaterial(sh_programID, color_vermell, sw_material);

			for (int i = 0; i < nptsU; i++)
			{	//glPushMatrix();
				  //glTranslatef(PC_u[i].x, PC_u[i].y, PC_u[i].z);
				ModelMatrix = glm::translate(MatriuTG, vec3(PC_u[i].x, PC_u[i].y, PC_u[i].z));
				// Pas ModelViewMatrix a shader
				glUniformMatrix4fv(glGetUniformLocation(sh_programID, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
				// Pas NormalMatrix a shader
				NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
				glUniformMatrix4fv(glGetUniformLocation(sh_programID, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
				draw_TriEBO_Object(GLU_SPHERE); //drawgluSphere(npunts);	//gluSphere(5.0, 20, 20);
				//glPopMatrix();
			}

		}
		// Definició propietats de reflexió (emissió, ambient, difusa, especular) del material pel color de l'objecte.
		SeleccionaColorMaterial(sh_programID, col_object, sw_mat);
		ModelMatrix = MatriuTG;
		// Pas ModelViewMatrix a shader
		glUniformMatrix4fv(glGetUniformLocation(sh_programID, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
		// Pas NormalMatrix a shader
		NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
		glUniformMatrix4fv(glGetUniformLocation(sh_programID, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
		draw_LinEBO_Object(CRV_BEZIER); //draw_LinVAO_Object(CRV_BEZIER); //draw_Bezier_Curve(nptsU, PC_u, pasCS, false);
		if (dib_TFrenet) draw_TFBezier_Curve(sh_programID, PC_u, nptsU, pasCS, false); // Dibuixar Triedre de Frenet
		break;

		// Corba Lemniscata
	case C_LEMNISCATA:
		// Definició propietats de reflexió (emissió, ambient, difusa, especular) del material pel color de l'objecte.
		SeleccionaColorMaterial(sh_programID, col_object, sw_mat);
		ModelMatrix = MatriuTG;
		// Pas ModelViewMatrix a shader
		glUniformMatrix4fv(glGetUniformLocation(sh_programID, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
		// Pas NormalMatrix a shader
		NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
		glUniformMatrix4fv(glGetUniformLocation(sh_programID, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
		draw_LinEBO_Object(CRV_LEMNISCATA3D); // draw_LinVAO_Object(CRV_LEMNISCATA3D); //draw_Lemniscata3D(800, pasCS*20.0);
		if (dib_TFrenet) draw_TFLemniscata3D(sh_programID, 800, pasCS * 20.0);
		break;

		// Corba Spline
	case C_BSPLINE:
		// Dibuixar Punts de Control
		if (sw_PC)
		{	// Definició propietats de reflexió (emissió, ambient, difusa, especular) del material pel color vermell.
			SeleccionaColorMaterial(sh_programID, color_vermell, sw_material);
			//objectVAO = loadgluSphere_VAO(5.0, 20, 20);
			for (int i = 0; i < nptsU; i++)
			{	//glPushMatrix();
				  //glTranslatef(PC_u[i].x, PC_u[i].y, PC_u[i].z);
				ModelMatrix = glm::translate(MatriuTG, vec3(PC_u[i].x, PC_u[i].y, PC_u[i].z));
				// Pas ModelViewMatrix a shader
				glUniformMatrix4fv(glGetUniformLocation(sh_programID, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
				// Pas NormalMatrix a shader
				NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
				glUniformMatrix4fv(glGetUniformLocation(sh_programID, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
				draw_TriEBO_Object(GLU_SPHERE); // drawgluSphere(npunts);	//gluSphere(5.0, 20, 20);
				//glPopMatrix();
			}

		}
		SeleccionaColorMaterial(sh_programID, col_object, sw_mat);
		ModelMatrix = MatriuTG;
		// Pas ModelViewMatrix a shader
		glUniformMatrix4fv(glGetUniformLocation(sh_programID, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
		// Pas NormalMatrix a shader
		NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
		glUniformMatrix4fv(glGetUniformLocation(sh_programID, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
		draw_LinEBO_Object(CRV_BSPLINE); //draw_LinVAO_Object(CRV_BSPLINE); //draw_BSpline_Curve(nptsU, PC_u, pasCS);
		if (dib_TFrenet) draw_TFBSpline_Curve(sh_programID, PC_u, nptsU, pasCS); // Dibuixar Triedre de Frenet
		break;

		// Corba Hermitte
	case C_HERMITTE:
		// Dibuixar Punts de Control
		if (sw_PC)
		{	// Definició propietats de reflexió (emissió, ambient, difusa, especular) del material pel color vermell.
			SeleccionaColorMaterial(sh_programID, color_vermell, sw_material);
			//objectVAO = loadgluSphere_VAO(5.0, 20, 20);
			for (int i = 0; i < nptsU; i++)
			{	//glPushMatrix();
				  //glTranslatef(PC_u[i].x, PC_u[i].y, PC_u[i].z);
				ModelMatrix = glm::translate(MatriuTG, vec3(PC_u[i].x, PC_u[i].y, PC_u[i].z));
				// Pas ModelViewMatrix a shader
				glUniformMatrix4fv(glGetUniformLocation(sh_programID, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
				// Pas NormalMatrix a shader
				NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
				glUniformMatrix4fv(glGetUniformLocation(sh_programID, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
				draw_TriEBO_Object(GLU_SPHERE); //gluSphere(5.0, 20, 20);
				//glPopMatrix();
			}
		}
		SeleccionaColorMaterial(sh_programID, col_object, sw_mat);
		ModelMatrix = MatriuTG;
		// Pas ModelViewMatrix a shader
		glUniformMatrix4fv(glGetUniformLocation(sh_programID, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
		// Pas NormalMatrix a shader
		NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
		glUniformMatrix4fv(glGetUniformLocation(sh_programID, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
		//draw_LinVAO_Object(CRV_HERMITTE); //draw_BSpline_Curve(nptsU, PC_u, pasCS);
		draw_LinEBO_Object(CRV_HERMITTE);
		if (dib_TFrenet) draw_TFHermitte_Curve(sh_programID, PC_u, nptsU, pasCS); // Dibuixar Triedre de Frenet
		break;

		// Corba Catmull-Rom
	case C_CATMULL_ROM:
		// Dibuixar Punts de Control
		if (sw_PC)
		{	// Definició propietats de reflexió (emissió, ambient, difusa, especular) del material pel color vermell.
			SeleccionaColorMaterial(sh_programID, color_vermell, sw_material);
			//objectVAO = loadgluSphere_VAO(5.0, 20, 20);
			for (int i = 0; i < nptsU; i++)
			{	//glPushMatrix();
				  //glTranslatef(PC_u[i].x, PC_u[i].y, PC_u[i].z);
				ModelMatrix = glm::translate(MatriuTG, vec3(PC_u[i].x, PC_u[i].y, PC_u[i].z));
				// Pas ModelViewMatrix a shader
				glUniformMatrix4fv(glGetUniformLocation(sh_programID, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
				// Pas NormalMatrix a shader
				NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
				glUniformMatrix4fv(glGetUniformLocation(sh_programID, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
				draw_TriEBO_Object(GLU_SPHERE); //gluSphere(5.0, 20, 20);
				//glPopMatrix();
			}
		}
		SeleccionaColorMaterial(sh_programID, col_object, sw_mat);
		ModelMatrix = MatriuTG;
		// Pas ModelViewMatrix a shader
		glUniformMatrix4fv(glGetUniformLocation(sh_programID, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
		// Pas NormalMatrix a shader
		NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
		glUniformMatrix4fv(glGetUniformLocation(sh_programID, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
		//draw_LinVAO_Object(CRV_CATMULL_ROM);
		draw_LinEBO_Object(CRV_CATMULL_ROM);
		if (dib_TFrenet) draw_TFCatmullRom_Curve(sh_programID, PC_u, nptsU, pasCS); // Dibuixar Triedre de Frenet
		break;

		// Matriu de Primitives SENSE pre-compilació prèvia en VBO (precompilació, draw i alliberació VBO en una funció)
	case MATRIUP:
		SeleccionaColorMaterial(sh_programID, col_object, sw_mat);
		for (i = 0; i < 10; i++)
			for (j = 0; j < 10; j++)
				for (k = 0; k < 10; k++)
				{	//glPushMatrix();
					  //glTranslated(i * 15.0, j * 15.0, k * 15.0);
					  //glScaled(5.0, 5.0, 5.0);
					TransMatrix = glm::translate(MatriuTG, vec3(i * 15.0f, j * 15.0f, k * 15.0));
					ModelMatrix = glm::scale(TransMatrix, vec3(5.0f, 5.0f, 5.0f));
					// Pas ModelView Matrix a shader
					glUniformMatrix4fv(glGetUniformLocation(sh_programID, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
					// Pas NormalMatrix a shader
					NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
					glUniformMatrix4fv(glGetUniformLocation(sh_programID, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
					glutSolidCube(1.0);
					//glutSolidTorus(1.0,2.0,20,20);
					//glutSolidOctahedron();
					//glutSolidTetrahedron();
					//glutSolidIcosahedron();
					//glutSolidRhombicDodecahedron();
					//glutSolidSierpinskiSponge(3,tras,1.0);
					//glutSolidTeapot(1.0);
					//gluCylinder(0.5,0.5,1.0,20,20);
					//gluSphere(1.0, 20, 20);
					//glPopMatrix();
				}
		for (i = 0; i < 10; i++)
			for (j = 0; j < 10; j++)
			{	//glPushMatrix();
				  //glTranslated(i * 15.0, j * 15.0, 160.0);
				  //glTranslated(i * 15.0, j * 15.0, 160.0);
				ModelMatrix = glm::translate(MatriuTG, vec3(i * 15.0f, j * 15.0f, 160.0f));
				// Pas ModelView Matrix a shader
				glUniformMatrix4fv(glGetUniformLocation(sh_programID, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
				// Pas NormalMatrix a shader
				NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
				glUniformMatrix4fv(glGetUniformLocation(sh_programID, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
				//glScaled(5.0, 5.0, 5.0);
				//glutSolidCube(1.0);
				glutSolidTorus(2.0, 3.0, 20, 20);
				//glutSolidOctahedron();
				//glutSolidTetrahedron();
				//glutSolidIcosahedron();
				//glutSolidRhombicDodecahedron();
				//glutSolidSierpinskiSponge(3,tras,1.0);
				//glutSolidTeapot(1.0);
				//gluCylinder(0.5,0.5,1.0,20,20);
				//gluSphere(1.0, 20, 20);
				//glPopMatrix();
			}
		// Dibuix una esfera
				//glPushMatrix();
				  //glTranslated(200.0, 200.0, 200.0);
				  //glScaled(5.0, 5.0, 5.0);
		TransMatrix = glm::translate(MatriuTG, vec3(200.0f, 200.0f, 200.0f));
		ModelMatrix = glm::scale(TransMatrix, vec3(5.0f, 5.0f, 5.0f));
		// Pas ModelView Matrix a shader
		glUniformMatrix4fv(glGetUniformLocation(sh_programID, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
		// Pas NormalMatrix a shader
		NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
		glUniformMatrix4fv(glGetUniformLocation(sh_programID, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
		gluSphere(1.0, 20, 20);
		//glPopMatrix();
		break;

		// Matriu de Primitives AMB pre-compilació prèvia en VBO 
		//	(separació en 3 funcions: *_VBO() per precompilació, draw*() per dibuix i deleteVBO() per alliberar VBO)
	case MATRIUP_VAO:
		SeleccionaColorMaterial(sh_programID, col_object, sw_mat);
		//glutSolidCube_VAO(1.0);		// Càrrega de la geometria del Cub al VAO

		for (i = 0; i < 10; i++)
			for (j = 0; j < 10; j++)
				for (k = 0; k < 10; k++)
				{	//glPushMatrix();
					  //glTranslated(i * 15.0, j * 15.0, k * 15.0);
					  //glScaled(5.0, 5.0, 5.0);
					TransMatrix = glm::translate(MatriuTG, vec3(i * 15.0f, j * 15.0f, k * 15.0f));
					ModelMatrix = glm::scale(TransMatrix, vec3(5.0f, 5.0f, 5.0f));
					// Pas ModelView Matrix a shader
					glUniformMatrix4fv(glGetUniformLocation(sh_programID, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
					// Pas NormalMatrix a shader
					NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
					glUniformMatrix4fv(glGetUniformLocation(sh_programID, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);

					draw_TriEBO_Object(GLUT_CUBE); //drawSolidCube();	// Dibuix del cub dins VBO.

				}
		//deleteVAO(GLUT_CUBE);	// Eliminació del VAO cub.

		//nvertexs = glutSolidTorus_VAO(2.0, 3.0, 20, 20);	// Càrrega de la geometria del Torus al VAO
		for (i = 0; i < 10; i++)
			for (j = 0; j < 10; j++)
			{	//glPushMatrix();
				  //glTranslated(i * 15.0, j * 15.0, 160.0);
				  //glTranslated(i * 15.0, j * 15.0, 160.0);
				ModelMatrix = glm::translate(MatriuTG, vec3(i * 15.0f, j * 15.0f, 160.0f));
				// Pas ModelView Matrix a shader
				glUniformMatrix4fv(glGetUniformLocation(sh_programID, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
				// Pas NormalMatrix a shader
				NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
				glUniformMatrix4fv(glGetUniformLocation(sh_programID, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
				draw_TriEBO_Object(GLUT_TORUS); //drawSolidTorus(nvertexs);
				//glPopMatrix();

			}
		//deleteVAO(GLUT_CUBE);		// Eliminació del VAO cub.
		//deleteVAO(GLUT_TORUS);    // Eliminació del VAO Torus.
// Dibuix una esfera
		//glPushMatrix();
		  //glTranslated(200.0, 200.0, 200.0);
		  //glScaled(5.0, 5.0, 5.0);
		TransMatrix = glm::translate(MatriuTG, vec3(200.0f, 200.0f, 200.0f));
		ModelMatrix = glm::scale(TransMatrix, vec3(5.0f, 5.0f, 5.0f));
		// Pas ModelView Matrix a shader
		glUniformMatrix4fv(glGetUniformLocation(sh_programID, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
		NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
		// Pas NormalMatrix a shader
		glUniformMatrix4fv(glGetUniformLocation(sh_programID, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
		draw_TriEBO_Object(GLU_SPHERE); //gluSphere(1.0, 20, 20);
		//glPopMatrix();
		break;

		// Dibuix de la resta d'objectes
	default:
		// Definició propietats de reflexió (emissió, ambient, difusa, especular) del material.
		SeleccionaColorMaterial(sh_programID, col_object, sw_mat);
		dibuixa(sh_programID, objecte, MatriuVista, MatriuTG);
		break;
	}
	//Renderitzat del cotxe
	if (dibuixarCotxe) {
		if (miCoche) {

			miCoche->render(sh_programID, MatriuVista);
		}
	}

	//offset
	//eje X ? izquierda / derecha.
	//eje Y ? adelante / atrás.
	//eje Z ? arriba / abajo.
	 

	if (circuit)
		circuit->render(sh_programID, MatriuVista, MatriuTG, col_object, sw_mat);
	if (muro)
		muro->render(sh_programID, MatriuVista, MatriuTG, col_object, sw_mat);
	renderEscenaDuplicados(sh_programID, MatriuVista, MatriuTG, col_object, sw_mat);



	 
// Enviar les comandes gràfiques a pantalla
//	glFlush();
}
 

void initEscenaDuplicados()
{
	// CONO
	if (cono) {
		ObjetoSeguidor* seg = new ObjetoSeguidor(cono, zonas, 0, mundo);
		std::map<int, std::pair<int, std::vector<glm::vec3>>> zonasConfig = {
			{1, {3, {  {-605.0f, 170.0f, 210.0f},{-105.0f, -50.0f, 210.0f}, {120.0f, -140.0f, 210.0f} }}}, //fet
			{2, {2, { {0.0f, 100.0f, 260.0f}, {-100.0f, -240.0f, 260.0f} }} }, //fet
			{3, {2, { {-105.0f, -200.0f, 260.0f}, {120.0f, -140.0f, 260.0f} }}},//fet
			{4, {1, { {0.0f, 90.0f, 240.0f} }}},//fet
			{5, {3, { {-200.0f, -200.0f, 350.0f}, {-60.0f, -280.0f, 350.0f}, {180.0f, -200.0f, 350.0f} }}}, //fet
			{6, {3, { {-105.0f, -200.0f, 260.0f},{0.0f, 50.0f, 260.0f}, {140.0f, -140.0f, 260.0f} }} }, //fet
			{7, {2, { {-255.0f, -200.0f, 220.0f}, {50.0f, -370.0f, 200.0f} }} }, //fet
			{8, {1, { {-235.0f, 50.0f, 230.0f}  }} } //fet
		};
		std::map<int, std::vector<int>> invisiblesPorZona;// = { {3, {0, 1}} };
		seg->crearDuplicados(zonasConfig, invisiblesPorZona, glm::vec3(0.0f), glm::vec3(10.0f));
		seguidores.push_back(seg);
	}
	//CONO_ESTATIC
	if (cono_estatic) {
		ObjetoSeguidor* seg = new ObjetoSeguidor(cono_estatic, zonas, 0, mundo);
		std::map<int, std::pair<int, std::vector<glm::vec3>>> zonasConfig = {
			{0, {101, { {-65.0f, -90.0f, 140.0f}, {-147.5f, -71.25f, 140.0f},{-142.5f, -70.0f, 140.0f}, {-140.0f, -65.0f, 140.0f},{-137.5f, -60.0f, 140.0f},  {-135.0f, -55.0f, 140.0f},{-132.5f, -50.0f, 140.0f} , {-129.0f, -46.5f, 140.0f},
					  {-125.0f, -47.0f, 140.0f},{-120.0f, -50.0f, 140.0f},{-115.0f, -55.0f, 140.0f},{-110.0f, -60.0f, 140.0f},{-105.0f, -65.0f, 140.0f},{-102.5f, -70.0f, 140.0f},
					  {-96.25f, -98.25f, 140.0f},{-100.0f, -95.0f, 140.0f},{-102.5f, -90.0f, 140.0f},{-105.0f, -85.0f, 140.0f}, {-107.5f, -80.0f, 140.0f},{-105.0f, -75.0f, 140.0f},
					  {-92.5f, -100.0f, 140.0f},{-87.5f, -100.0f, 140.0f},{-82.5f, -97.5f, 140.0f},{-77.5f, -95.0f, 140.0f},{-72.5f, -92.5f, 140.0f},{-67.5f, -90.0f, 140.0f},
					  {-62.5f, -90.0f, 140.0f},{-57.5f, -92.75f, 140.0f},{-52.5f, -95.0f, 140.0f},{-47.5f, -97.5f, 140.0f},{-42.5f, -100.0f, 140.0f},{-37.5f, -102.5f, 140.0f},{-32.5f, -105.0f, 140.0f},{-27.5f, -107.5f, 140.0f},{-22.5f, -110.0f, 140.0f},
					  {-17.5f, -110.0f, 140.0f},{-15.0f, -115.0f, 140.0f},{-17.5f, -120.0f, 140.0f},{-20.0f, -125.0f, 140.0f},{-22.5f, -130.0f, 140.0f},{-26.25f, -135.0f, 140.0f},{-28.75f, -139.0f, 140.0f},


					  {-0.0f, -105.0f, 140.0f},{-2.5f, -110.0f, 140.0f},{-5.0f, -115.0f, 140.0f},{-7.5f, -120.0f, 140.0f},{-10.0f, -125.0f, 140.0f},{-12.5f, -130.0f, 140.0f},{-15.5f, -135.0f, 140.0f},{-18.5f, -140.0f, 140.0f},{-21.0f, -144.0f, 140.0f},{-23.5f, -141.5f, 140.0f},
					  {-57.5f, -72.0f, 140.0f},{-52.5f, -75.75f, 140.0f},{-47.5f, -77.5f, 140.0f},{-42.5f, -80.0f, 140.0f},{-37.5f, -82.5f, 140.0f},{-32.5f, -85.0f, 140.0f},{-27.5f, -87.5f, 140.0f},{-22.5f, -90.0f, 140.0f},{-17.5f, -92.5f, 140.0f},{-12.5f, -95.0f, 140.0f},{-7.5f, -97.5f, 140.0f},{-2.5f, -100.0f, 140.0f},
					  {-82.5f, -82.0f, 140.0f},{-77.5f, -79.5f, 140.0f},{-72.5f, -77.0f, 140.0f},{-67.5f, -74.5f, 140.0f},{-62.5f, -72.0f, 140.0f},
					  {-85.75f, -80.25f, 140.0f},{-89.5f, -77.0f, 140.0f}, {-87.0f, -72.0f, 140.0f},{-84.5f, -67.0f, 140.0f},{-89.5f, -62.0f, 140.0f}, {-94.5f, -57.0f, 140.0f},{-99.5f, -52.0f, 140.0f}, {-104.5f, -47.0f, 140.0f},{-109.5f, -42.0f, 140.0f},{-114.5f, -37.0f, 140.0f},
					  {-119.5f, -34.5f, 140.0f},{-124.5f, -32.0f, 140.0f},{-129.5f, -29.5f, 140.0f},{-134.5f, -29.5f, 140.0f},
					  {-136.0f, -34.5f, 140.0f},{-138.5f, -39.5f, 140.0f}, {-141.0f, -44.5f, 140.0f},{-143.5f, -49.5f, 140.0f},
					  {-160.0f, -57.5f, 140.0f},{-157.5f, -62.5f, 140.0f},{-155.0f, -67.5f, 140.0f}, {-150.0f, -70.0f, 140.0f},
					  {-157.5f, -52.5f, 140.0f}, {-155.0f, -47.5f, 140.0f}, {-152.5f, -42.5f, 140.0f}, {-150.0f, -37.5f, 140.0f}, {-147.5f, -32.5f, 140.0f},


					  {-127.5f, -15.0f, 140.0f},{-125.0f, -10.0f, 140.0f},{-122.5f, -5.0f, 140.0f},{-120.0f, 0.0f, 140.0f},{-117.5f, 5.0f, 140.0f},
		}}}
		};
		std::map<int, std::vector<int>> invisiblesPorZona = { {0, {51}} };
		seg->crearDuplicados(zonasConfig, invisiblesPorZona, glm::vec3(0.0f), glm::vec3(10.0f));
		seguidores.push_back(seg);
	}
	// BARRERA
	if (barrera) {
		ObjetoSeguidor* seg = new ObjetoSeguidor(barrera, zonas, 0, mundo);
		std::map<int, std::pair<int, std::vector<glm::vec3>>> zonasConfig = {
			{1, {1, { {  0.0f,   -90.0f, 230.0f} }}}, //fet 
			{2, {2, { {-270.0f, 30.0f, 260.0f}, {80.0f, -10.0f, 260.0f} }} }, //fet
			{3, {1, { {-65.0f, -190.0f, 260.0f} }}}, //fet
			{4, {1, { {-15.0f, -300.0f, 240.0f} }}}, //fet
			{5, {3, { {-240.0f, -50.0f, 300.0f}, {-60.0f, -280.0f, 350.0f}, {270.0f, -130.0f, 350.0f} }}},  //fet
			{6, {2, { {30.0f, 50.0f, 260.0f}, {140.0f, -140.0f, 260.0f} }} },  //fet
			{7, {1, {  {-80.0f, -370.0f, 250.0f} }} },  //fet
			{8, {1, { {0.0f, 120.0f, 230.0f}  }} } //fet

		};
		std::map<int, std::vector<int>> invisibles;// { { 3, { 0 } } };
		seg->crearDuplicados(zonasConfig, invisibles, glm::vec3(0.0f), glm::vec3(10.0f));
		seguidores.push_back(seg);
	}

	// BLOC
	if (bloc) {
		ObjetoSeguidor* seg = new ObjetoSeguidor(bloc, zonas, 0, mundo);
		std::map<int, std::pair<int, std::vector<glm::vec3>>> zonasConfig = {
			{1, {1, { {  0.0f,   -90.0f, 230.0f} }}}, //fet
			{2, {1, { {80.0f, -10.0f, 260.0f} }} }, //fet
			{3, {2, { {125.0f, -140.0f, 260.0f} ,{-220.0f, -100.0f, 260.0f} }}}, //fet
			{4, {2, { {40.0f,   -85.0f, 300.0f}, {30.0f, -90.0f, 240.0f} }}}, //fet
			{5, {2, { {-55.0f, -270.0f, 360.0f}, {-150.0f, -245.0f, 360.0f} }}}, //fet
			{6, {2, { {30.0f, 50.0f, 300.0f}, {140.0f, -140.0f, 300.0f} }} }, //fet
			{7, {2, { {-243.0f, -190.0f, 270.0f}, {50.0f, -370.0f, 200.0f} }} }, //fet
			{8, {1, { {-204.5f, 60.0f, 230.0f}   }} }

		};
		std::map<int, std::vector<int>> invisibles;
		seg->crearDuplicados(zonasConfig, invisibles, glm::vec3(0.0f), glm::vec3(10.0f));
		seguidores.push_back(seg);
	}

	// BARRIL
	if (barril) {
		ObjetoSeguidor* seg = new ObjetoSeguidor(barril, zonas, 0, mundo);
		std::map<int, std::pair<int, std::vector<glm::vec3>>> zonasConfig = {
			{1, {3, {   {-605.0f, 170.0f, 210.0f},{-205.0f, 10.0f, 210.0f}, {  0.0f,   -90.0f, 230.0f} }}}, //fet
			{2, {2, { {-270.0f, 30.0f, 260.0f}, {70.0f, -100.0f, 260.0f} }} }, //fet
			{3, {1, { {-220.0f, -100.0f, 260.0f} }}}, //fet
			{4, {2, { {-130.0f, -480.0f, 248.0f}, {40.0f, -110.0f, 260.0f} }}}, //fet
			{5, {2, { {-240.0f, -50.0f, 300.0f}, {270.0f, -130.0f, 350.0f} }}}, //fet
			{6, {1, { {80.0f, 10.0f, 200.0f} }}}, //fet
			{7, {1, {  {-100.0f, -350.0f, 250.0f} }} },   //fet
			{8, {1, { {0.0f, 120.0f, 230.0f}  }} }  //fet

		};
		std::map<int, std::vector<int>> invisibles;
		seg->crearDuplicados(zonasConfig, invisibles, glm::vec3(0.0f), glm::vec3(10.0f));
		seguidores.push_back(seg);
	}

	//GIRAR
	if (senyal1) {
		ObjetoSeguidor* seg = new ObjetoSeguidor(senyal1, zonas, 0, mundo);
		std::map<int, std::pair<int, std::vector<glm::vec3>>> zonasConfig = {
			{0, {1, { {-112.5f, 5.0f, 125.0f} }}} //fet

		};
		std::map<int, std::vector<int>> invisibles;
		seg->crearDuplicados(zonasConfig, invisibles, glm::vec3(0.0f), glm::vec3(10.0f));
		seguidores.push_back(seg);
	}

	// SPEED
	if (senyal2) {
		ObjetoSeguidor* seg = new ObjetoSeguidor(senyal2, zonas, 0, mundo);
		std::map<int, std::pair<int, std::vector<glm::vec3>>> zonasConfig = {
			{2, {1, { { 50.0f, -30.0f, 235.0f} }} }, //fet
			{3, {1, { {-190.0f, -170.0f, 254.0f} }}}, //fet 
			{4, {1, { {-80.0f, 235.0f, 200.0f} }}}, //fet
			{5, {1, { {60.0f, -245.0f, 333.0f} }}},  //fet
			{6, {2, { {70.0f, -12.0f, 197.0f}, {-160.0f, -140.0f, 185.0f} }}}, //fet
			{7, {1, {  {-160.0f, -330.0f, 230.0f} }} },   //fet
			{8, {1, { {-65.0f, 110.0f,135.0f}  }} } //fet
		};
		std::map<int, std::vector<int>> invisibles;
		seg->crearDuplicados(zonasConfig, invisibles, glm::vec3(0.0f), glm::vec3(10.0f));
		seguidores.push_back(seg);
	} 

}
void renderEscenaDuplicados(GLuint sh_programID,
	glm::mat4 MatriuVista,
	glm::mat4 MatriuTG,
	CColor col_object,
	bool sw_mat[5])
{
	for (ObjetoSeguidor* seg : seguidores)
		seg->renderDuplicados(sh_programID, MatriuVista, MatriuTG, col_object, sw_mat);
}


// dibuixa: Funció que dibuixa objectes simples de la llibreria GLUT segons obj
void dibuixa(GLuint sh_programID, char obj, glm::mat4 MatriuVista, glm::mat4 MatriuTG)
{
//	std::string String;
//	const char* string;
	glm::mat4 NormalMatrix(1.0), ModelMatrix(1.0); // TransMatrix(1.0); // ScaleMatrix(1.0), RotMatrix(1.0);
	GLdouble tras[3] = { 0.0,0.0,0.0 }; //Sierpinski Sponge
	
	tras[0] = 0.0;	tras[1] = 0.0; tras[2] = 0.0;

	switch(obj)
	{

// Cub
	case CUB:
		//glPushMatrix();
		  //glScaled(5.0,5.0,5.0);
		ModelMatrix = glm::scale(MatriuTG, vec3(5.0f, 5.0f, 5.0f));
		// Pas ModelView Matrix a shader
		glUniformMatrix4fv(glGetUniformLocation(sh_programID, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
		// Pas NormalMatrix a shader
		NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
		glUniformMatrix4fv(glGetUniformLocation(sh_programID, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
		draw_TriEBO_Object(GLUT_CUBE);	//draw_TriVAO_Object(GLUT_CUBE);  //glutSolidCube(1.0);
		//glPopMatrix();

		break;

	case CUB_RGB:
		//glPushMatrix();
		  //glScaled(5.0,5.0,5.0);
		ModelMatrix = glm::scale(MatriuTG, vec3(5.0f, 5.0f, 5.0f));
		// Pas ModelView Matrix a shader
		glUniformMatrix4fv(glGetUniformLocation(sh_programID, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
		// Pas NormalMatrix a shader
		NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
		glUniformMatrix4fv(glGetUniformLocation(sh_programID, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
		draw_TriEBO_Object(GLUT_CUBE_RGB);	//draw_TriVAO_Object(GLUT_CUBE_RGB); // glutSolidCubeRGB(1.0);
		//glPopMatrix();
		break;

// Esfera
	case ESFERA:	
		//glPushMatrix();
		  //glScaled(5.0,5.0,5.0);
		ModelMatrix = glm::scale(MatriuTG, vec3(5.0f, 5.0f, 5.0f));
		// Pas ModelView Matrix a shader
		glUniformMatrix4fv(glGetUniformLocation(sh_programID, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
		// Pas NormalMatrix a shader
		NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
		glUniformMatrix4fv(glGetUniformLocation(sh_programID, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
		draw_TriEBO_Object(GLU_SPHERE); //draw_TriVAO_Object(GLU_SPHERE); //gluSphere(1.0, 40, 40);
		//glPopMatrix();
		break;

// Tetera
	case TETERA:
		//glPushMatrix();
		  //glScaled(5.0,5.0,5.0);
		  ModelMatrix = MatriuTG;
		  // Pas ModelView Matrix a shader
		  glUniformMatrix4fv(glGetUniformLocation(sh_programID, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
		  // Pas NormalMatrix a shader
		  NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
		  glUniformMatrix4fv(glGetUniformLocation(sh_programID, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
		  draw_TriVAO_Object(GLUT_TEAPOT); // glutSolidTeapot(1.0);
		//glPopMatrix();
		break;
	}
}

// OBJECTE ARC
void arc(GLuint sh_programID, glm::mat4 MatriuVista, glm::mat4 MatriuTG, bool sw_mat[5])
{	CColor col_object = { 0.0,0.0,0.0,1.0 };
	glm::mat4 NormalMatrix(1.0), ModelMatrix(1.0);

// Definir VAO del Cub
	//glutSolidCube_VAO(1.0);

// Pota Esquerra
	//glPushMatrix();
	  //glTranslated(0.0, 0.0, 2.5);
	  //glScaled(1.0, 1.0, 5.0);
	ModelMatrix = glm::translate(MatriuTG, vec3(0.0f, 0.0f, 2.5f));
	ModelMatrix = glm::scale(ModelMatrix, vec3(1.0f, 1.0f, 5.0f));
	// Pas ModelView Matrix a shader
	glUniformMatrix4fv(glGetUniformLocation(sh_programID, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
	// Pas NormalMatrix a shader
	NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
	glUniformMatrix4fv(glGetUniformLocation(sh_programID, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
	draw_TriEBO_Object(GLUT_CUBE); // drawSolidCube();	//  glutSolidCube(1.0);
	//glPopMatrix();

// Pota Dreta
	//glPushMatrix();
	//glTranslated(0.0, 5.0, 2.5);
	//glScaled(1.0, 1.0, 5.0);
	ModelMatrix = glm::translate(MatriuTG, vec3(0.0f, 5.0f, 2.5f));
	ModelMatrix = glm::scale(ModelMatrix, vec3(1.0f, 1.0f, 5.0f));
	// Pas ModelView Matrix a shader
	glUniformMatrix4fv(glGetUniformLocation(sh_programID, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
	// Pas NormalMatrix a shader
	NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
	glUniformMatrix4fv(glGetUniformLocation(sh_programID, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
	draw_TriEBO_Object(GLUT_CUBE); //drawSolidCube();	//  glutSolidCube(1.0);
	//glPopMatrix();

// Travesser
	//glPushMatrix();
	  //glTranslated(0.0, 2.5, 5.5);
	  //glRotated(90.0, 1.0, 0.0, 0.0);
	  //glScaled(1.0, 1.0, 6.0);
	ModelMatrix = glm::translate(MatriuTG, vec3(0.0f, 2.5f, 5.5f));
	ModelMatrix = glm::rotate(ModelMatrix, radians(90.f),vec3(1.0f, 0.0f, 0.0f));
	ModelMatrix = glm::scale(ModelMatrix, vec3(1.0f, 1.0f, 6.0f));
	// Pas ModelView Matrix a shader
	glUniformMatrix4fv(glGetUniformLocation(sh_programID, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
	NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
	// Pas NormalMatrix a shader
	glUniformMatrix4fv(glGetUniformLocation(sh_programID, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
	draw_TriEBO_Object(GLUT_CUBE);  //drawSolidCube();	//  glutSolidCube(1.0);
	//glPopMatrix();

// Eliminar VAO del Cub
	//deleteVAO(GLUT_CUBE);

// Esfera
	col_object.r = 0.0;		col_object.g = 1.0;		col_object.b = 1.0;		col_object.a = 1.0;	// Color blau clar
	SeleccionaColorMaterial(sh_programID, col_object, sw_mat);
	//glPushMatrix();
	  //glTranslated(0.0, 5.0, 6.5);
    ModelMatrix = glm::translate(MatriuTG, vec3(0.0f, 5.0f, 6.5f));
	// Pas ModelView Matrix a shader
	glUniformMatrix4fv(glGetUniformLocation(sh_programID, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
	// Pas NormalMatrix a shader
	NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
	glUniformMatrix4fv(glGetUniformLocation(sh_programID, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
	draw_TriEBO_Object(GLU_SPHERE); //gluSphere(0.5, 20, 20);
	//glPopMatrix();

// Tetera
	col_object.r = 1.0;		col_object.g = 1.0;		col_object.b = 0.0;		col_object.a = 1.0;	// Color groc
	SeleccionaColorMaterial(sh_programID, col_object, sw_mat);
	//glPushMatrix();
	  //glTranslated(0.0, 0.0, 6.0);
	  //glRotated(-90.0, 0.0, 0.0, 1.0);
	  //glRotated(90.0, 1.0, 0.0, 0.0);
	  //glScaled(0.25, 0.25, 0.25);
	ModelMatrix = glm::translate(MatriuTG, vec3(0.0f, 0.0f, 6.0f));
	ModelMatrix = glm::rotate(ModelMatrix, radians(-90.f), vec3(0.0f, 0.0f, 1.0f));
	ModelMatrix = glm::rotate(ModelMatrix, radians(90.f), vec3(1.0f, 0.0f, 0.0f));
	ModelMatrix = glm::scale(ModelMatrix, vec3(0.25f, 0.25f, 0.25f));
	// Pas ModelView Matrix a shader
	glUniformMatrix4fv(glGetUniformLocation(sh_programID, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
	// Pas NormalMatrix a shader
	NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
	glUniformMatrix4fv(glGetUniformLocation(sh_programID, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
	draw_TriVAO_Object(GLUT_TEAPOT); //glutSolidTeapot(1.0);
	//glPopMatrix();
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


// OBJECTE TIE: FETS PER ALUMNES -----------------------------------------------------------------

// Objecte TIE
void tie(GLint shaderId, glm::mat4 MatriuVista, glm::mat4 MatriuTG, bool sw_mat[4])
{	Motor(shaderId,MatriuVista,MatriuTG, sw_mat);
	Alas(shaderId, MatriuVista, MatriuTG, sw_mat);
	Canon(shaderId, MatriuVista, MatriuTG, sw_mat);
	Cuerpo(shaderId, MatriuVista, MatriuTG, sw_mat);
	Cabina(shaderId, MatriuVista, MatriuTG, sw_mat);
};

void Alas(GLint shaderId, glm::mat4 MatriuVista, glm::mat4 MatriuTG, bool sw_mat[4])
{
// Matrius Transformació
	glm::mat4 TransMatrix(1.0), ModelMatrix(1.0), NormalMatrix(1.0);
	CColor col_object = { 0.0,0.0,0.0,1.0 };
	
	draw_TriEBO_Object(GLU_DISK); //int nvertD = gluDisk_VAO(0.0f, 5.0f, 6, 1);

// Ala 1 Parte central
//Parte Exterior
	//glPushMatrix();
	  //glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
// Definició propietats de reflexió (emissió, ambient, difusa, especular) del material.
	  col_object.r = 1.0;	col_object.g = 1.0;		col_object.b = 1.0;	 col_object.a = 1.0;
	  SeleccionaColorMaterial(shaderId, col_object, sw_mat);
	  //glTranslatef(25.25f, 0.0f, 0.0f);
	  //glRotated(90.0f, 0.0f, 1.0f, 0.0f);
	  //glScaled(10.0f, 10.0f, 5.0f);
	  TransMatrix = glm::translate(MatriuTG, vec3(25.25f, 0.0f, 0.0f));
	  TransMatrix = glm::rotate(TransMatrix,radians(90.f), vec3(0.0f, 1.0f, 0.0f));
	  ModelMatrix = glm::scale(TransMatrix, vec3(10.0f, 10.0f, 5.0f));
	  // Pas ModelView Matrix a shader
	  glUniformMatrix4fv(glGetUniformLocation(shaderId, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
	  NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
	  // Pas NormalMatrix a shader
	  glUniformMatrix4fv(glGetUniformLocation(shaderId, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
	  
	  draw_TriEBO_Object(GLU_CYLINDER);	//gluCylinder(5.0f, 5.0f, 0.5f, 6, 1);
	//glPopMatrix();

	//Tapas
	//glPushMatrix();
//	  glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	  //glTranslatef(25.25f, 0.0f, 0.0f);
	  //glRotated(90.0f, 0.0f, 1.0f, 0.0f);
	  //glScaled(10.0f, 10.0f, 5.0f);
	  TransMatrix = glm::translate(MatriuTG, vec3(25.25f, 0.0f, 0.0f));
	  TransMatrix = glm::rotate(TransMatrix, radians(90.f), vec3(0.0f, 1.0f, 0.0f));
	  ModelMatrix = glm::scale(TransMatrix, vec3(10.0f, 10.0f, 5.0f));
	  // Pas ModelView Matrix a shader
	  glUniformMatrix4fv(glGetUniformLocation(shaderId, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
	  NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
	  // Pas NormalMatrix a shader
	  glUniformMatrix4fv(glGetUniformLocation(shaderId, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
	  //gluDisk(0.0f, 5.0f, 6, 1);
	  draw_TriEBO_Object(GLU_DISK); // drawgluDisk(nvertD);
	//glPopMatrix();

	//glPushMatrix();
//	  glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	  //glTranslatef(27.75f, 0.0f, 0.0f);
	  //glRotated(90.0f, 0.0f, 1.0f, 0.0f);
	  //glScaled(10.0f, 10.0f, 5.0f);
	  TransMatrix = glm::translate(MatriuTG, vec3(27.75f, 0.0f, 0.0f));
	  TransMatrix = glm::rotate(TransMatrix, radians(90.f), vec3(0.0f, 1.0f, 0.0f));
	  ModelMatrix = glm::scale(TransMatrix, vec3(10.0f, 10.0f, 5.0f));
	  // Pas ModelView Matrix a shader
	  glUniformMatrix4fv(glGetUniformLocation(shaderId, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
	  NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
	  // Pas NormalMatrix a shader
	  glUniformMatrix4fv(glGetUniformLocation(shaderId, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
	  //gluDisk(0.0f, 5.0f, 6, 1);
	  draw_TriEBO_Object(GLU_DISK); // drawgluDisk(nvertD);
	//glPopMatrix();

//Parte Interior
	//glPushMatrix();
	  //glColor4f(0.0f, 0.0f, 0.0f, 0.0f);
	  SetColor4d(0.0, 0.0, 0.0, 1.0);
// Definició propietats de reflexió (emissió, ambient, difusa, especular) del material.
	  col_object.r = 0.0;	col_object.g = 0.0;		col_object.b = 0.0;	 col_object.a = 1.0;
	  SeleccionaColorMaterial(shaderId, col_object, sw_mat);
	  //glTranslated(25.25f, 0.0f, 0.0f);
	  //glRotated(90.0f, 0.0f, 1.0f, 0.0f);
	  //glScaled(2.0f, 2.0f, 2.0f);
	  TransMatrix = glm::translate(MatriuTG, vec3(25.25f, 0.0f, 0.0f));
	  TransMatrix = glm::rotate(TransMatrix, radians(90.f), vec3(0.0f, 1.0f, 0.0f));
	  ModelMatrix = glm::scale(TransMatrix, vec3(2.0f, 2.0f, 2.0f));
	  // Pas ModelView Matrix a shader
	  glUniformMatrix4fv(glGetUniformLocation(shaderId, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
	  NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
	  // Pas NormalMatrix a shader
	  glUniformMatrix4fv(glGetUniformLocation(shaderId, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
	  draw_TriEBO_Object(GLUT_USER1); // gluCylinder(5.0f, 5.0f, 2.0f, 6, 1);
	//glPopMatrix();

//Tapas
	//glPushMatrix();
	  //glColor4f(0.0f, 0.0f, 0.0f, 0.0f);
	  //glTranslatef(25.25f, 0.0f, 0.0f);
	  //glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
	  //glScalef(2.0f, 2.0f, 2.0f);
	  TransMatrix = glm::translate(MatriuTG, vec3(25.75f, 0.0f, 0.0f));
	  TransMatrix = glm::rotate(TransMatrix, radians(90.f), vec3(0.0f, 1.0f, 0.0f));
	  ModelMatrix = glm::scale(TransMatrix, vec3(2.0f, 2.0f, 2.0f));
	  // Pas ModelView Matrix a shader
	  glUniformMatrix4fv(glGetUniformLocation(shaderId, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
	  NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
	  // Pas NormalMatrix a shader
	  glUniformMatrix4fv(glGetUniformLocation(shaderId, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
	  //gluDisk(0.0f, 5.0f, 6, 1);
	  draw_TriEBO_Object(GLU_DISK); //drawgluDisk(nvertD);
	//glPopMatrix();

	//glPushMatrix();
	  //glColor4f(0.0f, 0.0f, 0.0f, 0.0f);
	  //glTranslatef(29.25f, 0.0f, 0.0f);
	  //glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
	  //glScalef(2.0f, 2.0f, 2.0f);
	  TransMatrix = glm::translate(MatriuTG, vec3(29.25f, 0.0f, 0.0f));
	  TransMatrix = glm::rotate(TransMatrix, radians(90.f), vec3(0.0f, 1.0f, 0.0f));
	  ModelMatrix = glm::scale(TransMatrix, vec3(2.0f, 2.0f, 2.0f));
	  // Pas ModelView Matrix a shader
	  glUniformMatrix4fv(glGetUniformLocation(shaderId, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
	  NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
	  // Pas NormalMatrix a shader
	  glUniformMatrix4fv(glGetUniformLocation(shaderId, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
	  //gluDisk(0.0f, 5.0f, 6, 1);
	  draw_TriEBO_Object(GLU_DISK); //drawgluDisk(nvertD);
	//glPopMatrix();

//Decoracion Exterior
	int j;
	//glPushMatrix();
	  //glColor4f(0.0f, 0.0f, 0.0f, 0.0f);
	  //glTranslatef(26.5f, 0.0f, 0.0f);
	  //glScalef(15.75f, 13.75f, 13.75f);
	  //glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
	  //glRotatef(30.0f, 0.0f, 0.0f, 1.0f);
	  TransMatrix = glm::translate(MatriuTG, vec3(26.5f, 0.0f, 0.0f));
	  TransMatrix = glm::scale(TransMatrix, vec3(15.75f, 13.75f, 13.75f));
	  TransMatrix = glm::rotate(TransMatrix, radians(90.f), vec3(0.0f, 1.0f, 0.0f));
	  TransMatrix = glm::rotate(TransMatrix, radians(30.f), vec3(0.0f, 0.0f, 1.0f));
	  for (j = 0; j<3; j = j + 1)
		{	//glRotatef(60, 0, 0, 1);
			TransMatrix = glm::rotate(TransMatrix, radians(60.f), vec3(0.0f, 0.0f, 1.0f));
			//glPushMatrix();
			   //glScalef(12.7f * 0.583f, 0.1f, 0.3f);
				ModelMatrix = glm::scale(TransMatrix, vec3(12.7f * 0.583f, 0.1f, 0.3f));
				// Pas ModelView Matrix a shader
				glUniformMatrix4fv(glGetUniformLocation(shaderId, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
				NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
				// Pas NormalMatrix a shader
				glUniformMatrix4fv(glGetUniformLocation(shaderId, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
				draw_TriEBO_Object(GLUT_CUBE); //glutSolidCube(1.0f);
			//glPopMatrix();
		}
	//glPopMatrix();

	int u;
	//glPushMatrix();
	//glColor4f(0.0f, 0.0f, 0.0f, 0.0f);
	TransMatrix = MatriuTG;
	for (u = 0; u < 6; u = u + 1)
	{	//glRotated(60, 1.0, 0.0, 0.0);
		TransMatrix = glm::rotate(TransMatrix, radians(60.f), vec3(1.0f, 0.0f, 0.0f));
		//glPushMatrix();
			//glTranslatef(26.5f, 0.0f, 43.25f);
			//glRotatef(90, 0.0, 0.0, 1.0);
			//glScalef(10 * 5.155f, 5.0f, 3.0f);
			ModelMatrix = glm::translate(TransMatrix, vec3(26.5f, 0.0f, 43.25f));
			ModelMatrix = glm::rotate(ModelMatrix, radians(90.f), vec3(0.0f, 0.0f, 1.0f));
			ModelMatrix = glm::scale(ModelMatrix, vec3(10 * 5.155f, 5.0f, 3.0f));
			// Pas ModelView Matrix a shader
			glUniformMatrix4fv(glGetUniformLocation(shaderId, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
			NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
			// Pas NormalMatrix a shader
			glUniformMatrix4fv(glGetUniformLocation(shaderId, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
			draw_TriEBO_Object(GLUT_CUBE); //glutSolidCube(1.0f);
		//glPopMatrix();
	}
	//glPopMatrix();


//Ala 2 Parte central 
//Parte Exterior
	//glPushMatrix();
	  //glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	  SetColor4d(1.0, 1.0, 1.0, 1.0);
// Definició propietats de reflexió (emissió, ambient, difusa, especular) del material.
	  col_object.r = 1.0;	col_object.g = 1.0;		col_object.b = 1.0;	 col_object.a = 1.0;
	  SeleccionaColorMaterial(shaderId, col_object, sw_mat);
	  //glTranslated(-27.75f, 0.0f, 0.0f);
	  //glRotated(90.0f, 0.0f, 1.0f, 0.0f);
	  //glScaled(10.0, 10.0, 5.0);
	  TransMatrix = glm::translate(MatriuTG, vec3(-27.75f, 0.0f, 0.0f));
	  TransMatrix = glm::rotate(TransMatrix, radians(90.f), vec3(0.0f, 1.0f, 0.0f));
	  ModelMatrix = glm::scale(TransMatrix, vec3(10.0f, 10.0f, 5.0f));
	  // Pas ModelView Matrix a shader
	  glUniformMatrix4fv(glGetUniformLocation(shaderId, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
	  NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
	  // Pas NormalMatrix a shader
	  glUniformMatrix4fv(glGetUniformLocation(shaderId, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
	  draw_TriEBO_Object(GLU_CYLINDER); //gluCylinder(5.0f, 5.0f, 0.5f, 6, 1);
	//glPopMatrix();

//Tapas
	//glPushMatrix();
	  //glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	  //glTranslatef(-27.75f, 0.0f, 0.0f);
	  //glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
	  //glScalef(10.0f, 10.0f, 5.0f);
	  TransMatrix = glm::translate(MatriuTG, vec3(-27.75f, 0.0f, 0.0f));
	  TransMatrix = glm::rotate(TransMatrix, radians(90.f), vec3(0.0f, 1.0f, 0.0f));
	  ModelMatrix = glm::scale(TransMatrix, vec3(10.0f, 10.0f, 5.0f));
	  // Pas ModelView Matrix a shader
	  glUniformMatrix4fv(glGetUniformLocation(shaderId, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
	  NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
	  // Pas NormalMatrix a shader
	  glUniformMatrix4fv(glGetUniformLocation(shaderId, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
	  //gluDisk(0.0f, 5.0f, 6, 1);
	  draw_TriEBO_Object(GLU_DISK); //drawgluDisk(nvertD);
	//glPopMatrix();

	//glPushMatrix();
	  //glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	  //glTranslatef(-25.25f, 0.0f, 0.0f);
	  //glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
	  //glScalef(10.0f, 10.0f, 5.0f);
	  TransMatrix = glm::translate(MatriuTG, vec3(-25.25f, 0.0f, 0.0f));
	  TransMatrix = glm::rotate(TransMatrix, radians(90.f), vec3(0.0f, 1.0f, 0.0f));
	  ModelMatrix = glm::scale(TransMatrix, vec3(10.0f, 10.0f, 5.0f));
	  // Pas ModelView Matrix a shader
	  glUniformMatrix4fv(glGetUniformLocation(shaderId, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
	  NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
	  // Pas NormalMatrix a shader
	  glUniformMatrix4fv(glGetUniformLocation(shaderId, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
	  //gluDisk(0.0f, 5.0f, 6, 1);
	  draw_TriEBO_Object(GLU_DISK); //drawgluDisk(nvertD);
	//glPopMatrix();

//Parte Interior
	//glPushMatrix();
	  //glColor4f(0.0f, 0.0f, 0.0f, 0.0f);
	  // Definició propietats de reflexió (emissió, ambient, difusa, especular) del material.
	  col_object.r = 0.0;	col_object.g = 0.0;		col_object.b = 0.0;	 col_object.a = 1.0;
	  SeleccionaColorMaterial(shaderId, col_object, sw_mat);
	  //glTranslatef(-29.25f, 0.0f, 0.0f);
	  //glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
	  //glScalef(2.0f, 2.0f, 2.0f);
	  TransMatrix = glm::translate(MatriuTG, vec3(-29.25f, 0.0f, 0.0f));
	  TransMatrix = glm::rotate(TransMatrix, radians(90.f), vec3(0.0f, 1.0f, 0.0f));
	  ModelMatrix = glm::scale(TransMatrix, vec3(2.0f, 2.0f, 2.0f));
	  // Pas ModelView Matrix a shader
	  glUniformMatrix4fv(glGetUniformLocation(shaderId, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
	  NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
	  // Pas NormalMatrix a shader
	  glUniformMatrix4fv(glGetUniformLocation(shaderId, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
	  draw_TriEBO_Object(GLUT_USER1); // gluCylinder(5.0f, 5.0f, 2.0f, 6, 1);
	//glPopMatrix();

//Tapas
	//glPushMatrix();
	  //glColor4f(0.0f, 0.0f, 0.0f, 0.0f);
 	  //glTranslatef(-29.25f, 0.0f, 0.0f);
	  //glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
	  //glScalef(2.0f, 2.0f, 2.0f);
	  TransMatrix = glm::translate(MatriuTG, vec3(-29.25f, 0.0f, 0.0f));
	  TransMatrix = glm::rotate(TransMatrix, radians(90.f), vec3(0.0f, 1.0f, 0.0f));
	  ModelMatrix = glm::scale(TransMatrix, vec3(2.0f, 2.0f, 2.0f));
	  // Pas ModelView Matrix a shader
	  glUniformMatrix4fv(glGetUniformLocation(shaderId, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
	  NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
	  // Pas NormalMatrix a shader
	  glUniformMatrix4fv(glGetUniformLocation(shaderId, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
	  //gluDisk(0.0f, 5.0f, 6, 1);
	  draw_TriEBO_Object(GLU_DISK);  //drawgluDisk(nvertD);
	//glPopMatrix();

	//glPushMatrix();
	  //glColor4f(0.0f, 0.0f, 0.0f, 0.0f);
	  //SetColor4d(0.0, 0.0, 0.0, 0.0);
	  //glTranslatef(-25.25f, 0.0f, 0.0f);
	  //glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
	  //glScalef(2.0f, 2.0f, 2.0f);
	  TransMatrix = glm::translate(MatriuTG, vec3(-25.25f, 0.0f, 0.0f));
	  TransMatrix = glm::rotate(TransMatrix, radians(90.f), vec3(0.0f, 1.0f, 0.0f));
	  ModelMatrix = glm::scale(TransMatrix, vec3(2.0f, 2.0f, 2.0f));
	  // Pas ModelView Matrix a shader
	  glUniformMatrix4fv(glGetUniformLocation(shaderId, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
	  NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
	  // Pas NormalMatrix a shader
	  glUniformMatrix4fv(glGetUniformLocation(shaderId, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
	  //gluDisk(0.0f, 5.0f, 6, 1);
	  draw_TriEBO_Object(GLU_DISK); //drawgluDisk(nvertD);
	//glPopMatrix();

//Decoracion Exterior
	int w;
	//glPushMatrix();
	  //glColor4f(0.0f, 0.0f, 0.0f, 0.0f);
	  //SetColor4d(0.0, 0.0, 0.0, 0.0);
	  //glTranslatef(-26.5f, 0.0f, 0.0f);
	  //glScalef(15.75f, 13.75f, 13.75f);
	  //glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
	  //glRotatef(30.0f, 0.0f, 0.0f, 1.0f);
	  TransMatrix = glm::translate(MatriuTG, vec3(-26.5f, 0.0f, 0.0f));
	  TransMatrix = glm::scale(TransMatrix, vec3(15.75f, 13.75f, 13.75f));
	  TransMatrix = glm::rotate(TransMatrix, radians(90.f), vec3(0.0f, 1.0f, 0.0f));
	  TransMatrix = glm::rotate(TransMatrix, radians(30.f), vec3(0.0f, 0.0f, 1.0f));
	  for (w = 0; w<3; w = w + 1)
	  {	//glRotatef(60.0f, 0.0f, 0.0f, 1.0f);
		TransMatrix = glm::rotate(TransMatrix, radians(60.f), vec3(0.0f, 0.0f, 1.0f));
		//glPushMatrix();
			//glScalef(12.7f * 0.583f, 0.1f, 0.3f);
			ModelMatrix = glm::scale(TransMatrix, vec3(12.7f * 0.583f, 0.1f, 0.3f));
			// Pas ModelView Matrix a shader
			glUniformMatrix4fv(glGetUniformLocation(shaderId, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
			NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
			// Pas NormalMatrix a shader
			glUniformMatrix4fv(glGetUniformLocation(shaderId, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
			draw_TriEBO_Object(GLUT_CUBE); //glutSolidCube(1.0f);
		//glPopMatrix();
	}
	//glPopMatrix();

	int h;
	//glPushMatrix();
	  //glColor4f(0.0f, 0.0f, 0.0f, 0.0f);
	  //SetColor4d(0.0, 0.0, 0.0, 0.0);
	  TransMatrix = MatriuTG;
	  for (h = 0; h < 6; h = h + 1)
	  {	//glRotated(60, 1.0, 0.0, 0.0);
		TransMatrix = glm::rotate(TransMatrix, radians(60.f), vec3(1.0f, 0.0f, 0.0f));
		//glPushMatrix();
			//glTranslatef(-26.5f, 0.0f, 43.25f);
			//glRotatef(90.0f, 0.0f, 0.0f, 1.0f);
			//glScalef(10 * 5.155f, 5.0f, 3.0f);
			ModelMatrix = glm::translate(TransMatrix, vec3(-26.5f, 0.0f, 43.25f));
			ModelMatrix = glm::rotate(ModelMatrix, radians(90.f), vec3(0.0f, 0.0f, 1.0f));
			ModelMatrix = glm::scale(ModelMatrix, vec3(10 * 5.155f, 5.0f, 3.0f));
			// Pas ModelView Matrix a shader
			glUniformMatrix4fv(glGetUniformLocation(shaderId, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
			NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
			// Pas NormalMatrix a shader
			glUniformMatrix4fv(glGetUniformLocation(shaderId, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
			draw_TriEBO_Object(GLUT_CUBE); //glutSolidCube(1.0f);
		//glPopMatrix();
	  }
	//glPopMatrix();
	  //deleteVAO(GLU_DISK);
};


void Motor(GLint shaderId, glm::mat4 MatriuVista, glm::mat4 MatriuTG, bool sw_mat[4])
{
// Matrius  
	glm::mat4 TransMatrix(1.0), ModelMatrix(1.0), NormalMatrix(1.0);
	CColor col_object = { 0.0,0.0,0.0,1.0 };

//Helices Motor
	int j;
	//glPushMatrix();
	  //glColor4f(0.58f, 0.58f, 0.58f, 0.0f);
// Definició propietats de reflexió (emissió, ambient, difusa, especular) del material.
	col_object.r = 0.58;	col_object.g = 0.58;	col_object.b = 0.58;	 col_object.a = 1.0;
	SeleccionaColorMaterial(shaderId, col_object, sw_mat);
	//glTranslatef(0.0f, -18.0f, 0.0f);
	//glRotated(-90.0f, 0.0f, 0.0f, 1.0f);
	//glRotated(90.0f, 0.0f, 1.0f, 0.0f);
	//glRotatef(45, 0, 0, 1);
	TransMatrix = glm::translate(MatriuTG, vec3(0.0f, -18.0f, 0.0f));
	TransMatrix = glm::rotate(TransMatrix, radians(-90.f), vec3(0.0f, 0.0f, 1.0f));
	TransMatrix = glm::rotate(TransMatrix, radians(90.f), vec3(0.0f, 1.0f, 0.0f));
	TransMatrix = glm::rotate(TransMatrix, radians(45.f), vec3(0.0f, 0.0f, 1.0f));
	for (j = 0; j<6; j = j + 1)
	{	//glRotatef(30, 0, 0, 1);
	TransMatrix = glm::rotate(TransMatrix, radians(30.f), vec3(0.0f, 0.0f, 1.0f));
	//glPushMatrix();
	//glScalef(12 * 0.583f, 0.1f, 0.3f);
	ModelMatrix = glm::scale(TransMatrix, vec3(12 * 0.583f, 0.1f, 0.3f));
	// Pas ModelView Matrix a shader
	glUniformMatrix4fv(glGetUniformLocation(shaderId, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
	NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
	// Pas NormalMatrix a shader
	glUniformMatrix4fv(glGetUniformLocation(shaderId, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
	draw_TriEBO_Object(GLUT_CUBE); //glutSolidCube(1.0f);
	//glPopMatrix();
	}
	//glPopMatrix();

//Soporte Motor
	//glPushMatrix();
	//glColor4f(0.28f, 0.28f, 0.28f, 0.0f);
	// Definició propietats de reflexió (emissió, ambient, difusa, especular) del material.
	col_object.r = 0.28;	col_object.g = 0.28;	col_object.b = 0.28;	 col_object.a = 1.0;
	SeleccionaColorMaterial(shaderId, col_object, sw_mat);
	//glTranslatef(0.0f, -18.0f, 0.0f);
	//glRotated(-90.0f, 0.0f, 0.0f, 1.0f);
	//glRotated(90.0f, 0.0f, 1.0f, 0.0f);
	TransMatrix = glm::translate(MatriuTG, vec3(0.0f, -18.0f, 0.0f));
	TransMatrix = glm::rotate(TransMatrix, radians(-90.f), vec3(0.0f, 0.0f, 1.0f));
	ModelMatrix = glm::rotate(TransMatrix, radians(90.f), vec3(0.0f, 1.0f, 0.0f));
	// Pas ModelView Matrix a shader
	glUniformMatrix4fv(glGetUniformLocation(shaderId, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
	NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
	// Pas NormalMatrix a shader
	glUniformMatrix4fv(glGetUniformLocation(shaderId, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
	draw_TriEBO_Object(GLUT_TORUS); //glutSolidTorus(1.0, 5.0, 20, 20);
	//glPopMatrix();
};

void Canon(GLint shaderId, glm::mat4 MatriuVista, glm::mat4 MatriuTG, bool sw_mat[4])
{
// Matrius Transformació
	glm::mat4 TransMatrix(1.0), ModelMatrix(1.0), NormalMatrix(1.0);
	CColor col_object = { 0.0,0.0,0.0,1.0 };

//Cañones

// Salida cañon 1
	//glPushMatrix();
	//glColor4d(0.28, 0.28, 0.28, 0.0);
// Definició propietats de reflexió (emissió, ambient, difusa, especular) del material.
	col_object.r = 0.28;	col_object.g = 0.28;	col_object.b = 0.28;	 col_object.a = 1.0;
	SeleccionaColorMaterial(shaderId, col_object, sw_mat);
	//glTranslated(5.0, 8.0, -10.0);
	//glRotated(-90.0, 1.0, 0.0, 0.0);
	TransMatrix = glm::translate(MatriuTG, vec3(5.0f, 8.0f, -10.0f));
	ModelMatrix = glm::rotate(TransMatrix, radians(-90.f), vec3(1.0f, 0.0f, 0.0f));
	// Pas ModelView Matrix a shader
	glUniformMatrix4fv(glGetUniformLocation(shaderId, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
	NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
	// Pas NormalMatrix a shader
	glUniformMatrix4fv(glGetUniformLocation(shaderId, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
	draw_TriEBO_Object(GLUT_USER2); //gluCylinder(1.0f, 0.5f, 5.0f, 60, 1);
	//glPopMatrix();

// Salida cañon 2
	//glPushMatrix();
	//glColor4d(0.28, 0.28, 0.28, 0.0);
	//glTranslated(-5.0, 8.0, -10.0);
	//glRotated(-90.0, 1.0, 0.0, 0.0);
	TransMatrix = glm::translate(MatriuTG, vec3(-5.0f, 8.0f, -10.0f));
	ModelMatrix = glm::rotate(TransMatrix, radians(-90.f), vec3(1.0f, 0.0f, 0.0f));
	// Pas ModelView Matrix a shader
	glUniformMatrix4fv(glGetUniformLocation(shaderId, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
	NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
	// Pas NormalMatrix a shader
	glUniformMatrix4fv(glGetUniformLocation(shaderId, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
	draw_TriEBO_Object(GLUT_USER2); // gluCylinder(1.0f, 0.5f, 5.0f, 60, 1);
	//glPopMatrix();

// Cañon 1
	//glPushMatrix();
		//glColor4d(0.58, 0.58, 0.58, 0.0);
// Definició propietats de reflexió (emissió, ambient, difusa, especular) del material.
	col_object.r = 0.58;	col_object.g = 0.58;		col_object.b = 0.58;	 col_object.a = 1.0;
	SeleccionaColorMaterial(shaderId, col_object, sw_mat);
	//glTranslated(5.0, 10.0, -10.0);
	//glRotated(-90.0, 1.0, 0.0, 0.0);
	TransMatrix = glm::translate(MatriuTG, vec3(5.0f, 10.0f, -10.0f));
	ModelMatrix = glm::rotate(TransMatrix, radians(-90.f), vec3(1.0f, 0.0f, 0.0f));
	// Pas ModelView Matrix a shader
	glUniformMatrix4fv(glGetUniformLocation(shaderId, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
	NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
	// Pas NormalMatrix a shader
	glUniformMatrix4fv(glGetUniformLocation(shaderId, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
	draw_TriEBO_Object(GLUT_USER3); // gluCylinder(0.35f, 0.35f, 5.0f, 80, 1);
	//glPopMatrix();

//Cañon 2
	//glPushMatrix();
	//glColor4d(0.58, 0.58, 0.58, 1.0);
 	//glTranslated(-5.0, 10.0, -10.0);
	//glRotated(-90.0, 1.0, 0.0, 0.0);
	TransMatrix = glm::translate(MatriuTG, vec3(-5.0f, 10.0f, -10.0f));
	ModelMatrix = glm::rotate(TransMatrix, radians(-90.f), vec3(1.0f, 0.0f, 0.0f));
	// Pas ModelView Matrix a shader
	glUniformMatrix4fv(glGetUniformLocation(shaderId, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
	NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
	// Pas NormalMatrix a shader
	glUniformMatrix4fv(glGetUniformLocation(shaderId, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
	draw_TriEBO_Object(GLUT_USER3); // gluCylinder(0.35f, 0.35f, 5.0f, 80, 1);
	//glPopMatrix();
}

void Cuerpo(GLint shaderId, glm::mat4 MatriuVista, glm::mat4 MatriuTG, bool sw_mat[4])
{
// Matrius Transformació
	glm::mat4 TransMatrix(1.0), ModelMatrix(1.0), NormalMatrix(1.0);
	CColor col_object = { 0.0,0.0,0.0,1.0 };

// Activar transparència
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

//Sujeccion de las Alas

//Lado2
	//glPushMatrix();
	//glColor4d(0.16, 0.16, 0.16, 1.0);
// Definició propietats de reflexió (emissió, ambient, difusa, especular) del material.
	col_object.r = 0.16;	col_object.g = 0.16;	col_object.b = 0.16;	 col_object.a = 0.5;
	SeleccionaColorMaterial(shaderId, col_object, sw_mat);
	//glRotated(-90.0, 0.0, 1.0, 0.0);
	//glRotated(90.0, 0.0, 0.0, 1.0);
	//glScaled(2.0, 2.0, 2.5);
	TransMatrix = glm::rotate(MatriuTG, radians(-90.f), vec3(0.0f, 1.0f, 0.0f));
	TransMatrix = glm::rotate(TransMatrix, radians(90.f), vec3(0.0f, 0.0f, 1.0f));
	ModelMatrix = glm::scale(TransMatrix, vec3(2.0f, 2.0f, 2.5f));
	// Pas ModelView Matrix a shader
	glUniformMatrix4fv(glGetUniformLocation(shaderId, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
	NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
	// Pas NormalMatrix a shader
	glUniformMatrix4fv(glGetUniformLocation(shaderId, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
	draw_TriEBO_Object(GLUT_USER4); // gluCylinder(4.0f, 2.0f, 10.25f, 40, 1);
	//glPopMatrix();

//X2
	//glPushMatrix();
	//glColor4d(0.16, 0.16, 0.16, 1.0);
	//glTranslated(-25.0, 0.0, 0.0);
	//glRotated(90.0, 0.0, 1.0, 0.0);
	TransMatrix = glm::translate(MatriuTG, vec3(-25.0f, 0.0f, 0.0f));
	ModelMatrix = glm::rotate(TransMatrix, radians(90.f), vec3(0.0f, 1.0f, 0.0f));
	// Pas ModelView Matrix a shader
	glUniformMatrix4fv(glGetUniformLocation(shaderId, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
	NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
	// Pas NormalMatrix a shader
	glUniformMatrix4fv(glGetUniformLocation(shaderId, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
	draw_TriEBO_Object(GLUT_TORUS); // glutSolidTorus(1.0, 5.0, 20, 20);
	//glPopMatrix();

//Lado 1
	//glPushMatrix();
	//glColor4f(0.16f, 0.16f, 0.16f, 1.0f);
	//SetColor4d(0.16, 0.16, 0.16, 1.0);
	//glRotated(90.0, 0.0, 1.0, 0.0);
	//glRotated(90.0, 0.0, 0.0, 1.0);
	//glScaled(2.0, 2.0, 2.5);
	TransMatrix = glm::rotate(MatriuTG, radians(90.f), vec3(0.0f, 1.0f, 0.0f));
	TransMatrix = glm::rotate(TransMatrix, radians(90.f), vec3(0.0f, 0.0f, 1.0f));
	ModelMatrix = glm::scale(TransMatrix, vec3(2.0f, 2.0f, 2.5f));
	// Pas ModelView Matrix a shader
	glUniformMatrix4fv(glGetUniformLocation(shaderId, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
	NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
	// Pas NormalMatrix a shader
	glUniformMatrix4fv(glGetUniformLocation(shaderId, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
	draw_TriEBO_Object(GLUT_USER4); // gluCylinder(4.0f, 2.0f, 10.25f, 40, 1);
	//glPopMatrix();

//X1
	//glPushMatrix();
	//glColor4d(0.16, 0.16, 0.16, 1.0);
	//SetColor4d(0.16, 0.16, 0.16, 1.0);
	//glTranslated(25.25, 0.0, 0.0);
	//glRotated(90.0, 0.0, 1.0, 0.0);
	TransMatrix = glm::translate(MatriuTG, vec3(25.25f, 0.0f, 0.0f));
	ModelMatrix = glm::rotate(TransMatrix, radians(90.f), vec3(0.0f, 1.0f, 0.0f));
	// Pas ModelView Matrix a shader
	glUniformMatrix4fv(glGetUniformLocation(shaderId, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
	NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
	// Pas NormalMatrix a shader
	glUniformMatrix4fv(glGetUniformLocation(shaderId, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
	draw_TriEBO_Object(GLUT_TORUS); //glutSolidTorus(1.0, 5.0, 20, 20);
	//glPopMatrix();

//Cuerpo
	//glPushMatrix();
	//glColor4d(0.16, 0.16, 0.16, 1.0);
	//SetColor4d(0.16, 0.16, 0.16, 1.0);
	//glScaled(1.75f, 1.75f, 1.5f);
	ModelMatrix = glm::scale(MatriuTG, vec3(1.75f, 1.75f, 1.5f));
	// Pas ModelView Matrix a shader
	glUniformMatrix4fv(glGetUniformLocation(shaderId, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
	NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
	// Pas NormalMatrix a shader
	glUniformMatrix4fv(glGetUniformLocation(shaderId, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
	draw_TriEBO_Object(GLU_SPHERE); //gluSphere(10.0f, 80, 80);
	//glPopMatrix();

 // Desactivar transparència
	glDisable(GL_BLEND);
}

void Cabina(GLint shaderId, glm::mat4 MatriuVista, glm::mat4 MatriuTG, bool sw_mat[4])
{
// Matrius Transformació
	glm::mat4 TransMatrix(1.0), ModelMatrix(1.0), NormalMatrix(1.0);
	CColor col_object = { 0.0,0.0,0.0,1.0 };

// Activar transparència
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

// Tapa Cabina
	//glPushMatrix();
	//glColor4d(1.0, 0.058, 0.058, 0.5);
	// Definició propietats de reflexió (emissió, ambient, difusa, especular) del material.
	col_object.r = 1.0;	col_object.g = 0.058;	col_object.b = 0.058;	 col_object.a = 0.5;
	SeleccionaColorMaterial(shaderId, col_object, sw_mat);
	//glTranslated(0.0, 19.45, 0.0);
	//glRotated(-90.0, 0.0, 0.0, 1.0);
	//glRotated(90.0, 0.0, 1.0, 0.0);
	//glScaled(1.75, 1.75, 1.75);
	TransMatrix = glm::translate(MatriuTG, vec3(0.0f, 19.45f, 0.0f));
	TransMatrix = glm::rotate(TransMatrix, radians(-90.f), vec3(0.0f, 0.0f, 1.0f));
	TransMatrix = glm::rotate(TransMatrix, radians(90.f), vec3(0.0f, 1.0f, 0.0f));
	ModelMatrix = glm::scale(TransMatrix, vec3(1.75f, 1.75f, 1.75f));
	// Pas ModelView Matrix a shader
	glUniformMatrix4fv(glGetUniformLocation(shaderId, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
	NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
	// Pas NormalMatrix a shader
	glUniformMatrix4fv(glGetUniformLocation(shaderId, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
	draw_TriEBO_Object(GLUT_USER6); //gluDisk(0.0f, 1.5f, 8, 1);
	//glPopMatrix();

// Cristal Cabina
	//glPushMatrix();
	//glColor4d(1.0, 0.058, 0.058, 0.5);
	//glTranslated(0.0f, 19.45f, 0.0f);
	//glRotated(-90.0f, 0.0f, 0.0f, 1.0f);
	//glRotated(90.0f, 0.0f, 1.0f, 0.0f);
	//glScaled(1.75f, 1.75f, 1.75f);
	TransMatrix = glm::translate(MatriuTG, vec3(0.0f, 19.45f, 0.0f));
	TransMatrix = glm::rotate(TransMatrix, radians(-90.f), vec3(0.0f, 0.0f, 1.0f));
	TransMatrix = glm::rotate(TransMatrix, radians(90.f), vec3(0.0f, 1.0f, 0.0f));
	ModelMatrix = glm::scale(TransMatrix, vec3(1.75f, 1.75f, 1.75f));
	// Pas ModelView Matrix a shader
	glUniformMatrix4fv(glGetUniformLocation(shaderId, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
	NormalMatrix = transpose(inverse(MatriuVista * ModelMatrix));
	// Pas NormalMatrix a shader
	glUniformMatrix4fv(glGetUniformLocation(shaderId, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
	draw_TriEBO_Object(GLUT_USER5); //gluCylinder(1.5f, 4.5f, 2.0f, 8, 1);
	//glPopMatrix();

// Desactivar transparència
	glDisable(GL_BLEND);
}

extern MenuController* g_MenuController;

void initFisicas() {
	//Configuración de colisiones predeterminada
	collisionConfiguration = new btDefaultCollisionConfiguration();

	//Dispatcher: gestiona los algoritmos de colisión
	dispatcher = new btCollisionDispatcher(collisionConfiguration);

	//Broadphase: detección "grosa" de colisiones (cajas envolventes)
	broadphase = new btDbvtBroadphase();

	//Solver: resuelve las restricciones (físicas, contactos, articulaciones)
	solver = new btSequentialImpulseConstraintSolver;

	//Crear el mundo físico
	mundo = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);

	//Establecer gravedad.
	mundo->setGravity(btVector3(0, 0, -9.81f));

	printf("Bullet Physics inicializado correctamente.\n");
}

// Función para convertir un objeto gráfico OBJ en un colisionador estático para el suelo
void crearColisionadorEstatico(OBJ* objetoJuego) {
	if (!objetoJuego || !objetoJuego->objecteOBJ || !objetoJuego->objecteOBJ->m_pTriangleMesh) {
		printf("Error: El objeto no tiene malla fisica generada.\n");
		return;
	}

	//Crear la forma de colisión optimizada para mallas estáticas
	btBvhTriangleMeshShape* formaSuelo = new btBvhTriangleMeshShape(objetoJuego->objecteOBJ->m_pTriangleMesh, true);

	//Configurar la posición y escala inicial
	btTransform transformacion;
	transformacion.setIdentity();

	//uUsamos la posición que hemos definido en OBJ::render para 'circuit'
	transformacion.setOrigin(btVector3(710.0f, 120.0f, 1650.0f));

	//Crear el estado de movimiento
	btDefaultMotionState* estadoMovimiento = new btDefaultMotionState(transformacion);

	// Construir el RigidBody
	btRigidBody::btRigidBodyConstructionInfo infoCuerpo(0.0f, estadoMovimiento, formaSuelo, btVector3(0, 0, 0));
	btRigidBody* cuerpoRigido = new btRigidBody(infoCuerpo);

	//Aplicar la escala GIGANTE que tienes en el render
	cuerpoRigido->getCollisionShape()->setLocalScaling(btVector3(100.0f, 100.0f, 100.0f));

	//Añadir al mundo
	if (mundo) {
		mundo->addRigidBody(cuerpoRigido);
		printf("Suelo añadido al mundo fisico.\n");
	}
}

void stepFisicas(float deltaTime) {
	if (mundo) {
		mundo->stepSimulation(deltaTime, 10, 1.0f / 120.0f);
		detectarColisiones();
	}
	if (miCoche) miCoche->update();
}

void cleanFisicas() {
	// Borrar en orden inverso a la creación
	if (mundo) {
		delete mundo;
		mundo = nullptr;
	}
	if (solver) { delete solver; solver = nullptr; }
	if (broadphase) { delete broadphase; broadphase = nullptr; }
	if (dispatcher) { delete dispatcher; dispatcher = nullptr; }
	if (collisionConfiguration) { delete collisionConfiguration; collisionConfiguration = nullptr; }

	printf("Bullet Physics limpiado correctamente.\n");
}
// Añade esto al final de escena.cpp
void iniciarFisicasCoche() {
	if (miCoche != nullptr && mundo != nullptr) {
		miCoche->initFisicas(mundo);

		// --- AÑADIDO: Marcamos el rigidbody del coche ---
		// Obtenemos el cuerpo rigido (asumiendo que tienes un getter, si no, accede a la variable publica)
		btRigidBody* rbCoche = miCoche->getRigidBody();
		if (rbCoche) {
			// Usamos un puntero al propio objeto coche o un ID específico (ej: (void*)1)
			// Esto nos servirá para identificarlo en la colisión.
			rbCoche->setUserPointer((void*)miCoche);
		}
	}
	else {
		printf("Error: No se pudo iniciar el coche (miCoche o mundo son nulos)\n");
	}
}
// FI OBJECTE TIE: FETS PER ALUMNES -----------------------------------------------------------------

void detectarColisiones() {
	if (!mundo || !g_MenuController->GetContext()->isGameRunning) return;

	int numManifolds = mundo->getDispatcher()->getNumManifolds();

	for (int i = 0; i < numManifolds; i++) {
		btPersistentManifold* contactManifold = mundo->getDispatcher()->getManifoldByIndexInternal(i);

		const btCollisionObject* obA = contactManifold->getBody0();
		const btCollisionObject* obB = contactManifold->getBody1();

		bool esCocheA = (obA->getUserPointer() == (void*)miCoche);
		bool esCocheB = (obB->getUserPointer() == (void*)miCoche);

		if (!esCocheA && !esCocheB) continue;

		// Si hay contactos...
		int numContacts = contactManifold->getNumContacts();
		for (int j = 0; j < numContacts; j++) {
			btManifoldPoint& pt = contactManifold->getContactPoint(j);

			if (pt.getDistance() < 0.f) {
				// Identificamos quién es el objeto que NO es el coche
				const btCollisionObject* elOtro = esCocheA ? obB : obA;

				// 🏁 1. COMPROBAR SI ES LA META
				if (elOtro->getUserPointer() == (void*)"META") {
					g_MenuController->GetContext()->isGameRunning = false;
					g_MenuController->GetContext()->finalTime = g_MenuController->GetContext()->gameTime;

					if (g_MenuController) {
						g_MenuController->SwitchState(new EndGameState());
					}
					return; // Salimos, el juego ha terminado
				}

				// 🚗 2. LÓGICA DE DAÑO (Solo si no es invulnerable)
				if (tiempoInvulnerabilidad <= 0.0f) {
					tiempoInvulnerabilidad = 1.0f;
					if (g_MenuController) {
						g_MenuController->loseHP(10);
					}
				}

				return; // Salimos tras procesar el primer choque
			}
		}
	}

	// Actualizar tiempo de invulnerabilidad si no ha habido choques en este frame
	if (tiempoInvulnerabilidad > 0.0f) {
		tiempoInvulnerabilidad -= 1.0f / 60.0f;
	}
}
