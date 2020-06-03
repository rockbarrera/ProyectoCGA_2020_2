#define _USE_MATH_DEFINES
#include <cmath>
//glew include
#include <GL/glew.h>

//std includes
#include <string>
#include <iostream>
#include <stdlib.h>

// contains new std::shuffle definition
#include <algorithm>
#include <random>

//glfw include
#include <GLFW/glfw3.h>

// program include
#include "Headers/TimeManager.h"

// Shader include
#include "Headers/Shader.h"

// Model geometric includes
#include "Headers/Sphere.h"
#include "Headers/Cylinder.h"
#include "Headers/Box.h"
#include "Headers/FirstPersonCamera.h"
#include "Headers/ThirdPersonCamera.h"


//GLM include
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Headers/Texture.h"

// Include loader Model class
#include "Headers/Model.h"

// Include Terrain
#include "Headers/Terrain.h"

#include "Headers/AnimationUtils.h"

// Include Colision headers functions
#include "Headers/Colisiones.h"

// ShadowBox include
#include "Headers/ShadowBox.h"

// OpenAL include
#include <AL/alut.h>

#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))

int screenWidth;
int screenHeight;

const unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;

//Inclusión del joystick
int present;
int buttonCount;
const unsigned char* buttons;
int axesCount;
const float* axes;
const char* name;

GLFWwindow *window;

Shader shader;
//Shader con skybox
Shader shaderSkybox;
//Shader con multiples luces
Shader shaderMulLighting;
//Shader para el terreno
Shader shaderTerrain;
//Shader para las particulas de fountain
Shader shaderParticlesGeiser;
//Shader para las particulas de fuego
Shader shaderParticlesFire;
//Shader para visualizar el buffer de profundidad
Shader shaderViewDepth;
//Shader para dibujar el buffer de profunidad
Shader shaderDepth;

std::shared_ptr<Camera> camera(new ThirdPersonCamera());
std::shared_ptr<FirstPersonCamera> cameraFPS(new FirstPersonCamera()); //Cámara en primera persona

int stateCamera = 1; //1 para la TPS y 2 para la FPS

float distanceFromTarget = 7.0;

Sphere skyboxSphere(20, 20);
Box boxCollider;
Sphere sphereCollider(10, 10);
Box boxViewDepth;
Box boxLightViewBox;

ShadowBox * shadowBox;

// Models complex instances

// Hierba
Model modelGrass;

//Geiser
Model modelGeiser;

//Palm
Model modelPalm;

//Door
Model modelDoor;

//Mountain
Model modelMountain;

//Tree
Model modelTree;

// Model animate instance
// Mayow
Model mayowModelAnimate;
// Terrain model instance
Terrain terrain(-1, -1, 200, 16, "../Textures/terrenoJurassic2.png");

GLuint textureCespedID;
GLuint textureTerrainBackgroundID, textureTerrainRID, textureTerrainGID, textureTerrainBID, textureTerrainBlendMapID;
GLuint textureParticlesGeiserID, textureParticleFireID, texId;
GLuint skyboxTextureID;

GLenum types[6] = {
GL_TEXTURE_CUBE_MAP_POSITIVE_X,
GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
GL_TEXTURE_CUBE_MAP_NEGATIVE_Z };


std::string fileNames[6] = { "../Textures/mp_hexagon/hexagon_ft.tga",
		"../Textures/mp_hexagon/hexagon_bk.tga",
		"../Textures/mp_hexagon/hexagon_up.tga",
		"../Textures/mp_hexagon/hexagon_dn.tga",
		"../Textures/mp_hexagon/hexagon_rt.tga",
		"../Textures/mp_hexagon/hexagon_lf.tga" };

bool exitApp = false;
int lastMousePosX, offsetX = 0;
int lastMousePosY, offsetY = 0;

// Model matrix definitions
//glm::mat4 modelMatrixDart = glm::mat4(1.0f);
glm::mat4 modelMatrixMayow = glm::mat4(1.0f);
glm::mat4 modelMatrixFountain1 = glm::mat4(1.0f);
glm::mat4 modelMatrixGeiser = glm::mat4(1.0f);
glm::mat4 modelMatrixDoor = glm::mat4(1.0f);
glm::mat4 modelMatrixMountain = glm::mat4(1.0f);

int animationIndex = 0;
int velModel = 1;
int modelSelected = 2;
bool enableCountSelected = true;
bool enableCountSelectedGamePad = true;
bool inputMethod = false;

// Variables to animations keyframes

// Var animate helicopter
float rotHelHelY = 0.0;

// Var animate lambo dor
int stateDoor = 0;
float dorRotCount = 0.0;

// Palm positions
std::vector<glm::vec3> palmPositions = { glm::vec3(-91.42f, 0.0f, 94.41f),
										 glm::vec3(-94.55f, 0.0f, 72.92f),
										 glm::vec3(-92.75f, 0.0f, 60.11f),
										 glm::vec3(-96.89f, 0.0f, 45.54f),
										 glm::vec3(-85.11f, 0.0f, 32.74f),
										 glm::vec3(-75.93f, 0.0f, 24.28f),
										 glm::vec3(-96.32f, 0.0f, 22.95f),
										 glm::vec3(-87.14f, 0.0f, 13.78f)
};

//Geiser positions
std::vector<glm::vec3> geiserPositions = { glm::vec3(46.5 , 0, 40.38),
										   glm::vec3(40.33 , 0, 45.33),
										   glm::vec3(42.35 , 0, 50.32),
										   glm::vec3(35.57 , 0, 50.94),
										   glm::vec3(28.49 , 0, 55.33),
										   glm::vec3(23.39 , 0, 60.26),
										   glm::vec3(10.95 , 0, 57.4),
										   glm::vec3(17.76 , 0, 59.16),
										   glm::vec3(28.63 , 0, 78.61),
										   glm::vec3(14.62 , 0, 69.47),
										   glm::vec3(8.28 , 0, 65.11),
										   glm::vec3(22.44 , 0, 94.5),
										   glm::vec3(6.0 , 0, 58.96),
										   glm::vec3(35.54f, 0.0f, 67.44f),
										   glm::vec3(19.08, 0, 85.7),
										   glm::vec3(28.6, 0, 89.94),
										   glm::vec3(35.43, 0, 85.31),
										   glm::vec3(43.62, 0, 77.12),
										   glm::vec3(44.91, 0, 61.08),
										   glm::vec3(56.59, 0, 65.04),
										   glm::vec3(57.07, 0, 75.97),
										   glm::vec3(38.9, 0, 94.98),
										   glm::vec3(49.71, 0, 89.14),
										   glm::vec3(71.26, 0, 83.14),
										   glm::vec3(55.13, 0, 51.66),
										   glm::vec3(63.62, 0, 52.03),
										   glm::vec3(75.25, 0, 70.92),
};

//Fire positions
std::vector<glm::vec3> firePositions = { glm::vec3(-2.4 , 9 , 0),
										glm::vec3(2.4 , 9 , 0),
										glm::vec3(-2.4 , 8.1 , 0.7),
										glm::vec3(2.5 , 8.1 , 0.7),
										glm::vec3(-3.3 , 5.4 , 1.1),
										glm::vec3(3.4 , 5.4 , 1.1),
										glm::vec3(-2.6 , 2.7 , 1.7),
										glm::vec3(3.1 , 2.7 , 1.7)
};

//Grass positions geiser
std::vector<glm::vec3> grassPositionsGeiser = { glm::vec3(0.0, 0.0, -80.0),
												glm::vec3(58.52, 0, 51.37),
												glm::vec3(54.01, 0, 55.27),
												glm::vec3(50.60, 0, 54.50),
												glm::vec3(50.03, 0, 58.29),
												glm::vec3(65.40, 0, 54.59),
												glm::vec3(62.10, 0, 57.55),
												glm::vec3(61.46, 0, 64.48),
												glm::vec3(57.83, 0, 72.33),
												glm::vec3(58.18, 0, 82.97),
												glm::vec3(54.57, 0, 89.39),
												glm::vec3(63.85, 0, 77.06),
												glm::vec3(69.60, 0, 74.11),
												glm::vec3(78.59, 0, 83.95),
												glm::vec3(54.84, 0, 64.96),
												glm::vec3(44.48, 0, 64.38),
												glm::vec3(49.43, 0, 73.17),
												glm::vec3(47.42, 0, 78.90),
												glm::vec3(46.49, 0, 89.13),
												glm::vec3(35.35, 0, 89.67),
												glm::vec3(30.04, 0, 83.75),
												glm::vec3(31.73, 0, 93.39),
												glm::vec3(20.54, 0, 80.14),
												glm::vec3(28.73, 0, 70.86),
												glm::vec3(41.04, 0, 70.19),
												glm::vec3(1.24, 0, 57.5),
												glm::vec3(0.67, 0, 65.97),
												glm::vec3(7.58, 0, 71.57),
												glm::vec3(12.31, 0, 63.65),
												glm::vec3(17.40, 0, 55.73),
												glm::vec3(25.02, 0, 57.69),
												glm::vec3(31.72, 0, 54.92),
												glm::vec3(37.88, 0, 49.03),
												glm::vec3(44.55, 0, 44.53)
};

// Blending model unsorted
std::map<std::string, glm::vec3> blendingUnsorted = {
		{"geiser0", geiserPositions[0]},
		{"geiser1", geiserPositions[1]},
		{"geiser2", geiserPositions[2]},
		{"geiser3", geiserPositions[3]},
		{"geiser4", geiserPositions[4]},
		{"geiser5", geiserPositions[5]},
		{"geiser6", geiserPositions[6]},
		{"geiser7", geiserPositions[7]},
		{"geiser8", geiserPositions[8]},
		{"geiser9", geiserPositions[9]},
		{"geiser10", geiserPositions[10]},
		{"geiser11", geiserPositions[11]},
		{"geiser12", geiserPositions[12]},
		{"geiser13", geiserPositions[13]},
		{"geiser14", geiserPositions[14]},
		{"geiser15", geiserPositions[15]},
		{"geiser16", geiserPositions[16]},
		{"geiser17", geiserPositions[17]},
		{"geiser18", geiserPositions[18]},
		{"geiser19", geiserPositions[19]},
		{"geiser20", geiserPositions[20]},
		{"geiser21", geiserPositions[21]},
		{"geiser22", geiserPositions[22]},
		{"geiser23", geiserPositions[23]},
		{"geiser24", geiserPositions[24]},
		{"geiser25", geiserPositions[25]},
		{"geiser26", geiserPositions[26]},
		{"fire0", firePositions[0]},
		{"fire1", firePositions[1]},
		{"fire2", firePositions[2]},
		{"fire3", firePositions[3]},
		{"fire4", firePositions[4]},
		{"fire5", firePositions[5]},
		{"fire6", firePositions[6]},
		{"fire7", firePositions[7]}
};

double deltaTime;
double currTime, lastTime;

// Definition for the particle system
GLuint initVel, startTime;
GLuint VAOParticles;
GLuint nParticles = 3000;
double currTimeParticlesAnimation, lastTimeParticlesAnimation;

// Definition for the particle system fire
GLuint initVelFire, startTimeFire;
GLuint VAOParticlesFire;
GLuint nParticlesFire = 2000;
GLuint posBuf[2], velBuf[2], age[2];
GLuint particleArray[2];
GLuint feedback[2];
GLuint drawBuf = 1;
float particleSize = 0.5, particleLifetime = 3.0;
double currTimeParticlesAnimationFire, lastTimeParticlesAnimationFire;

// Colliders
std::map<std::string, std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> > collidersOBB;
std::map<std::string, std::tuple<AbstractModel::SBB, glm::mat4, glm::mat4> > collidersSBB;

// Framesbuffers
GLuint depthMap, depthMapFBO;

/**********************
 * OpenAL config
 */

// OpenAL Defines
#define NUM_BUFFERS 3
#define NUM_SOURCES 3
#define NUM_ENVIRONMENTS 1
// Listener
ALfloat listenerPos[] = { 0.0, 0.0, 4.0 };
ALfloat listenerVel[] = { 0.0, 0.0, 0.0 };
ALfloat listenerOri[] = { 0.0, 0.0, 1.0, 0.0, 1.0, 0.0 };
// Source 0
ALfloat source0Pos[] = { -2.0, 0.0, 0.0 };
ALfloat source0Vel[] = { 0.0, 0.0, 0.0 };

// Source 1 Sonido geiser
ALfloat source1Pos[] = { 40.27, 0.0, 77.43 };
ALfloat source1Vel[] = { 0.0, 0.0, 0.0 };

// Source 2 Sonido Antorcha
ALfloat source2Pos[] = { 2.0, 0.0, 0.0 };
ALfloat source2Vel[] = { 0.0, 0.0, 0.0 };
// Buffers
ALuint buffer[NUM_BUFFERS];
ALuint source[NUM_SOURCES];
ALuint environment[NUM_ENVIRONMENTS];
// Configs
ALsizei size, freq;
ALenum format;
ALvoid *data;
int ch;
ALboolean loop;
std::vector<bool> sourcesPlay = {false, true, true};

// Se definen todos las funciones.
void reshapeCallback(GLFWwindow *Window, int widthRes, int heightRes);
void keyCallback(GLFWwindow *window, int key, int scancode, int action,
		int mode);
void mouseCallback(GLFWwindow *window, double xpos, double ypos);
void mouseButtonCallback(GLFWwindow *window, int button, int state, int mod);
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void initParticleBuffers();
void init(int width, int height, std::string strTitle, bool bFullScreen);
void destroy();
bool processInput(bool continueApplication = true);
void gamePad(); //Para operaciones específicas con el gamePad
void prepareScene();
void prepareDepthScene();
void renderScene(bool renderParticles = true);

void initParticleBuffers() {
	// Generate the buffers
	glGenBuffers(1, &initVel);   // Initial velocity buffer
	glGenBuffers(1, &startTime); // Start time buffer

	// Allocate space for all buffers
	int size = nParticles * 3 * sizeof(float);
	glBindBuffer(GL_ARRAY_BUFFER, initVel);
	glBufferData(GL_ARRAY_BUFFER, size, NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, startTime);
	glBufferData(GL_ARRAY_BUFFER, nParticles * sizeof(float), NULL, GL_STATIC_DRAW);

	// Fill the first velocity buffer with random velocities
	glm::vec3 v(0.0f);
	float velocity, theta, phi;
	GLfloat *data = new GLfloat[nParticles * 3];
	for (unsigned int i = 0; i < nParticles; i++) {

		theta = glm::mix(0.0f, glm::pi<float>() / 6.0f, ((float)rand() / RAND_MAX));
		phi = glm::mix(0.0f, glm::two_pi<float>(), ((float)rand() / RAND_MAX));

		// Se genera la velocidad de la partícula en coordenadas esféricas
		v.x = sinf(theta) * cosf(phi);
		v.y = cosf(theta);
		v.z = sinf(theta) * sinf(phi);

		velocity = glm::mix(0.6f, 0.8f, ((float)rand() / RAND_MAX));
		v = glm::normalize(v) * velocity;

		data[3 * i] = v.x;
		data[3 * i + 1] = v.y;
		data[3 * i + 2] = v.z;
	}
	glBindBuffer(GL_ARRAY_BUFFER, initVel);
	glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);

	// Fill the start time buffer
	delete[] data;
	data = new GLfloat[nParticles];
	float time = 0.0f;
	float rate = 0.00075f;
	for (unsigned int i = 0; i < nParticles; i++) {
		data[i] = time;
		time += rate;
	}
	glBindBuffer(GL_ARRAY_BUFFER, startTime);
	glBufferSubData(GL_ARRAY_BUFFER, 0, nParticles * sizeof(float), data);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	delete[] data;

	glGenVertexArrays(1, &VAOParticles);
	glBindVertexArray(VAOParticles);
	glBindBuffer(GL_ARRAY_BUFFER, initVel);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, startTime);
	glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
}

void initParticleBuffersFire() {
	// Generate the buffers
	glGenBuffers(2, posBuf);    // position buffers
	glGenBuffers(2, velBuf);    // velocity buffers
	glGenBuffers(2, age);       // age buffers

	// Allocate space for all buffers
	int size = nParticlesFire * sizeof(GLfloat);
	glBindBuffer(GL_ARRAY_BUFFER, posBuf[0]);
	glBufferData(GL_ARRAY_BUFFER, 3 * size, 0, GL_DYNAMIC_COPY);
	glBindBuffer(GL_ARRAY_BUFFER, posBuf[1]);
	glBufferData(GL_ARRAY_BUFFER, 3 * size, 0, GL_DYNAMIC_COPY);
	glBindBuffer(GL_ARRAY_BUFFER, velBuf[0]);
	glBufferData(GL_ARRAY_BUFFER, 3 * size, 0, GL_DYNAMIC_COPY);
	glBindBuffer(GL_ARRAY_BUFFER, velBuf[1]);
	glBufferData(GL_ARRAY_BUFFER, 3 * size, 0, GL_DYNAMIC_COPY);
	glBindBuffer(GL_ARRAY_BUFFER, age[0]);
	glBufferData(GL_ARRAY_BUFFER, size, 0, GL_DYNAMIC_COPY);
	glBindBuffer(GL_ARRAY_BUFFER, age[1]);
	glBufferData(GL_ARRAY_BUFFER, size, 0, GL_DYNAMIC_COPY);

	// Fill the first age buffer
	std::vector<GLfloat> initialAge(nParticlesFire);
	float rate = particleLifetime / nParticlesFire;
	for(unsigned int i = 0; i < nParticlesFire; i++ ) {
		int index = i - nParticlesFire;
		float result = rate * index;
		initialAge[i] = result;
	}
	// Shuffle them for better looking results
	//Random::shuffle(initialAge);
	auto rng = std::default_random_engine {};
	std::shuffle(initialAge.begin(), initialAge.end(), rng);
	glBindBuffer(GL_ARRAY_BUFFER, age[0]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, size, initialAge.data());

	glBindBuffer(GL_ARRAY_BUFFER,0);

	// Create vertex arrays for each set of buffers
	glGenVertexArrays(2, particleArray);

	// Set up particle array 0
	glBindVertexArray(particleArray[0]);
	glBindBuffer(GL_ARRAY_BUFFER, posBuf[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, velBuf[0]);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, age[0]);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);

	// Set up particle array 1
	glBindVertexArray(particleArray[1]);
	glBindBuffer(GL_ARRAY_BUFFER, posBuf[1]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, velBuf[1]);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, age[1]);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);

	// Setup the feedback objects
	glGenTransformFeedbacks(2, feedback);

	// Transform feedback 0
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, feedback[0]);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, posBuf[0]);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, velBuf[0]);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 2, age[0]);

	// Transform feedback 1
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, feedback[1]);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, posBuf[1]);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, velBuf[1]);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 2, age[1]);

	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
}

