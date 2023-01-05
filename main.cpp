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
	unsigned int WIDTH = 1280;
	unsigned int HEIGHT = 800;
	srand(time(0));
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
	gladLoadGL();
	glViewport(0, 0, WIDTH, HEIGHT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glFrontFace(GL_CW);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	Shader shader;
	shader.Create("Shaders/default.vert", "Shaders/default.frag");
	WorldProcessor proc;
	proc.SetSeed(rand() % 100000);
	Material mat("Textures/main.png", &shader);
	World world(proc, &mat);
	StructureGenerator gen;
	gen.Initialize(&world);
	gen.AddStructureType(SHIP);
	world.renderDistance = 5;
	world.Create(world.renderDistance, &gen);
	int frameCounter = 0;
	int fps = 0;
	double endTime = glfwGetTime()+1;
	glfwSwapInterval(0);
	bool fullscreen = false;
	PlayerEntity player(WIDTH, HEIGHT, { 0,0,0 }, &shader, &world);
	EntityManager entityManager(&world, &mat);
	bool debugMode = false; 
	glm::vec4 sky = glm::vec4(161 / 255.0f, 1.0f, 243 / 255.0f, 1.0f);
	float testRot = 0.0f;
	float spawnCooldown = 0.0f;
	while (!glfwWindowShouldClose(window))
	{
		ChunkID playerId = { floorf(player.GetPosition().x / 16.0f), floorf(player.GetPosition().z / 16.0f) };
		BlockPos pos = { std::roundf(player.GetPosition().x), std::roundf(player.GetPosition().y), std::roundf(player.GetPosition().z) };
		BlockPos genpos = { pos.x + (rand() % 50 - 25), 0, pos.z+(rand() % 50 - 25) };
		world.Update(playerId, &gen);
		double currentTime = glfwGetTime();
		if (currentTime < endTime) {
			frameCounter++;
			testRot -= 0.5f;
		}
		else {
			endTime = glfwGetTime()+1;
			fps = frameCounter;
			frameCounter = 0;
		}
		if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS && currentTime >= spawnCooldown) {
			gen.EvaluatePosition(pos, SHIP);
			spawnCooldown = currentTime + 0.3f;
		}
		std::string title = "Bobcraft, FPS: " + std::to_string(fps);
		glfwSetWindowTitle(window, title.c_str());
		glClearColor(sky.r, sky.g, sky.b, sky.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		shader.Bind();
		player.Update(window);
		entityManager.Update();
		world.Render();
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
			glfwSetWindowShouldClose(window, 1);
		}
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	world.Delete();
	shader.Unload();
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}