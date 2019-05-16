//glew include
#include <GL/glew.h>

//std includes
#include <sstream>
#include <string>
#include <iostream>
#define _USE_MATH_DEFINES
#include <math.h>

//glfw include
#include <GLFW/glfw3.h>

//GLM include
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
// program include
#include "Headers/TimeManager.h"
#include "Headers/Shader.h"
// Geometry primitives
#include "Headers/Sphere.h"
#include "Headers/Cylinder.h"
#include "Headers/Box.h"
#include "Headers/FirstPersonCamera.h"
//Texture includes
#include "Headers/Texture.h"
//Model includes
#include "Headers/Model.h"

#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))

std::shared_ptr<FirstPersonCamera> camera(new FirstPersonCamera());

Sphere sphere(20, 20);
Cylinder cylinder(20, 20, 0.5, 0.5);
Box box, box2;
Box boxWater;

Sphere sphereAnimacion(20, 20);
Cylinder cylinderAnimacion(20, 20, 0.5, 0.5);
Cylinder cylinderAnimacion2(20, 20, 0.5, 0.5);

Shader shaderColor;
Shader shaderTexture;
Shader shaderCubeTexture;
Shader shaderMateriales;
Shader shaderDirectionLight;
Shader shaderPointLight;
Shader shaderSpotLight;
Shader shaderLighting;

Model modelRock;
Model modelArbol;
Model modelAirCraft;
Model arturito;
Model modelTrain;

GLuint textureID1, textureHojasID, textureID3, textureCespedID, textureWaterID, textureCubeTexture, textureMetalID,
textureConcretoID, textureRocaID, textureParedID;
GLuint cubeTextureID;

std::vector<std::vector<glm::mat4>> getKeyFrames(std::string fileName) {
	std::vector<std::vector<glm::mat4>> keyFrames;
	std::string line;
	std::ifstream infile(fileName);
	std::string s = line;
	while (std::getline(infile, line))
	{
		std::vector<glm::mat4> transforms;
		s = line;
		size_t pos1 = 0;
		std::string token1;
		std::string delimiter1 = "|";
		while ((pos1 = s.find(delimiter1)) != std::string::npos) {
			token1 = s.substr(0, pos1);
			//std::cout << token1 << std::endl;

			size_t pos2 = 0;
			std::string token2;
			std::string delimiter2 = ",";

			int i = 0;
			int j = 0;
			glm::mat4 transform;
			float aaa[16];
			while ((pos2 = token1.find(delimiter2)) != std::string::npos) {
				token2 = token1.substr(0, pos2);
				aaa[i++] = atof(token2.c_str());
				/*if (j == 0)
					transform[i].x = atof(token2.c_str());
				if (j == 1)
					transform[i].y = atof(token2.c_str());
				if (j == 2)
					transform[i].z = atof(token2.c_str());
				if (j == 3)
					transform[i].w = atof(token2.c_str());*/
					/*j++;
					if (j > 3) {
						i++;
						j = 0;
					}*/
				token1.erase(0, pos2 + delimiter2.length());
			}
			aaa[i++] = atof(token1.c_str());
			transform = glm::make_mat4(aaa);
			transforms.push_back(transform);
			s.erase(0, pos1 + delimiter1.length());
		}
		keyFrames.push_back(transforms);
	}
	return keyFrames;
}

GLenum types[6] = {
	GL_TEXTURE_CUBE_MAP_POSITIVE_X,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
	GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
	GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
};

std::string fileNames[6] = {
	"../../Textures/mp_sincity/sincity_ft.tga",
	"../../Textures/mp_sincity/sincity_bk.tga",
	"../../Textures/mp_sincity/sincity_up.tga",
	"../../Textures/mp_sincity/sincity_dn.tga",
	"../../Textures/mp_sincity/sincity_rt.tga",
	"../../Textures/mp_sincity/sincity_lf.tga"
};

int screenWidth;
int screenHeight;

GLFWwindow * window;

bool exitApp = false;
int lastMousePosX, offsetX;
int lastMousePosY, offsetY;

double deltaTime;