// Implementacion de todas las funciones.
void init(int width, int height, std::string strTitle, bool bFullScreen) {

	if (!glfwInit()) {
		std::cerr << "Failed to initialize GLFW" << std::endl;
		exit(-1);
	}

	screenWidth = width;
	screenHeight = height;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	if (bFullScreen)
		window = glfwCreateWindow(width, height, strTitle.c_str(),
				glfwGetPrimaryMonitor(), nullptr);
	else
		window = glfwCreateWindow(width, height, strTitle.c_str(), nullptr,
				nullptr);

	if (window == nullptr) {
		std::cerr
				<< "Error to create GLFW window, you can try download the last version of your video card that support OpenGL 3.3+"
				<< std::endl;
		destroy();
		exit(-1);
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(0);

	glfwSetWindowSizeCallback(window, reshapeCallback);
	glfwSetKeyCallback(window, keyCallback);
	glfwSetCursorPosCallback(window, mouseCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
	glfwSetScrollCallback(window, scrollCallback);
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	

	// Init glew
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (GLEW_OK != err) {
		std::cerr << "Failed to initialize glew" << std::endl;
		exit(-1);
	}

	glViewport(0, 0, screenWidth, screenHeight);
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	// Inicialización de los shaders
	shader.initialize("../Shaders/colorShader.vs", "../Shaders/colorShader.fs");
	shaderSkybox.initialize("../Shaders/skyBox.vs", "../Shaders/skyBox_fog.fs");
	shaderMulLighting.initialize("../Shaders/iluminacion_textura_animation_shadow.vs", "../Shaders/multipleLights_shadow.fs");
	shaderTerrain.initialize("../Shaders/terrain_shadow.vs", "../Shaders/terrain_shadow.fs");
	shaderParticlesGeiser.initialize("../Shaders/particlesFountain.vs", "../Shaders/particlesFountain.fs");
	shaderParticlesFire.initialize("../Shaders/particlesFire.vs", "../Shaders/particlesFire.fs", {"Position", "Velocity", "Age"});
	shaderViewDepth.initialize("../Shaders/texturizado.vs", "../Shaders/texturizado_depth_view.fs");
	shaderDepth.initialize("../Shaders/shadow_mapping_depth.vs", "../Shaders/shadow_mapping_depth.fs");

	// Inicializacion de los objetos.
	skyboxSphere.init();
	skyboxSphere.setShader(&shaderSkybox);
	skyboxSphere.setScale(glm::vec3(100.0f, 100.0f, 100.0f));

	boxCollider.init();
	boxCollider.setShader(&shader);
	boxCollider.setColor(glm::vec4(1.0, 1.0, 1.0, 1.0));

	sphereCollider.init();
	sphereCollider.setShader(&shader);
	sphereCollider.setColor(glm::vec4(1.0, 1.0, 1.0, 1.0));

	boxViewDepth.init();
	boxViewDepth.setShader(&shaderViewDepth);

	boxLightViewBox.init();
	boxLightViewBox.setShader(&shaderViewDepth);

	terrain.init();
	terrain.setShader(&shaderTerrain);
	terrain.setPosition(glm::vec3(100, 0, 100));

	//Grass
	modelGrass.loadModel("../models/grass/grassJurassic.obj");
	modelGrass.setShader(&shaderMulLighting);

	//Geiser
	modelGeiser.loadModel("../models/Geiser/geiser.obj");
	modelGeiser.setShader(&shaderMulLighting);

	//Palm
	modelPalm.loadModel("../models/Palm/palm.obj");
	modelPalm.setShader(&shaderMulLighting);

	//Door
	modelDoor.loadModel("../models/Door/DoorFinal.obj");
	modelDoor.setShader(&shaderMulLighting);

	//Mountain
	modelMountain.loadModel("../models/Mountain/Volcano2.obj");
	modelMountain.setShader(&shaderMulLighting);

	//Mayow
	mayowModelAnimate.loadModel("../models/Walk/Walk.fbx");
	mayowModelAnimate.setShader(&shaderMulLighting);

	camera->setPosition(glm::vec3(0.0, 0.0, 10.0));
	camera->setDistanceFromTarget(distanceFromTarget);
	camera->setSensitivity(1.0);

	cameraFPS->setPosition(glm::vec3(0.0, 6.0, 0.0));

	// Definimos el tamanio de la imagen
	int imageWidth, imageHeight;
	FIBITMAP *bitmap;
	unsigned char *data;

	// Carga de texturas para el skybox
	Texture skyboxTexture = Texture("");
	glGenTextures(1, &skyboxTextureID);
	// Tipo de textura CUBE MAP
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTextureID);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	for (int i = 0; i < ARRAY_SIZE_IN_ELEMENTS(types); i++) {
		skyboxTexture = Texture(fileNames[i]);
		FIBITMAP *bitmap = skyboxTexture.loadImage(true);
		unsigned char *data = skyboxTexture.convertToData(bitmap, imageWidth,
				imageHeight);
		if (data) {
			glTexImage2D(types[i], 0, GL_RGBA, imageWidth, imageHeight, 0,
			GL_BGRA, GL_UNSIGNED_BYTE, data);
		} else
			std::cout << "Failed to load texture" << std::endl;
		skyboxTexture.freeImage(bitmap);
	}

	// Definiendo la textura a utilizar
	Texture textureCesped("../Textures/ca.jpg");
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	bitmap = textureCesped.loadImage();
	// Convertimos el mapa de bits en un arreglo unidimensional de tipo unsigned char
	data = textureCesped.convertToData(bitmap, imageWidth,
			imageHeight);
	// Creando la textura con id 1
	glGenTextures(1, &textureCespedID);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureCespedID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Verifica si se pudo abrir la textura
	if (data) {
		// Transferis los datos de la imagen a memoria
		// Tipo de textura, Mipmaps, Formato interno de openGL, ancho, alto, Mipmaps,
		// Formato interno de la libreria de la imagen, el tipo de dato y al apuntador
		// a los datos
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0,
		GL_BGRA, GL_UNSIGNED_BYTE, data);
		// Generan los niveles del mipmap (OpenGL es el ecargado de realizarlos)
		glGenerateMipmap(GL_TEXTURE_2D);
	} else
		std::cout << "Failed to load texture" << std::endl;
	// Libera la memoria de la textura
	textureCesped.freeImage(bitmap);

	// Definiendo la textura a utilizar
	Texture textureTerrainBackground("../Textures/ca.jpg");
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	bitmap = textureTerrainBackground.loadImage();
	// Convertimos el mapa de bits en un arreglo unidimensional de tipo unsigned char
	data = textureTerrainBackground.convertToData(bitmap, imageWidth,
			imageHeight);
	// Creando la textura con id 1
	glGenTextures(1, &textureTerrainBackgroundID);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureTerrainBackgroundID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Verifica si se pudo abrir la textura
	if (data) {
		// Transferis los datos de la imagen a memoria
		// Tipo de textura, Mipmaps, Formato interno de openGL, ancho, alto, Mipmaps,
		// Formato interno de la libreria de la imagen, el tipo de dato y al apuntador
		// a los datos
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0,
		GL_BGRA, GL_UNSIGNED_BYTE, data);
		// Generan los niveles del mipmap (OpenGL es el ecargado de realizarlos)
		glGenerateMipmap(GL_TEXTURE_2D);
	} else
		std::cout << "Failed to load texture" << std::endl;
	// Libera la memoria de la textura
	textureTerrainBackground.freeImage(bitmap);

	// Definiendo la textura a utilizar
	Texture textureTerrainR("../Textures/arena.jpg");
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	bitmap = textureTerrainR.loadImage();
	// Convertimos el mapa de bits en un arreglo unidimensional de tipo unsigned char
	data = textureTerrainR.convertToData(bitmap, imageWidth,
			imageHeight);
	// Creando la textura con id 1
	glGenTextures(1, &textureTerrainRID);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureTerrainRID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Verifica si se pudo abrir la textura
	if (data) {
		// Transferis los datos de la imagen a memoria
		// Tipo de textura, Mipmaps, Formato interno de openGL, ancho, alto, Mipmaps,
		// Formato interno de la libreria de la imagen, el tipo de dato y al apuntador
		// a los datos
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0,
		GL_BGRA, GL_UNSIGNED_BYTE, data);
		// Generan los niveles del mipmap (OpenGL es el ecargado de realizarlos)
		glGenerateMipmap(GL_TEXTURE_2D);
	} else
		std::cout << "Failed to load texture" << std::endl;
	// Libera la memoria de la textura
	textureTerrainR.freeImage(bitmap);

	// Definiendo la textura a utilizar
	Texture textureTerrainG("../Textures/lava.jpg");
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	bitmap = textureTerrainG.loadImage();
	// Convertimos el mapa de bits en un arreglo unidimensional de tipo unsigned char
	data = textureTerrainG.convertToData(bitmap, imageWidth,
			imageHeight);
	// Creando la textura con id 1
	glGenTextures(1, &textureTerrainGID);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureTerrainGID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Verifica si se pudo abrir la textura
	if (data) {
		// Transferis los datos de la imagen a memoria
		// Tipo de textura, Mipmaps, Formato interno de openGL, ancho, alto, Mipmaps,
		// Formato interno de la libreria de la imagen, el tipo de dato y al apuntador
		// a los datos
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0,
		GL_BGRA, GL_UNSIGNED_BYTE, data);
		// Generan los niveles del mipmap (OpenGL es el ecargado de realizarlos)
		glGenerateMipmap(GL_TEXTURE_2D);
	} else
		std::cout << "Failed to load texture" << std::endl;
	// Libera la memoria de la textura
	textureTerrainG.freeImage(bitmap);

	// Definiendo la textura a utilizar
	Texture textureTerrainB("../Textures/terraceria.png");
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	bitmap = textureTerrainB.loadImage();
	// Convertimos el mapa de bits en un arreglo unidimensional de tipo unsigned char
	data = textureTerrainB.convertToData(bitmap, imageWidth,
			imageHeight);
	// Creando la textura con id 1
	glGenTextures(1, &textureTerrainBID);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureTerrainBID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Verifica si se pudo abrir la textura
	if (data) {
		// Transferis los datos de la imagen a memoria
		// Tipo de textura, Mipmaps, Formato interno de openGL, ancho, alto, Mipmaps,
		// Formato interno de la libreria de la imagen, el tipo de dato y al apuntador
		// a los datos
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0,
		GL_BGRA, GL_UNSIGNED_BYTE, data);
		// Generan los niveles del mipmap (OpenGL es el ecargado de realizarlos)
		glGenerateMipmap(GL_TEXTURE_2D);
	} else
		std::cout << "Failed to load texture" << std::endl;
	// Libera la memoria de la textura
	textureTerrainB.freeImage(bitmap);

	// Definiendo la textura a utilizar
	Texture textureTerrainBlendMap("../Textures/blendMapJurassic.png");
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	bitmap = textureTerrainBlendMap.loadImage(true);
	// Convertimos el mapa de bits en un arreglo unidimensional de tipo unsigned char
	data = textureTerrainBlendMap.convertToData(bitmap, imageWidth,
			imageHeight);
	// Creando la textura con id 1
	glGenTextures(1, &textureTerrainBlendMapID);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureTerrainBlendMapID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Verifica si se pudo abrir la textura
	if (data) {
		// Transferis los datos de la imagen a memoria
		// Tipo de textura, Mipmaps, Formato interno de openGL, ancho, alto, Mipmaps,
		// Formato interno de la libreria de la imagen, el tipo de dato y al apuntador
		// a los datos
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0,
		GL_BGRA, GL_UNSIGNED_BYTE, data);
		// Generan los niveles del mipmap (OpenGL es el ecargado de realizarlos)
		glGenerateMipmap(GL_TEXTURE_2D);
	} else
		std::cout << "Failed to load texture" << std::endl;
	// Libera la memoria de la textura
	textureTerrainBlendMap.freeImage(bitmap);

	Texture textureParticlesGeiser("../Textures/vapor.png");
	bitmap = textureParticlesGeiser.loadImage();
	data = textureParticlesGeiser.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &textureParticlesGeiserID);
	glBindTexture(GL_TEXTURE_2D, textureParticlesGeiserID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0,
			GL_BGRA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	textureParticlesGeiser.freeImage(bitmap);

	Texture textureParticleFire("../Textures/fire.png");
	bitmap = textureParticleFire.loadImage();
	data = textureParticleFire.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &textureParticleFireID);
	glBindTexture(GL_TEXTURE_2D, textureParticleFireID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0,
			GL_BGRA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	textureParticleFire.freeImage(bitmap);

	std::uniform_real_distribution<float> distr01 = std::uniform_real_distribution<float>(0.0f, 1.0f);
	std::mt19937 generator;
	std::random_device rd;
	generator.seed(rd());
	int size = nParticlesFire * 2;
	std::vector<GLfloat> randData(size);
	for( int i = 0; i < randData.size(); i++ ) {
		randData[i] = distr01(generator);
	}

	glGenTextures(1, &texId);
	glBindTexture(GL_TEXTURE_1D, texId);
	glTexStorage1D(GL_TEXTURE_1D, 1, GL_R32F, size);
	glTexSubImage1D(GL_TEXTURE_1D, 0, 0, size, GL_RED, GL_FLOAT, randData.data());
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	shaderParticlesFire.setInt("Pass", 1);
	shaderParticlesFire.setInt("ParticleTex", 0);
	shaderParticlesFire.setInt("RandomTex", 1);
	shaderParticlesFire.setFloat("ParticleLifetime", particleLifetime);
	shaderParticlesFire.setFloat("ParticleSize", particleSize);
	shaderParticlesFire.setVectorFloat3("Accel", glm::value_ptr(glm::vec3(0.0f,0.1f,0.0f)));
	shaderParticlesFire.setVectorFloat3("Emitter", glm::value_ptr(glm::vec3(0.0f)));

	glm::mat3 basis;
	glm::vec3 u, v, n;
	v = glm::vec3(0,1,0);
	n = glm::cross(glm::vec3(1,0,0), v);
	if( glm::length(n) < 0.00001f ) {
		n = glm::cross(glm::vec3(0,1,0), v);
	}
	u = glm::cross(v,n);
	basis[0] = glm::normalize(u);
	basis[1] = glm::normalize(v);
	basis[2] = glm::normalize(n);
	shaderParticlesFire.setMatrix3("EmitterBasis", 1, false, glm::value_ptr(basis));

	/*******************************************
	 * Inicializacion de los buffers de la fuente
	 *******************************************/
	initParticleBuffers();

	/*******************************************
	 * Inicializacion de los buffers del fuego
	 *******************************************/
	initParticleBuffersFire();

	/*******************************************
	 * Inicializacion del framebuffer para
	 * almacenar el buffer de profunidadad
	 *******************************************/
	glGenFramebuffers(1, &depthMapFBO);
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
				 SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	/*glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);*/
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	/*******************************************
	 * OpenAL init
	 *******************************************/
	alutInit(0, nullptr);
	alListenerfv(AL_POSITION, listenerPos);
	alListenerfv(AL_VELOCITY, listenerVel);
	alListenerfv(AL_ORIENTATION, listenerOri);
	alGetError(); // clear any error messages
	if (alGetError() != AL_NO_ERROR) {
		printf("- Error creating buffers !!\n");
		exit(1);
	}
	else {
		printf("init() - No errors yet.");
	}
	// Config source 0
	// Generate buffers, or else no sound will happen!
	alGenBuffers(NUM_BUFFERS, buffer);
	buffer[0] = alutCreateBufferFromFile("../sounds/caminar.wav");
	buffer[1] = alutCreateBufferFromFile("../sounds/geiser.wav");
	buffer[2] = alutCreateBufferFromFile("../sounds/antorcha.wav");
	int errorAlut = alutGetError();
	if (errorAlut != ALUT_ERROR_NO_ERROR){
		printf("- Error open files with alut %d !!\n", errorAlut);
		exit(2);
	}


	alGetError(); /* clear error */
	alGenSources(NUM_SOURCES, source);

	if (alGetError() != AL_NO_ERROR) {
		printf("- Error creating sources !!\n");
		exit(2);
	}
	else {
		printf("init - no errors after alGenSources\n");
	}
	alSourcef(source[0], AL_PITCH, 1.0f);
	alSourcef(source[0], AL_GAIN, 0.3f);
	alSourcefv(source[0], AL_POSITION, source0Pos);
	alSourcefv(source[0], AL_VELOCITY, source0Vel);
	alSourcei(source[0], AL_BUFFER, buffer[0]);
	alSourcei(source[0], AL_LOOPING, AL_TRUE);
	alSourcef(source[0], AL_MAX_DISTANCE, 2000);

	//Funte de sonido de los geiser

	alSourcef(source[1], AL_PITCH, 3.0f);
	alSourcef(source[1], AL_GAIN, 6.0f);
	alSourcefv(source[1], AL_POSITION, source1Pos);
	alSourcefv(source[1], AL_VELOCITY, source1Vel);
	alSourcei(source[1], AL_BUFFER, buffer[1]);
	alSourcei(source[1], AL_LOOPING, AL_TRUE);
	alSourcef(source[1], AL_MAX_DISTANCE, 1500);

	//Fuente de sonido de la antorcha (puerta)
	alSourcef(source[2], AL_PITCH, 1.0f);
	alSourcef(source[2], AL_GAIN, 4.0f);
	alSourcefv(source[2], AL_POSITION, source2Pos);
	alSourcefv(source[2], AL_VELOCITY, source2Vel);
	alSourcei(source[2], AL_BUFFER, buffer[2]);
	alSourcei(source[2], AL_LOOPING, AL_TRUE);
	alSourcef(source[2], AL_MAX_DISTANCE, 500);
}

void destroy() {
	glfwDestroyWindow(window);
	glfwTerminate();
	// --------- IMPORTANTE ----------
	// Eliminar los shader y buffers creados.

	// Shaders Delete
	shader.destroy();
	shaderMulLighting.destroy();
	shaderSkybox.destroy();
	shaderTerrain.destroy();
	shaderParticlesGeiser.destroy();
	shaderParticlesFire.destroy();

	// Basic objects Delete
	skyboxSphere.destroy();
	boxCollider.destroy();
	sphereCollider.destroy();
	boxViewDepth.destroy();
	boxLightViewBox.destroy();

	// Terrains objects Delete
	terrain.destroy();

	// Custom objects Delete
	modelGrass.destroy();
	modelGeiser.destroy();
	modelPalm.destroy();
	modelDoor.destroy();
	modelMountain.destroy();

	// Custom objects animate
	mayowModelAnimate.destroy();

	// Textures Delete
	glBindTexture(GL_TEXTURE_2D, 0);
	glDeleteTextures(1, &textureCespedID);
	glDeleteTextures(1, &textureTerrainBackgroundID);
	glDeleteTextures(1, &textureTerrainRID);
	glDeleteTextures(1, &textureTerrainGID);
	glDeleteTextures(1, &textureTerrainBID);
	glDeleteTextures(1, &textureTerrainBlendMapID);
	glDeleteTextures(1, &textureParticlesGeiserID);
	glDeleteTextures(1, &textureParticleFireID);

	// Cube Maps Delete
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	glDeleteTextures(1, &skyboxTextureID);

	// Remove the buffer of the fountain particles
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &initVel);
	glDeleteBuffers(1, &startTime);
	glBindVertexArray(0);
	glDeleteVertexArrays(1, &VAOParticles);

	// Remove the buffer of the fire particles
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(2, posBuf);
	glDeleteBuffers(2, velBuf);
	glDeleteBuffers(2, age);
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
	glDeleteTransformFeedbacks(2, feedback);
	glBindVertexArray(0);
	glDeleteVertexArrays(1, &VAOParticlesFire);
}

void reshapeCallback(GLFWwindow *Window, int widthRes, int heightRes) {
	screenWidth = widthRes;
	screenHeight = heightRes;
	glViewport(0, 0, widthRes, heightRes);
}

void keyCallback(GLFWwindow *window, int key, int scancode, int action,
		int mode) {
	if (action == GLFW_PRESS) {
		switch (key) {
		case GLFW_KEY_ESCAPE:
			exitApp = true;
			break;
		}
	}
}

void mouseCallback(GLFWwindow *window, double xpos, double ypos) {
	offsetX = xpos - lastMousePosX;
	offsetY = ypos - lastMousePosY;
	lastMousePosX = xpos;
	lastMousePosY = ypos;
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset){
	distanceFromTarget -= yoffset;
	camera->setDistanceFromTarget(distanceFromTarget);
}

void mouseButtonCallback(GLFWwindow *window, int button, int state, int mod) {
	if (state == GLFW_PRESS) {
		switch (button) {
		case GLFW_MOUSE_BUTTON_RIGHT:
			std::cout << "lastMousePos.y:" << lastMousePosY << std::endl;
			break;
		case GLFW_MOUSE_BUTTON_LEFT:
			std::cout << "lastMousePos.x:" << lastMousePosX << std::endl;
			break;
		case GLFW_MOUSE_BUTTON_MIDDLE:
			std::cout << "lastMousePos.x:" << lastMousePosX << std::endl;
			std::cout << "lastMousePos.y:" << lastMousePosY << std::endl;
			break;
		}
	}
}


bool processInput(bool continueApplication) {
	if (exitApp || glfwWindowShouldClose(window) != 0) {
		return false;
	}

	//Seleccionar método de entrada
	if (enableCountSelected && glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS){
		enableCountSelected = false;
		inputMethod = false; //Para manejar teclado
		glfwSetWindowTitle(window, "Window GLFW - Teclado");
	}
	else if(glfwGetKey(window, GLFW_KEY_1) == GLFW_RELEASE) {
		enableCountSelected = true;
	}
	
	if (enableCountSelected && glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
		enableCountSelected = false;
		inputMethod = true; //Para manejar gamePad
		glfwSetWindowTitle(window, "Window GLFW - GamePad");
	}
	else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_RELEASE) {
		enableCountSelected = true;
	}
	
	if (!inputMethod) {
		//Seleccionar cámara TPS ó FPS
		if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
			stateCamera = 1;
		}
		if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
			stateCamera = 2;
		}
		//Fin de selección de cámara TPS ó FPS

		if (stateCamera == 2) {
			if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
				cameraFPS->moveFrontCamera(true, deltaTime);
			if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
				cameraFPS->moveFrontCamera(false, deltaTime);
			if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
				cameraFPS->moveRightCamera(false, deltaTime);
			if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
				cameraFPS->moveRightCamera(true, deltaTime);
			if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
				cameraFPS->mouseMoveCamera(offsetX, offsetY, deltaTime);
			offsetX = 0;
			offsetY = 0;
		}
		else if (stateCamera == 1) {
			if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
				camera->mouseMoveCamera(offsetX, 0.0, deltaTime);
			if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
				camera->mouseMoveCamera(0.0, offsetY, deltaTime);
			offsetX = 0;
			offsetY = 0;
		}



		// Seleccionar modelo
		if (enableCountSelected && glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS) {
			enableCountSelected = false;
			modelSelected++;
			if (modelSelected > 2)
				modelSelected = 0;
			if (modelSelected == 1)

				if (modelSelected == 2)
					std::cout << "modelSelected:" << modelSelected << std::endl;
		}
		else if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_RELEASE)
			enableCountSelected = true;


		if (stateCamera == 1) {
			if (modelSelected == 2 && glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
				modelMatrixMayow = glm::rotate(modelMatrixMayow, glm::radians(1.0f), glm::vec3(0, 1, 0));
				animationIndex = 1;
			}
			else if (modelSelected == 2 && glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
				modelMatrixMayow = glm::rotate(modelMatrixMayow, glm::radians(-1.0f), glm::vec3(0, 1, 0));
				animationIndex = 1;
			}if (modelSelected == 2 && glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
				modelMatrixMayow = glm::translate(modelMatrixMayow, glm::vec3(0, 0, 0.02));
				animationIndex = 1;
			}
			else if (modelSelected == 2 && glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
				modelMatrixMayow = glm::translate(modelMatrixMayow, glm::vec3(0, 0, -0.02));
				animationIndex = 1;
			}

			if (modelSelected == 2 && (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_RELEASE &&
									   glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_RELEASE &&
									   glfwGetKey(window, GLFW_KEY_UP) == GLFW_RELEASE &&
									   glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_RELEASE)) 
				animationIndex = 0;
		}
	}
	else {
		gamePad();
	}
	
	glfwPollEvents();
	return continueApplication;
}

