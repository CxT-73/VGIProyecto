//******** PRACTICA VISUALITZACIÓ GRÀFICA INTERACTIVA (Escola Enginyeria - UAB
//******** Entorn bàsic VS2022 MONOFINESTRA amb OpenGL 4.6, interfície GLFW 3.4, ImGui i llibreries GLM
//******** Ferran Poveda, Marc Vivet, Carme Julià, Débora Gil, Enric Martí (Setembre 2025)
// visualitzacio.h : Interface de visualitzacio.cpp
//

#ifndef GLVIS_H
#define GLVIS_H

/* ------------------------------------------------------------------------- */
/*                                Funcions                                   */
/* ------------------------------------------------------------------------- */
class Coche;
// Entorn VGI: Il.luminació
void Iluminacio(char ilumin, bool ifix, bool ilu2sides, bool ll_amb, LLUM* lumin, char obj, bool frnt_fcs, bool bc_lin, int step);


// ----------- Entorn VGI: PROJECCIONS
// Entorn VGI: Projecció Ortografica
glm::mat4 Projeccio_Orto();
// Entorn VGI: Projecció Perspectiva
glm::mat4 Projeccio_Perspectiva(GLuint sh_programID, int minx, int miny, GLsizei w, GLsizei h, double zoom);
glm::mat4 Projeccio_Perspectiva(GLsizei w, GLsizei h, double fov_grados);

// ----------- Entorn VGI: CÀMERES
// Entorn VGI: Càmera Ortogràfica
glm::mat4 Vista_Ortografica(GLuint sh_programID, int prj, GLdouble Raux, CColor col_fons, CColor col_object, char objecte, GLdouble mida, int step,
	bool frnt_fcs, bool oculta, bool testv, bool bck_ln, char iluminacio, bool llum_amb, LLUM* lumin,
	bool ifix, bool il2sides, bool eix, CMask3D reixa, CPunt3D hreixa);
// Entorn VGI: Càmera Esfèrica
glm::mat4 Vista_Esferica(GLuint sh_programID, CEsfe3D opv,char VPol,bool pant,CPunt3D tr,CPunt3D trF,
				 CColor col_fons,CColor col_object,char objecte,double mida,int step,
				 bool frnt_fcs, bool oculta, bool testv, bool bck_ln, char iluminacio, bool llum_amb, LLUM* lumin, 
				 bool ifix, bool il2sides, bool eix, CMask3D reixa, CPunt3D hreixa);
// Entorn VGI: Càmera Navega
glm::mat4 Vista_Navega(GLuint sh_programID,CPunt3D pv,//bool pvb,
				  GLdouble n[3],GLdouble v[3], bool pant,CPunt3D tr,CPunt3D trF,
				  CColor col_fons,CColor col_object,char objecte,bool color, int step, 
				  bool frnt_fcs, bool oculta, bool testv,bool bck_ln, char iluminacio, bool llum_amb, LLUM* lumin, 
				  bool ifix, bool il2sides,bool eix, CMask3D reixa, CPunt3D hreixa);
// Entorn VGI: Càmera Geode
glm::mat4 Vista_Geode(GLuint sh_programID, CEsfe3D opv, char VPol, bool pant, CPunt3D tr, CPunt3D trF,
	CColor col_fons, CColor col_object, char objecte, double mida, int step,
	bool frnt_fcs, bool oculta, bool testv, bool bck_ln,
	char iluminacio, bool llum_amb, LLUM* lumi, bool ifix, bool il2sides,
	bool eix, CMask3D reixa, CPunt3D hreixa);

// Camera seguiment cotxe
glm::mat4 Vista_Seguimiento(GLuint sh_programID, Coche* coche, CEsfe3D opv, bool mobil, CColor col_fons,
	bool oculta, bool testv, bool bck_ln, char iluminacio, bool llum_amb,
	LLUM* lumi, bool ifix, bool il2sides);

glm::mat4 Vista_PrimeraPersona(GLuint sh_programID, Coche* coche, CColor col_fons,
	bool oculta, bool testv, bool bck_ln, char iluminacio, bool llum_amb, LLUM* lumi, bool ifix, bool il2sides);

glm::mat4 Vista_Espejo_Central(GLuint sh_programID, Coche* coche, CColor col_fons,
	bool oculta, bool testv, bool bck_ln, char iluminacio, bool llum_amb, LLUM* lumi, bool ifix, bool il2sides);

glm::mat4 Vista_Retrovisor(GLuint sh_programID, Coche* coche,bool esIzquierdo, CColor col_fons,
	bool oculta, bool testv, bool bck_ln, char iluminacio, bool llum_amb, LLUM* lumi, bool ifix, bool il2sides);

glm::mat4 Vista_Lliure(GLuint sh_programID, CColor col_fons, CEsfe3D opv, glm::vec3 g_FreeCamPos,
	bool oculta, bool testv, bool bck_ln, char iluminacio, bool llum_amb,
	LLUM* lumi, bool ifix, bool il2sides);
glm::mat4 Vista_Pausa(GLuint sh_programID, Coche* coche, CEsfe3D opv, bool mobil, CColor col_fons,
	bool oculta, bool testv, bool bck_ln, char iluminacio, bool llum_amb,
	LLUM* lumi, bool ifix, bool il2sides);

// Entorn VGI: Càrrega TG
glm::mat4 instancia(bool TR, INSTANCIA tg, INSTANCIA tgF);

// Entorn VGI: Dibuix dels eixos de coordenades
GLuint deixos(void);
void draw_Eixos(GLuint vboId);

// Entorn VGI: GRID (gridXY, gridYZ, gridXZ, gridXYZ) -------------------------
void draw_Grid(CMask3D quadricula, CPunt3D hquadricula);
void gridXY(int size, GLdouble h);
void gridXZ(int size, GLdouble h);
void gridYZ(int size, GLdouble h);
void gridXYZ(int size);

// Entorn VGI: Esborrats de pantalla a fons variable (Fons), negre (Fons) o a blanc (FonsW)
void Fons(CColor int_fons);
void FonsN(void);
void FonsB(void);

// Entorn VGI: Carregar fitxer imatge de formats admesos per la llibreria DevIL/OpenIL 
//				com a textura
GLint loadIMA_SOIL(const char *filename);
unsigned int loadCubemap(std::vector<std::string> faces);
void SetTextureParameters(GLint num, GLint IDText, bool extensT, bool filterT, bool blendT, bool genMipmpT);
#endif