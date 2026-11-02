/*
Animación:
Simple o básica:Por banderas y condicionales (más de 1 transformación geométrica se ve modificada)
Compleja: Por medio de funciones y algoritmos.
Textura Animada
*/
//para cargar imagen
#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <math.h>
#include <glew.h>
#include <glfw3.h>

#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>
#include <gtc\quaternion.hpp>
#include <algorithm>
//para probar el importer
//#include<assimp/Importer.hpp>

#include "Window.h"
#include "Mesh_tn.h"
#include "Shader_light.h"
#include "Camera.h"
#include "Texture.h"
#include "Sphere.h"
#include"Model.h"
#include "Skybox.h"

//para iluminación
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"
const float toRadians = 3.14159265f / 180.0f;

//variables para animación                                              

///Cohete espacial
glm::vec3 cohetePad(60.0f, 8.48f, -120.0f);        // centro del cohete en la plataforma
glm::vec3 cohetePos = cohetePad;
glm::quat coheteQ = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
float escalaCohete = 1.2f;                         
float coheteT = 0.0f;                              // reloj del ciclo
float coheteVel = 0.0f;                            // rapidez (u/s)
float coheteRoll = 0.0f;                           // giro sobre su propio eje (grados)
float coheteGamma = 0.0f;                          // inclinacion respecto a la vertical (radianes)
float coheteRumboAct = 0.0f;                       // direccion actual en XZ (grados)
int   coheteFase = 0;                              
float coheteTFase = 0.0f;                          // tiempo dentro de la fase actual
float coheteAcum = 0.0f;                           // grados recorridos dentro de la vuelta / el loop
float coheteMuestraAcum = 0.0f;                    // acumulador para grabar la ruta
float coheteIdx = 0.0f;                            // posicion de reproduccion del regreso
std::vector<glm::vec3> coheteRutaP;                // ruta grabada (posicion)
std::vector<glm::quat> coheteRutaQ;                // ruta grabada (orientacion)
const float cohetePausa = 3.0f;                    // cuenta regresiva (s)
const float coheteA0 = 2.5f;                       // aceleracion inicial (u/s^2)
const float coheteK = 0.5f;                        // aumento de aceleracion por segundo
const float coheteVMax = 25.0f;                    // rapidez maxima
const float coheteHGiro = 8.0f;                    // altura a la que empieza a inclinarse
const float coheteTGiro = 3.0f;                    // duracion de la inclinacion (s)
const float coheteRumbo = 0.0f;                    // direccion inicial en XZ (0 = hacia +X)
const float coheteRadio = 40.0f;                   // radio de la vuelta circular
const float coheteRadioLoop = 11.0f;               // radio del loop vertical
const float coheteSlalomT = 3.5f;                  // duracion del slalom (s)
const float coheteSlalomAmp = 35.0f;               // amplitud del zigzag (grados)
const float coheteRollVel = 40.0f;                 // giro propio normal (grados/s)
const float coheteRollTonel = 360.0f;              // giro propio durante el tonel (grados/s)
const float coheteGammaSalida = 45.0f;             // inclinacion al salir (grados respecto a la vertical)
const float coheteTSalidaDur = 3.0f;               // duracion del enderezado final (s)
const float coheteAltMax = 80.0f;                  // altura (sobre la plataforma) donde inicia el regreso
const float coheteRegresoVel = 1.0f;               // 1 = misma velocidad al regresar, 2 = el doble
const float coheteTVolteo = 1.6f;                  // duracion del giro de 180 grados para voltearse (s)
const float coheteMuestraDt = 1.0f / 30.0f;        // cada cuanto se graba un punto de la ruta (s)