void gamePad() {
	//para poner el gamepad
	present = glfwJoystickPresent(GLFW_JOYSTICK_1);
	axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &axesCount);
	buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &buttonCount);
	name = glfwGetJoystickName(GLFW_JOYSTICK_1);
	glm::vec3 posCam;
	//std::cout << "Nombre del joystick: " << name << std::endl;
	//std::cout << "Estado del gamepad: " << present << std::endl;
	//------------------------------------------Inicia GamePad
	if (present == 1)
	{
		//std::cout << "Ejes disponibles: " << axesCount << std::endl;
		/*std::cout << std::endl;
		std::cout << "Left stick X Axis: " << axes[0] << std::endl;
		std::cout << "Left stick Y Axis: " << axes[1] << std::endl;
		std::cout << "Right stick X Axis: " << axes[2] << std::endl;
		std::cout << "Right stick Y Axis: " << axes[3] << std::endl;
		std::cout << "Left trigger/L2: " << axes[4] << std::endl;
		std::cout << "Right trigger/R2: " << axes[5] << std::endl;*/

		//----------------------Para el movimiento traslación y rotación del personaje con el gamepad
		if (stateCamera == 1) { //Cámara TPS

			switch (modelSelected) { 
			case 2:

				modelMatrixMayow = glm::translate(modelMatrixMayow,
					glm::vec3(0, 0, 0.02 * velModel * axes[1]));
				modelMatrixMayow = glm::rotate(modelMatrixMayow,
					glm::radians(-0.8f * axes[0]),
					glm::vec3(0, 1, 0));
				float ax = axes[0];
				float ay = axes[1];
				if (ax == 0.000000000 && ay == 0.000000000) {
					animationIndex = 0; //Detenido
				}
				else {
					animationIndex = 1; //Caminando
				}
				
				camera->mouseMoveCamera(axes[2], axes[3], deltaTime);

				break;
			}
		}
		else if (stateCamera == 2) { //Cámara FPS
			cameraFPS->moveFrontCamera(true, axes[1] * 0.05);
			cameraFPS->moveRightCamera(true, axes[0] * 0.05);
			cameraFPS->mouseMoveCamera(axes[2]*2.0, axes[3]*2.0, deltaTime);
			offsetX = 0;
			offsetY = 0;
		}
		//------------------------------------ Fin de rotación y traslación de los modelos

		//------------------------------------------------------------------Botones del mando
		

		/*Botón A*/
		if (GLFW_PRESS == buttons[0])
		{
			//std::cout << "A button presed" << std::endl;
			//camera->moveFrontCamera(true, deltaTime);
			if (stateCamera == 1)
				velModel = 2; //Aumentar la velocidad
		}
		if (GLFW_RELEASE == buttons[0])
		{
			//std::cout << "A button is released" << std::endl;
			if (stateCamera == 1)
				velModel = 1; //Disminuir la velocidad
		}

		/*Botón B*/
		if (GLFW_PRESS == buttons[1])
		{
			std::cout << "B button presed: " + std::to_string(cameraFPS->getPosition().x) + ", " + std::to_string(cameraFPS->getPosition().z) << std::endl;
		}
		if (GLFW_RELEASE == buttons[1])
		{
			//std::cout << "B button is released" << std::endl;
		}

		/*Botón X*/
		if (GLFW_PRESS == buttons[2])
		{
			std::cout << "X button presed" << std::endl;
			//camera->moveFrontCamera(true, deltaTime);
		}
		if (GLFW_RELEASE == buttons[2])
		{
			//std::cout << "B button is released" << std::endl;
		}

		/*Botón Y*/
		if (GLFW_PRESS == buttons[3])
		{
			std::cout << "Y button presed" << std::endl;
			//camera->moveFrontCamera(true, deltaTime);
		}
		if (GLFW_RELEASE == buttons[3])
		{
			//std::cout << "B button is released" << std::endl;
		}

		/*Boton LB*/
		if (GLFW_PRESS == buttons[4])
		{
			std::cout << "LB button presed" << std::endl;
			//camera->moveFrontCamera(true, deltaTime);
		}
		if (GLFW_RELEASE == buttons[4])
		{
			//std::cout << "LB button is released" << std::endl;
		}

		/*Botón RB*/
		if (GLFW_PRESS == buttons[5])
		{
			std::cout << "RB button presed" << std::endl;
			//camera->moveFrontCamera(true, deltaTime);
		}
		if (GLFW_RELEASE == buttons[5])
		{
			//std::cout << "RB button is released" << std::endl;
		}

		/*Boton Select*/
		if (enableCountSelectedGamePad && GLFW_PRESS == buttons[6])
		{
			std::cout << "Select button presed" << std::endl;
			enableCountSelectedGamePad = false; //Controlar la selección de la cámara
			stateCamera++;
			if (stateCamera > 2) stateCamera = 1;

		}
		else if (GLFW_RELEASE == buttons[6])
		{
			//std::cout << "Select button is released" << std::endl;
			enableCountSelectedGamePad = true;
		}

		/*Boton Start*/
		if (GLFW_PRESS == buttons[7])
		{
			std::cout << "Start button presed" << std::endl;
			//camera->moveFrontCamera(true, deltaTime);
		}
		if (GLFW_RELEASE == buttons[7])
		{
			//std::cout << "Start button is released" << std::endl;
		}

		/*Boton joystick izquierdo*/
		if (GLFW_PRESS == buttons[8])
		{
			std::cout << "Left Joystick button presed" << std::endl;
			//camera->moveFrontCamera(true, deltaTime);
			switch (stateCamera) //Camara TPS
			{
			case 1:
				if(distanceFromTarget > 1.0) //Para topar la cámara
					distanceFromTarget -= 0.1;
				camera->setDistanceFromTarget(distanceFromTarget);
				break;
			}
		}
		else if (GLFW_RELEASE == buttons[8])
		{
			//std::cout << "Start button is released" << std::endl;

		}

		/*Boton joystick derecho*/
		if (GLFW_PRESS == buttons[9])
		{
			std::cout << "Rigth Joystick button presed" << std::endl;
			//camera->moveFrontCamera(true, deltaTime);
			switch (stateCamera)  //Camara TPS
			{
			case 1:
				distanceFromTarget += 0.1;
				camera->setDistanceFromTarget(distanceFromTarget);
				break;
			}
		}
		else if (GLFW_RELEASE == buttons[9])
		{
			//std::cout << "Start button is released" << std::endl;
		}

		/*Boton Arriba*/
		if (GLFW_PRESS == buttons[10])
		{
			std::cout << "Up button presed" << std::endl;
			//camera->moveFrontCamera(true, deltaTime);
		}
		if (GLFW_RELEASE == buttons[10])
		{
			//std::cout << "Up button is released" << std::endl;
		}

		/*Boton Derecha*/
		if (GLFW_PRESS == buttons[11])
		{
			std::cout << "Rigth button presed" << std::endl;
			//camera->moveFrontCamera(true, deltaTime);
		}
		if (GLFW_RELEASE == buttons[11])
		{
			//std::cout << "Up button is released" << std::endl;
		}

		/*Boton Abajo*/
		if (GLFW_PRESS == buttons[12])
		{
			std::cout << "Down button presed" << std::endl;
			//camera->moveFrontCamera(true, deltaTime);
		}
		if (GLFW_RELEASE == buttons[12])
		{
			//std::cout << "Down button is released" << std::endl;
		}

		/*Boton Izquierda*/
		if (GLFW_PRESS == buttons[13])
		{
			std::cout << "Left button presed" << std::endl;
			//camera->moveFrontCamera(true, deltaTime);
		}
		if (GLFW_RELEASE == buttons[13])
		{
			//std::cout << "Left button is released" << std::endl;
		}
		//------------------------------------------------------Fin botones del mando

		
		//std::cout << "Botones disponibles: " << buttonCount << std::endl;
		//system("cls");

	}
	else {
		glfwSetWindowTitle(window, "Window GLFW - GamePad No hay GamePad conectado");
	}
	//--------------------Fin gamepad
}

