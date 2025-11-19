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
#include <btBulletDynamicsCommon.h>
#include <BulletDynamics/Vehicle/btRaycastVehicle.h>
#include "Coche.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
extern GLFWwindow* window;

// Objetos escena
Coche* miCoche = nullptr;
OBJ* cono = nullptr;
OBJ* circuit = nullptr;
OBJ* barrera = nullptr;
OBJ* bloc = nullptr;
OBJ* barril = nullptr;

//Físicas de Bullet 
btDiscreteDynamicsWorld* mundoFisico = nullptr;
btBroadphaseInterface* broadphase = nullptr;
btCollisionDispatcher* dispatcher = nullptr;
btSequentialImpulseConstraintSolver* solver = nullptr;
btDefaultCollisionConfiguration* config = nullptr;


// Carga un OBJ (solo usamos "v" y "f") y crea un suelo estático en Bullet
btRigidBody* crearSueloDesdeOBJ(const char* rutaOBJ,
	btDiscreteDynamicsWorld* mundo,
	const btVector3& offset = btVector3(0, 0, 0),
	float escala = 100)
{
	std::ifstream file(rutaOBJ);
	if (!file.is_open()) {
		std::cerr << "ERROR al abrir OBJ para fisica: " << rutaOBJ << std::endl;
		return nullptr;
	}

	std::vector<btVector3> vertices;
	std::string line;

	btTriangleMesh* mesh = new btTriangleMesh();

	while (std::getline(file, line)) {
		if (line.size() < 2) continue;

		std::istringstream ss(line);
		std::string type;
		ss >> type;

		// Vértices
		if (type == "v") {
			float x, y, z;
			ss >> x >> y >> z;
			vertices.emplace_back(x * escala, y * escala, z * escala);
		}
		// Caras
		else if (type == "f") {
			std::string vStr;
			std::vector<int> idx;

			// Cada token es algo tipo "12/34/56" o "12//56" o "12"
			while (ss >> vStr) {
				if (vStr.empty()) continue;
				std::replace(vStr.begin(), vStr.end(), '/', ' ');
				std::istringstream vs(vStr);
				int vi = 0;
				vs >> vi;                    // índice de vértice (1-based)
				if (vi == 0) continue;

				// Soporte índices negativos (desde el final)
				if (vi < 0) vi = (int)vertices.size() + vi + 1;

				idx.push_back(vi - 1);       // a 0-based
			}

			// Si la cara tiene más de 3 vértices, triangulamos en "fan"
			if (idx.size() < 3) continue;
			for (size_t i = 1; i + 1 < idx.size(); ++i) {
				const btVector3& a = vertices[idx[0]];
				const btVector3& b = vertices[idx[i]];
				const btVector3& c = vertices[idx[i + 1]];

				mesh->addTriangle(a + offset, b + offset, c + offset);
			}
		}
	}

	file.close();

	if (mesh->getNumTriangles() == 0) {
		std::cerr << "OBJ sin triángulos para fisica: " << rutaOBJ << std::endl;
		delete mesh;
		return nullptr;
	}

	bool useQuantizedAabb = true;
	btBvhTriangleMeshShape* shape =
		new btBvhTriangleMeshShape(mesh, useQuantizedAabb);

	btTransform tr;
	tr.setIdentity();
	tr.setOrigin(btVector3(0, 0, 0));   // ya hemos aplicado offset en los vértices

	btDefaultMotionState* motion = new btDefaultMotionState(tr);

	btRigidBody::btRigidBodyConstructionInfo info(
		0.0f,          // masa 0 → estático
		motion,
		shape,
		btVector3(0, 0, 0)
	);

	btRigidBody* body = new btRigidBody(info);
	mundo->addRigidBody(body);
	return body;
}


void inicializaFisica()
{
	// ... lo que ya tienes:
	config = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(config);
	broadphase = new btDbvtBroadphase();
	solver = new btSequentialImpulseConstraintSolver();
	mundoFisico = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, config);
	mundoFisico->setGravity(btVector3(0, -9.81f, 0));

	// Crear suelos desde tus OBJ
	// OJO: ajusta ruta, escala y offset para que coincida con lo que dibujas
	btRigidBody* sueloCircuito = crearSueloDesdeOBJ(
		"../x64/Release/OBJFiles/Circuit_m3/track.obj",
		mundoFisico,
		btVector3(0, 0, 0),   // mismo origen que el render (de momento)
		100.0f                // si en el render escalas a 100, pon lo mismo aquí
	);

	// Crear coche
	miCoche = new Coche(mundoFisico);

	std::cout << "Mundo físico, suelos y coche inicializados.\n";
}

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
	if (mundoFisico == nullptr)
		inicializaFisica();
	
	if (miCoche && mundoFisico)
	{
		float motor = 0.0f;
		float giro = 0.0f;
		float freno = 0.0f;

		// Control por teclado (usa GLFW)
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) motor = 1500.0f;  // Acelerar
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) freno = 50.0f;    // Frenar
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) giro = 0.3f;      // Izquierda
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) giro = -0.3f;     // Derecha

		miCoche->aplicarInput(motor, giro, freno);
		miCoche->update(1.0f / 60.0f);
	}

	glUseProgram(sh_programID);
	if (texturID[0] != 0) SetTextureParameters(0, texturID[0], true, true, textur_map, false);
	glUniform1i(glGetUniformLocation(sh_programID, "textur"), textur ? GL_TRUE : GL_FALSE);
	glUniform1i(glGetUniformLocation(sh_programID, "modulate"), textur ? GL_TRUE : GL_FALSE);
	glUniform1i(glGetUniformLocation(sh_programID, "flag_invert_y"), flagInvertY);
	glBindAttribLocation(sh_programID, 0, "in_Vertex");
	glBindAttribLocation(sh_programID, 1, "in_Color");
	glBindAttribLocation(sh_programID, 2, "in_Normal");
	glBindAttribLocation(sh_programID, 3, "in_TexCoord");
	SeleccionaColorMaterial(sh_programID, col_object, sw_mat);

	if (miCoche) miCoche->Coche::render(sh_programID, MatriuVista, MatriuTG, col_object, sw_mat);

	if (cono) cono->render(sh_programID, MatriuVista, MatriuTG, col_object, sw_mat);
	if (circuit) circuit->render(sh_programID, MatriuVista, MatriuTG, col_object, sw_mat);
	if (barrera) barrera->render(sh_programID, MatriuVista, MatriuTG, col_object, sw_mat);
	if (bloc) bloc->render(sh_programID, MatriuVista, MatriuTG, col_object, sw_mat);
	if (barril) barril->render(sh_programID, MatriuVista, MatriuTG, col_object, sw_mat);
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