//Holocron
const glm::vec3 holoSigno[8] = {           // octante de cada esquina (HoloEsq1 ... HoloEsq8)
	glm::vec3(-1.0f,  1.0f, -1.0f),
	glm::vec3(1.0f,  1.0f, -1.0f),
	glm::vec3(-1.0f, -1.0f, -1.0f),
	glm::vec3(1.0f, -1.0f, -1.0f),
	glm::vec3(1.0f,  1.0f,  1.0f),
	glm::vec3(-1.0f,  1.0f,  1.0f),
	glm::vec3(1.0f, -1.0f,  1.0f),
	glm::vec3(-1.0f, -1.0f,  1.0f)
};
glm::vec3 holoPos(20.0f, 8.0f, -40.0f);    // posicion del holocron (flota sobre el piso)
float escalaHolo = 1.0f;                   
float holoT = 0.0f;                        // reloj del ciclo
float holoP = 0.0f;                        // 0 = cerrado, 1 = abierto
float holoGiro = 0.0f;                     // giro de todo el holocron sobre Y (grados)
float holoDesenrosca = 0.0f;               // angulo de desenroscado de cada esquina sobre su diagonal (grados)
const float holoTCerrado = 1.5f;           // cerrado al inicio (s)
const float holoTAbre = 3.0f;              // se abre desenroscandose (s)
const float holoTAbierto = 3.0f;           // abierto girando (s)
const float holoTCierra = 3.0f;            // se cierra enroscandose (s)
const float holoTFin = 1.0f;               // cerrado al final (s)
const float holoDist = 4.5f;               // cuanto se aleja cada esquina
const float holoCentroEsq = 2.6f;          // centro aproximado de cada esquina respecto a los ejes

Window mainWindow;
std::vector<MeshModel*> meshList;
std::vector<Shader> shaderList;

Camera camera;

Texture pisoTexture;
Texture humoTexture;

//MODELOS
Model Cohete_M;
Model Holo_Centro;
Model Holo_Esq[8];

Skybox skybox;

//materiales
Material Material_brillante;
Material Material_opaco;


//Sphere cabeza = Sphere(0.5, 20, 20);
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

// luz direccional
DirectionalLight mainLight;
//para declarar varias luces de tipo pointlight
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

// Vertex Shader
static const char* vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";

void CreateObjects()
{
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	GLfloat vertices[] = {
		//	x      y      z			u	  v			nx	  ny    nz
			-1.0f, -1.0f, -0.6f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, -1.0f, 1.0f,		0.5f, 0.0f,		0.0f, 0.0f, 0.0f,
			1.0f, -1.0f, -0.6f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,		0.5f, 1.0f,		0.0f, 0.0f, 0.0f
	};

	unsigned int floorIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat floorVertices[] = {
		-10.0f, 0.0f, -10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,	0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,		10.0f, 10.0f,	0.0f, -1.0f, 0.0f
	};
	unsigned int vegetacionIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	   4,5,6,
	   4,6,7
	};

	GLfloat vegetacionVertices[] = {
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.0f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.0f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.0f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,

		0.0f, -0.5f, -0.5f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.5f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.5f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, -0.5f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,


	};


	unsigned int flechaIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	};

	GLfloat flechaVertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		1.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		1.0f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,

	};

	unsigned int scoreIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	};

	GLfloat scoreVertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		1.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		1.0f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,

	};

	unsigned int numeroIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	};

	GLfloat numeroVertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.67f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		0.25f, 0.67f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		0.25f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,

	};

	MeshModel* obj1 = new MeshModel();
	obj1->CreateMeshModel(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	MeshModel* obj2 = new MeshModel();
	obj2->CreateMeshModel(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	MeshModel* obj3 = new MeshModel();
	obj3->CreateMeshModel(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);


	MeshModel* obj4 = new MeshModel();
	obj4->CreateMeshModel(vegetacionVertices, vegetacionIndices, 64, 12);
	meshList.push_back(obj4);

	MeshModel* obj5 = new MeshModel();
	obj5->CreateMeshModel(flechaVertices, flechaIndices, 32, 6);
	meshList.push_back(obj5);

	MeshModel* obj6 = new MeshModel();
	obj6->CreateMeshModel(scoreVertices, scoreIndices, 32, 6);
	meshList.push_back(obj6); // todos los números

	MeshModel* obj7 = new MeshModel();
	obj7->CreateMeshModel(numeroVertices, numeroIndices, 32, 6);
	meshList.push_back(obj7); // solo un número

}


void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}