void applicationLoop() {
	bool psi = true;

	glm::mat4 view;
	glm::vec3 axis;
	glm::vec3 target;
	float angleTarget;

	//modelMatrixDart = glm::translate(modelMatrixDart, glm::vec3(3.0, 0.0, 20.0));

	modelMatrixMayow = glm::translate(modelMatrixMayow, glm::vec3(0.0f, 0.05f, -90.0f));
	modelMatrixMayow = glm::rotate(modelMatrixMayow, glm::radians(-90.0f), glm::vec3(0, 1, 0));

	/*modelMatrixFountain = glm::translate(modelMatrixFountain, glm::vec3(5.0, 0.0, -40.0));
	modelMatrixFountain[3][1] = terrain.getHeightTerrain(modelMatrixFountain[3][0] , modelMatrixFountain[3][2]) + 0.2;
	modelMatrixFountain = glm::scale(modelMatrixFountain, glm::vec3(10.0f, 10.0f, 10.0f));*/

	modelMatrixGeiser = glm::translate(modelMatrixGeiser, glm::vec3(20.0f, 0.0f, 30.0f));
	modelMatrixGeiser[3][1] = terrain.getHeightTerrain(modelMatrixGeiser[3][0], modelMatrixGeiser[3][2]);

	modelMatrixDoor = glm::translate(modelMatrixDoor, glm::vec3(0.0, 0.0, -95.0));
	modelMatrixDoor[3][1] = terrain.getHeightTerrain(modelMatrixDoor[3][0], modelMatrixDoor[3][2]);
	modelMatrixDoor = glm::rotate(modelMatrixDoor, glm::radians(180.0f), glm::vec3(0, 1, 0));

	modelMatrixMountain = glm::translate(modelMatrixMountain, glm::vec3(-10.0, 0.0, 10.0));
	modelMatrixMountain[3][1] = terrain.getHeightTerrain(modelMatrixMountain[3][0], modelMatrixMountain[3][2]);
	//modelMatrixMountain = glm::rotate(modelMatrixMountain, glm::radians(90.0f), glm::vec3(0, 1, 0));
	modelMatrixMountain = glm::scale(modelMatrixMountain, glm::vec3(0.2f, 0.2f, 0.2f));

	// Variables to interpolation key frames
	/*fileName = "../animaciones/animation_dart_joints.txt";
	keyFramesDartJoints = getKeyRotFrames(fileName);
	keyFramesDart = getKeyFrames("../animaciones/animation_dart.txt");*/

	lastTime = TimeManager::Instance().GetTime();

	// Time for the particles animation
	currTimeParticlesAnimation = lastTime;
	lastTimeParticlesAnimation = lastTime;

	currTimeParticlesAnimationFire = lastTime;
	lastTimeParticlesAnimationFire = lastTime;

	glm::vec3 lightPos = glm::vec3(10.0, 10.0, 0.0);

	shadowBox = new ShadowBox(-lightPos, camera.get(), 15.0f, 0.1f, 45.0f);

	while (psi) {
		currTime = TimeManager::Instance().GetTime();
		if(currTime - lastTime < 0.016666667){
			glfwPollEvents();
			continue;
		}
		lastTime = currTime;
		TimeManager::Instance().CalculateFrameRate(true);
		deltaTime = TimeManager::Instance().DeltaTime;
		psi = processInput(true);

		std::map<std::string, bool> collisionDetection;

		// Variables donde se guardan las matrices de cada articulacion por 1 frame
		std::vector<float> matrixDartJoints;
		std::vector<glm::mat4> matrixDart;

		glm::mat4 projection = glm::perspective(glm::radians(45.0f),
				(float) screenWidth / (float) screenHeight, 0.1f, 100.0f);

		if(modelSelected == 2){
			axis = glm::axis(glm::quat_cast(modelMatrixMayow));
			angleTarget = glm::angle(glm::quat_cast(modelMatrixMayow));
			target = modelMatrixMayow[3];
		}
		else{
			
		}

		if(std::isnan(angleTarget))
			angleTarget = 0.0;
		if(axis.y < 0)
			angleTarget = -angleTarget;
		if(modelSelected == 1)
			angleTarget -= glm::radians(90.0f);
		camera->setCameraTarget(target);
		camera->setAngleTarget(angleTarget);
		camera->updateCamera();

		if (stateCamera == 1) { //La matriz de vista varía dependiendo de la selección de la cámara
			view = camera->getViewMatrix();
		}

		if (stateCamera == 2) {
			view = cameraFPS->getViewMatrix();
		}

		shadowBox->update(screenWidth, screenHeight);
		glm::vec3 centerBox = shadowBox->getCenter();

		// Projection light shadow mapping
		glm::mat4 lightProjection = glm::mat4(1.0f), lightView = glm::mat4(1.0f);
		glm::mat4 lightSpaceMatrix;

		lightProjection[0][0] = 2.0f / shadowBox->getWidth();
		lightProjection[1][1] = 2.0f / shadowBox->getHeight();
		lightProjection[2][2] = -2.0f / shadowBox->getLength();
		lightProjection[3][3] = 1.0f;

		lightView = glm::lookAt(centerBox, centerBox + glm::normalize(-lightPos), glm::vec3(0.0, 1.0, 0.0));

		lightSpaceMatrix = lightProjection * lightView;
		shaderDepth.setMatrix4("lightSpaceMatrix", 1, false, glm::value_ptr(lightSpaceMatrix));

		// Settea la matriz de vista y projection al shader con solo color
		shader.setMatrix4("projection", 1, false, glm::value_ptr(projection));
		shader.setMatrix4("view", 1, false, glm::value_ptr(view));

		// Settea la matriz de vista y projection al shader con skybox
		shaderSkybox.setMatrix4("projection", 1, false,
				glm::value_ptr(projection));
		shaderSkybox.setMatrix4("view", 1, false,
				glm::value_ptr(glm::mat4(glm::mat3(view))));
		// Settea la matriz de vista y projection al shader con multiples luces
		shaderMulLighting.setMatrix4("projection", 1, false,
					glm::value_ptr(projection));
		shaderMulLighting.setMatrix4("view", 1, false,
				glm::value_ptr(view));
		shaderMulLighting.setMatrix4("lightSpaceMatrix", 1, false,
				glm::value_ptr(lightSpaceMatrix));
		// Settea la matriz de vista y projection al shader con multiples luces
		shaderTerrain.setMatrix4("projection", 1, false,
					glm::value_ptr(projection));
		shaderTerrain.setMatrix4("view", 1, false,
				glm::value_ptr(view));
		shaderTerrain.setMatrix4("lightSpaceMatrix", 1, false,
				glm::value_ptr(lightSpaceMatrix));
		// Settea la matriz de vista y projection al shader para el fountain
		shaderParticlesGeiser.setMatrix4("projection", 1, false,
					glm::value_ptr(projection));
		shaderParticlesGeiser.setMatrix4("view", 1, false,
				glm::value_ptr(view));
		// Settea la matriz de vista y projection al shader para el fuego
		shaderParticlesFire.setInt("Pass", 2);
		shaderParticlesFire.setMatrix4("projection", 1, false, glm::value_ptr(projection));
		shaderParticlesFire.setMatrix4("view", 1, false, glm::value_ptr(view));

		/*******************************************
		 * Propiedades de neblina
		 *******************************************/
		shaderMulLighting.setVectorFloat3("fogColor", glm::value_ptr(glm::vec3(0.5, 0.5, 0.4)));
		shaderTerrain.setVectorFloat3("fogColor", glm::value_ptr(glm::vec3(0.5, 0.5, 0.4)));
		shaderSkybox.setVectorFloat3("fogColor", glm::value_ptr(glm::vec3(0.5, 0.5, 0.4)));

		/*******************************************
		 * Propiedades Luz direccional
		 *******************************************/
		shaderMulLighting.setVectorFloat3("viewPos", glm::value_ptr(camera->getPosition()));
		shaderMulLighting.setVectorFloat3("directionalLight.light.ambient", glm::value_ptr(glm::vec3(0.2, 0.2, 0.2)));
		shaderMulLighting.setVectorFloat3("directionalLight.light.diffuse", glm::value_ptr(glm::vec3(0.5, 0.5, 0.5)));
		shaderMulLighting.setVectorFloat3("directionalLight.light.specular", glm::value_ptr(glm::vec3(0.2, 0.2, 0.2)));
		shaderMulLighting.setVectorFloat3("directionalLight.direction", glm::value_ptr(glm::vec3(-0.707106781, -0.707106781, 0.0)));

		/*******************************************
		 * Propiedades Luz direccional Terrain
		 *******************************************/
		shaderTerrain.setVectorFloat3("viewPos", glm::value_ptr(camera->getPosition()));
		shaderTerrain.setVectorFloat3("directionalLight.light.ambient", glm::value_ptr(glm::vec3(0.2, 0.2, 0.2)));
		shaderTerrain.setVectorFloat3("directionalLight.light.diffuse", glm::value_ptr(glm::vec3(0.5, 0.5, 0.5)));
		shaderTerrain.setVectorFloat3("directionalLight.light.specular", glm::value_ptr(glm::vec3(0.2, 0.2, 0.2)));
		shaderTerrain.setVectorFloat3("directionalLight.direction", glm::value_ptr(glm::vec3(-0.707106781, -0.707106781, 0.0)));

		/*******************************************
		 * Propiedades SpotLights
		 *******************************************/
		//glm::vec3 spotPosition = glm::vec3(modelMatrixHeli * glm::vec4(0.32437, 0.226053, 1.79149, 1.0));
		//shaderMulLighting.setInt("spotLightCount", 1);
		//shaderTerrain.setInt("spotLightCount", 1);
		//shaderMulLighting.setVectorFloat3("spotLights[0].light.ambient", glm::value_ptr(glm::vec3(0.0, 0.0, 0.0)));
		//shaderMulLighting.setVectorFloat3("spotLights[0].light.diffuse", glm::value_ptr(glm::vec3(0.2, 0.3, 0.2)));
		//shaderMulLighting.setVectorFloat3("spotLights[0].light.specular", glm::value_ptr(glm::vec3(0.3, 0.3, 0.3)));
		//shaderMulLighting.setVectorFloat3("spotLights[0].position", glm::value_ptr(spotPosition));
		//shaderMulLighting.setVectorFloat3("spotLights[0].direction", glm::value_ptr(glm::vec3(0, -1, 0)));
		//shaderMulLighting.setFloat("spotLights[0].constant", 1.0);
		//shaderMulLighting.setFloat("spotLights[0].linear", 0.074);
		//shaderMulLighting.setFloat("spotLights[0].quadratic", 0.03);
		//shaderMulLighting.setFloat("spotLights[0].cutOff", cos(glm::radians(12.5f)));
		//shaderMulLighting.setFloat("spotLights[0].outerCutOff", cos(glm::radians(15.0f)));
		//shaderTerrain.setVectorFloat3("spotLights[0].light.ambient", glm::value_ptr(glm::vec3(0.0, 0.0, 0.0)));
		//shaderTerrain.setVectorFloat3("spotLights[0].light.diffuse", glm::value_ptr(glm::vec3(0.2, 0.3, 0.2)));
		//shaderTerrain.setVectorFloat3("spotLights[0].light.specular", glm::value_ptr(glm::vec3(0.3, 0.3, 0.3)));
		//shaderTerrain.setVectorFloat3("spotLights[0].position", glm::value_ptr(spotPosition));
		//shaderTerrain.setVectorFloat3("spotLights[0].direction", glm::value_ptr(glm::vec3(0, -1, 0)));
		//shaderTerrain.setFloat("spotLights[0].constant", 1.0);
		//shaderTerrain.setFloat("spotLights[0].linear", 0.074);
		//shaderTerrain.setFloat("spotLights[0].quadratic", 0.03);
		//shaderTerrain.setFloat("spotLights[0].cutOff", cos(glm::radians(12.5f)));
		//shaderTerrain.setFloat("spotLights[0].outerCutOff", cos(glm::radians(15.0f)));

		/*******************************************
		 * Propiedades PointLights
		 *******************************************/
		shaderMulLighting.setInt("pointLightCount", firePositions.size());
		shaderTerrain.setInt("pointLightCount", firePositions.size());
		for (int i = 0; i < firePositions.size(); i++){
			glm::mat4 matrixAdjustTorch = modelMatrixDoor;
			matrixAdjustTorch = glm::translate(matrixAdjustTorch, firePositions[i]);
			//matrixAdjustTorch = glm::rotate(matrixAdjustLamp, glm::radians(lamp2Orientation[i]), glm::vec3(0, 1, 0));
			matrixAdjustTorch = glm::scale(matrixAdjustTorch, glm::vec3(1.0, 1.0, 1.0));
			matrixAdjustTorch = glm::translate(matrixAdjustTorch, glm::vec3(0.759521, 5.00174, 0));
			glm::vec3 firePosition = glm::vec3(matrixAdjustTorch[3]);
			shaderMulLighting.setVectorFloat3("pointLights[" + std::to_string(i) + "].light.ambient", glm::value_ptr(glm::vec3(0.2, 0.16, 0.01)));
			shaderMulLighting.setVectorFloat3("pointLights[" + std::to_string(i) + "].light.diffuse", glm::value_ptr(glm::vec3(0.4, 0.32, 0.02)));
			shaderMulLighting.setVectorFloat3("pointLights[" + std::to_string(i) + "].light.specular", glm::value_ptr(glm::vec3(0.6, 0.58, 0.03)));
			shaderMulLighting.setVectorFloat3("pointLights[" + std::to_string(i) + "].position", glm::value_ptr(firePosition));
			shaderMulLighting.setFloat("pointLights[" + std::to_string(i) + "].constant", 1.0);
			shaderMulLighting.setFloat("pointLights[" + std::to_string(i) + "].linear", 0.09);
			shaderMulLighting.setFloat("pointLights[" + std::to_string(i) + "].quadratic", 0.01);
			shaderTerrain.setVectorFloat3("pointLights[" + std::to_string(i) + "].light.ambient", glm::value_ptr(glm::vec3(0.2, 0.16, 0.01)));
			shaderTerrain.setVectorFloat3("pointLights[" + std::to_string(i) + "].light.diffuse", glm::value_ptr(glm::vec3(0.4, 0.32, 0.02)));
			shaderTerrain.setVectorFloat3("pointLights[" + std::to_string(i) + "].light.specular", glm::value_ptr(glm::vec3(0.6, 0.58, 0.03)));
			shaderTerrain.setVectorFloat3("pointLights[" + std::to_string(i) + "].position", glm::value_ptr(firePosition));
			shaderTerrain.setFloat("pointLights[" + std::to_string(i) + "].constant", 1.0);
			shaderTerrain.setFloat("pointLights[" + std::to_string(i) + "].linear", 0.09);
			shaderTerrain.setFloat("pointLights[" + std::to_string(i) + "].quadratic", 0.02);
		}

		/*******************************************
		 * 1.- We render the depth buffer
		 *******************************************/
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// render scene from light's point of view
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		//glCullFace(GL_FRONT);
		prepareDepthScene();
		renderScene(false);
		//glCullFace(GL_BACK);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		/*******************************************
		 * Debug to view the texture view map
		 *******************************************/
		// reset viewport
		/*glViewport(0, 0, screenWidth, screenHeight);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// render Depth map to quad for visual debugging
		shaderViewDepth.setMatrix4("projection", 1, false, glm::value_ptr(glm::mat4(1.0)));
		shaderViewDepth.setMatrix4("view", 1, false, glm::value_ptr(glm::mat4(1.0)));
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		boxViewDepth.setScale(glm::vec3(2.0, 2.0, 1.0));
		boxViewDepth.render();*/

		/*******************************************
		 * 2.- We render the normal objects
		 *******************************************/
		glViewport(0, 0, screenWidth, screenHeight);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		prepareScene();
		glActiveTexture(GL_TEXTURE10);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		shaderMulLighting.setInt("shadowMap", 10);
		shaderTerrain.setInt("shadowMap", 10);
		/*******************************************
		 * Skybox
		 *******************************************/
		GLint oldCullFaceMode;
		GLint oldDepthFuncMode;
		// deshabilita el modo del recorte de caras ocultas para ver las esfera desde adentro
		glGetIntegerv(GL_CULL_FACE_MODE, &oldCullFaceMode);
		glGetIntegerv(GL_DEPTH_FUNC, &oldDepthFuncMode);
		shaderSkybox.setFloat("skybox", 0);
		glCullFace(GL_FRONT);
		glDepthFunc(GL_LEQUAL);
		glActiveTexture(GL_TEXTURE0);
		skyboxSphere.render();
		glCullFace(oldCullFaceMode);
		glDepthFunc(oldDepthFuncMode);
		renderScene();
		/*******************************************
		 * Debug to box light box
		 *******************************************/
		/*glm::vec3 front = glm::normalize(-lightPos);
		glm::vec3 right = glm::normalize(glm::cross(glm::vec3(0, 1, 0), front));
		glm::vec3 up = glm::normalize(glm::cross(front, right));
		glDisable(GL_CULL_FACE);
		glm::mat4 boxViewTransform = glm::mat4(1.0f);
		boxViewTransform = glm::translate(boxViewTransform, centerBox);
		boxViewTransform[0] = glm::vec4(right, 0.0);
		boxViewTransform[1] = glm::vec4(up, 0.0);
		boxViewTransform[2] = glm::vec4(front, 0.0);
		boxViewTransform = glm::scale(boxViewTransform, glm::vec3(shadowBox->getWidth(), shadowBox->getHeight(), shadowBox->getLength()));
		boxLightViewBox.enableWireMode();
		boxLightViewBox.render(boxViewTransform);
		glEnable(GL_CULL_FACE);*/

		/*******************************************
		 * Creacion de colliders
		 * IMPORTANT do this before interpolations
		 *******************************************/

		// Collider de mayow
		AbstractModel::OBB mayowCollider;
		glm::mat4 modelmatrixColliderMayow = glm::mat4(modelMatrixMayow);
		modelmatrixColliderMayow = glm::rotate(modelmatrixColliderMayow,
				glm::radians(-90.0f), glm::vec3(1, 0, 0));
		// Set the orientation of collider before doing the scale
		mayowCollider.u = glm::quat_cast(modelmatrixColliderMayow);
		modelmatrixColliderMayow = glm::scale(modelmatrixColliderMayow, glm::vec3(2.0, 2.0, 2.0));
		modelmatrixColliderMayow = glm::translate(modelmatrixColliderMayow,
				glm::vec3(mayowModelAnimate.getObb().c.x,
						mayowModelAnimate.getObb().c.y,
						mayowModelAnimate.getObb().c.z));
		mayowCollider.e = mayowModelAnimate.getObb().e * glm::vec3(2.0, 2.0, 2.0);
		mayowCollider.c = glm::vec3(modelmatrixColliderMayow[3]);
		addOrUpdateColliders(collidersOBB, "mayow", mayowCollider, modelMatrixMayow);

		// Collider palms
		for (int i = 0; i < palmPositions.size(); i++) {
			AbstractModel::OBB palmCollider;
			glm::mat4 modelMatrixColliderPalm = glm::mat4(1.0);
			modelMatrixColliderPalm = glm::translate(modelMatrixColliderPalm, palmPositions[i]);
			//modelMatrixColliderPalm = glm::rotate(modelMatrixColliderPalm, glm::radians(lamp2Orientation[i]),
			//	glm::vec3(0, 1, 0));
			addOrUpdateColliders(collidersOBB, "palm-" + std::to_string(i), palmCollider, modelMatrixColliderPalm);
			// Set the orientation of collider before doing the scale
			palmCollider.u = glm::quat_cast(modelMatrixColliderPalm);
			modelMatrixColliderPalm = glm::scale(modelMatrixColliderPalm, glm::vec3(1.0, 1.0, 1.0));
			modelMatrixColliderPalm = glm::translate(modelMatrixColliderPalm, modelPalm.getObb().c);
			palmCollider.c = glm::vec3(modelMatrixColliderPalm[3]);
			palmCollider.e = modelPalm.getObb().e * glm::vec3(1.0, 1.0, 1.0);
			std::get<0>(collidersOBB.find("palm-" + std::to_string(i))->second) = palmCollider;
		}

		// Collider Geisers
		for (int i = 0; i < geiserPositions.size(); i++) {
			AbstractModel::OBB geiserCollider;
			glm::mat4 modelMatrixColliderGeiser = glm::mat4(1.0);
			modelMatrixColliderGeiser = glm::translate(modelMatrixColliderGeiser, geiserPositions[i]);
			addOrUpdateColliders(collidersOBB, "geiser-" + std::to_string(i), geiserCollider, modelMatrixColliderGeiser);
			// Set the orientation of collider before doing the scale
			geiserCollider.u = glm::quat_cast(modelMatrixColliderGeiser);
			modelMatrixColliderGeiser = glm::scale(modelMatrixColliderGeiser, glm::vec3(1.0, 1.0, 1.0));
			modelMatrixColliderGeiser = glm::translate(modelMatrixColliderGeiser, modelPalm.getObb().c);
			geiserCollider.c = glm::vec3(modelMatrixColliderGeiser[3]);
			geiserCollider.e = modelGeiser.getObb().e * glm::vec3(1.0, 1.0, 1.0);
			std::get<0>(collidersOBB.find("geiser-" + std::to_string(i))->second) = geiserCollider;
		}
	

		/*******************************************
		 * Render de colliders
		 *******************************************/
		for (std::map<std::string, std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> >::iterator it =
				collidersOBB.begin(); it != collidersOBB.end(); it++) {
			glm::mat4 matrixCollider = glm::mat4(1.0);
			matrixCollider = glm::translate(matrixCollider, std::get<0>(it->second).c);
			matrixCollider = matrixCollider * glm::mat4(std::get<0>(it->second).u);
			matrixCollider = glm::scale(matrixCollider, std::get<0>(it->second).e * 2.0f);
			boxCollider.setColor(glm::vec4(1.0, 1.0, 1.0, 1.0));
			boxCollider.enableWireMode();
			boxCollider.render(matrixCollider);
		}

		for (std::map<std::string, std::tuple<AbstractModel::SBB, glm::mat4, glm::mat4> >::iterator it =
				collidersSBB.begin(); it != collidersSBB.end(); it++) {
			glm::mat4 matrixCollider = glm::mat4(1.0);
			matrixCollider = glm::translate(matrixCollider, std::get<0>(it->second).c);
			matrixCollider = glm::scale(matrixCollider, glm::vec3(std::get<0>(it->second).ratio * 2.0f));
			sphereCollider.setColor(glm::vec4(1.0, 1.0, 1.0, 1.0));
			sphereCollider.enableWireMode();
			sphereCollider.render(matrixCollider);
		}

		// Esto es para ilustrar la transformacion inversa de los coliders
		/*glm::vec3 cinv = glm::inverse(mayowCollider.u) * glm::vec4(rockCollider.c, 1.0);
		glm::mat4 invColliderS = glm::mat4(1.0);
		invColliderS = glm::translate(invColliderS, cinv);
		invColliderS =  invColliderS * glm::mat4(mayowCollider.u);
		invColliderS = glm::scale(invColliderS, glm::vec3(rockCollider.ratio * 2.0, rockCollider.ratio * 2.0, rockCollider.ratio * 2.0));
		sphereCollider.setColor(glm::vec4(1.0, 1.0, 0.0, 1.0));
		sphereCollider.enableWireMode();
		sphereCollider.render(invColliderS);
		glm::vec3 cinv2 = glm::inverse(mayowCollider.u) * glm::vec4(mayowCollider.c, 1.0);
		glm::mat4 invColliderB = glm::mat4(1.0);
		invColliderB = glm::translate(invColliderB, cinv2);
		invColliderB = glm::scale(invColliderB, mayowCollider.e * 2.0f);
		boxCollider.setColor(glm::vec4(1.0, 1.0, 0.0, 1.0));
		boxCollider.enableWireMode();
		boxCollider.render(invColliderB);
		// Se regresa el color blanco
		sphereCollider.setColor(glm::vec4(1.0, 1.0, 1.0, 1.0));
		boxCollider.setColor(glm::vec4(1.0, 1.0, 1.0, 1.0));*/

		/*******************************************
		 * Test Colisions
		 *******************************************/
		for (std::map<std::string,
				std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> >::iterator it =
				collidersOBB.begin(); it != collidersOBB.end(); it++) {
			bool isCollision = false;
			for (std::map<std::string,
					std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> >::iterator jt =
					collidersOBB.begin(); jt != collidersOBB.end(); jt++) {
				if (it != jt
						&& testOBBOBB(std::get<0>(it->second),
								std::get<0>(jt->second))) {
					std::cout << "Colision " << it->first << " with "
							<< jt->first << std::endl;
					isCollision = true;
				}
			}
			addOrUpdateCollisionDetection(collisionDetection, it->first, isCollision);
		}

		for (std::map<std::string,
				std::tuple<AbstractModel::SBB, glm::mat4, glm::mat4> >::iterator it =
				collidersSBB.begin(); it != collidersSBB.end(); it++) {
			bool isCollision = false;
			for (std::map<std::string,
					std::tuple<AbstractModel::SBB, glm::mat4, glm::mat4> >::iterator jt =
					collidersSBB.begin(); jt != collidersSBB.end(); jt++) {
				if (it != jt
						&& testSphereSphereIntersection(std::get<0>(it->second),
								std::get<0>(jt->second))) {
					std::cout << "Colision " << it->first << " with "
							<< jt->first << std::endl;
					isCollision = true;
				}
			}
			addOrUpdateCollisionDetection(collisionDetection, it->first, isCollision);
		}

		for (std::map<std::string,
				std::tuple<AbstractModel::SBB, glm::mat4, glm::mat4> >::iterator it =
				collidersSBB.begin(); it != collidersSBB.end(); it++) {
			bool isCollision = false;
			std::map<std::string,
					std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> >::iterator jt =
					collidersOBB.begin();
			for (; jt != collidersOBB.end(); jt++) {
				if (testSphereOBox(std::get<0>(it->second),
								std::get<0>(jt->second))) {
					std::cout << "Colision " << it->first << " with "
							<< jt->first << std::endl;
					isCollision = true;
					addOrUpdateCollisionDetection(collisionDetection, jt->first, isCollision);
				}
			}
			addOrUpdateCollisionDetection(collisionDetection, it->first, isCollision);
		}

		std::map<std::string, bool>::iterator colIt;
		for (colIt = collisionDetection.begin(); colIt != collisionDetection.end();
				colIt++) {
			std::map<std::string,
					std::tuple<AbstractModel::SBB, glm::mat4, glm::mat4> >::iterator it =
					collidersSBB.find(colIt->first);
			std::map<std::string,
					std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> >::iterator jt =
					collidersOBB.find(colIt->first);
			if (it != collidersSBB.end()) {
				if (!colIt->second)
					addOrUpdateColliders(collidersSBB, it->first);
			}
			if (jt != collidersOBB.end()) {
				if (!colIt->second)
					addOrUpdateColliders(collidersOBB, jt->first);
				else {
					if (jt->first.compare("mayow") == 0)
						modelMatrixMayow = std::get<1>(jt->second);
					/*if (jt->first.compare("dart") == 0)
						modelMatrixDart = std::get<1>(jt->second);*/
				}
			}
		}

		/*******************************************
		 * Interpolation key frames with disconect objects
		 *******************************************/
		/*if(record && modelSelected == 1){
			matrixDartJoints.push_back(rotDartHead);
			matrixDartJoints.push_back(rotDartLeftArm);
			matrixDartJoints.push_back(rotDartLeftHand);
			matrixDartJoints.push_back(rotDartRightArm);
			matrixDartJoints.push_back(rotDartRightHand);
			matrixDartJoints.push_back(rotDartLeftLeg);
			matrixDartJoints.push_back(rotDartRightLeg);
			if (saveFrame) {
				appendFrame(myfile, matrixDartJoints);
				saveFrame = false;
			}
		}
		else if(keyFramesDartJoints.size() > 0){
			// Para reproducir el frame
			interpolationDartJoints = numPasosDartJoints / (float) maxNumPasosDartJoints;
			numPasosDartJoints++;
			if (interpolationDartJoints > 1.0) {
				numPasosDartJoints = 0;
				interpolationDartJoints = 0;
				indexFrameDartJoints = indexFrameDartJointsNext;
				indexFrameDartJointsNext++;
			}
			if (indexFrameDartJointsNext > keyFramesDartJoints.size() - 1)
				indexFrameDartJointsNext = 0;
			rotDartHead = interpolate(keyFramesDartJoints, indexFrameDartJoints, indexFrameDartJointsNext, 0, interpolationDartJoints);
			rotDartLeftArm = interpolate(keyFramesDartJoints, indexFrameDartJoints, indexFrameDartJointsNext, 1, interpolationDartJoints);
			rotDartLeftHand = interpolate(keyFramesDartJoints, indexFrameDartJoints, indexFrameDartJointsNext, 2, interpolationDartJoints);
			rotDartRightArm = interpolate(keyFramesDartJoints, indexFrameDartJoints, indexFrameDartJointsNext, 3, interpolationDartJoints);
			rotDartRightHand = interpolate(keyFramesDartJoints, indexFrameDartJoints, indexFrameDartJointsNext, 4, interpolationDartJoints);
			rotDartLeftLeg = interpolate(keyFramesDartJoints, indexFrameDartJoints, indexFrameDartJointsNext, 5, interpolationDartJoints);
			rotDartRightLeg = interpolate(keyFramesDartJoints, indexFrameDartJoints, indexFrameDartJointsNext, 6, interpolationDartJoints);
		}*/

		/*if (record && modelSelected == 2) {
			matrixDart.push_back(modelMatrixDart);
			if (saveFrame) {
				appendFrame(myfile, matrixDart);
				saveFrame = false;
			}
		}
		else if (keyFramesDart.size() > 0) {
			// Para reproducir el frame
			interpolationDart = numPasosDart / (float)maxNumPasosDart;
			numPasosDart++;
			if (interpolationDart > 1.0) {
				numPasosDart = 0;
				interpolationDart = 0;
				indexFrameDart = indexFrameDartNext;
				indexFrameDartNext++;
			}
			if (indexFrameDartNext > keyFramesDart.size() - 1)
				indexFrameDartNext = 0;
			modelMatrixDart = interpolate(keyFramesDart, indexFrameDart, indexFrameDartNext, 0, interpolationDart);
		}*/

		// Constantes de animaciones
		//rotHelHelY += 0.5;
		//animationIndex = 1;

		/*******************************************
		 * State machines
		 *******************************************/

		// State machine for the lambo car
		switch(stateDoor){
		case 0:
			dorRotCount += 0.5;
			if(dorRotCount > 75)
				stateDoor = 1;
			break;
		case 1:
			dorRotCount -= 0.5;
			if(dorRotCount < 0){
				dorRotCount = 0.0;
				stateDoor = 0;
			}
			break;
		}

		glfwSwapBuffers(window);

		/****************************+
		 * Open AL sound data
		 */

		source0Pos[0] = modelMatrixMayow[3].x;
		source0Pos[1] = modelMatrixMayow[3].y;
		source0Pos[2] = modelMatrixMayow[3].z;
		alSourcefv(source[0], AL_POSITION, source0Pos);

		source2Pos[0] = modelMatrixDoor[3].x;
		source2Pos[1] = modelMatrixDoor[3].y;
		source2Pos[2] = modelMatrixDoor[3].z;
		alSourcefv(source[2], AL_POSITION, source2Pos);

		if (stateCamera == 1) {

			// Listener for the Thris person camera
			listenerPos[0] = modelMatrixMayow[3].x;
			listenerPos[1] = modelMatrixMayow[3].y;
			listenerPos[2] = modelMatrixMayow[3].z;
			alListenerfv(AL_POSITION, listenerPos);

			glm::vec3 upModel = glm::normalize(modelMatrixMayow[1]);
			glm::vec3 frontModel = glm::normalize(modelMatrixMayow[2]);

			listenerOri[0] = frontModel.x;
			listenerOri[1] = frontModel.y;
			listenerOri[2] = frontModel.z;
			listenerOri[3] = upModel.x;
			listenerOri[4] = upModel.y;
			listenerOri[5] = upModel.z;
		}
		else if (stateCamera == 2) {
			// Listener for the First person camera
			listenerPos[0] = cameraFPS->getPosition().x;
			listenerPos[1] = cameraFPS->getPosition().y;
			listenerPos[2] = cameraFPS->getPosition().z;
			alListenerfv(AL_POSITION, listenerPos);
			listenerOri[0] = cameraFPS->getFront().x;
			listenerOri[1] = cameraFPS->getFront().y;
			listenerOri[2] = cameraFPS->getFront().z;
			listenerOri[3] = cameraFPS->getUp().x;
			listenerOri[4] = cameraFPS->getUp().y;
			listenerOri[5] = cameraFPS->getUp().z;
		}
		
		alListenerfv(AL_ORIENTATION, listenerOri);

		for(unsigned int i = 0; i < sourcesPlay.size(); i++){
			if(sourcesPlay[i]){ //Para que lo reproduzca todo el tiempo
				sourcesPlay[i] = false;
				alSourcePlay(source[i]);
				if (animationIndex == 1 && i == 0)
					alSourcePlay(source[i]);
			}
			if (animationIndex == 0 && i == 0) {
				sourcesPlay[i] = true;
				alSourceStop(source[i]);
			}
		}
	}
}

