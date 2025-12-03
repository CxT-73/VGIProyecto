//******** PRACTICA VISUALITZACIÓ GRÀFICA INTERACTIVA (Escola Enginyeria - UAB)
//******** Entorn bàsic VS2022 MONOFINESTRA amb OpenGL 4.6, interfície GLFW 3.4, ImGui i llibreries GLM
//******** Enric Martí (Setembre 2025)
// objLoader.h: Inteface of the class COBJModel.
//
//	  Versió 2.0:	- Adaptació funcions a crear un VAO per a cada material del fitxer
//////////////////////////////////////////////////////////////////////////////////////

#ifndef OBJLOADER_H
#define OBJLOADER_H

#define OBJLOADER_CLASS_DECL     __declspec(dllexport)

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <vector>
#include <btBulletDynamicsCommon.h>
// Màxima mida vector VAOList
#define MAX_SIZE_VAOLIST 125

// Needed structures
struct Vector3D
{
	float fX;
	float fY;
	float fZ;
};

struct Vector2D
{
	float fX;
	float fY;
};

struct OBJFileInfo
{
	unsigned int iVertexCount;
	unsigned int iTexCoordCount;
	unsigned int iNormalCount;
	unsigned int iFaceCount;
	unsigned int iMaterialCount;
};

struct Face
{
	unsigned int	iNumVertices;
	unsigned int	iMaterialIndex;
	Vector3D		*pVertices;
	Vector3D		*pNormals;
	Vector2D		*pTexCoords;
};

struct Material
{
  char	szName[1024];		 // Name of the material
  char	szTexture[_MAX_PATH];// Filename of the texture
  uint	iTextureID;			 // OpenGL name of the texture
  float fDiffuse[3];		 // Diffuse component
  float fAmbient[3];		 // Ambient component
  float fSpecular[3];		 // Specular component
  float fEmmissive[3];		 // Emmissive component
  float fShininess;			 // Specular exponent
};

class OBJLOADER_CLASS_DECL COBJModel
{
public:
    // Funcions existents (SOLO UNA VEZ CADA UNA)
    void _stdcall DrawModel(int prim_Id);
    int _stdcall LoadModel(char* szFileName);
    _stdcall COBJModel();
    virtual _stdcall ~COBJModel();
    void _stdcall EliminaLlista(int prim_Id);

    // Funcions CVAO
    void _stdcall netejaVAOList_OBJ();
    void _stdcall netejaTextures_OBJ();
    void _stdcall draw_TriVAO_OBJ(GLuint sh_programID);

    // --- NUEVO: Variable para Bullet (SOLO ESTO ES LO NUEVO) ---
    btTriangleMesh* m_pTriangleMesh = nullptr;
    // -----------------------------------------------------------

private:
    void _stdcall ReadNextString(char szString[], FILE* hStream);
    int _stdcall LoadTexture2(const char szFileName[_MAX_PATH]);
    void _stdcall UseMaterial(const Material* pMaterial);
    void _stdcall UseMaterial_ShaderID(GLuint sh_programID, Material pMaterial);
    void _stdcall GetTokenParameter(char szString[], const unsigned int iStrSize, FILE* hFile);
    void _stdcall MakePath(char szFileAndPath[]);
    bool _stdcall LoadMaterialLib(const char szFileName[], Material* pMaterials,
        unsigned int* iCurMaterialIndex, char szBasePath[]);
    CVAO _stdcall RenderToVAOList(const Face* pFaces, const unsigned int iFaceCount,
        const Material* pMaterials);
    void _stdcall loadToVAOList(const Face* pFaces, const unsigned int iFaceCount,
        const Material* pMaterials);
    void _stdcall GetFaceNormal(float fNormalOut[3], const Face* pFace);
    void _stdcall ParseFaceString(char szFaceString[], Face* FaceOut, const Vector3D* pVertices,
        const Vector3D* pNormals, const Vector2D* pTexCoords, const unsigned int iMaterialIndex);
    void _stdcall GetFileInfo(FILE* hStream, OBJFileInfo* Stat, const char szConstBasePath[]);
    void _stdcall GenTexCoords();

    // CVAO
    GLint numMaterials = 0;
    int vector_Materials[MAX_SIZE_VAOLIST];
    CVAO VAOList_OBJ[MAX_SIZE_VAOLIST];
    Material vMaterials[MAX_SIZE_VAOLIST];

    // Funcions CVAO
    void _stdcall initVAOList_OBJ();
    void _stdcall Set_VAOList_OBJ(GLint k, CVAO auxVAO);
    void _stdcall deleteVAOList_OBJ(GLint k);
    void _stdcall draw_TriVAO_Object_OBJ(GLint k);
};

// Callback function for comparing two faces with qsort
static int CompareFaceByMaterial(const void *Arg1, const void *Arg2);

//Returns a newly created COBJModel object. 
OBJLOADER_CLASS_DECL COBJModel* _stdcall InitObject();

//Destroys a COBJModel object
OBJLOADER_CLASS_DECL void _stdcall UnInitObject(COBJModel *obj);

#endif // !defined(AFX_OBJMODEL_H__32C5F722_AD3D_11D1_8F4D_E0B57CC10800__INCLUDED_)


#pragma once

#include "stdafx.h" 


class COBJModel;

class OBJ {
public:
	std::string nom;
	COBJModel* objecteOBJ;

	glm::vec3 posicion;   // traslación
	glm::vec3 rotacion;   // rotación en grados (x,y,z)
	glm::vec3 escala;  

    //COLISIONS OBJECTES
    btRigidBody* m_rigidBody;
    btCollisionShape* m_collisionShape;
    OBJ(const std::string& nombreObjeto, const glm::vec3& pos)
        : nom(nombreObjeto), posicion(pos), rotacion(0.0f), escala(1.0f),
        m_rigidBody(nullptr), m_collisionShape(nullptr)
    {
        objecteOBJ = new COBJModel();
        char* ruta = new char[nombreObjeto.length() + 1];
        strcpy(ruta, nombreObjeto.c_str());  // copia ? ahora sí es char*

        objecteOBJ->LoadModel(ruta);

        delete[] ruta; // evitar fugas


    }

	OBJ(const std::string& nombreObjeto);
	~OBJ();
    void initFisicas(btDiscreteDynamicsWorld* mundo,
        glm::vec3 offset = glm::vec3(0.0f) );

	void setTransform(glm::vec3 pos, glm::vec3 rot, glm::vec3 scl) {
		posicion = pos;
		rotacion = rot;
		escala = scl;
	}
    void destroyFisicas(btDiscreteDynamicsWorld* mundo);
    OBJ(const std::string& nombreObjeto,
        const glm::vec3& pos,
        COBJModel* sharedModel)
    {
        nom = nombreObjeto;
        objecteOBJ = sharedModel;   // Reusar modelo
        posicion = pos;
        rotacion = glm::vec3(0);
        escala = glm::vec3(1);

        m_rigidBody = nullptr;
        m_collisionShape = nullptr;
         
    }
	void render(GLuint sh_programID, glm::mat4 MatriuVista, glm::mat4 MatriuTG,
		CColor col_object, bool sw_mat[5]);
};