// Se definen todos las funciones.
void reshapeCallback(GLFWwindow* Window, int widthRes, int heightRes);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void mouseButtonCallback(GLFWwindow* window, int button, int state, int mod);
void init(int width, int height, std::string strTitle, bool bFullScreen);
void destroyWindow();
void destroy();
bool processInput(bool continueApplication = true);

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
		destroyWindow();
		exit(-1);
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(0);

	glfwSetWindowSizeCallback(window, reshapeCallback);
	glfwSetKeyCallback(window, keyCallback);
	glfwSetCursorPosCallback(window, mouseCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
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

	shaderColor.initialize("../../Shaders/transformaciones.vs", "../../Shaders/transformaciones.fs");
	shaderTexture.initialize("../../Shaders/texturizado_res.vs", "../../Shaders/texturizado_res.fs");
	shaderCubeTexture.initialize("../../Shaders/cubeTexture.vs", "../../Shaders/cubeTexture.fs");
	shaderMateriales.initialize("../../Shaders/iluminacion_materiales_res.vs", "../../Shaders/iluminacion_materiales_res.fs");
	shaderDirectionLight.initialize("../../Shaders/typeLight.vs", "../../Shaders/directionalLight.fs");
	shaderPointLight.initialize("../../Shaders/typeLight.vs", "../../Shaders/pointLight.fs");
	shaderSpotLight.initialize("../../Shaders/typeLight.vs", "../../Shaders/spotLight.fs");
	shaderLighting.initialize("../../Shaders/typeLight.vs", "../../Shaders/multipleLights.fs");

	sphereAnimacion.init();
	sphereAnimacion.setShader(&shaderLighting);
	sphereAnimacion.setColor(glm::vec3(0.3, 0.3, 1.0));

	cylinderAnimacion.init();
	cylinderAnimacion.setShader(&shaderLighting);
	cylinderAnimacion.setColor(glm::vec3(0.8, 0.3, 1.0));

	cylinderAnimacion2.init();
	cylinderAnimacion2.setShader(&shaderLighting);
	cylinderAnimacion2.setColor(glm::vec3(0.2, 0.7, 0.3));

	sphere.init();
	cylinder.init();
	box.init();
	box.scaleUVS(glm::vec2(10.0, 10.0));
	box2.init();
	box2.scaleUVS(glm::vec2(1.0, 1.0));
	boxWater.init();
	boxWater.scaleUVS(glm::vec2(1.0, 1.0));
	modelRock.loadModel("../../models/rock/rock.obj");
	modelArbol.loadModel("../../models/Conifer_1_Obj/Tree_Conifer_1.obj");
	modelAirCraft.loadModel("../../models/Aircraft_obj/E 45 Aircraft_obj.obj");

	camera->setPosition(glm::vec3(0.0f, 15.0f, 30.0f));

	// Textura Ladrillos
	int imageWidth, imageHeight;
	Texture texture("../../Textures/texturaLadrillos.jpg");
	FIBITMAP* bitmap = texture.loadImage(false);
	unsigned char * data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &textureID1);
	glBindTexture(GL_TEXTURE_2D, textureID1);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	texture.freeImage(bitmap);

	// Texture Goku
	texture = Texture("../../Textures/hojas.jpg");
	bitmap = texture.loadImage(false);
	data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &textureHojasID);
	glBindTexture(GL_TEXTURE_2D, textureHojasID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	texture.freeImage(bitmap);

	// Textura cuadritos
	texture = Texture("../../Textures/test.png");
	bitmap = texture.loadImage(false);
	data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &textureID3);
	glBindTexture(GL_TEXTURE_2D, textureID3);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	texture.freeImage(bitmap);

	texture = Texture("../../Textures/cesped.jpg");
	bitmap = texture.loadImage(false);
	data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &textureCespedID);
	glBindTexture(GL_TEXTURE_2D, textureCespedID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	texture.freeImage(bitmap);

	texture = Texture("../../Textures/concreto.jpg");
	bitmap = texture.loadImage(false);
	data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &textureConcretoID);
	glBindTexture(GL_TEXTURE_2D, textureConcretoID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	texture.freeImage(bitmap);

	Texture texture3("../../Textures/rocaV.jpg");
	FIBITMAP* bitmap3 = texture3.loadImage();
	unsigned char * data3 = texture3.convertToData(bitmap3, imageWidth, imageHeight);
	glGenTextures(1, &textureRocaID);
	//se enlaza el tipo de textura al ID textureID (Texture 2D)
	glBindTexture(GL_TEXTURE_2D, textureRocaID);
	// set the texture wrapping parameters
	//repetir en eje S y eje T (en este caso el X y Y)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (data3) {
		//TipoTextura,0(MIPMAPS),Formato interno del buffer, ancho, alto, borde, formato de la libreria, tipo de dato, datos de la imagen
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, data3);
		//le indica a openGL que se encargue de general los mipmaps
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Failed to load texture" << std::endl;
	}
	texture3.freeImage(bitmap3);

	texture = Texture("../../Textures/water2.jpg");
	bitmap = texture.loadImage(false);
	data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &textureWaterID);
	glBindTexture(GL_TEXTURE_2D, textureWaterID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	texture.freeImage(bitmap);

	texture = Texture("../../Textures/metal.jpg");
	bitmap = texture.loadImage(false);
	data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &textureMetalID);
	glBindTexture(GL_TEXTURE_2D, textureMetalID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	texture.freeImage(bitmap);

	texture = Texture("../../Textures/pared.jpg");
	bitmap = texture.loadImage(false);
	data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &textureParedID);
	glBindTexture(GL_TEXTURE_2D, textureParedID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	texture.freeImage(bitmap);

	glGenTextures(1, &cubeTextureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeTextureID);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	for (int i = 0; i < ARRAY_SIZE_IN_ELEMENTS(types); i++) {
		texture = Texture(fileNames[i]);
		FIBITMAP* bitmap = texture.loadImage(true);
		data = texture.convertToData(bitmap, imageWidth, imageHeight);
		if (data) {
			glTexImage2D(types[i], 0, GL_RGBA, imageWidth, imageHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
		}
		else
			std::cout << "Failed to load texture" << std::endl;
		texture.freeImage(bitmap);
	}

}

void destroyWindow() {
	glfwDestroyWindow(window);
	glfwTerminate();
}

void destroy() {
	destroyWindow();

	shaderColor.destroy();
	shaderTexture.destroy();
	shaderCubeTexture.destroy();
	shaderMateriales.destroy();
	shaderDirectionLight.destroy();
	shaderPointLight.destroy();
	shaderSpotLight.destroy();
	sphere.destroy();
	cylinder.destroy();

	sphereAnimacion.destroy();
	cylinderAnimacion.destroy();
	cylinderAnimacion2.destroy();

	box.destroy();
	box2.destroy();
}

void reshapeCallback(GLFWwindow* Window, int widthRes, int heightRes) {
	screenWidth = widthRes;
	screenHeight = heightRes;
	glViewport(0, 0, widthRes, heightRes);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (action == GLFW_PRESS) {
		switch (key) {
		case GLFW_KEY_ESCAPE:
			exitApp = true;
			break;
		}
	}
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
	offsetX = xpos - lastMousePosX;
	offsetY = ypos - lastMousePosY;
	lastMousePosX = xpos;
	lastMousePosY = ypos;
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		camera->mouseMoveCamera(offsetX, offsetY, deltaTime);
}

void mouseButtonCallback(GLFWwindow* window, int button, int state, int mod) {
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
	TimeManager::Instance().CalculateFrameRate(false);
	deltaTime = TimeManager::Instance().DeltaTime;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera->moveFrontCamera(true, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera->moveFrontCamera(false, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera->moveRightCamera(false, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera->moveRightCamera(true, deltaTime);
	glfwPollEvents();
	return continueApplication;
}

void applicationLoop() {
	bool psi = true;
	double lastTime = TimeManager::Instance().GetTime();

	float angle = 0.0;
	float ratio = 20.0;

	float aircraftZ = 0.0;
	bool direcionAirCraft = true;
	float rotationAirCraft = 0.0;
	bool finishRotation = true;

	//se obtienen los frames del brazo.
	std::vector<std::vector<glm::mat4>> keyFramesBrazo = getKeyFrames("../../animaciones/animationMano.txt");
	//numero de pasos para moverse de un frame a otro (bascimente la velocidad)
	int numPasosAnimBrazo = 500;
	int numPasosAnimBrazoCurr = 0;

	//indices del arreglo keyFramesBrazo el actual y el siguiente
	int indexKeyFrameBrazoCurr = 0;
	int indexKeyFrameBrazoNext = 1;
	float interpolation = 0.0;

	while (psi) {
		psi = processInput(true);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

		// Matrix de proyeccion en perspectiva
		glm::mat4 projection = glm::perspective(glm::radians(45.0f),
			(float)screenWidth / screenWidth, 0.01f, 100.0f);
		// matrix de vista
		glm::mat4 view = camera->getViewMatrix();

		cylinderAnimacion.setProjectionMatrix(projection);
		cylinderAnimacion.setViewMatrix(view);
		sphereAnimacion.setProjectionMatrix(projection);
		sphereAnimacion.setViewMatrix(view);

		shaderTexture.turnOn();
		// Importante si se quiere renderizar modelos y texturas
		/*
		glActiveTexture(GL_TEXTURE0);
		cylinder.setShader(&shaderTexture);
		cylinder.setProjectionMatrix(projection);
		cylinder.setViewMatrix(view);
		cylinder.setPosition(glm::vec3(-3.0f, 2.0f, -3.0f));
		glBindTexture(GL_TEXTURE_2D, textureHojasID);
		cylinder.render(0, cylinder.getSlices() * cylinder.getStacks() * 2 * 3);
		glBindTexture(GL_TEXTURE_2D, textureID1);
		cylinder.render(cylinder.getSlices() * cylinder.getStacks() * 2 * 3, cylinder.getSlices() * 3);
		glBindTexture(GL_TEXTURE_2D, textureID3);
		cylinder.render(cylinder.getSlices() * cylinder.getStacks() * 2 * 3 + cylinder.getSlices() * 3, cylinder.getSlices() * 3);
		glBindTexture(GL_TEXTURE_2D, 0);
		shaderTexture.turnOff();
		*/

		// Iluminación
		glm::mat4 lightModelmatrix = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(1.0f, 0.0f, 0.0f));
		lightModelmatrix = glm::translate(lightModelmatrix, glm::vec3(0.0f, 0.0f, -ratio));

		shaderMateriales.turnOn();
		glUniform3fv(shaderMateriales.getUniformLocation("light.position"), 1, glm::value_ptr(glm::vec3(lightModelmatrix * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f))));
		glUniform3fv(shaderMateriales.getUniformLocation("viewPos"), 1, glm::value_ptr(camera->getPosition()));
		glUniform3f(shaderMateriales.getUniformLocation("light.ambient"), 0.2, 0.2, 0.2);
		glUniform3f(shaderMateriales.getUniformLocation("light.diffuse"), 0.2, 0.3, 0.6);
		glUniform3f(shaderMateriales.getUniformLocation("light.specular"), 0.5, 0.3, 0.2);
		glUniform3f(shaderMateriales.getUniformLocation("material.ambient"), 1.0, 0.2, 0.6);
		glUniform3f(shaderMateriales.getUniformLocation("material.diffuse"), 0.4, 0.5, 0.8);
		glUniform3f(shaderMateriales.getUniformLocation("material.specular"), 0.5, 0.3, 0.2);
		glUniform1f(shaderMateriales.getUniformLocation("material.shininess"), 32.0);
		//cylinder.render();
		shaderMateriales.turnOff();

		shaderLighting.turnOn();
		glUniform3fv(shaderLighting.getUniformLocation("viewPos"), 1, glm::value_ptr(camera->getPosition()));
		//Directional light
		glUniform3f(shaderLighting.getUniformLocation("directionalLight.light.ambient"), 0.025, 0.025, 0.025);
		glUniform3f(shaderLighting.getUniformLocation("directionalLight.light.diffuse"), 0.1, 0.1, 0.1);
		glUniform3f(shaderLighting.getUniformLocation("directionalLight.light.specular"), 0.15, 0.15, 0.15);
		glUniform3fv(shaderLighting.getUniformLocation("directionalLight.direction"), 1, glm::value_ptr(glm::vec3(0, -1.0, 0.0)));
		//Numero de luces spot y point
		int locCount = shaderLighting.getUniformLocation("pointLightCount");
		glUniform1i(shaderLighting.getUniformLocation("pointLightCount"), 1);
		glUniform1i(shaderLighting.getUniformLocation("spotLightCount"), 1);
		// Point light
		glUniform3fv(shaderLighting.getUniformLocation("pointLights[0].position"), 1, glm::value_ptr(glm::vec3(lightModelmatrix * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f))));
		glUniform1f(shaderLighting.getUniformLocation("pointLights[0].constant"), 1.0f);
		glUniform1f(shaderLighting.getUniformLocation("pointLights[0].linear"), 0.14f);
		glUniform1f(shaderLighting.getUniformLocation("pointLights[0].quadratics"), 0.07f);
		glUniform3f(shaderLighting.getUniformLocation("pointLights[0].light.ambient"), 0.025, 0.025, 0.025);
		glUniform3f(shaderLighting.getUniformLocation("pointLights[0].light.diffuse"), 0.2, 0.3, 0.15);
		glUniform3f(shaderLighting.getUniformLocation("pointLights[0].light.specular"), 0.5, 0.1, 0.2);
		// Spot light
		glUniform3fv(shaderLighting.getUniformLocation("spotLights[0].position"), 1, glm::value_ptr(camera->getPosition()));
		glUniform3fv(shaderLighting.getUniformLocation("spotLights[0].direction"), 1, glm::value_ptr(camera->getFront()));
		glUniform1f(shaderLighting.getUniformLocation("spotLights[0].cutOff"), glm::cos(glm::radians(12.5f)));
		glUniform1f(shaderLighting.getUniformLocation("spotLights[0].outerCutOff"), glm::cos(glm::radians(15.0f)));
		glUniform1f(shaderLighting.getUniformLocation("spotLights[0].constant"), 1.0f);
		glUniform1f(shaderLighting.getUniformLocation("spotLights[0].linear"), 0.14f);
		glUniform1f(shaderLighting.getUniformLocation("spotLights[0].quadratics"), 0.07f);
		glUniform3f(shaderLighting.getUniformLocation("spotLights[0].light.ambient"), 0.025, 0.025, 0.025);
		glUniform3f(shaderLighting.getUniformLocation("spotLights[0].light.diffuse"), 0.7, 0.2, 0.6);
		glUniform3f(shaderLighting.getUniformLocation("spotLights[0].light.specular"), 0.1, 0.7, 0.8);
		shaderLighting.turnOff();

		modelArbol.setShader(&shaderLighting);
		modelArbol.setProjectionMatrix(projection);
		modelArbol.setViewMatrix(view);
		modelArbol.setPosition(glm::vec3(-3.0, 2.5, 0.0));
		modelArbol.setScale(glm::vec3(0.10, 0.10, 0.10));
		modelArbol.render();
		/*
		modelAirCraft.setShader(&shaderLighting);
		modelAirCraft.setProjectionMatrix(projection);
		modelAirCraft.setViewMatrix(view);
		modelAirCraft.setScale(glm::vec3(1.0, 1.0, 1.0));
		glm::mat4 matrixAirCraft = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, aircraftZ));
		matrixAirCraft = glm::translate(matrixAirCraft, glm::vec3(10.0, 2.0, 15.0));
		matrixAirCraft = glm::rotate(matrixAirCraft, rotationAirCraft, glm::vec3(0, 1, 0));
		modelAirCraft.render(matrixAirCraft);
		*/
		/*
		glm::quat firstQuat;
		glm::quat secondQuat;
		glm::quat finalQuat;
		glm::mat4 interpoltaedMatrix;
		glm::vec4 transformComp1;
		glm::vec4 transformComp2;
		glm::vec4 finalTrans;

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureID3);
		if (keyFramesBrazo[indexKeyFrameBrazoCurr].size() == 7 && keyFramesBrazo[indexKeyFrameBrazoNext].size() == 7) {

			//matriz de rotacion actual
			firstQuat = glm::quat_cast(keyFramesBrazo[indexKeyFrameBrazoCurr][0]);
			secondQuat = glm::quat_cast(keyFramesBrazo[indexKeyFrameBrazoNext][0]);
			//slerp hace la interpolacion de quaternion (matriz de rotacion)
			finalQuat = glm::slerp(firstQuat, secondQuat, interpolation);
			//se convierte el quaternion a una matriz de 4x4
			interpoltaedMatrix = glm::mat4_cast(finalQuat);
			//se obteiene la translacion del frame i -1
			transformComp1 = keyFramesBrazo[indexKeyFrameBrazoCurr][0] * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
			//se obteiene la translacion del frame i
			transformComp2 = keyFramesBrazo[indexKeyFrameBrazoNext][0] * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
			// se realiza la interpolacion entre el frame i-1 y el frame i
			//transformcomp1 es frame i-1
			//transformcomp2 es frame i
			finalTrans = (float)(1.0 - interpolation) * transformComp1 + transformComp2 * interpolation;
			//unimos la matriz de interpolacion del quaternion y la interpolacion de la translacion
			interpoltaedMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(finalTrans)) * interpoltaedMatrix;

			// Animacion KeyFrames
			glm::mat4 matrixGlobalAnimation = glm::translate(glm::mat4(1.0f), glm::vec3(4.0f, 2.0f, 0.0f));
			// Se modela siempre con los ejes de giro en el eje z
			// Articulacion 1
			glm::mat4 keyFrameJoint = matrixGlobalAnimation * interpoltaedMatrix;
			glm::mat4 cylinderMatrixJ0 = glm::rotate(keyFrameJoint, 1.5708f, glm::vec3(1.0, 0.0f, 0.0));
			cylinderMatrixJ0 = glm::scale(cylinderMatrixJ0, glm::vec3(0.08f, 0.08f, 0.08f));
			sphereAnimacion.render(cylinderMatrixJ0);

			// Articulacion 2
			firstQuat = glm::quat_cast(keyFramesBrazo[indexKeyFrameBrazoCurr][1]);
			secondQuat = glm::quat_cast(keyFramesBrazo[indexKeyFrameBrazoNext][1]);
			finalQuat = glm::slerp(firstQuat, secondQuat, interpolation);
			interpoltaedMatrix = glm::mat4_cast(finalQuat);
			transformComp1 = keyFramesBrazo[indexKeyFrameBrazoCurr][1] * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
			transformComp2 = keyFramesBrazo[indexKeyFrameBrazoNext][1] * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
			finalTrans = (float)(1.0 - interpolation) * transformComp1 + transformComp2 * interpolation;
			interpoltaedMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(finalTrans)) * interpoltaedMatrix;

			keyFrameJoint = matrixGlobalAnimation * interpoltaedMatrix;
			glm::mat4 cylinderMatrixJ1 = glm::rotate(keyFrameJoint, 1.5708f, glm::vec3(1.0, 0.0f, 0.0));
			cylinderMatrixJ1 = glm::scale(cylinderMatrixJ1, glm::vec3(0.1f, 0.1f, 0.1f));
			sphereAnimacion.render(cylinderMatrixJ1);

			// Articulacion 3
			firstQuat = glm::quat_cast(keyFramesBrazo[indexKeyFrameBrazoCurr][2]);
			secondQuat = glm::quat_cast(keyFramesBrazo[indexKeyFrameBrazoNext][2]);
			finalQuat = glm::slerp(firstQuat, secondQuat, interpolation);
			interpoltaedMatrix = glm::mat4_cast(finalQuat);
			transformComp1 = keyFramesBrazo[indexKeyFrameBrazoCurr][2] * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
			transformComp2 = keyFramesBrazo[indexKeyFrameBrazoNext][2] * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
			finalTrans = (float)(1.0 - interpolation) * transformComp1 + transformComp2 * interpolation;
			interpoltaedMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(finalTrans)) * interpoltaedMatrix;

			keyFrameJoint = matrixGlobalAnimation * interpoltaedMatrix;
			glm::mat4 cylinderMatrixJ2 = glm::rotate(keyFrameJoint, 1.5708f, glm::vec3(1.0, 0.0f, 0.0));
			cylinderMatrixJ2 = glm::scale(cylinderMatrixJ2, glm::vec3(0.11f, 0.11f, 0.11f));
			sphereAnimacion.render(cylinderMatrixJ2);

			// Hueso 1
			glm::mat4 cylinderMatrixL1 = glm::translate(keyFrameJoint, glm::vec3(0.0f, 0.0f, 0.125f));
			cylinderMatrixL1 = glm::rotate(cylinderMatrixL1, 1.5708f, glm::vec3(1.0, 0.0f, 0.0));
			cylinderMatrixL1 = glm::scale(cylinderMatrixL1, glm::vec3(0.1f, 0.25f, 0.1f));
			cylinderAnimacion.render(cylinderMatrixL1);

			// Articulacion 4
			firstQuat = glm::quat_cast(keyFramesBrazo[indexKeyFrameBrazoCurr][3]);
			secondQuat = glm::quat_cast(keyFramesBrazo[indexKeyFrameBrazoNext][3]);
			finalQuat = glm::slerp(firstQuat, secondQuat, interpolation);
			interpoltaedMatrix = glm::mat4_cast(finalQuat);
			transformComp1 = keyFramesBrazo[indexKeyFrameBrazoCurr][3] * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
			transformComp2 = keyFramesBrazo[indexKeyFrameBrazoNext][3] * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
			finalTrans = (float)(1.0 - interpolation) * transformComp1 + transformComp2 * interpolation;
			interpoltaedMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(finalTrans)) * interpoltaedMatrix;

			keyFrameJoint = matrixGlobalAnimation * interpoltaedMatrix;
			glm::mat4 cylinderMatrixJ3 = glm::rotate(keyFrameJoint, 1.5708f, glm::vec3(1.0, 0.0f, 0.0));
			cylinderMatrixJ3 = glm::scale(cylinderMatrixJ3, glm::vec3(0.11f, 0.11f, 0.11f));
			sphereAnimacion.render(cylinderMatrixJ3);

			// Articulacion 5
			firstQuat = glm::quat_cast(keyFramesBrazo[indexKeyFrameBrazoCurr][4]);
			secondQuat = glm::quat_cast(keyFramesBrazo[indexKeyFrameBrazoNext][4]);
			finalQuat = glm::slerp(firstQuat, secondQuat, interpolation);
			interpoltaedMatrix = glm::mat4_cast(finalQuat);
			transformComp1 = keyFramesBrazo[indexKeyFrameBrazoCurr][4] * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
			transformComp2 = keyFramesBrazo[indexKeyFrameBrazoNext][4] * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
			finalTrans = (float)(1.0 - interpolation) * transformComp1 + transformComp2 * interpolation;
			interpoltaedMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(finalTrans)) * interpoltaedMatrix;

			keyFrameJoint = matrixGlobalAnimation * interpoltaedMatrix;
			glm::mat4 cylinderMatrixJ4 = glm::rotate(keyFrameJoint, 1.5708f, glm::vec3(1.0, 0.0f, 0.0));
			cylinderMatrixJ4 = glm::scale(cylinderMatrixJ4, glm::vec3(0.11f, 0.11f, 0.11f));
			sphereAnimacion.render(cylinderMatrixJ4);

			// Hueso 2
			glm::mat4 cylinderMatrixL2 = glm::translate(keyFrameJoint, glm::vec3(0.0f, 0.0f, 0.125f));
			cylinderMatrixL2 = glm::rotate(cylinderMatrixL2, 1.5708f, glm::vec3(1.0, 0.0f, 0.0));
			cylinderMatrixL2 = glm::scale(cylinderMatrixL2, glm::vec3(0.1f, 0.25f, 0.1f));
			cylinderAnimacion.render(cylinderMatrixL2);

			// Articulacion 6
			firstQuat = glm::quat_cast(keyFramesBrazo[indexKeyFrameBrazoCurr][5]);
			secondQuat = glm::quat_cast(keyFramesBrazo[indexKeyFrameBrazoNext][5]);
			finalQuat = glm::slerp(firstQuat, secondQuat, interpolation);
			interpoltaedMatrix = glm::mat4_cast(finalQuat);
			transformComp1 = keyFramesBrazo[indexKeyFrameBrazoCurr][5] * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
			transformComp2 = keyFramesBrazo[indexKeyFrameBrazoNext][5] * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
			finalTrans = (float)(1.0 - interpolation) * transformComp1 + transformComp2 * interpolation;
			interpoltaedMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(finalTrans)) * interpoltaedMatrix;

			keyFrameJoint = matrixGlobalAnimation * interpoltaedMatrix;
			glm::mat4 cylinderMatrixJ5 = glm::rotate(keyFrameJoint, 1.5708f, glm::vec3(1.0, 0.0f, 0.0));
			cylinderMatrixJ5 = glm::scale(cylinderMatrixJ5, glm::vec3(0.11f, 0.11f, 0.11f));
			sphereAnimacion.render(cylinderMatrixJ5);

		}

		numPasosAnimBrazoCurr++;
		interpolation = numPasosAnimBrazoCurr / (float)numPasosAnimBrazo;

		if (interpolation >= 1.0) {
			interpolation = 0.0;
			numPasosAnimBrazoCurr = 0;
			indexKeyFrameBrazoCurr = indexKeyFrameBrazoNext;
			indexKeyFrameBrazoNext++;
		}

		if (indexKeyFrameBrazoNext > keyFramesBrazo.size() - 1) {
			indexKeyFrameBrazoCurr = 0;
			indexKeyFrameBrazoNext = 1;
		}
		*/

		/*arturito.setShader(&shaderLighting);
		arturito.setProjectionMatrix(projection);
		arturito.setViewMatrix(view);
		arturito.setScale(glm::vec3(1.0, 1.0, 1.0));
		glm::mat4 matrixArturito = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, aircraftZ));
		matrixArturito = glm::translate(matrixArturito, glm::vec3(-10.0, 2.0, 15.0));
		matrixArturito = glm::rotate(matrixArturito, rotationAirCraft, glm::vec3(0, 1, 0));
		arturito.render(matrixArturito);*/


		//Suelo
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureConcretoID);
		box.setShader(&shaderLighting);
		box.setProjectionMatrix(projection);
		box.setViewMatrix(view);
		box.setPosition(glm::vec3(0.0, 1.0, 0.0));
		box.setScale(glm::vec3(120.0, 0.2, 160.0));
		box.render();

		//Jardinera
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureRocaID);
		box2.setShader(&shaderLighting);
		box2.setProjectionMatrix(projection);
		box2.setViewMatrix(view);
		box2.setPosition(glm::vec3(0.0, 1.25, 0.0));
		box2.setScale(glm::vec3(11.0, 2.5, 11.0));
		box2.render();

		//hojas
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureHojasID);
		box2.setShader(&shaderLighting);
		box2.setProjectionMatrix(projection);
		box2.setViewMatrix(view);
		box2.setPosition(glm::vec3(0.0, 2.51, 0.0));
		box2.setScale(glm::vec3(8.0, 0.001, 8.0));
		box2.render();

		//bardas

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureRocaID);
		box2.setShader(&shaderLighting);
		box2.setProjectionMatrix(projection);
		box2.setViewMatrix(view);
		box2.setPosition(glm::vec3(-2.75, 1.25, 15.5));
		box2.setScale(glm::vec3(1.5, 2.5, 6.0));
		box2.render();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureRocaID);
		box2.setShader(&shaderLighting);
		box2.setProjectionMatrix(projection);
		box2.setViewMatrix(view);
		box2.setPosition(glm::vec3(3.0, 1.25, 17.75));
		box2.setScale(glm::vec3(11.0, 2.5, 1.5));
		box2.render();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureRocaID);
		box2.setShader(&shaderLighting);
		box2.setProjectionMatrix(projection);
		box2.setViewMatrix(view);
		box2.setPosition(glm::vec3(10.5, 1.25, 6.25));
		box2.setScale(glm::vec3(3.0, 2.5, 1.5));
		box2.render();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureRocaID);
		box2.setShader(&shaderLighting);
		box2.setProjectionMatrix(projection);
		box2.setViewMatrix(view);
		box2.setPosition(glm::vec3(12.75, 1.25, 8.5));
		box2.setScale(glm::vec3(1.5, 2.5, 6.0));
		box2.render();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureRocaID);
		box2.setShader(&shaderLighting);
		box2.setProjectionMatrix(projection);
		box2.setViewMatrix(view);
		box2.setPosition(glm::vec3(17.5, 1.25, 10.75));
		box2.setScale(glm::vec3(8.0, 2.5, 1.5));
		box2.render();

		/////////////////////////aqui empezaré con lo mio
		
		//derecha1
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureWaterID);
		box2.setShader(&shaderLighting);
		box2.setProjectionMatrix(projection);
		box2.setViewMatrix(view);
		box2.setPosition(glm::vec3(27.0, 23.1, 41.9));
		box2.setScale(glm::vec3(26.0, 44.0, 0.0));
		box2.render();

		//derecha-derecha2
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureParedID);
		box2.setShader(&shaderLighting);
		box.setProjectionMatrix(projection);
		box.setViewMatrix(view);
		box.setScale(glm::vec3(26.0, 44.0, 0.0));
		glm::mat4 matrixBox = glm::translate(glm::mat4(1.0f), glm::vec3(40.0, 23.1, -16.0));	
		matrixBox = glm::rotate(matrixBox, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		box.render(matrixBox);

		//derecha6
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureParedID);
		box2.setShader(&shaderLighting);
		box2.setProjectionMatrix(projection);
		box2.setViewMatrix(view);
		box2.setPosition(glm::vec3(27.0, 23.1, -48.5));
		box2.setScale(glm::vec3(26.0, 44.0, 0.0));
		box2.render();

		//izquierda3
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureParedID);
		box2.setShader(&shaderLighting);
		box2.setProjectionMatrix(projection);
		box2.setViewMatrix(view);
		box2.setPosition(glm::vec3(-36.0, 23.1, 65.0));
		box2.setScale(glm::vec3(26.0, 44.0, 0.0));
		box2.render();

		//izquierda5
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureParedID);
		box2.setShader(&shaderLighting);
		box2.setProjectionMatrix(projection);
		box2.setViewMatrix(view);
		box2.setPosition(glm::vec3(-36.0, 23.1, -36.0));
		box2.setScale(glm::vec3(26.0, 44.0, 0.0));
		box2.render();

		//izquierda izquierda4
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureParedID);
		box2.setShader(&shaderLighting);
		box.setProjectionMatrix(projection);
		box.setViewMatrix(view);
		box.setScale(glm::vec3(76.0, 44.0, 0.0));
		glm::mat4 matrixBox2 = glm::translate(glm::mat4(1.0f), glm::vec3(-49.0, 23.1, 2.0));
		matrixBox2 = glm::rotate(matrixBox2, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		box.render(matrixBox2);

		//izquierda-izquierda7
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureParedID);
		box2.setShader(&shaderLighting);
		box.setProjectionMatrix(projection);
		box.setViewMatrix(view);
		box.setScale(glm::vec3(63.15, 44.0, 0.0));
		glm::mat4 matrixBox3 = glm::translate(glm::mat4(1.0f), glm::vec3(-23.0, 23.1, 33.45));
		matrixBox3 = glm::rotate(matrixBox3, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		box.render(matrixBox3);

		//izquierda-izquierda7-2
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureParedID);
		box2.setShader(&shaderLighting);
		box.setProjectionMatrix(projection);
		box.setViewMatrix(view);
		box.setScale(glm::vec3(63.0, 44.0, 0.0));
		glm::mat4 matrixBox4 = glm::translate(glm::mat4(1.0f), glm::vec3(-49.0, 23.1, 33.5));
		matrixBox4 = glm::rotate(matrixBox4, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		box.render(matrixBox4);

		//izquierda8
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureParedID);
		box2.setShader(&shaderLighting);
		box2.setProjectionMatrix(projection);
		box2.setViewMatrix(view);
		box2.setPosition(glm::vec3(-4.5, 23.1, 1.9));
		box2.setScale(glm::vec3(37.0, 44.0, 0.0));
		box2.render();

		//izquierda-izquierda9
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureParedID);
		box2.setShader(&shaderLighting);
		box.setProjectionMatrix(projection);
		box.setViewMatrix(view);
		box.setScale(glm::vec3(40.0, 44.0f, 0.0));
		glm::mat4 matrixBox5 = glm::translate(glm::mat4(1.0f), glm::vec3(14.0, 23.1, 21.9));
		matrixBox5 = glm::rotate(matrixBox5, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		box.render(matrixBox5);

		//izquierda-izquierda2-2
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureParedID);
		box2.setShader(&shaderLighting);
		box.setProjectionMatrix(projection);
		box.setViewMatrix(view);
		box.setScale(glm::vec3(40.0, 44.0, 0.0));
		glm::mat4 matrixBox6 = glm::translate(glm::mat4(1.0f), glm::vec3(40.0, 23.1, 21.9));
		matrixBox6 = glm::rotate(matrixBox6, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		box.render(matrixBox6);

		//izquierda-izquierda10
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureParedID);
		box2.setShader(&shaderLighting);
		box.setProjectionMatrix(projection);
		box.setViewMatrix(view);
		box.setScale(glm::vec3(14.5, 44.0, 0.0));
		glm::mat4 matrixBox7 = glm::translate(glm::mat4(1.0f), glm::vec3(-23.0, 23.1F, -28.75));
		matrixBox7 = glm::rotate(matrixBox7, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		box.render(matrixBox7);

		//izquierda-izquierda11
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureParedID);
		box2.setShader(&shaderLighting);
		box.setProjectionMatrix(projection);
		box.setViewMatrix(view);
		box.setScale(glm::vec3(27.0, 44.0, 0.0));
		glm::mat4 matrixBox8 = glm::translate(glm::mat4(1.0f), glm::vec3(14.0, 23.1, -35.0));
		matrixBox8 = glm::rotate(matrixBox8, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		box.render(matrixBox8);

		//izquierda12
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureParedID);
		box2.setShader(&shaderLighting);
		box2.setProjectionMatrix(projection);
		box2.setViewMatrix(view);
		box2.setPosition(glm::vec3(-4.5, 23.1, -21.5));
		box2.setScale(glm::vec3(37.0, 44.0, 0.0));
		box2.render();

		//techo derecha
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureParedID);
		box2.setShader(&shaderLighting);
		box.setProjectionMatrix(projection);
		box.setViewMatrix(view);
		box.setScale(glm::vec3(27.0, 44.0, 0.0));
		glm::mat4 matrixBox9 = glm::translate(glm::mat4(1.0f), glm::vec3(27.0, 45.0, -3.3));
		matrixBox9 = glm::rotate(matrixBox9, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 1.0f));
		box.render(matrixBox9);

		//techo izquierda
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureParedID);
		box2.setShader(&shaderLighting);
		box.setProjectionMatrix(projection);
		box.setViewMatrix(view);
		box.setScale(glm::vec3(26.0, 101.0, 0.0));
		glm::mat4 matrixBox10 = glm::translate(glm::mat4(1.0f), glm::vec3(-36.0, 45.0, 14.5));
		matrixBox10 = glm::rotate(matrixBox10, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 1.0f));
		box.render(matrixBox10);

		//techo enmedio
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureParedID);
		box2.setShader(&shaderLighting);
		box.setProjectionMatrix(projection);
		box.setViewMatrix(view);
		box.setScale(glm::vec3(39.0, 23.5, 0.0));
		glm::mat4 matrixBox11 = glm::translate(glm::mat4(1.0f), glm::vec3(-4.0, 45.0, -9.8));
		matrixBox11 = glm::rotate(matrixBox11, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 1.0f));
		box.render(matrixBox11);

		//PILAR-FRONTAL
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureParedID);
		box2.setShader(&shaderLighting);
		box2.setProjectionMatrix(projection);
		box2.setViewMatrix(view);
		box2.setPosition(glm::vec3(-12.0, 23.1, 2.0));
		box2.setScale(glm::vec3(2.0, 44.0, 1.0));
		box2.render();

		//PILAR-FRONTAL2
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureParedID);
		box2.setShader(&shaderLighting);
		box2.setProjectionMatrix(projection);
		box2.setViewMatrix(view);
		box2.setPosition(glm::vec3(2.0, 23.1, 2.0));
		box2.setScale(glm::vec3(2.0, 44.0, 1.0));
		box2.render();

		//PILAR-H
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureParedID);
		box2.setShader(&shaderLighting);
		box2.setProjectionMatrix(projection);
		box2.setViewMatrix(view);
		box2.setPosition(glm::vec3(-4.5, 12.0, 2.2));
		box2.setScale(glm::vec3(36.9, 2.0, 1.0));
		box2.render();

		//PILAR-H2
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureParedID);
		box2.setShader(&shaderLighting);
		box2.setProjectionMatrix(projection);
		box2.setViewMatrix(view);
		box2.setPosition(glm::vec3(-4.5, 22.0, 2.2));
		box2.setScale(glm::vec3(36.9, 2.0, 1.0));
		box2.render();

		//PILAR-H3
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureParedID);
		box2.setShader(&shaderLighting);
		box2.setProjectionMatrix(projection);
		box2.setViewMatrix(view);
		box2.setPosition(glm::vec3(-4.5, 32.0, 2.2));
		box2.setScale(glm::vec3(36.9, 2.0, 1.0));
		box2.render();

		//PILAR-H4
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureParedID);
		box2.setShader(&shaderLighting);
		box2.setProjectionMatrix(projection);
		box2.setViewMatrix(view);
		box2.setPosition(glm::vec3(-4.5, 42.0, 2.2));
		box2.setScale(glm::vec3(36.9, 2.0, 1.0));
		box2.render();

		//PILAR-H5
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureParedID);
		box2.setShader(&shaderLighting);
		box2.setProjectionMatrix(projection);
		box2.setViewMatrix(view);
		box2.setPosition(glm::vec3(-4.5, 44.0, 2.2));
		box2.setScale(glm::vec3(36.9, 2.0, 1.0));
		box2.render();

		//pilar izquierda
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureParedID);
		box2.setShader(&shaderLighting);
		box.setProjectionMatrix(projection);
		box.setViewMatrix(view);
		box.setScale(glm::vec3(2.0, 44.0, 1.0));
		glm::mat4 matrixBox12 = glm::translate(glm::mat4(1.0f), glm::vec3(-23.0, 23.1, 64.4));
		matrixBox12 = glm::rotate(matrixBox12, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		box.render(matrixBox12);

		//PILAR IZQUIERDA2
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureParedID);
		box2.setShader(&shaderLighting);
		box.setProjectionMatrix(projection);
		box.setViewMatrix(view);
		box.setScale(glm::vec3(2.0, 44.0, 1.0));
		glm::mat4 matrixBox13 = glm::translate(glm::mat4(1.0f), glm::vec3(-23.0, 23.1, 2.8));
		matrixBox13 = glm::rotate(matrixBox13, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		box.render(matrixBox13);

		//PILAR IZQUIERDA3
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureParedID);
		box2.setShader(&shaderLighting);
		box.setProjectionMatrix(projection);
		box.setViewMatrix(view);
		box.setScale(glm::vec3(2.0, 44.0, 1.0));
		glm::mat4 matrixBox14 = glm::translate(glm::mat4(1.0f), glm::vec3(-23.0, 23.1, 26.4));
		matrixBox14 = glm::rotate(matrixBox14, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		box.render(matrixBox14);

		//PILAR IZQUIERDA4
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureParedID);
		box2.setShader(&shaderLighting);
		box.setProjectionMatrix(projection);
		box.setViewMatrix(view);
		box.setScale(glm::vec3(2.0, 44.0, 1.0));
		glm::mat4 matrixBox15 = glm::translate(glm::mat4(1.0f), glm::vec3(-23.0, 23.1, 46.4));
		matrixBox15 = glm::rotate(matrixBox15, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		box.render(matrixBox15);

		//PILAR DERECHO
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureParedID);
		box2.setShader(&shaderLighting);
		box.setProjectionMatrix(projection);
		box.setViewMatrix(view);
		box.setScale(glm::vec3(2.0, 44.0, 1.0));
		glm::mat4 matrixBox16 = glm::translate(glm::mat4(1.0f), glm::vec3(14.0, 23.1, 2.8));
		matrixBox16 = glm::rotate(matrixBox16, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		box.render(matrixBox16);

		//PILAR derecho2
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureParedID);
		box2.setShader(&shaderLighting);
		box.setProjectionMatrix(projection);
		box.setViewMatrix(view);
		box.setScale(glm::vec3(2.0, 44.0, 1.0));
		glm::mat4 matrixBox17 = glm::translate(glm::mat4(1.0f), glm::vec3(14.0, 23.1, 41.4));
		matrixBox17 = glm::rotate(matrixBox17, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		box.render(matrixBox17);
		
		//PILAR derecho3
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureParedID);
		box2.setShader(&shaderLighting);
		box.setProjectionMatrix(projection);
		box.setViewMatrix(view);
		box.setScale(glm::vec3(2.0, 44.0, 1.0));
		glm::mat4 matrixBox18 = glm::translate(glm::mat4(1.0f), glm::vec3(14.0, 23.1, 22.2));
		matrixBox18 = glm::rotate(matrixBox18, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		box.render(matrixBox18);

		//pilar  DH5
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureParedID);
		box2.setShader(&shaderLighting);
		box.setProjectionMatrix(projection);
		box.setViewMatrix(view);
		box.setScale(glm::vec3(36.9, 2.0, 1.0));
		glm::mat4 matrixBox19 = glm::translate(glm::mat4(1.0f), glm::vec3(14.0, 44.0, 22.0));
		matrixBox19 = glm::rotate(matrixBox19, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		box.render(matrixBox19);

		//PILAR DH4
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureParedID);
		box2.setShader(&shaderLighting);
		box.setProjectionMatrix(projection);
		box.setViewMatrix(view);
		box.setScale(glm::vec3(36.9, 2.0, 1.0));
		glm::mat4 matrixBox20 = glm::translate(glm::mat4(1.0f), glm::vec3(14.0, 42.0, 22.0));
		matrixBox20 = glm::rotate(matrixBox20, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		box.render(matrixBox20);


		/*
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureWaterID);
		boxWater.setShader(&shaderLighting);
		boxWater.setProjectionMatrix(projection);
		boxWater.setViewMatrix(view);
		boxWater.setPosition(glm::vec3(3.0, 2.0, -5.0));
		boxWater.setScale(glm::vec3(10.0, 0.001, 10.0));
		boxWater.offsetUVS(glm::vec2(0.0001, 0.0001));
		boxWater.render();
		*/

		if (angle > 2 * M_PI)
			angle = 0.0;
		else
			angle += 0.001;

		sphere.setShader(&shaderColor);
		sphere.setColor(glm::vec3(0.4f, 0.3f, 0.6f));
		sphere.setProjectionMatrix(projection);
		sphere.setViewMatrix(view);
		sphere.setScale(glm::vec3(1.0f, 1.0f, 1.0f));
		sphere.enableWireMode();
		sphere.render(lightModelmatrix);

		// Se Dibuja el Skybox
		shaderCubeTexture.turnOn();
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubeTextureID);
		GLint oldCullFaceMode;
		GLint oldDepthFuncMode;
		glGetIntegerv(GL_CULL_FACE_MODE, &oldCullFaceMode);
		glGetIntegerv(GL_DEPTH_FUNC, &oldDepthFuncMode);
		GLuint cubeTextureId = shaderCubeTexture.getUniformLocation("skybox");
		glUniform1f(cubeTextureId, 0);
		glCullFace(GL_FRONT);
		glDepthFunc(GL_LEQUAL);
		sphere.setShader(&shaderCubeTexture);
		sphere.setProjectionMatrix(projection);
		sphere.setViewMatrix(glm::mat4(glm::mat3(view)));
		sphere.setScale(glm::vec3(20.0f, 20.0f, 20.0f));
		sphere.render();
		glCullFace(oldCullFaceMode);
		glDepthFunc(oldDepthFuncMode);
		shaderCubeTexture.turnOff();

		if (finishRotation) {
			if (direcionAirCraft)
				aircraftZ -= 0.01;
			else
				aircraftZ += 0.01;
			if (direcionAirCraft && aircraftZ < -6.0) {
				direcionAirCraft = false;
				finishRotation = false;
				aircraftZ = -6.0;
			}if (!direcionAirCraft && aircraftZ > 6.0) {
				direcionAirCraft = true;
				finishRotation = false;
				aircraftZ = 6.0;
			}
		}
		else {
			rotationAirCraft += 0.01;
			if (!direcionAirCraft) {
				if (rotationAirCraft > glm::radians(180.0f)) {
					finishRotation = true;
					rotationAirCraft = glm::radians(180.0f);
				}
			}
			else {
				if (rotationAirCraft > glm::radians(360.0f)) {
					finishRotation = true;
					rotationAirCraft = glm::radians(0.0f);
				}
			}
		}

		glfwSwapBuffers(window);
	}
}

int main(int argc, char ** argv) {
	init(800, 700, "========Zenteno Vision========", false);
	applicationLoop();
	destroy();
	return 1;
}