#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include"BufferObjects/Shader.h"
#include"Camera.h"
#include"BufferObjects/Material.h"
#include"World/WorldProcessor.h"
#include"World/World.h"
#include"BufferObjects/EntityMesh.h"
#include"Entity/PlayerEntity.h"
#include"Entity/EntityManager.h"
#define OGT_VOX_IMPLEMENTATION
#include"World/Structures/StructureManager.h"

int main()
{
	//Initialize starting screen size
	unsigned int WIDTH = 1920;
	unsigned int HEIGHT = 1080;
	// I dunno why 500 just good
	srand(500);
	//Create a glfw window and make a context with no monitor specified so its window mode (Better for development)
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_DECORATED, NULL);
	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "BOBGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	//Load Glad and give it the screen size and enable some depth test and enalbe face culling in a clockwise order
	gladLoadGL();
	glViewport(0, 0, WIDTH, HEIGHT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glFrontFace(GL_CW);
	//Initalize transparency with alpha
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// Create the world shader before the game loop with given file paths
	Shader shader;
	shader.Create("Shaders/default.vert", "Shaders/default.frag");
	//Create world processor once so every chunk or eventually worlds has a reference to one processor
	WorldProcessor proc;
	proc.SetSeed(500);
	//Create the material with given texture since it has no lighting nothing more is needed
	Material mat("Textures/main.png", &shader);
	//Create the world object with given processor and material
	World world(proc, &mat);
	//Create structuregenerator before "creating" the world so the world can use the generator
	StructureGenerator gen;
	gen.Initialize(&world);
	gen.AddStructureType(SHIP);
	//Set world render distance and create a world
	world.renderDistance = 10;
	world.Create(world.renderDistance, &gen);
	int frameCounter = 0;
	int fps = 0;
	double endTime = glfwGetTime()+1;
	// NO VSYNC
	glfwSwapInterval(0);
	bool fullscreen = false;
	//Create the player object
	PlayerEntity player(WIDTH, HEIGHT, { 0,0,0 }, &shader, &world);
	EntityManager entityManager(&world, &mat);
	bool debugMode = false; 
	//Define the skycolor to be used in the shader later 
	glm::vec4 skyColor = glm::vec4(161 / 255.0f, 1.0f, 243 / 255.0f, 1.0f);
	while (!glfwWindowShouldClose(window))
	{
		//Get the current id of the chunk player is in
		ChunkID playerId = { floorf(player.GetPosition().x / 16.0f), floorf(player.GetPosition().z / 16.0f) };
		BlockPos pos = { std::roundf(player.GetPosition().x), std::roundf(player.GetPosition().y), std::roundf(player.GetPosition().z) };
		//Update the world with the player chunk as the center 
		world.Update(playerId, &gen);
		double currentTime = glfwGetTime();
		std::string title = "Bobcraft, FPS: " + std::to_string(fps);
		glfwSetWindowTitle(window, title.c_str());
		//Clear the sky color of the screen
		glClearColor(skyColor.r, skyColor.g, skyColor.b, skyColor.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		shader.Bind();
		//Update the player and camera within
		player.Update(window);
		//Render entities and update their positions
		entityManager.Update();
		//Render world last so entities appear on top
		world.Render();
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
			glfwSetWindowShouldClose(window, 1);
		}
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	//Delete all things before destroying window
	world.Delete();
	shader.Unload();
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}