//Reinicia el ciclo del cohete y empieza a grabar la ruta desde la plataforma
void reiniciarCohete()
{
	coheteT = 0.0f; coheteVel = 0.0f; coheteRoll = 0.0f; coheteGamma = 0.0f;
	coheteRumboAct = coheteRumbo;
	coheteFase = 0; coheteTFase = 0.0f; coheteAcum = 0.0f;
	coheteMuestraAcum = 0.0f; coheteIdx = 0.0f;
	cohetePos = cohetePad;
	coheteQ = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
	coheteRutaP.clear(); coheteRutaQ.clear();
	coheteRutaP.push_back(cohetePos); coheteRutaQ.push_back(coheteQ);
}

//Cohete: ascenso, giro, vuelta circular, loop, slalom con tonel, salida, volteo 180°, regreso de frente y enderezado en plataforma
void actualizarCohete(float dt)
{
	if (coheteRutaP.empty()) reiniciarCohete();

	// maniobra de apogeo con inercia (desacelera subiendo, cabecea en vertical y acelera de regreso)
	if (coheteFase == 6)
	{
		coheteTFase += dt;
		float s = glm::clamp(coheteTFase / coheteTVolteo, 0.0f, 1.0f);
		float suave = s * s * (3.0f - 2.0f * s);

		glm::vec3 pFin = coheteRutaP.back();
		glm::vec3 dIda = (coheteRutaP.size() >= 2)
			? glm::normalize(pFin - coheteRutaP[coheteRutaP.size() - 2])
			: glm::vec3(0.7071f, 0.7071f, 0.0f);
		glm::vec3 n = glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f) - dIda.y * dIda);

		// Sigue subiendo por inercia hasta la cima del arco y vuelve a bajar hacia pFin
		cohetePos = pFin + dIda * (8.0f * sinf(3.14159265f * s));

		// La nariz cabecea en el plano vertical de +dIda a -dIda sin influencia de coheteRoll
		float angGiro = 3.14159265f * suave;
		glm::vec3 dir = glm::normalize(dIda * cosf(angGiro) + n * sinf(angGiro));

		glm::vec3 eje = glm::vec3(dir.z, 0.0f, -dir.x);
		float len = glm::length(eje);
		eje = (len > 0.0001f) ? (eje / len) : glm::vec3(0.0f, 0.0f, -1.0f);

		coheteRoll += coheteRollVel * dt;
		glm::quat inclina = glm::angleAxis(acosf(glm::clamp(dir.y, -1.0f, 1.0f)), eje);
		glm::quat gira = glm::angleAxis(glm::radians(coheteRoll), glm::vec3(0.0f, 1.0f, 0.0f));
		coheteQ = inclina * gira;

		if (s >= 1.0f)
		{
			coheteFase = 7;
			coheteTFase = 0.0f;
			coheteIdx = (float)(coheteRutaP.size() - 1);
		}
		return;
	}

	// regreso apuntando a la tangente de la ruta y aterrizaje vertical en la plataforma
	if (coheteFase == 7)
	{
		coheteIdx -= dt * coheteRegresoVel / coheteMuestraDt;
		if (coheteIdx <= 0.0f)
		{
			reiniciarCohete();
			return;
		}
		int i0 = (int)coheteIdx;
		float f = coheteIdx - i0;
		cohetePos = glm::mix(coheteRutaP[i0], coheteRutaP[i0 + 1], f);

		// Tangente interpolada de la trayectoria para que la nariz siga la curva sin saltos
		int iPrev = glm::max(i0 - 1, 0);
		glm::vec3 d0 = coheteRutaP[iPrev] - coheteRutaP[i0];
		glm::vec3 d1 = coheteRutaP[i0] - coheteRutaP[i0 + 1];
		glm::vec3 delta = glm::mix(d0, d1, f);
		glm::vec3 dirRuta = (glm::length(delta) > 0.0001f)
			? glm::normalize(delta)
			: glm::vec3(0.0f, -1.0f, 0.0f);

		// En la aproximacion final a la plataforma, levanta la nariz hacia +Y
		// para bajar el tramo vertical derecho sobre sus propulsores
		float sAterriza = glm::clamp((160.0f - coheteIdx) / 90.0f, 0.0f, 1.0f);
		float endereza = sAterriza * sAterriza * (3.0f - 2.0f * sAterriza);
		glm::vec3 dir = glm::normalize(glm::mix(dirRuta, glm::vec3(0.0f, 1.0f, 0.0f), endereza));

		glm::vec3 eje = glm::vec3(dir.z, 0.0f, -dir.x);
		float len = glm::length(eje);
		eje = (len > 0.0001f) ? (eje / len) : glm::vec3(0.0f, 0.0f, -1.0f);

		coheteRoll += (1.0f - endereza) * coheteRollVel * dt;
		glm::quat inclina = glm::angleAxis(acosf(glm::clamp(dir.y, -1.0f, 1.0f)), eje);
		glm::quat gira = glm::slerp(
			glm::angleAxis(glm::radians(coheteRoll), glm::vec3(0.0f, 1.0f, 0.0f)),
			glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
			endereza
		);
		coheteQ = inclina * gira;
		return;
	}

	coheteT += dt;
	if (coheteT < cohetePausa)                               // cuenta regresiva en la plataforma
	{
		if (coheteT > cohetePausa - 1.5f)                    // ultimo 1.5 s: vibracion del motor
			cohetePos = cohetePad + glm::vec3(0.15f * sinf(90.0f * coheteT), 0.0f, 0.15f * cosf(110.0f * coheteT));
		return;
	}

	coheteTFase += dt;
	float a = coheteA0 + coheteK * (coheteT - cohetePausa);  // a(t) = a0 + k t
	coheteVel = glm::min(coheteVel + a * dt, coheteVMax);    // v = v + a dt

	switch (coheteFase)
	{
	case 0:   // ascenso vertical
		if (cohetePos.y - cohetePad.y >= coheteHGiro) { coheteFase = 1; coheteTFase = 0.0f; }
		break;
	case 1:   // giro
	{
		float s = glm::clamp(coheteTFase / coheteTGiro, 0.0f, 1.0f);
		coheteGamma = glm::radians(90.0f) * (3.0f * s * s - 2.0f * s * s * s);
		if (s >= 1.0f) { coheteFase = 2; coheteTFase = 0.0f; coheteAcum = 0.0f; }
		break;
	}
	case 2:   // vuelta circular a altura constante (w = v / R)
	{
		float dr = glm::min(glm::degrees(coheteVel / coheteRadio) * dt, 360.0f - coheteAcum);
		coheteRumboAct -= dr;
		coheteAcum += dr;
		if (coheteAcum >= 359.999f) { coheteRumboAct = coheteRumbo; coheteFase = 3; coheteTFase = 0.0f; coheteAcum = 0.0f; }
		break;
	}
	case 3:   // loop vertical
	{
		float dg = glm::min(coheteVel / coheteRadioLoop * dt, glm::radians(360.0f - coheteAcum));
		coheteGamma -= dg;
		coheteAcum += glm::degrees(dg);
		if (coheteAcum >= 359.999f) { coheteGamma = glm::radians(90.0f); coheteFase = 4; coheteTFase = 0.0f; }
		break;
	}
	case 4:   // slalom: el rumbo oscila como seno mientras hace el tonel
	{
		float s = glm::clamp(coheteTFase / coheteSlalomT, 0.0f, 1.0f);
		coheteRumboAct = coheteRumbo + coheteSlalomAmp * sinf(2.0f * 3.14159265f * s);
		if (s >= 1.0f) { coheteRumboAct = coheteRumbo; coheteFase = 5; coheteTFase = 0.0f; }
		break;
	}
	case 5:   // salida
	{
		float s = glm::clamp(coheteTFase / coheteTSalidaDur, 0.0f, 1.0f);
		coheteGamma = glm::radians(90.0f + (coheteGammaSalida - 90.0f) * (3.0f * s * s - 2.0f * s * s * s));
		if (cohetePos.y - cohetePad.y > coheteAltMax)
		{
			coheteFase = 6;
			coheteTFase = 0.0f;
		}
		break;
	}
	}

	float rumbo = glm::radians(coheteRumboAct);
	glm::vec3 h(cosf(rumbo), 0.0f, sinf(rumbo));             // direccion horizontal
	glm::vec3 dir = glm::vec3(0.0f, 1.0f, 0.0f) * cosf(coheteGamma) + h * sinf(coheteGamma);
	cohetePos += dir * (coheteVel * dt);                     // p = p + v * dir * dt

	coheteRoll += ((coheteFase == 4) ? coheteRollTonel : coheteRollVel) * dt;
	glm::quat inclina = glm::angleAxis(coheteGamma, glm::normalize(glm::vec3(h.z, 0.0f, -h.x)));
	glm::quat gira = glm::angleAxis(glm::radians(coheteRoll), glm::vec3(0.0f, 1.0f, 0.0f));
	coheteQ = inclina * gira;                                // la nariz siempre apunta a dir

	coheteMuestraAcum += dt;                                 // graba la ruta a intervalos fijos
	while (coheteMuestraAcum >= coheteMuestraDt)
	{
		coheteMuestraAcum -= coheteMuestraDt;
		coheteRutaP.push_back(cohetePos);
		coheteRutaQ.push_back(coheteQ);
	}
	if (coheteFase == 6)
	{
		coheteRutaP.push_back(cohetePos);
		coheteRutaQ.push_back(coheteQ);
	}
}