void prepareScene(){

	skyboxSphere.setShader(&shaderSkybox);

	terrain.setShader(&shaderTerrain);

	//Grass
	modelGrass.setShader(&shaderMulLighting);

	//Mayow
	mayowModelAnimate.setShader(&shaderMulLighting);

	//Door
	modelDoor.setShader(&shaderMulLighting);

	//Mountain
	modelMountain.setShader(&shaderMulLighting);

	//Geiser
	modelGeiser.setShader(&shaderMulLighting);

	//Palm
	modelPalm.setShader(&shaderMulLighting);
}

void prepareDepthScene(){

	skyboxSphere.setShader(&shaderDepth);

	terrain.setShader(&shaderDepth);

	//Grass
	modelGrass.setShader(&shaderDepth);

	//Mayow
	mayowModelAnimate.setShader(&shaderDepth);

	//Door
	modelDoor.setShader(&shaderDepth);

	//Mountanin
	modelMountain.setShader(&shaderDepth);

	//Geiser
	modelGeiser.setShader(&shaderDepth);

	//Palm
	modelPalm.setShader(&shaderDepth);
}

void renderScene(bool renderParticles){
	/*******************************************
	 * Terrain Cesped
	 *******************************************/
	glm::mat4 modelCesped = glm::mat4(1.0);
	modelCesped = glm::translate(modelCesped, glm::vec3(0.0, 0.0, 0.0));
	modelCesped = glm::scale(modelCesped, glm::vec3(200.0, 0.001, 200.0));
	// Se activa la textura del background
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureTerrainBackgroundID);
	shaderTerrain.setInt("backgroundTexture", 0);
	// Se activa la textura de tierra
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, textureTerrainRID);
	shaderTerrain.setInt("rTexture", 1);
	// Se activa la textura de hierba
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, textureTerrainGID);
	shaderTerrain.setInt("gTexture", 2);
	// Se activa la textura del camino
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, textureTerrainBID);
	shaderTerrain.setInt("bTexture", 3);
	// Se activa la textura del blend map
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, textureTerrainBlendMapID);
	shaderTerrain.setInt("blendMapTexture", 4);
	shaderTerrain.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(40, 40)));
	terrain.render();
	shaderTerrain.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(0, 0)));
	glBindTexture(GL_TEXTURE_2D, 0);

	/*******************************************
	 * Custom objects obj
	 *******************************************/

	// Forze to enable the unit texture to 0 always ----------------- IMPORTANT
	glActiveTexture(GL_TEXTURE0);

	// Grass
	glDisable(GL_CULL_FACE);
	for (int i = 0; i < grassPositionsGeiser.size(); i++) {
		grassPositionsGeiser[i].y = terrain.getHeightTerrain(grassPositionsGeiser[i].x, grassPositionsGeiser[i].z);
		modelGrass.setPosition(grassPositionsGeiser[i]);
		modelGrass.setScale(glm::vec3(0.3, 0.3, 0.3));
		modelGrass.render();
	}
	glEnable(GL_CULL_FACE);

	//Geiser
	glDisable(GL_CULL_FACE);
	for (int i = 0; i < geiserPositions.size(); i++) {
		geiserPositions[i].y = terrain.getHeightTerrain(geiserPositions[i].x, geiserPositions[i].z);
		modelGeiser.setPosition(geiserPositions[i]);
		modelGeiser.render();
	}
	//modelGeiser.render(modelMatrixGeiser);
	glEnable(GL_CULL_FACE);

	//Palm
	glDisable(GL_CULL_FACE);
	for (int i = 0; i < palmPositions.size(); i++) {
		palmPositions[i].y = terrain.getHeightTerrain(palmPositions[i].x, palmPositions[i].z);
		modelPalm.setPosition(palmPositions[i]);
		modelPalm.render();
	}
	glEnable(GL_CULL_FACE);

	//Door
	glDisable(GL_CULL_FACE);
	modelDoor.render(modelMatrixDoor);
	glEnable(GL_CULL_FACE);

	//Mountanin
	glDisable(GL_CULL_FACE);
	modelMountain.render(modelMatrixMountain);
	glEnable(GL_CULL_FACE);

	/*******************************************
	 * Custom Anim objects obj
	 *******************************************/
	modelMatrixMayow[3][1] = terrain.getHeightTerrain(modelMatrixMayow[3][0], modelMatrixMayow[3][2]);
	glm::mat4 modelMatrixMayowBody = glm::mat4(modelMatrixMayow);
	modelMatrixMayowBody = glm::scale(modelMatrixMayowBody, glm::vec3(2.0, 2.0, 2.0));
	mayowModelAnimate.setAnimationIndex(0);
	mayowModelAnimate.render(modelMatrixMayowBody);

	/**********
	 * Update the position with alpha objects
	 */

	/**********
	 * Sorter with alpha objects
	 */
	std::map<float, std::pair<std::string, glm::vec3>> blendingSorted;
	std::map<std::string, glm::vec3>::iterator itblend;
	for(itblend = blendingUnsorted.begin(); itblend != blendingUnsorted.end(); itblend++){
		float distanceFromView = glm::length(camera->getPosition() - itblend->second);
		blendingSorted[distanceFromView] = std::make_pair(itblend->first, itblend->second);
	}

	/**********
	 * Render de las transparencias
	 */
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_CULL_FACE);
	for(std::map<float, std::pair<std::string, glm::vec3> >::reverse_iterator it = blendingSorted.rbegin(); it != blendingSorted.rend(); it++){

		if(renderParticles && it->second.first.compare("geiser0") == 0){
			/**********
			 * Init Render particles systems
			 */
			glm::mat4 modelMatrixParticlesGeiser = glm::mat4(1.0);
			modelMatrixParticlesGeiser = glm::translate(modelMatrixParticlesGeiser, it->second.second);
			modelMatrixParticlesGeiser[3][1] = terrain.getHeightTerrain(modelMatrixParticlesGeiser[3][0], modelMatrixParticlesGeiser[3][2]);
			modelMatrixParticlesGeiser = glm::scale(modelMatrixParticlesGeiser, glm::vec3(1.0, 2.0, 1.0));
			currTimeParticlesAnimation = TimeManager::Instance().GetTime();
			if(currTimeParticlesAnimation - lastTimeParticlesAnimation > 10.0)
				lastTimeParticlesAnimation = currTimeParticlesAnimation;
			//glDisable(GL_DEPTH_TEST);
			glDepthMask(GL_FALSE);
			// Set the point size
			glPointSize(10.0f);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textureParticlesGeiserID);
			shaderParticlesGeiser.turnOn();
			shaderParticlesGeiser.setFloat("Time", float(currTimeParticlesAnimation - lastTimeParticlesAnimation));
			shaderParticlesGeiser.setFloat("ParticleLifetime", 3.5f);
			shaderParticlesGeiser.setInt("ParticleTex", 0);
			shaderParticlesGeiser.setVectorFloat3("Gravity", glm::value_ptr(glm::vec3(0.0f, 0.3f, 0.0f)));
			shaderParticlesGeiser.setMatrix4("model", 1, false, glm::value_ptr(modelMatrixParticlesGeiser));
			glBindVertexArray(VAOParticles);
			glDrawArrays(GL_POINTS, 0, nParticles);
			glDepthMask(GL_TRUE);
			//glEnable(GL_DEPTH_TEST);
			shaderParticlesGeiser.turnOff();

			/**********
			 * End Render particles systems
			 */
		}
		else if (renderParticles && it->second.first.compare("geiser1") == 0) {
			/**********
			 * Init Render particles systems
			 */
			glm::mat4 modelMatrixParticlesGeiser = glm::mat4(1.0);
			modelMatrixParticlesGeiser = glm::translate(modelMatrixParticlesGeiser, it->second.second);
			modelMatrixParticlesGeiser[3][1] = terrain.getHeightTerrain(modelMatrixParticlesGeiser[3][0], modelMatrixParticlesGeiser[3][2]);
			modelMatrixParticlesGeiser = glm::scale(modelMatrixParticlesGeiser, glm::vec3(1.0, 2.0, 1.0));
			currTimeParticlesAnimation = TimeManager::Instance().GetTime();
			if (currTimeParticlesAnimation - lastTimeParticlesAnimation > 10.0)
				lastTimeParticlesAnimation = currTimeParticlesAnimation;
			//glDisable(GL_DEPTH_TEST);
			glDepthMask(GL_FALSE);
			// Set the point size
			glPointSize(10.0f);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textureParticlesGeiserID);
			shaderParticlesGeiser.turnOn();
			shaderParticlesGeiser.setFloat("Time", float(currTimeParticlesAnimation - lastTimeParticlesAnimation));
			shaderParticlesGeiser.setFloat("ParticleLifetime", 3.5f);
			shaderParticlesGeiser.setInt("ParticleTex", 0);
			shaderParticlesGeiser.setVectorFloat3("Gravity", glm::value_ptr(glm::vec3(0.0f, 0.3f, 0.0f)));
			shaderParticlesGeiser.setMatrix4("model", 1, false, glm::value_ptr(modelMatrixParticlesGeiser));
			glBindVertexArray(VAOParticles);
			glDrawArrays(GL_POINTS, 0, nParticles);
			glDepthMask(GL_TRUE);
			//glEnable(GL_DEPTH_TEST);
			shaderParticlesGeiser.turnOff();

			/**********
			 * End Render particles systems
			 */
		}
		else if (renderParticles && it->second.first.compare("geiser2") == 0) {
			/**********
			 * Init Render particles systems
			 */
			glm::mat4 modelMatrixParticlesGeiser = glm::mat4(1.0);
			modelMatrixParticlesGeiser = glm::translate(modelMatrixParticlesGeiser, it->second.second);
			modelMatrixParticlesGeiser[3][1] = terrain.getHeightTerrain(modelMatrixParticlesGeiser[3][0], modelMatrixParticlesGeiser[3][2]);
			modelMatrixParticlesGeiser = glm::scale(modelMatrixParticlesGeiser, glm::vec3(1.0, 2.0, 1.0));
			currTimeParticlesAnimation = TimeManager::Instance().GetTime();
			if (currTimeParticlesAnimation - lastTimeParticlesAnimation > 10.0)
				lastTimeParticlesAnimation = currTimeParticlesAnimation;
			//glDisable(GL_DEPTH_TEST);
			glDepthMask(GL_FALSE);
			// Set the point size
			glPointSize(10.0f);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textureParticlesGeiserID);
			shaderParticlesGeiser.turnOn();
			shaderParticlesGeiser.setFloat("Time", float(currTimeParticlesAnimation - lastTimeParticlesAnimation));
			shaderParticlesGeiser.setFloat("ParticleLifetime", 3.5f);
			shaderParticlesGeiser.setInt("ParticleTex", 0);
			shaderParticlesGeiser.setVectorFloat3("Gravity", glm::value_ptr(glm::vec3(0.0f, 0.3f, 0.0f)));
			shaderParticlesGeiser.setMatrix4("model", 1, false, glm::value_ptr(modelMatrixParticlesGeiser));
			glBindVertexArray(VAOParticles);
			glDrawArrays(GL_POINTS, 0, nParticles);
			glDepthMask(GL_TRUE);
			//glEnable(GL_DEPTH_TEST);
			shaderParticlesGeiser.turnOff();

			/**********
			 * End Render particles systems
			 */
		}
		else if (renderParticles && it->second.first.compare("geiser3") == 0) {
			/**********
			 * Init Render particles systems
			 */
			glm::mat4 modelMatrixParticlesGeiser = glm::mat4(1.0);
			modelMatrixParticlesGeiser = glm::translate(modelMatrixParticlesGeiser, it->second.second);
			modelMatrixParticlesGeiser[3][1] = terrain.getHeightTerrain(modelMatrixParticlesGeiser[3][0], modelMatrixParticlesGeiser[3][2]);
			modelMatrixParticlesGeiser = glm::scale(modelMatrixParticlesGeiser, glm::vec3(1.0, 2.0, 1.0));
			currTimeParticlesAnimation = TimeManager::Instance().GetTime();
			if (currTimeParticlesAnimation - lastTimeParticlesAnimation > 10.0)
				lastTimeParticlesAnimation = currTimeParticlesAnimation;
			//glDisable(GL_DEPTH_TEST);
			glDepthMask(GL_FALSE);
			// Set the point size
			glPointSize(10.0f);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textureParticlesGeiserID);
			shaderParticlesGeiser.turnOn();
			shaderParticlesGeiser.setFloat("Time", float(currTimeParticlesAnimation - lastTimeParticlesAnimation));
			shaderParticlesGeiser.setFloat("ParticleLifetime", 3.5f);
			shaderParticlesGeiser.setInt("ParticleTex", 0);
			shaderParticlesGeiser.setVectorFloat3("Gravity", glm::value_ptr(glm::vec3(0.0f, 0.3f, 0.0f)));
			shaderParticlesGeiser.setMatrix4("model", 1, false, glm::value_ptr(modelMatrixParticlesGeiser));
			glBindVertexArray(VAOParticles);
			glDrawArrays(GL_POINTS, 0, nParticles);
			glDepthMask(GL_TRUE);
			//glEnable(GL_DEPTH_TEST);
			shaderParticlesGeiser.turnOff();

			/**********
			 * End Render particles systems
			 */
		}
		else if (renderParticles && it->second.first.compare("geiser4") == 0) {
		/**********
		 * Init Render particles systems
		 */
		 glm::mat4 modelMatrixParticlesGeiser = glm::mat4(1.0);
		 modelMatrixParticlesGeiser = glm::translate(modelMatrixParticlesGeiser, it->second.second);
		 modelMatrixParticlesGeiser[3][1] = terrain.getHeightTerrain(modelMatrixParticlesGeiser[3][0], modelMatrixParticlesGeiser[3][2]);
		 modelMatrixParticlesGeiser = glm::scale(modelMatrixParticlesGeiser, glm::vec3(1.0, 2.0, 1.0));
		 currTimeParticlesAnimation = TimeManager::Instance().GetTime();
		 if (currTimeParticlesAnimation - lastTimeParticlesAnimation > 10.0)
			 lastTimeParticlesAnimation = currTimeParticlesAnimation;
		 //glDisable(GL_DEPTH_TEST);
		 glDepthMask(GL_FALSE);
		 // Set the point size
		 glPointSize(10.0f);
		 glActiveTexture(GL_TEXTURE0);
		 glBindTexture(GL_TEXTURE_2D, textureParticlesGeiserID);
		 shaderParticlesGeiser.turnOn();
		 shaderParticlesGeiser.setFloat("Time", float(currTimeParticlesAnimation - lastTimeParticlesAnimation));
		 shaderParticlesGeiser.setFloat("ParticleLifetime", 3.5f);
		 shaderParticlesGeiser.setInt("ParticleTex", 0);
		 shaderParticlesGeiser.setVectorFloat3("Gravity", glm::value_ptr(glm::vec3(0.0f, 0.3f, 0.0f)));
		 shaderParticlesGeiser.setMatrix4("model", 1, false, glm::value_ptr(modelMatrixParticlesGeiser));
		 glBindVertexArray(VAOParticles);
		 glDrawArrays(GL_POINTS, 0, nParticles);
		 glDepthMask(GL_TRUE);
		 //glEnable(GL_DEPTH_TEST);
		 shaderParticlesGeiser.turnOff();

		 /**********
		  * End Render particles systems
		  */
		}
		else if (renderParticles && it->second.first.compare("geiser5") == 0) {
		/**********
		 * Init Render particles systems
		 */
		 glm::mat4 modelMatrixParticlesGeiser = glm::mat4(1.0);
		 modelMatrixParticlesGeiser = glm::translate(modelMatrixParticlesGeiser, it->second.second);
		 modelMatrixParticlesGeiser[3][1] = terrain.getHeightTerrain(modelMatrixParticlesGeiser[3][0], modelMatrixParticlesGeiser[3][2]);
		 modelMatrixParticlesGeiser = glm::scale(modelMatrixParticlesGeiser, glm::vec3(1.0, 2.0, 1.0));
		 currTimeParticlesAnimation = TimeManager::Instance().GetTime();
		 if (currTimeParticlesAnimation - lastTimeParticlesAnimation > 10.0)
			 lastTimeParticlesAnimation = currTimeParticlesAnimation;
		 //glDisable(GL_DEPTH_TEST);
		 glDepthMask(GL_FALSE);
		 // Set the point size
		 glPointSize(10.0f);
		 glActiveTexture(GL_TEXTURE0);
		 glBindTexture(GL_TEXTURE_2D, textureParticlesGeiserID);
		 shaderParticlesGeiser.turnOn();
		 shaderParticlesGeiser.setFloat("Time", float(currTimeParticlesAnimation - lastTimeParticlesAnimation));
		 shaderParticlesGeiser.setFloat("ParticleLifetime", 3.5f);
		 shaderParticlesGeiser.setInt("ParticleTex", 0);
		 shaderParticlesGeiser.setVectorFloat3("Gravity", glm::value_ptr(glm::vec3(0.0f, 0.3f, 0.0f)));
		 shaderParticlesGeiser.setMatrix4("model", 1, false, glm::value_ptr(modelMatrixParticlesGeiser));
		 glBindVertexArray(VAOParticles);
		 glDrawArrays(GL_POINTS, 0, nParticles);
		 glDepthMask(GL_TRUE);
		 //glEnable(GL_DEPTH_TEST);
		 shaderParticlesGeiser.turnOff();
	
		 /**********
		  * End Render particles systems
		  */
		}
		else if (renderParticles && it->second.first.compare("geiser6") == 0) {
		/**********
		 * Init Render particles systems
		 */
		 glm::mat4 modelMatrixParticlesGeiser = glm::mat4(1.0);
		 modelMatrixParticlesGeiser = glm::translate(modelMatrixParticlesGeiser, it->second.second);
		 modelMatrixParticlesGeiser[3][1] = terrain.getHeightTerrain(modelMatrixParticlesGeiser[3][0], modelMatrixParticlesGeiser[3][2]);
		 modelMatrixParticlesGeiser = glm::scale(modelMatrixParticlesGeiser, glm::vec3(1.0, 2.0, 1.0));
		 currTimeParticlesAnimation = TimeManager::Instance().GetTime();
		 if (currTimeParticlesAnimation - lastTimeParticlesAnimation > 10.0)
			 lastTimeParticlesAnimation = currTimeParticlesAnimation;
		 //glDisable(GL_DEPTH_TEST);
		 glDepthMask(GL_FALSE);
		 // Set the point size
		 glPointSize(10.0f);
		 glActiveTexture(GL_TEXTURE0);
		 glBindTexture(GL_TEXTURE_2D, textureParticlesGeiserID);
		 shaderParticlesGeiser.turnOn();
		 shaderParticlesGeiser.setFloat("Time", float(currTimeParticlesAnimation - lastTimeParticlesAnimation));
		 shaderParticlesGeiser.setFloat("ParticleLifetime", 3.5f);
		 shaderParticlesGeiser.setInt("ParticleTex", 0);
		 shaderParticlesGeiser.setVectorFloat3("Gravity", glm::value_ptr(glm::vec3(0.0f, 0.3f, 0.0f)));
		 shaderParticlesGeiser.setMatrix4("model", 1, false, glm::value_ptr(modelMatrixParticlesGeiser));
		 glBindVertexArray(VAOParticles);
		 glDrawArrays(GL_POINTS, 0, nParticles);
		 glDepthMask(GL_TRUE);
		 //glEnable(GL_DEPTH_TEST);
		 shaderParticlesGeiser.turnOff();

		 /**********
		  * End Render particles systems
		  */
		}
		else if (renderParticles && it->second.first.compare("geiser7") == 0) {
		/**********
		 * Init Render particles systems
		 */
		 glm::mat4 modelMatrixParticlesGeiser = glm::mat4(1.0);
		 modelMatrixParticlesGeiser = glm::translate(modelMatrixParticlesGeiser, it->second.second);
		 modelMatrixParticlesGeiser[3][1] = terrain.getHeightTerrain(modelMatrixParticlesGeiser[3][0], modelMatrixParticlesGeiser[3][2]);
		 modelMatrixParticlesGeiser = glm::scale(modelMatrixParticlesGeiser, glm::vec3(1.0, 2.0, 1.0));
		 currTimeParticlesAnimation = TimeManager::Instance().GetTime();
		 if (currTimeParticlesAnimation - lastTimeParticlesAnimation > 10.0)
			 lastTimeParticlesAnimation = currTimeParticlesAnimation;
		 //glDisable(GL_DEPTH_TEST);
		 glDepthMask(GL_FALSE);
		 // Set the point size
		 glPointSize(10.0f);
		 glActiveTexture(GL_TEXTURE0);
		 glBindTexture(GL_TEXTURE_2D, textureParticlesGeiserID);
		 shaderParticlesGeiser.turnOn();
		 shaderParticlesGeiser.setFloat("Time", float(currTimeParticlesAnimation - lastTimeParticlesAnimation));
		 shaderParticlesGeiser.setFloat("ParticleLifetime", 3.5f);
		 shaderParticlesGeiser.setInt("ParticleTex", 0);
		 shaderParticlesGeiser.setVectorFloat3("Gravity", glm::value_ptr(glm::vec3(0.0f, 0.3f, 0.0f)));
		 shaderParticlesGeiser.setMatrix4("model", 1, false, glm::value_ptr(modelMatrixParticlesGeiser));
		 glBindVertexArray(VAOParticles);
		 glDrawArrays(GL_POINTS, 0, nParticles);
		 glDepthMask(GL_TRUE);
		 //glEnable(GL_DEPTH_TEST);
		 shaderParticlesGeiser.turnOff();
	
		 /**********
		  * End Render particles systems
		  */
		}
		else if (renderParticles && it->second.first.compare("geiser8") == 0) {
		/**********
		 * Init Render particles systems
		 */
		 glm::mat4 modelMatrixParticlesGeiser = glm::mat4(1.0);
		 modelMatrixParticlesGeiser = glm::translate(modelMatrixParticlesGeiser, it->second.second);
		 modelMatrixParticlesGeiser[3][1] = terrain.getHeightTerrain(modelMatrixParticlesGeiser[3][0], modelMatrixParticlesGeiser[3][2]);
		 modelMatrixParticlesGeiser = glm::scale(modelMatrixParticlesGeiser, glm::vec3(1.0, 2.0, 1.0));
		 currTimeParticlesAnimation = TimeManager::Instance().GetTime();
		 if (currTimeParticlesAnimation - lastTimeParticlesAnimation > 10.0)
			 lastTimeParticlesAnimation = currTimeParticlesAnimation;
		 //glDisable(GL_DEPTH_TEST);
		 glDepthMask(GL_FALSE);
		 // Set the point size
		 glPointSize(10.0f);
		 glActiveTexture(GL_TEXTURE0);
		 glBindTexture(GL_TEXTURE_2D, textureParticlesGeiserID);
		 shaderParticlesGeiser.turnOn();
		 shaderParticlesGeiser.setFloat("Time", float(currTimeParticlesAnimation - lastTimeParticlesAnimation));
		 shaderParticlesGeiser.setFloat("ParticleLifetime", 3.5f);
		 shaderParticlesGeiser.setInt("ParticleTex", 0);
		 shaderParticlesGeiser.setVectorFloat3("Gravity", glm::value_ptr(glm::vec3(0.0f, 0.3f, 0.0f)));
		 shaderParticlesGeiser.setMatrix4("model", 1, false, glm::value_ptr(modelMatrixParticlesGeiser));
		 glBindVertexArray(VAOParticles);
		 glDrawArrays(GL_POINTS, 0, nParticles);
		 glDepthMask(GL_TRUE);
		 //glEnable(GL_DEPTH_TEST);
		 shaderParticlesGeiser.turnOff();

		 /**********
		  * End Render particles systems
		  */
		}
		else if (renderParticles && it->second.first.compare("geiser9") == 0) {
		/**********
		 * Init Render particles systems
		 */
		 glm::mat4 modelMatrixParticlesGeiser = glm::mat4(1.0);
		 modelMatrixParticlesGeiser = glm::translate(modelMatrixParticlesGeiser, it->second.second);
		 modelMatrixParticlesGeiser[3][1] = terrain.getHeightTerrain(modelMatrixParticlesGeiser[3][0], modelMatrixParticlesGeiser[3][2]);
		 modelMatrixParticlesGeiser = glm::scale(modelMatrixParticlesGeiser, glm::vec3(1.0, 2.0, 1.0));
		 currTimeParticlesAnimation = TimeManager::Instance().GetTime();
		 if (currTimeParticlesAnimation - lastTimeParticlesAnimation > 10.0)
			 lastTimeParticlesAnimation = currTimeParticlesAnimation;
		 //glDisable(GL_DEPTH_TEST);
		 glDepthMask(GL_FALSE);
		 // Set the point size
		 glPointSize(10.0f);
		 glActiveTexture(GL_TEXTURE0);
		 glBindTexture(GL_TEXTURE_2D, textureParticlesGeiserID);
		 shaderParticlesGeiser.turnOn();
		 shaderParticlesGeiser.setFloat("Time", float(currTimeParticlesAnimation - lastTimeParticlesAnimation));
		 shaderParticlesGeiser.setFloat("ParticleLifetime", 3.5f);
		 shaderParticlesGeiser.setInt("ParticleTex", 0);
		 shaderParticlesGeiser.setVectorFloat3("Gravity", glm::value_ptr(glm::vec3(0.0f, 0.3f, 0.0f)));
		 shaderParticlesGeiser.setMatrix4("model", 1, false, glm::value_ptr(modelMatrixParticlesGeiser));
		 glBindVertexArray(VAOParticles);
		 glDrawArrays(GL_POINTS, 0, nParticles);
		 glDepthMask(GL_TRUE);
		 //glEnable(GL_DEPTH_TEST);
		 shaderParticlesGeiser.turnOff();

		 /**********
		  * End Render particles systems
		  */
		}
		else if (renderParticles && it->second.first.compare("geiser10") == 0) {
		/**********
		 * Init Render particles systems
		 */
		 glm::mat4 modelMatrixParticlesGeiser = glm::mat4(1.0);
		 modelMatrixParticlesGeiser = glm::translate(modelMatrixParticlesGeiser, it->second.second);
		 modelMatrixParticlesGeiser[3][1] = terrain.getHeightTerrain(modelMatrixParticlesGeiser[3][0], modelMatrixParticlesGeiser[3][2]);
		 modelMatrixParticlesGeiser = glm::scale(modelMatrixParticlesGeiser, glm::vec3(1.0, 2.0, 1.0));
		 currTimeParticlesAnimation = TimeManager::Instance().GetTime();
		 if (currTimeParticlesAnimation - lastTimeParticlesAnimation > 10.0)
			 lastTimeParticlesAnimation = currTimeParticlesAnimation;
		 //glDisable(GL_DEPTH_TEST);
		 glDepthMask(GL_FALSE);
		 // Set the point size
		 glPointSize(10.0f);
		 glActiveTexture(GL_TEXTURE0);
		 glBindTexture(GL_TEXTURE_2D, textureParticlesGeiserID);
		 shaderParticlesGeiser.turnOn();
		 shaderParticlesGeiser.setFloat("Time", float(currTimeParticlesAnimation - lastTimeParticlesAnimation));
		 shaderParticlesGeiser.setFloat("ParticleLifetime", 3.5f);
		 shaderParticlesGeiser.setInt("ParticleTex", 0);
		 shaderParticlesGeiser.setVectorFloat3("Gravity", glm::value_ptr(glm::vec3(0.0f, 0.3f, 0.0f)));
		 shaderParticlesGeiser.setMatrix4("model", 1, false, glm::value_ptr(modelMatrixParticlesGeiser));
		 glBindVertexArray(VAOParticles);
		 glDrawArrays(GL_POINTS, 0, nParticles);
		 glDepthMask(GL_TRUE);
		 //glEnable(GL_DEPTH_TEST);
		 shaderParticlesGeiser.turnOff();
		
		 /**********
		  * End Render particles systems
		  */
		}
		else if (renderParticles && it->second.first.compare("geiser11") == 0) {
		/**********
		 * Init Render particles systems
		 */
		 glm::mat4 modelMatrixParticlesGeiser = glm::mat4(1.0);
		 modelMatrixParticlesGeiser = glm::translate(modelMatrixParticlesGeiser, it->second.second);
		 modelMatrixParticlesGeiser[3][1] = terrain.getHeightTerrain(modelMatrixParticlesGeiser[3][0], modelMatrixParticlesGeiser[3][2]);
		 modelMatrixParticlesGeiser = glm::scale(modelMatrixParticlesGeiser, glm::vec3(1.0, 2.0, 1.0));
		 currTimeParticlesAnimation = TimeManager::Instance().GetTime();
		 if (currTimeParticlesAnimation - lastTimeParticlesAnimation > 10.0)
			 lastTimeParticlesAnimation = currTimeParticlesAnimation;
		 //glDisable(GL_DEPTH_TEST);
		 glDepthMask(GL_FALSE);
		 // Set the point size
		 glPointSize(10.0f);
		 glActiveTexture(GL_TEXTURE0);
		 glBindTexture(GL_TEXTURE_2D, textureParticlesGeiserID);
		 shaderParticlesGeiser.turnOn();
		 shaderParticlesGeiser.setFloat("Time", float(currTimeParticlesAnimation - lastTimeParticlesAnimation));
		 shaderParticlesGeiser.setFloat("ParticleLifetime", 3.5f);
		 shaderParticlesGeiser.setInt("ParticleTex", 0);
		 shaderParticlesGeiser.setVectorFloat3("Gravity", glm::value_ptr(glm::vec3(0.0f, 0.3f, 0.0f)));
		 shaderParticlesGeiser.setMatrix4("model", 1, false, glm::value_ptr(modelMatrixParticlesGeiser));
		 glBindVertexArray(VAOParticles);
		 glDrawArrays(GL_POINTS, 0, nParticles);
		 glDepthMask(GL_TRUE);
		 //glEnable(GL_DEPTH_TEST);
		 shaderParticlesGeiser.turnOff();

		 /**********
		  * End Render particles systems
		  */
		}
		else if (renderParticles && it->second.first.compare("geiser12") == 0) {
		/**********
		 * Init Render particles systems
		 */
		 glm::mat4 modelMatrixParticlesGeiser = glm::mat4(1.0);
		 modelMatrixParticlesGeiser = glm::translate(modelMatrixParticlesGeiser, it->second.second);
		 modelMatrixParticlesGeiser[3][1] = terrain.getHeightTerrain(modelMatrixParticlesGeiser[3][0], modelMatrixParticlesGeiser[3][2]);
		 modelMatrixParticlesGeiser = glm::scale(modelMatrixParticlesGeiser, glm::vec3(1.0, 2.0, 1.0));
		 currTimeParticlesAnimation = TimeManager::Instance().GetTime();
		 if (currTimeParticlesAnimation - lastTimeParticlesAnimation > 10.0)
			 lastTimeParticlesAnimation = currTimeParticlesAnimation;
		 //glDisable(GL_DEPTH_TEST);
		 glDepthMask(GL_FALSE);
		 // Set the point size
		 glPointSize(10.0f);
		 glActiveTexture(GL_TEXTURE0);
		 glBindTexture(GL_TEXTURE_2D, textureParticlesGeiserID);
		 shaderParticlesGeiser.turnOn();
		 shaderParticlesGeiser.setFloat("Time", float(currTimeParticlesAnimation - lastTimeParticlesAnimation));
		 shaderParticlesGeiser.setFloat("ParticleLifetime", 3.5f);
		 shaderParticlesGeiser.setInt("ParticleTex", 0);
		 shaderParticlesGeiser.setVectorFloat3("Gravity", glm::value_ptr(glm::vec3(0.0f, 0.3f, 0.0f)));
		 shaderParticlesGeiser.setMatrix4("model", 1, false, glm::value_ptr(modelMatrixParticlesGeiser));
		 glBindVertexArray(VAOParticles);
		 glDrawArrays(GL_POINTS, 0, nParticles);
		 glDepthMask(GL_TRUE);
		 //glEnable(GL_DEPTH_TEST);
		 shaderParticlesGeiser.turnOff();

		 /**********
		  * End Render particles systems
		  */
		}
		else if (renderParticles && it->second.first.compare("geiser13") == 0) {
		/**********
		 * Init Render particles systems
		 */
		 glm::mat4 modelMatrixParticlesGeiser = glm::mat4(1.0);
		 modelMatrixParticlesGeiser = glm::translate(modelMatrixParticlesGeiser, it->second.second);
		 modelMatrixParticlesGeiser[3][1] = terrain.getHeightTerrain(modelMatrixParticlesGeiser[3][0], modelMatrixParticlesGeiser[3][2]);
		 modelMatrixParticlesGeiser = glm::scale(modelMatrixParticlesGeiser, glm::vec3(1.0, 2.0, 1.0));
		 currTimeParticlesAnimation = TimeManager::Instance().GetTime();
		 if (currTimeParticlesAnimation - lastTimeParticlesAnimation > 10.0)
			 lastTimeParticlesAnimation = currTimeParticlesAnimation;
		 //glDisable(GL_DEPTH_TEST);
		 glDepthMask(GL_FALSE);
		 // Set the point size
		 glPointSize(10.0f);
		 glActiveTexture(GL_TEXTURE0);
		 glBindTexture(GL_TEXTURE_2D, textureParticlesGeiserID);
		 shaderParticlesGeiser.turnOn();
		 shaderParticlesGeiser.setFloat("Time", float(currTimeParticlesAnimation - lastTimeParticlesAnimation));
		 shaderParticlesGeiser.setFloat("ParticleLifetime", 3.5f);
		 shaderParticlesGeiser.setInt("ParticleTex", 0);
		 shaderParticlesGeiser.setVectorFloat3("Gravity", glm::value_ptr(glm::vec3(0.0f, 0.3f, 0.0f)));
		 shaderParticlesGeiser.setMatrix4("model", 1, false, glm::value_ptr(modelMatrixParticlesGeiser));
		 glBindVertexArray(VAOParticles);
		 glDrawArrays(GL_POINTS, 0, nParticles);
		 glDepthMask(GL_TRUE);
		 //glEnable(GL_DEPTH_TEST);
		 shaderParticlesGeiser.turnOff();

		 /**********
		  * End Render particles systems
		  */
		}
		else if (renderParticles && it->second.first.compare("geiser14") == 0) {
		/**********
		 * Init Render particles systems
		 */
		 glm::mat4 modelMatrixParticlesGeiser = glm::mat4(1.0);
		 modelMatrixParticlesGeiser = glm::translate(modelMatrixParticlesGeiser, it->second.second);
		 modelMatrixParticlesGeiser[3][1] = terrain.getHeightTerrain(modelMatrixParticlesGeiser[3][0], modelMatrixParticlesGeiser[3][2]);
		 modelMatrixParticlesGeiser = glm::scale(modelMatrixParticlesGeiser, glm::vec3(1.0, 2.0, 1.0));
		 currTimeParticlesAnimation = TimeManager::Instance().GetTime();
		 if (currTimeParticlesAnimation - lastTimeParticlesAnimation > 10.0)
			 lastTimeParticlesAnimation = currTimeParticlesAnimation;
		 //glDisable(GL_DEPTH_TEST);
		 glDepthMask(GL_FALSE);
		 // Set the point size
		 glPointSize(10.0f);
		 glActiveTexture(GL_TEXTURE0);
		 glBindTexture(GL_TEXTURE_2D, textureParticlesGeiserID);
		 shaderParticlesGeiser.turnOn();
		 shaderParticlesGeiser.setFloat("Time", float(currTimeParticlesAnimation - lastTimeParticlesAnimation));
		 shaderParticlesGeiser.setFloat("ParticleLifetime", 3.5f);
		 shaderParticlesGeiser.setInt("ParticleTex", 0);
		 shaderParticlesGeiser.setVectorFloat3("Gravity", glm::value_ptr(glm::vec3(0.0f, 0.3f, 0.0f)));
		 shaderParticlesGeiser.setMatrix4("model", 1, false, glm::value_ptr(modelMatrixParticlesGeiser));
		 glBindVertexArray(VAOParticles);
		 glDrawArrays(GL_POINTS, 0, nParticles);
		 glDepthMask(GL_TRUE);
		 //glEnable(GL_DEPTH_TEST);
		 shaderParticlesGeiser.turnOff();

		 /**********
		  * End Render particles systems
		  */
		}
		else if (renderParticles && it->second.first.compare("geiser15") == 0) {
		/**********
		 * Init Render particles systems
		 */
		 glm::mat4 modelMatrixParticlesGeiser = glm::mat4(1.0);
		 modelMatrixParticlesGeiser = glm::translate(modelMatrixParticlesGeiser, it->second.second);
		 modelMatrixParticlesGeiser[3][1] = terrain.getHeightTerrain(modelMatrixParticlesGeiser[3][0], modelMatrixParticlesGeiser[3][2]);
		 modelMatrixParticlesGeiser = glm::scale(modelMatrixParticlesGeiser, glm::vec3(1.0, 2.0, 1.0));
		 currTimeParticlesAnimation = TimeManager::Instance().GetTime();
		 if (currTimeParticlesAnimation - lastTimeParticlesAnimation > 10.0)
			 lastTimeParticlesAnimation = currTimeParticlesAnimation;
		 //glDisable(GL_DEPTH_TEST);
		 glDepthMask(GL_FALSE);
		 // Set the point size
		 glPointSize(10.0f);
		 glActiveTexture(GL_TEXTURE0);
		 glBindTexture(GL_TEXTURE_2D, textureParticlesGeiserID);
		 shaderParticlesGeiser.turnOn();
		 shaderParticlesGeiser.setFloat("Time", float(currTimeParticlesAnimation - lastTimeParticlesAnimation));
		 shaderParticlesGeiser.setFloat("ParticleLifetime", 3.5f);
		 shaderParticlesGeiser.setInt("ParticleTex", 0);
		 shaderParticlesGeiser.setVectorFloat3("Gravity", glm::value_ptr(glm::vec3(0.0f, 0.3f, 0.0f)));
		 shaderParticlesGeiser.setMatrix4("model", 1, false, glm::value_ptr(modelMatrixParticlesGeiser));
		 glBindVertexArray(VAOParticles);
		 glDrawArrays(GL_POINTS, 0, nParticles);
		 glDepthMask(GL_TRUE);
		 //glEnable(GL_DEPTH_TEST);
		 shaderParticlesGeiser.turnOff();

		 /**********
		  * End Render particles systems
		  */
		}
		else if (renderParticles && it->second.first.compare("geiser16") == 0) {
		/**********
		 * Init Render particles systems
		 */
		 glm::mat4 modelMatrixParticlesGeiser = glm::mat4(1.0);
		 modelMatrixParticlesGeiser = glm::translate(modelMatrixParticlesGeiser, it->second.second);
		 modelMatrixParticlesGeiser[3][1] = terrain.getHeightTerrain(modelMatrixParticlesGeiser[3][0], modelMatrixParticlesGeiser[3][2]);
		 modelMatrixParticlesGeiser = glm::scale(modelMatrixParticlesGeiser, glm::vec3(1.0, 2.0, 1.0));
		 currTimeParticlesAnimation = TimeManager::Instance().GetTime();
		 if (currTimeParticlesAnimation - lastTimeParticlesAnimation > 10.0)
			 lastTimeParticlesAnimation = currTimeParticlesAnimation;
		 //glDisable(GL_DEPTH_TEST);
		 glDepthMask(GL_FALSE);
		 // Set the point size
		 glPointSize(10.0f);
		 glActiveTexture(GL_TEXTURE0);
		 glBindTexture(GL_TEXTURE_2D, textureParticlesGeiserID);
		 shaderParticlesGeiser.turnOn();
		 shaderParticlesGeiser.setFloat("Time", float(currTimeParticlesAnimation - lastTimeParticlesAnimation));
		 shaderParticlesGeiser.setFloat("ParticleLifetime", 3.5f);
		 shaderParticlesGeiser.setInt("ParticleTex", 0);
		 shaderParticlesGeiser.setVectorFloat3("Gravity", glm::value_ptr(glm::vec3(0.0f, 0.3f, 0.0f)));
		 shaderParticlesGeiser.setMatrix4("model", 1, false, glm::value_ptr(modelMatrixParticlesGeiser));
		 glBindVertexArray(VAOParticles);
		 glDrawArrays(GL_POINTS, 0, nParticles);
		 glDepthMask(GL_TRUE);
		 //glEnable(GL_DEPTH_TEST);
		 shaderParticlesGeiser.turnOff();
	
		 /**********
		  * End Render particles systems
		  */
		}
		else if (renderParticles && it->second.first.compare("geiser17") == 0) {
		/**********
		 * Init Render particles systems
		 */
		 glm::mat4 modelMatrixParticlesGeiser = glm::mat4(1.0);
		 modelMatrixParticlesGeiser = glm::translate(modelMatrixParticlesGeiser, it->second.second);
		 modelMatrixParticlesGeiser[3][1] = terrain.getHeightTerrain(modelMatrixParticlesGeiser[3][0], modelMatrixParticlesGeiser[3][2]);
		 modelMatrixParticlesGeiser = glm::scale(modelMatrixParticlesGeiser, glm::vec3(1.0, 2.0, 1.0));
		 currTimeParticlesAnimation = TimeManager::Instance().GetTime();
		 if (currTimeParticlesAnimation - lastTimeParticlesAnimation > 10.0)
			 lastTimeParticlesAnimation = currTimeParticlesAnimation;
		 //glDisable(GL_DEPTH_TEST);
		 glDepthMask(GL_FALSE);
		 // Set the point size
		 glPointSize(10.0f);
		 glActiveTexture(GL_TEXTURE0);
		 glBindTexture(GL_TEXTURE_2D, textureParticlesGeiserID);
		 shaderParticlesGeiser.turnOn();
		 shaderParticlesGeiser.setFloat("Time", float(currTimeParticlesAnimation - lastTimeParticlesAnimation));
		 shaderParticlesGeiser.setFloat("ParticleLifetime", 3.5f);
		 shaderParticlesGeiser.setInt("ParticleTex", 0);
		 shaderParticlesGeiser.setVectorFloat3("Gravity", glm::value_ptr(glm::vec3(0.0f, 0.3f, 0.0f)));
		 shaderParticlesGeiser.setMatrix4("model", 1, false, glm::value_ptr(modelMatrixParticlesGeiser));
		 glBindVertexArray(VAOParticles);
		 glDrawArrays(GL_POINTS, 0, nParticles);
		 glDepthMask(GL_TRUE);
		 //glEnable(GL_DEPTH_TEST);
		 shaderParticlesGeiser.turnOff();
	
		 /**********
		  * End Render particles systems
		  */
		}
		else if (renderParticles && it->second.first.compare("geiser18") == 0) {
		/**********
		 * Init Render particles systems
		 */
		 glm::mat4 modelMatrixParticlesGeiser = glm::mat4(1.0);
		 modelMatrixParticlesGeiser = glm::translate(modelMatrixParticlesGeiser, it->second.second);
		 modelMatrixParticlesGeiser[3][1] = terrain.getHeightTerrain(modelMatrixParticlesGeiser[3][0], modelMatrixParticlesGeiser[3][2]);
		 modelMatrixParticlesGeiser = glm::scale(modelMatrixParticlesGeiser, glm::vec3(1.0, 2.0, 1.0));
		 currTimeParticlesAnimation = TimeManager::Instance().GetTime();
		 if (currTimeParticlesAnimation - lastTimeParticlesAnimation > 10.0)
			 lastTimeParticlesAnimation = currTimeParticlesAnimation;
		 //glDisable(GL_DEPTH_TEST);
		 glDepthMask(GL_FALSE);
		 // Set the point size
		 glPointSize(10.0f);
		 glActiveTexture(GL_TEXTURE0);
		 glBindTexture(GL_TEXTURE_2D, textureParticlesGeiserID);
		 shaderParticlesGeiser.turnOn();
		 shaderParticlesGeiser.setFloat("Time", float(currTimeParticlesAnimation - lastTimeParticlesAnimation));
		 shaderParticlesGeiser.setFloat("ParticleLifetime", 3.5f);
		 shaderParticlesGeiser.setInt("ParticleTex", 0);
		 shaderParticlesGeiser.setVectorFloat3("Gravity", glm::value_ptr(glm::vec3(0.0f, 0.3f, 0.0f)));
		 shaderParticlesGeiser.setMatrix4("model", 1, false, glm::value_ptr(modelMatrixParticlesGeiser));
		 glBindVertexArray(VAOParticles);
		 glDrawArrays(GL_POINTS, 0, nParticles);
		 glDepthMask(GL_TRUE);
		 //glEnable(GL_DEPTH_TEST);
		 shaderParticlesGeiser.turnOff();
	
		 /**********
		  * End Render particles systems
		  */
		}
		else if (renderParticles && it->second.first.compare("geiser19") == 0) {
		/**********
		 * Init Render particles systems
		 */
		 glm::mat4 modelMatrixParticlesGeiser = glm::mat4(1.0);
		 modelMatrixParticlesGeiser = glm::translate(modelMatrixParticlesGeiser, it->second.second);
		 modelMatrixParticlesGeiser[3][1] = terrain.getHeightTerrain(modelMatrixParticlesGeiser[3][0], modelMatrixParticlesGeiser[3][2]);
		 modelMatrixParticlesGeiser = glm::scale(modelMatrixParticlesGeiser, glm::vec3(1.0, 2.0, 1.0));
		 currTimeParticlesAnimation = TimeManager::Instance().GetTime();
		 if (currTimeParticlesAnimation - lastTimeParticlesAnimation > 10.0)
			 lastTimeParticlesAnimation = currTimeParticlesAnimation;
		 //glDisable(GL_DEPTH_TEST);
		 glDepthMask(GL_FALSE);
		 // Set the point size
		 glPointSize(10.0f);
		 glActiveTexture(GL_TEXTURE0);
		 glBindTexture(GL_TEXTURE_2D, textureParticlesGeiserID);
		 shaderParticlesGeiser.turnOn();
		 shaderParticlesGeiser.setFloat("Time", float(currTimeParticlesAnimation - lastTimeParticlesAnimation));
		 shaderParticlesGeiser.setFloat("ParticleLifetime", 3.5f);
		 shaderParticlesGeiser.setInt("ParticleTex", 0);
		 shaderParticlesGeiser.setVectorFloat3("Gravity", glm::value_ptr(glm::vec3(0.0f, 0.3f, 0.0f)));
		 shaderParticlesGeiser.setMatrix4("model", 1, false, glm::value_ptr(modelMatrixParticlesGeiser));
		 glBindVertexArray(VAOParticles);
		 glDrawArrays(GL_POINTS, 0, nParticles);
		 glDepthMask(GL_TRUE);
		 //glEnable(GL_DEPTH_TEST);
		 shaderParticlesGeiser.turnOff();
	
		 /**********
		  * End Render particles systems
		  */
		}
		else if (renderParticles && it->second.first.compare("geiser20") == 0) {
		/**********
		 * Init Render particles systems
		 */
		 glm::mat4 modelMatrixParticlesGeiser = glm::mat4(1.0);
		 modelMatrixParticlesGeiser = glm::translate(modelMatrixParticlesGeiser, it->second.second);
		 modelMatrixParticlesGeiser[3][1] = terrain.getHeightTerrain(modelMatrixParticlesGeiser[3][0], modelMatrixParticlesGeiser[3][2]);
		 modelMatrixParticlesGeiser = glm::scale(modelMatrixParticlesGeiser, glm::vec3(1.0, 2.0, 1.0));
		 currTimeParticlesAnimation = TimeManager::Instance().GetTime();
		 if (currTimeParticlesAnimation - lastTimeParticlesAnimation > 10.0)
			 lastTimeParticlesAnimation = currTimeParticlesAnimation;
		 //glDisable(GL_DEPTH_TEST);
		 glDepthMask(GL_FALSE);
		 // Set the point size
		 glPointSize(10.0f);
		 glActiveTexture(GL_TEXTURE0);
		 glBindTexture(GL_TEXTURE_2D, textureParticlesGeiserID);
		 shaderParticlesGeiser.turnOn();
		 shaderParticlesGeiser.setFloat("Time", float(currTimeParticlesAnimation - lastTimeParticlesAnimation));
		 shaderParticlesGeiser.setFloat("ParticleLifetime", 3.5f);
		 shaderParticlesGeiser.setInt("ParticleTex", 0);
		 shaderParticlesGeiser.setVectorFloat3("Gravity", glm::value_ptr(glm::vec3(0.0f, 0.3f, 0.0f)));
		 shaderParticlesGeiser.setMatrix4("model", 1, false, glm::value_ptr(modelMatrixParticlesGeiser));
		 glBindVertexArray(VAOParticles);
		 glDrawArrays(GL_POINTS, 0, nParticles);
		 glDepthMask(GL_TRUE);
		 //glEnable(GL_DEPTH_TEST);
		 shaderParticlesGeiser.turnOff();
		
		 /**********
		  * End Render particles systems
		  */
		}
		else if (renderParticles && it->second.first.compare("geiser21") == 0) {
		/**********
		 * Init Render particles systems
		 */
		 glm::mat4 modelMatrixParticlesGeiser = glm::mat4(1.0);
		 modelMatrixParticlesGeiser = glm::translate(modelMatrixParticlesGeiser, it->second.second);
		 modelMatrixParticlesGeiser[3][1] = terrain.getHeightTerrain(modelMatrixParticlesGeiser[3][0], modelMatrixParticlesGeiser[3][2]);
		 modelMatrixParticlesGeiser = glm::scale(modelMatrixParticlesGeiser, glm::vec3(1.0, 2.0, 1.0));
		 currTimeParticlesAnimation = TimeManager::Instance().GetTime();
		 if (currTimeParticlesAnimation - lastTimeParticlesAnimation > 10.0)
			 lastTimeParticlesAnimation = currTimeParticlesAnimation;
		 //glDisable(GL_DEPTH_TEST);
		 glDepthMask(GL_FALSE);
		 // Set the point size
		 glPointSize(10.0f);
		 glActiveTexture(GL_TEXTURE0);
		 glBindTexture(GL_TEXTURE_2D, textureParticlesGeiserID);
		 shaderParticlesGeiser.turnOn();
		 shaderParticlesGeiser.setFloat("Time", float(currTimeParticlesAnimation - lastTimeParticlesAnimation));
		 shaderParticlesGeiser.setFloat("ParticleLifetime", 3.5f);
		 shaderParticlesGeiser.setInt("ParticleTex", 0);
		 shaderParticlesGeiser.setVectorFloat3("Gravity", glm::value_ptr(glm::vec3(0.0f, 0.3f, 0.0f)));
		 shaderParticlesGeiser.setMatrix4("model", 1, false, glm::value_ptr(modelMatrixParticlesGeiser));
		 glBindVertexArray(VAOParticles);
		 glDrawArrays(GL_POINTS, 0, nParticles);
		 glDepthMask(GL_TRUE);
		 //glEnable(GL_DEPTH_TEST);
		 shaderParticlesGeiser.turnOff();

		 /**********
		  * End Render particles systems
		  */
		}
		else if (renderParticles && it->second.first.compare("geiser22") == 0) {
		/**********
		 * Init Render particles systems
		 */
		 glm::mat4 modelMatrixParticlesGeiser = glm::mat4(1.0);
		 modelMatrixParticlesGeiser = glm::translate(modelMatrixParticlesGeiser, it->second.second);
		 modelMatrixParticlesGeiser[3][1] = terrain.getHeightTerrain(modelMatrixParticlesGeiser[3][0], modelMatrixParticlesGeiser[3][2]);
		 modelMatrixParticlesGeiser = glm::scale(modelMatrixParticlesGeiser, glm::vec3(1.0, 2.0, 1.0));
		 currTimeParticlesAnimation = TimeManager::Instance().GetTime();
		 if (currTimeParticlesAnimation - lastTimeParticlesAnimation > 10.0)
			 lastTimeParticlesAnimation = currTimeParticlesAnimation;
		 //glDisable(GL_DEPTH_TEST);
		 glDepthMask(GL_FALSE);
		 // Set the point size
		 glPointSize(10.0f);
		 glActiveTexture(GL_TEXTURE0);
		 glBindTexture(GL_TEXTURE_2D, textureParticlesGeiserID);
		 shaderParticlesGeiser.turnOn();
		 shaderParticlesGeiser.setFloat("Time", float(currTimeParticlesAnimation - lastTimeParticlesAnimation));
		 shaderParticlesGeiser.setFloat("ParticleLifetime", 3.5f);
		 shaderParticlesGeiser.setInt("ParticleTex", 0);
		 shaderParticlesGeiser.setVectorFloat3("Gravity", glm::value_ptr(glm::vec3(0.0f, 0.3f, 0.0f)));
		 shaderParticlesGeiser.setMatrix4("model", 1, false, glm::value_ptr(modelMatrixParticlesGeiser));
		 glBindVertexArray(VAOParticles);
		 glDrawArrays(GL_POINTS, 0, nParticles);
		 glDepthMask(GL_TRUE);
		 //glEnable(GL_DEPTH_TEST);
		 shaderParticlesGeiser.turnOff();
	
		 /**********
		  * End Render particles systems
		  */
		}
		else if (renderParticles && it->second.first.compare("geiser23") == 0) {
		/**********
		 * Init Render particles systems
		 */
		 glm::mat4 modelMatrixParticlesGeiser = glm::mat4(1.0);
		 modelMatrixParticlesGeiser = glm::translate(modelMatrixParticlesGeiser, it->second.second);
		 modelMatrixParticlesGeiser[3][1] = terrain.getHeightTerrain(modelMatrixParticlesGeiser[3][0], modelMatrixParticlesGeiser[3][2]);
		 modelMatrixParticlesGeiser = glm::scale(modelMatrixParticlesGeiser, glm::vec3(1.0, 2.0, 1.0));
		 currTimeParticlesAnimation = TimeManager::Instance().GetTime();
		 if (currTimeParticlesAnimation - lastTimeParticlesAnimation > 10.0)
			 lastTimeParticlesAnimation = currTimeParticlesAnimation;
		 //glDisable(GL_DEPTH_TEST);
		 glDepthMask(GL_FALSE);
		 // Set the point size
		 glPointSize(10.0f);
		 glActiveTexture(GL_TEXTURE0);
		 glBindTexture(GL_TEXTURE_2D, textureParticlesGeiserID);
		 shaderParticlesGeiser.turnOn();
		 shaderParticlesGeiser.setFloat("Time", float(currTimeParticlesAnimation - lastTimeParticlesAnimation));
		 shaderParticlesGeiser.setFloat("ParticleLifetime", 3.5f);
		 shaderParticlesGeiser.setInt("ParticleTex", 0);
		 shaderParticlesGeiser.setVectorFloat3("Gravity", glm::value_ptr(glm::vec3(0.0f, 0.3f, 0.0f)));
		 shaderParticlesGeiser.setMatrix4("model", 1, false, glm::value_ptr(modelMatrixParticlesGeiser));
		 glBindVertexArray(VAOParticles);
		 glDrawArrays(GL_POINTS, 0, nParticles);
		 glDepthMask(GL_TRUE);
		 //glEnable(GL_DEPTH_TEST);
		 shaderParticlesGeiser.turnOff();

		 /**********
		  * End Render particles systems
		  */
		}
		else if (renderParticles && it->second.first.compare("geiser24") == 0) {
		/**********
		 * Init Render particles systems
		 */
		 glm::mat4 modelMatrixParticlesGeiser = glm::mat4(1.0);
		 modelMatrixParticlesGeiser = glm::translate(modelMatrixParticlesGeiser, it->second.second);
		 modelMatrixParticlesGeiser[3][1] = terrain.getHeightTerrain(modelMatrixParticlesGeiser[3][0], modelMatrixParticlesGeiser[3][2]);
		 modelMatrixParticlesGeiser = glm::scale(modelMatrixParticlesGeiser, glm::vec3(1.0, 2.0, 1.0));
		 currTimeParticlesAnimation = TimeManager::Instance().GetTime();
		 if (currTimeParticlesAnimation - lastTimeParticlesAnimation > 10.0)
			 lastTimeParticlesAnimation = currTimeParticlesAnimation;
		 //glDisable(GL_DEPTH_TEST);
		 glDepthMask(GL_FALSE);
		 // Set the point size
		 glPointSize(10.0f);
		 glActiveTexture(GL_TEXTURE0);
		 glBindTexture(GL_TEXTURE_2D, textureParticlesGeiserID);
		 shaderParticlesGeiser.turnOn();
		 shaderParticlesGeiser.setFloat("Time", float(currTimeParticlesAnimation - lastTimeParticlesAnimation));
		 shaderParticlesGeiser.setFloat("ParticleLifetime", 3.5f);
		 shaderParticlesGeiser.setInt("ParticleTex", 0);
		 shaderParticlesGeiser.setVectorFloat3("Gravity", glm::value_ptr(glm::vec3(0.0f, 0.3f, 0.0f)));
		 shaderParticlesGeiser.setMatrix4("model", 1, false, glm::value_ptr(modelMatrixParticlesGeiser));
		 glBindVertexArray(VAOParticles);
		 glDrawArrays(GL_POINTS, 0, nParticles);
		 glDepthMask(GL_TRUE);
		 //glEnable(GL_DEPTH_TEST);
		 shaderParticlesGeiser.turnOff();

		 /**********
		  * End Render particles systems
		  */
		}
		else if (renderParticles && it->second.first.compare("geiser25") == 0) {
		/**********
		 * Init Render particles systems
		 */
		 glm::mat4 modelMatrixParticlesGeiser = glm::mat4(1.0);
		 modelMatrixParticlesGeiser = glm::translate(modelMatrixParticlesGeiser, it->second.second);
		 modelMatrixParticlesGeiser[3][1] = terrain.getHeightTerrain(modelMatrixParticlesGeiser[3][0], modelMatrixParticlesGeiser[3][2]);
		 modelMatrixParticlesGeiser = glm::scale(modelMatrixParticlesGeiser, glm::vec3(1.0, 2.0, 1.0));
		 currTimeParticlesAnimation = TimeManager::Instance().GetTime();
		 if (currTimeParticlesAnimation - lastTimeParticlesAnimation > 10.0)
			 lastTimeParticlesAnimation = currTimeParticlesAnimation;
		 //glDisable(GL_DEPTH_TEST);
		 glDepthMask(GL_FALSE);
		 // Set the point size
		 glPointSize(10.0f);
		 glActiveTexture(GL_TEXTURE0);
		 glBindTexture(GL_TEXTURE_2D, textureParticlesGeiserID);
		 shaderParticlesGeiser.turnOn();
		 shaderParticlesGeiser.setFloat("Time", float(currTimeParticlesAnimation - lastTimeParticlesAnimation));
		 shaderParticlesGeiser.setFloat("ParticleLifetime", 3.5f);
		 shaderParticlesGeiser.setInt("ParticleTex", 0);
		 shaderParticlesGeiser.setVectorFloat3("Gravity", glm::value_ptr(glm::vec3(0.0f, 0.3f, 0.0f)));
		 shaderParticlesGeiser.setMatrix4("model", 1, false, glm::value_ptr(modelMatrixParticlesGeiser));
		 glBindVertexArray(VAOParticles);
		 glDrawArrays(GL_POINTS, 0, nParticles);
		 glDepthMask(GL_TRUE);
		 //glEnable(GL_DEPTH_TEST);
		 shaderParticlesGeiser.turnOff();

		 /**********
		  * End Render particles systems
		  */
		}
		else if (renderParticles && it->second.first.compare("geiser26") == 0) {
		/**********
		 * Init Render particles systems
		 */
		 glm::mat4 modelMatrixParticlesGeiser = glm::mat4(1.0);
		 modelMatrixParticlesGeiser = glm::translate(modelMatrixParticlesGeiser, it->second.second);
		 modelMatrixParticlesGeiser[3][1] = terrain.getHeightTerrain(modelMatrixParticlesGeiser[3][0], modelMatrixParticlesGeiser[3][2]);
		 modelMatrixParticlesGeiser = glm::scale(modelMatrixParticlesGeiser, glm::vec3(1.0, 2.0, 1.0));
		 currTimeParticlesAnimation = TimeManager::Instance().GetTime();
		 if (currTimeParticlesAnimation - lastTimeParticlesAnimation > 10.0)
			 lastTimeParticlesAnimation = currTimeParticlesAnimation;
		 //glDisable(GL_DEPTH_TEST);
		 glDepthMask(GL_FALSE);
		 // Set the point size
		 glPointSize(10.0f);
		 glActiveTexture(GL_TEXTURE0);
		 glBindTexture(GL_TEXTURE_2D, textureParticlesGeiserID);
		 shaderParticlesGeiser.turnOn();
		 shaderParticlesGeiser.setFloat("Time", float(currTimeParticlesAnimation - lastTimeParticlesAnimation));
		 shaderParticlesGeiser.setFloat("ParticleLifetime", 3.5f);
		 shaderParticlesGeiser.setInt("ParticleTex", 0);
		 shaderParticlesGeiser.setVectorFloat3("Gravity", glm::value_ptr(glm::vec3(0.0f, 0.3f, 0.0f)));
		 shaderParticlesGeiser.setMatrix4("model", 1, false, glm::value_ptr(modelMatrixParticlesGeiser));
		 glBindVertexArray(VAOParticles);
		 glDrawArrays(GL_POINTS, 0, nParticles);
		 glDepthMask(GL_TRUE);
		 //glEnable(GL_DEPTH_TEST);
		 shaderParticlesGeiser.turnOff();

		 /**********
		  * End Render particles systems
		  */
		}
		else if(renderParticles && it->second.first.compare("fire0") == 0){
			/**********
			 * Init Render particles systems
			 */
			lastTimeParticlesAnimationFire = currTimeParticlesAnimationFire;
			currTimeParticlesAnimationFire = TimeManager::Instance().GetTime();

			shaderParticlesFire.setInt("Pass", 1);
			shaderParticlesFire.setFloat("Time", currTimeParticlesAnimationFire);
			shaderParticlesFire.setFloat("DeltaT", currTimeParticlesAnimationFire - lastTimeParticlesAnimationFire);

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_1D, texId);
			glEnable(GL_RASTERIZER_DISCARD);
			glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, feedback[drawBuf]);
			glBeginTransformFeedback(GL_POINTS);
			glBindVertexArray(particleArray[1-drawBuf]);
			glVertexAttribDivisor(0,0);
			glVertexAttribDivisor(1,0);
			glVertexAttribDivisor(2,0);
			glDrawArrays(GL_POINTS, 0, nParticlesFire);
			glEndTransformFeedback();
			glDisable(GL_RASTERIZER_DISCARD);

			shaderParticlesFire.setInt("Pass", 2);
			glm::mat4 modelFireParticles = /*glm::mat4(1.0)*/ modelMatrixDoor;
			modelFireParticles = glm::translate(modelFireParticles, it->second.second);
			//modelFireParticles[3][1] = terrain.getHeightTerrain(modelFireParticles[3][0], modelFireParticles[3][2]);
			modelFireParticles = glm::scale(modelFireParticles, glm::vec3(0.06, 1.0, 1.0));
			shaderParticlesFire.setMatrix4("model", 1, false, glm::value_ptr(modelFireParticles));

			shaderParticlesFire.turnOn();
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textureParticleFireID);
			glDepthMask(GL_FALSE);
			glBindVertexArray(particleArray[drawBuf]);
			glVertexAttribDivisor(0,1);
			glVertexAttribDivisor(1,1);
			glVertexAttribDivisor(2,1);
			glDrawArraysInstanced(GL_TRIANGLES, 0, 6, nParticlesFire);
			glBindVertexArray(0);
			glDepthMask(GL_TRUE);
			drawBuf = 1 - drawBuf;
			shaderParticlesFire.turnOff();

			/****************************+
			 * Open AL sound data
			 */
			//source1Pos[0] = modelFireParticles[3].x;
			//source1Pos[1] = modelFireParticles[3].y;
			//source1Pos[2] = modelFireParticles[3].z;
			//alSourcefv(source[1], AL_POSITION, source1Pos);

			/**********
			 * End Render particles systems
			 */
		}
		else if (renderParticles && it->second.first.compare("fire1") == 0) {
		/**********
		 * Init Render particles systems
		 */
		 lastTimeParticlesAnimationFire = currTimeParticlesAnimationFire;
		 currTimeParticlesAnimationFire = TimeManager::Instance().GetTime();

		 shaderParticlesFire.setInt("Pass", 1);
		 shaderParticlesFire.setFloat("Time", currTimeParticlesAnimationFire);
		 shaderParticlesFire.setFloat("DeltaT", currTimeParticlesAnimationFire - lastTimeParticlesAnimationFire);

		 glActiveTexture(GL_TEXTURE1);
		 glBindTexture(GL_TEXTURE_1D, texId);
		 glEnable(GL_RASTERIZER_DISCARD);
		 glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, feedback[drawBuf]);
		 glBeginTransformFeedback(GL_POINTS);
		 glBindVertexArray(particleArray[1 - drawBuf]);
		 glVertexAttribDivisor(0, 0);
		 glVertexAttribDivisor(1, 0);
		 glVertexAttribDivisor(2, 0);
		 glDrawArrays(GL_POINTS, 0, nParticlesFire);
		 glEndTransformFeedback();
		 glDisable(GL_RASTERIZER_DISCARD);

		 shaderParticlesFire.setInt("Pass", 2);
		 glm::mat4 modelFireParticles = /*glm::mat4(1.0)*/ modelMatrixDoor;
		 modelFireParticles = glm::translate(modelFireParticles, it->second.second);
		 //modelFireParticles[3][1] = terrain.getHeightTerrain(modelFireParticles[3][0], modelFireParticles[3][2]);
		 modelFireParticles = glm::scale(modelFireParticles, glm::vec3(0.06, 1.0, 1.0));
		 shaderParticlesFire.setMatrix4("model", 1, false, glm::value_ptr(modelFireParticles));

		 shaderParticlesFire.turnOn();
		 glActiveTexture(GL_TEXTURE0);
		 glBindTexture(GL_TEXTURE_2D, textureParticleFireID);
		 glDepthMask(GL_FALSE);
		 glBindVertexArray(particleArray[drawBuf]);
		 glVertexAttribDivisor(0, 1);
		 glVertexAttribDivisor(1, 1);
		 glVertexAttribDivisor(2, 1);
		 glDrawArraysInstanced(GL_TRIANGLES, 0, 6, nParticlesFire);
		 glBindVertexArray(0);
		 glDepthMask(GL_TRUE);
		 drawBuf = 1 - drawBuf;
		 shaderParticlesFire.turnOff();

		 /****************************+
		  * Open AL sound data
		  */
		  //source1Pos[0] = modelFireParticles[3].x;
		  //source1Pos[1] = modelFireParticles[3].y;
		  //source1Pos[2] = modelFireParticles[3].z;
		  //alSourcefv(source[1], AL_POSITION, source1Pos);

		  /**********
		   * End Render particles systems
		   */
		}
		else if (renderParticles && it->second.first.compare("fire2") == 0) {
		/**********
		 * Init Render particles systems
		 */
		 lastTimeParticlesAnimationFire = currTimeParticlesAnimationFire;
		 currTimeParticlesAnimationFire = TimeManager::Instance().GetTime();

		 shaderParticlesFire.setInt("Pass", 1);
		 shaderParticlesFire.setFloat("Time", currTimeParticlesAnimationFire);
		 shaderParticlesFire.setFloat("DeltaT", currTimeParticlesAnimationFire - lastTimeParticlesAnimationFire);

		 glActiveTexture(GL_TEXTURE1);
		 glBindTexture(GL_TEXTURE_1D, texId);
		 glEnable(GL_RASTERIZER_DISCARD);
		 glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, feedback[drawBuf]);
		 glBeginTransformFeedback(GL_POINTS);
		 glBindVertexArray(particleArray[1 - drawBuf]);
		 glVertexAttribDivisor(0, 0);
		 glVertexAttribDivisor(1, 0);
		 glVertexAttribDivisor(2, 0);
		 glDrawArrays(GL_POINTS, 0, nParticlesFire);
		 glEndTransformFeedback();
		 glDisable(GL_RASTERIZER_DISCARD);

		 shaderParticlesFire.setInt("Pass", 2);
		 glm::mat4 modelFireParticles = /*glm::mat4(1.0)*/ modelMatrixDoor;
		 modelFireParticles = glm::translate(modelFireParticles, it->second.second);
		 //modelFireParticles[3][1] = terrain.getHeightTerrain(modelFireParticles[3][0], modelFireParticles[3][2]);
		 modelFireParticles = glm::scale(modelFireParticles, glm::vec3(0.06, 1.0, 1.0));
		 shaderParticlesFire.setMatrix4("model", 1, false, glm::value_ptr(modelFireParticles));

		 shaderParticlesFire.turnOn();
		 glActiveTexture(GL_TEXTURE0);
		 glBindTexture(GL_TEXTURE_2D, textureParticleFireID);
		 glDepthMask(GL_FALSE);
		 glBindVertexArray(particleArray[drawBuf]);
		 glVertexAttribDivisor(0, 1);
		 glVertexAttribDivisor(1, 1);
		 glVertexAttribDivisor(2, 1);
		 glDrawArraysInstanced(GL_TRIANGLES, 0, 6, nParticlesFire);
		 glBindVertexArray(0);
		 glDepthMask(GL_TRUE);
		 drawBuf = 1 - drawBuf;
		 shaderParticlesFire.turnOff();

		 /****************************+
		  * Open AL sound data
		  */
		  //source1Pos[0] = modelFireParticles[3].x;
		  //source1Pos[1] = modelFireParticles[3].y;
		  //source1Pos[2] = modelFireParticles[3].z;
		  //alSourcefv(source[1], AL_POSITION, source1Pos);

		  /**********
		   * End Render particles systems
		   */
		}
		else if (renderParticles && it->second.first.compare("fire3") == 0) {
		/**********
		 * Init Render particles systems
		 */
		 lastTimeParticlesAnimationFire = currTimeParticlesAnimationFire;
		 currTimeParticlesAnimationFire = TimeManager::Instance().GetTime();

		 shaderParticlesFire.setInt("Pass", 1);
		 shaderParticlesFire.setFloat("Time", currTimeParticlesAnimationFire);
		 shaderParticlesFire.setFloat("DeltaT", currTimeParticlesAnimationFire - lastTimeParticlesAnimationFire);

		 glActiveTexture(GL_TEXTURE1);
		 glBindTexture(GL_TEXTURE_1D, texId);
		 glEnable(GL_RASTERIZER_DISCARD);
		 glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, feedback[drawBuf]);
		 glBeginTransformFeedback(GL_POINTS);
		 glBindVertexArray(particleArray[1 - drawBuf]);
		 glVertexAttribDivisor(0, 0);
		 glVertexAttribDivisor(1, 0);
		 glVertexAttribDivisor(2, 0);
		 glDrawArrays(GL_POINTS, 0, nParticlesFire);
		 glEndTransformFeedback();
		 glDisable(GL_RASTERIZER_DISCARD);

		 shaderParticlesFire.setInt("Pass", 2);
		 glm::mat4 modelFireParticles = /*glm::mat4(1.0)*/ modelMatrixDoor;
		 modelFireParticles = glm::translate(modelFireParticles, it->second.second);
		 //modelFireParticles[3][1] = terrain.getHeightTerrain(modelFireParticles[3][0], modelFireParticles[3][2]);
		 modelFireParticles = glm::scale(modelFireParticles, glm::vec3(0.06, 1.0, 1.0));
		 shaderParticlesFire.setMatrix4("model", 1, false, glm::value_ptr(modelFireParticles));

		 shaderParticlesFire.turnOn();
		 glActiveTexture(GL_TEXTURE0);
		 glBindTexture(GL_TEXTURE_2D, textureParticleFireID);
		 glDepthMask(GL_FALSE);
		 glBindVertexArray(particleArray[drawBuf]);
		 glVertexAttribDivisor(0, 1);
		 glVertexAttribDivisor(1, 1);
		 glVertexAttribDivisor(2, 1);
		 glDrawArraysInstanced(GL_TRIANGLES, 0, 6, nParticlesFire);
		 glBindVertexArray(0);
		 glDepthMask(GL_TRUE);
		 drawBuf = 1 - drawBuf;
		 shaderParticlesFire.turnOff();

		 /****************************+
		  * Open AL sound data
		  */
		  //source1Pos[0] = modelFireParticles[3].x;
		  //source1Pos[1] = modelFireParticles[3].y;
		  //source1Pos[2] = modelFireParticles[3].z;
		  //alSourcefv(source[1], AL_POSITION, source1Pos);

		  /**********
		   * End Render particles systems
		   */
		}
		else if (renderParticles && it->second.first.compare("fire4") == 0) {
		/**********
		 * Init Render particles systems
		 */
		 lastTimeParticlesAnimationFire = currTimeParticlesAnimationFire;
		 currTimeParticlesAnimationFire = TimeManager::Instance().GetTime();

		 shaderParticlesFire.setInt("Pass", 1);
		 shaderParticlesFire.setFloat("Time", currTimeParticlesAnimationFire);
		 shaderParticlesFire.setFloat("DeltaT", currTimeParticlesAnimationFire - lastTimeParticlesAnimationFire);

		 glActiveTexture(GL_TEXTURE1);
		 glBindTexture(GL_TEXTURE_1D, texId);
		 glEnable(GL_RASTERIZER_DISCARD);
		 glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, feedback[drawBuf]);
		 glBeginTransformFeedback(GL_POINTS);
		 glBindVertexArray(particleArray[1 - drawBuf]);
		 glVertexAttribDivisor(0, 0);
		 glVertexAttribDivisor(1, 0);
		 glVertexAttribDivisor(2, 0);
		 glDrawArrays(GL_POINTS, 0, nParticlesFire);
		 glEndTransformFeedback();
		 glDisable(GL_RASTERIZER_DISCARD);

		 shaderParticlesFire.setInt("Pass", 2);
		 glm::mat4 modelFireParticles = /*glm::mat4(1.0)*/ modelMatrixDoor;
		 modelFireParticles = glm::translate(modelFireParticles, it->second.second);
		 //modelFireParticles[3][1] = terrain.getHeightTerrain(modelFireParticles[3][0], modelFireParticles[3][2]);
		 modelFireParticles = glm::scale(modelFireParticles, glm::vec3(0.06, 1.0, 1.0));
		 shaderParticlesFire.setMatrix4("model", 1, false, glm::value_ptr(modelFireParticles));

		 shaderParticlesFire.turnOn();
		 glActiveTexture(GL_TEXTURE0);
		 glBindTexture(GL_TEXTURE_2D, textureParticleFireID);
		 glDepthMask(GL_FALSE);
		 glBindVertexArray(particleArray[drawBuf]);
		 glVertexAttribDivisor(0, 1);
		 glVertexAttribDivisor(1, 1);
		 glVertexAttribDivisor(2, 1);
		 glDrawArraysInstanced(GL_TRIANGLES, 0, 6, nParticlesFire);
		 glBindVertexArray(0);
		 glDepthMask(GL_TRUE);
		 drawBuf = 1 - drawBuf;
		 shaderParticlesFire.turnOff();

		 /****************************+
		  * Open AL sound data
		  */
		  //source1Pos[0] = modelFireParticles[3].x;
		  //source1Pos[1] = modelFireParticles[3].y;
		  //source1Pos[2] = modelFireParticles[3].z;
		  //alSourcefv(source[1], AL_POSITION, source1Pos);

		  /**********
		   * End Render particles systems
		   */
		}
		else if (renderParticles && it->second.first.compare("fire5") == 0) {
		/**********
		 * Init Render particles systems
		 */
		 lastTimeParticlesAnimationFire = currTimeParticlesAnimationFire;
		 currTimeParticlesAnimationFire = TimeManager::Instance().GetTime();

		 shaderParticlesFire.setInt("Pass", 1);
		 shaderParticlesFire.setFloat("Time", currTimeParticlesAnimationFire);
		 shaderParticlesFire.setFloat("DeltaT", currTimeParticlesAnimationFire - lastTimeParticlesAnimationFire);

		 glActiveTexture(GL_TEXTURE1);
		 glBindTexture(GL_TEXTURE_1D, texId);
		 glEnable(GL_RASTERIZER_DISCARD);
		 glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, feedback[drawBuf]);
		 glBeginTransformFeedback(GL_POINTS);
		 glBindVertexArray(particleArray[1 - drawBuf]);
		 glVertexAttribDivisor(0, 0);
		 glVertexAttribDivisor(1, 0);
		 glVertexAttribDivisor(2, 0);
		 glDrawArrays(GL_POINTS, 0, nParticlesFire);
		 glEndTransformFeedback();
		 glDisable(GL_RASTERIZER_DISCARD);

		 shaderParticlesFire.setInt("Pass", 2);
		 glm::mat4 modelFireParticles = /*glm::mat4(1.0)*/ modelMatrixDoor;
		 modelFireParticles = glm::translate(modelFireParticles, it->second.second);
		 //modelFireParticles[3][1] = terrain.getHeightTerrain(modelFireParticles[3][0], modelFireParticles[3][2]);
		 modelFireParticles = glm::scale(modelFireParticles, glm::vec3(0.06, 1.0, 1.0));
		 shaderParticlesFire.setMatrix4("model", 1, false, glm::value_ptr(modelFireParticles));

		 shaderParticlesFire.turnOn();
		 glActiveTexture(GL_TEXTURE0);
		 glBindTexture(GL_TEXTURE_2D, textureParticleFireID);
		 glDepthMask(GL_FALSE);
		 glBindVertexArray(particleArray[drawBuf]);
		 glVertexAttribDivisor(0, 1);
		 glVertexAttribDivisor(1, 1);
		 glVertexAttribDivisor(2, 1);
		 glDrawArraysInstanced(GL_TRIANGLES, 0, 6, nParticlesFire);
		 glBindVertexArray(0);
		 glDepthMask(GL_TRUE);
		 drawBuf = 1 - drawBuf;
		 shaderParticlesFire.turnOff();

		 /****************************+
		  * Open AL sound data
		  */
		  //source1Pos[0] = modelFireParticles[3].x;
		  //source1Pos[1] = modelFireParticles[3].y;
		  //source1Pos[2] = modelFireParticles[3].z;
		  //alSourcefv(source[1], AL_POSITION, source1Pos);

		  /**********
		   * End Render particles systems
		   */
		}
		else if (renderParticles && it->second.first.compare("fire6") == 0) {
		/**********
		 * Init Render particles systems
		 */
		 lastTimeParticlesAnimationFire = currTimeParticlesAnimationFire;
		 currTimeParticlesAnimationFire = TimeManager::Instance().GetTime();

		 shaderParticlesFire.setInt("Pass", 1);
		 shaderParticlesFire.setFloat("Time", currTimeParticlesAnimationFire);
		 shaderParticlesFire.setFloat("DeltaT", currTimeParticlesAnimationFire - lastTimeParticlesAnimationFire);

		 glActiveTexture(GL_TEXTURE1);
		 glBindTexture(GL_TEXTURE_1D, texId);
		 glEnable(GL_RASTERIZER_DISCARD);
		 glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, feedback[drawBuf]);
		 glBeginTransformFeedback(GL_POINTS);
		 glBindVertexArray(particleArray[1 - drawBuf]);
		 glVertexAttribDivisor(0, 0);
		 glVertexAttribDivisor(1, 0);
		 glVertexAttribDivisor(2, 0);
		 glDrawArrays(GL_POINTS, 0, nParticlesFire);
		 glEndTransformFeedback();
		 glDisable(GL_RASTERIZER_DISCARD);

		 shaderParticlesFire.setInt("Pass", 2);
		 glm::mat4 modelFireParticles = /*glm::mat4(1.0)*/ modelMatrixDoor;
		 modelFireParticles = glm::translate(modelFireParticles, it->second.second);
		 //modelFireParticles[3][1] = terrain.getHeightTerrain(modelFireParticles[3][0], modelFireParticles[3][2]);
		 modelFireParticles = glm::scale(modelFireParticles, glm::vec3(0.06, 1.0, 1.0));
		 shaderParticlesFire.setMatrix4("model", 1, false, glm::value_ptr(modelFireParticles));

		 shaderParticlesFire.turnOn();
		 glActiveTexture(GL_TEXTURE0);
		 glBindTexture(GL_TEXTURE_2D, textureParticleFireID);
		 glDepthMask(GL_FALSE);
		 glBindVertexArray(particleArray[drawBuf]);
		 glVertexAttribDivisor(0, 1);
		 glVertexAttribDivisor(1, 1);
		 glVertexAttribDivisor(2, 1);
		 glDrawArraysInstanced(GL_TRIANGLES, 0, 6, nParticlesFire);
		 glBindVertexArray(0);
		 glDepthMask(GL_TRUE);
		 drawBuf = 1 - drawBuf;
		 shaderParticlesFire.turnOff();

		 /****************************+
		  * Open AL sound data
		  */
		  //source1Pos[0] = modelFireParticles[3].x;
		  //source1Pos[1] = modelFireParticles[3].y;
		  //source1Pos[2] = modelFireParticles[3].z;
		  //alSourcefv(source[1], AL_POSITION, source1Pos);

		  /**********
		   * End Render particles systems
		   */
		}
		else if (renderParticles && it->second.first.compare("fire7") == 0) {
		/**********
		 * Init Render particles systems
		 */
		 lastTimeParticlesAnimationFire = currTimeParticlesAnimationFire;
		 currTimeParticlesAnimationFire = TimeManager::Instance().GetTime();

		 shaderParticlesFire.setInt("Pass", 1);
		 shaderParticlesFire.setFloat("Time", currTimeParticlesAnimationFire);
		 shaderParticlesFire.setFloat("DeltaT", currTimeParticlesAnimationFire - lastTimeParticlesAnimationFire);

		 glActiveTexture(GL_TEXTURE1);
		 glBindTexture(GL_TEXTURE_1D, texId);
		 glEnable(GL_RASTERIZER_DISCARD);
		 glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, feedback[drawBuf]);
		 glBeginTransformFeedback(GL_POINTS);
		 glBindVertexArray(particleArray[1 - drawBuf]);
		 glVertexAttribDivisor(0, 0);
		 glVertexAttribDivisor(1, 0);
		 glVertexAttribDivisor(2, 0);
		 glDrawArrays(GL_POINTS, 0, nParticlesFire);
		 glEndTransformFeedback();
		 glDisable(GL_RASTERIZER_DISCARD);

		 shaderParticlesFire.setInt("Pass", 2);
		 glm::mat4 modelFireParticles = /*glm::mat4(1.0)*/ modelMatrixDoor;
		 modelFireParticles = glm::translate(modelFireParticles, it->second.second);
		 //modelFireParticles[3][1] = terrain.getHeightTerrain(modelFireParticles[3][0], modelFireParticles[3][2]);
		 modelFireParticles = glm::scale(modelFireParticles, glm::vec3(0.06, 1.0, 1.0));
		 shaderParticlesFire.setMatrix4("model", 1, false, glm::value_ptr(modelFireParticles));

		 shaderParticlesFire.turnOn();
		 glActiveTexture(GL_TEXTURE0);
		 glBindTexture(GL_TEXTURE_2D, textureParticleFireID);
		 glDepthMask(GL_FALSE);
		 glBindVertexArray(particleArray[drawBuf]);
		 glVertexAttribDivisor(0, 1);
		 glVertexAttribDivisor(1, 1);
		 glVertexAttribDivisor(2, 1);
		 glDrawArraysInstanced(GL_TRIANGLES, 0, 6, nParticlesFire);
		 glBindVertexArray(0);
		 glDepthMask(GL_TRUE);
		 drawBuf = 1 - drawBuf;
		 shaderParticlesFire.turnOff();

		 /****************************+
		  * Open AL sound data
		  */
		  //source1Pos[0] = modelFireParticles[3].x;
		  //source1Pos[1] = modelFireParticles[3].y;
		  //source1Pos[2] = modelFireParticles[3].z;
		  //alSourcefv(source[1], AL_POSITION, source1Pos);

		  /**********
		   * End Render particles systems
		   */
		}
	}
	glEnable(GL_CULL_FACE);
}

int main(int argc, char **argv) {
	init(800, 700, "Window GLFW - Teclado", false);
	applicationLoop();
	destroy();
	return 1;
}