//Holocron: ciclo cerrado, se abre desenroscandose, gira abierto y se cierra enroscandose
void actualizarHolocron(float dt)
{
	float t1 = holoTCerrado;
	float t2 = t1 + holoTAbre;
	float t3 = t2 + holoTAbierto;
	float t4 = t3 + holoTCierra;
	float total = t4 + holoTFin;

	holoT += dt;
	if (holoT >= total) holoT -= total;
	holoGiro = 360.0f * holoT / total;           // una vuelta completa por ciclo

	if (holoT < t1)
	{
		holoP = 0.0f;
		holoDesenrosca = 0.0f;
	}
	else if (holoT < t2)
	{
		float s = (holoT - t1) / holoTAbre;
		holoP = s * s * (3.0f - 2.0f * s);
		holoDesenrosca = holoP * 240.0f;         // se desenrosca 240 grados al abrirse
	}
	else if (holoT < t3)
	{
		holoP = 1.0f;
		float s = (holoT - t2) / holoTAbierto;
		float suave = s * s * (3.0f - 2.0f * s);
		holoDesenrosca = 240.0f + suave * 120.0f; // completa los 360 grados mientras esta abierto
	}
	else if (holoT < t4)
	{
		float s = (t4 - holoT) / holoTCierra;
		holoP = s * s * (3.0f - 2.0f * s);
		holoDesenrosca = holoP * 360.0f;         // se vuelve a enroscar al cerrarse
	}
	else
	{
		holoP = 0.0f;
		holoDesenrosca = 0.0f;
	}
}

int main()
{
	mainWindow = Window(1366, 768); 
	mainWindow.Initialise();

	CreateObjects();
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 1.0f, 0.5f);


	pisoTexture = Texture("Textures/piso.tga");
	pisoTexture.LoadTextureA();
	humoTexture = Texture("Textures/humo.png");
	humoTexture.LoadTextureA();

	// ---- Carga de modelos ----

	Cohete_M = Model();
	Cohete_M.LoadModel("Models/cohete.obj");

	Holo_Centro = Model();
	Holo_Centro.LoadModel("Models/HoloCentro.obj");
	for (int i = 0; i < 8; i++)
	{
		Holo_Esq[i] = Model();
		Holo_Esq[i].LoadModel("Models/HoloEsq" + std::to_string(i + 1) + ".obj");
	}


	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_rt.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_lf.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_dn.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_bk.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_ft.tga");

	skybox = Skybox(skyboxFaces);

	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);


	//luz direccional, sólo 1 y siempre debe de existir
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.6f, 0.5f,
		0.0f, 0.0f, -1.0f);

	//contador de luces puntuales
	unsigned int pointLightCount = 0;

	unsigned int spotLightCount = 0;
	//linterna
	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		5.0f);
	spotLightCount++;


	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0, uniformTextureOffset = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);

	glm::vec3 lowerLight(0.0f, 0.0f, 0.0f);
	glm::mat4 model(1.0);
	glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec2 toffset = glm::vec2(0.0f, 0.0f);
	glm::mat4 matCohete(1.0f);
	glm::mat4 matHolo(1.0f), matHoloCentro(1.0f), matHoloPieza(1.0f);
	glm::vec3 holoDir(0.0f), holoEje(0.0f), holoCentro(0.0f);
	float holoAng = 0.0f;

	////Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		GLfloat dtReal = now - lastTime;      // segundos reales
		if (dtReal > 0.1f) dtReal = 0.1f;
		deltaTime = now - lastTime; //Estandarizar el tiempo entre diferente hardware
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		actualizarCohete(dtReal);
		actualizarHolocron(dtReal);

		//Recibir eventos del usuario
		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		//Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		skybox.DrawSkybox(camera.calculateViewMatrix(), projection);
		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformColor = shaderList[0].getColorLocation();
		uniformTextureOffset = shaderList[0].getOffsetLocation();

		//información en el shader de intensidad especular y brillo
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		// luz ligada a la cámara de tipo flash
		lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f;
		spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());

		//información al shader de fuentes de iluminación
		shaderList[0].SetDirectionalLight(&mainLight);
		shaderList[0].SetPointLights(pointLights, pointLightCount);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);


		//Reinicializando variables cada ciclo de reloj
		model = glm::mat4(1.0);
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		toffset = glm::vec2(0.0f, 0.0f);

		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		pisoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[2]->RenderMeshModel();

		//COHETE
		matCohete = glm::translate(glm::mat4(1.0f), cohetePos);
		matCohete *= glm::mat4_cast(coheteQ);
		matCohete = glm::scale(matCohete, glm::vec3(escalaCohete));
		matCohete = glm::translate(matCohete, glm::vec3(-0.14f, -10.096f, 0.0f));   // centro del cohete al origen
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(matCohete));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Cohete_M.RenderModel();

		//HOLOCRON (centro + 8 esquinas que se desenroscan sobre su propia diagonal)
		glm::vec3 posFlotante = holoPos + glm::vec3(0.0f, 0.5f * sinf(2.0f * holoT), 0.0f);
		matHolo = glm::translate(glm::mat4(1.0f), posFlotante);
		matHolo = glm::rotate(matHolo, glm::radians(holoGiro), glm::vec3(0.0f, 1.0f, 0.0f));
		matHolo = glm::scale(matHolo, glm::vec3(escalaHolo));

		// El nucleo central gira en sentido opuesto cuando el holocron se abre
		matHoloCentro = glm::rotate(matHolo, glm::radians(-holoDesenrosca * 0.5f), glm::vec3(0.0f, 1.0f, 0.0f));
		color = glm::vec3(0.6f, 0.8f, 1.0f);                  // centro azulado
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(matHoloCentro));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Holo_Centro.RenderModel();

		color = glm::vec3(1.0f, 0.8f, 0.3f);                  // esquinas doradas
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		for (int i = 0; i < 8; i++)
		{
			holoDir = glm::normalize(holoSigno[i]);           // diagonal radial de la esquina
			holoEje = holoDir;                                // gira sobre su propia diagonal (efecto desenroscar)
			holoCentro = holoSigno[i] * holoCentroEsq;        // pivote = centro de la esquina
			holoAng = holoDesenrosca;                         // 0 cuando esta cerrado

			matHoloPieza = glm::translate(matHolo, holoDir * (holoP * holoDist));
			matHoloPieza = glm::translate(matHoloPieza, holoCentro);
			matHoloPieza = glm::rotate(matHoloPieza, glm::radians(holoAng), holoEje);
			matHoloPieza = glm::translate(matHoloPieza, -holoCentro);
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(matHoloPieza));
			Holo_Esq[i].RenderModel();
		}

		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}