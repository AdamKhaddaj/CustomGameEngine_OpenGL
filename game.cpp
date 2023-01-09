#include <iostream>
#include <time.h>
#include <sstream>
#include <stack>
#include "game.h"
#include "path_config.h"
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

namespace Game {
	const bool DISABLE_ENEMY = false;
	const bool DISABLE_MAZE_COLLISIONS = false;

	const std::string MUSIC = std::string(MATERIAL_DIRECTORY) + std::string("/music.wav");

	const int FPS = 60;

	const std::string WINDOW_TITLE = "The Maze";

	const unsigned int WINDOW_WIDTH = FRAME_BUFFER_WIDTH;
	const unsigned int WINDOW_HEIGHT = FRAME_BUFFER_HEIGHT;

	const bool WINDOW_FULL_SCREEN = false;

	const glm::vec3 CAMERA_POSITION(2.0f, 1.0f, 2.0f);
	const glm::vec3 CAMERA_FORWARD(-1.0f, 0.0f, -1.0f);
	const glm::vec3 CAMERA_UP(0.0f, 1.0f, 0.0f);

	const float CAMERA_NEAR_PLANE = 0.001f;
	const float CAMERA_FAR_PLANE = 1000.0f;

	const float CAMERA_FOV = 60.0f;

	const float PLAYER_MOVE_SPEED = 0.1f;

	Game::Game() {}

	Game::~Game() {
		glfwTerminate();
	}

	void Game::Initialize() {
		InitializeWindow();
		InitializeView();
		InitializeEventHandlers();
	}

	void Game::SetupResources() {
		std::string filename;

		// World
		{
			// Create terrain and maze resources
			resourceManager.CreateTerrain();
			resourceManager.CreateMaze();

			// Load Skybox

			std::string px = std::string(MATERIAL_DIRECTORY) + std::string("/sb0.png");
			std::string nx = std::string(MATERIAL_DIRECTORY) + std::string("/sb1.png");
			std::string py = std::string(MATERIAL_DIRECTORY) + std::string("/sb2.png");
			std::string ny = std::string(MATERIAL_DIRECTORY) + std::string("/sb3.png");
			std::string pz = std::string(MATERIAL_DIRECTORY) + std::string("/sb4.png");
			std::string nz = std::string(MATERIAL_DIRECTORY) + std::string("/sb5.png");

			resourceManager.LoadCubemap("SkyboxTexture", px.c_str(), nx.c_str(), py.c_str(), ny.c_str(), pz.c_str(), nz.c_str());
			resourceManager.CreateSkybox();

			// Particles

			resourceManager.CreateFountainParticles("FountainParticles");
			resourceManager.CreateLeafParticles(500);
			resourceManager.CreateMonsterParticles(300);
			resourceManager.CreateLineParticles("Portal", 1000);
		}

		// Models
		{
			// Cylinder
			resourceManager.CreateCylinder("Cylinder", 2.0f, 1.0f, 32.0f, 32.0f);

			// Crow
			filename = std::string(MATERIAL_DIRECTORY) + std::string("/crow.obj");
			resourceManager.LoadResource(ResourceType::Mesh, "CrowBody", filename.c_str());

			// Water hole
			filename = std::string(MATERIAL_DIRECTORY) + std::string("/water.obj");
			resourceManager.LoadResource(ResourceType::Mesh, "WaterHole", filename.c_str());

			// Create rock resources

			filename = std::string(MATERIAL_DIRECTORY) + std::string("/rock1.obj");
			resourceManager.LoadResource(ResourceType::Mesh, "Rock1", filename.c_str());

			filename = std::string(MATERIAL_DIRECTORY) + std::string("/rock2.obj");
			resourceManager.LoadResource(ResourceType::Mesh, "Rock2", filename.c_str());

			filename = std::string(MATERIAL_DIRECTORY) + std::string("/rock3.obj");
			resourceManager.LoadResource(ResourceType::Mesh, "Rock3", filename.c_str());

			// Create pillar resources

			filename = std::string(MATERIAL_DIRECTORY) + std::string("/pillar1.obj");
			resourceManager.LoadResource(ResourceType::Mesh, "Pillar1", filename.c_str());

			filename = std::string(MATERIAL_DIRECTORY) + std::string("/pillar2.obj");
			resourceManager.LoadResource(ResourceType::Mesh, "Pillar2", filename.c_str());

			filename = std::string(MATERIAL_DIRECTORY) + std::string("/pillar3.obj");
			resourceManager.LoadResource(ResourceType::Mesh, "Pillar3", filename.c_str());

			// Create stage resource
			filename = std::string(MATERIAL_DIRECTORY) + std::string("/stage.obj");
			resourceManager.LoadResource(ResourceType::Mesh, "Stage", filename.c_str());

			// Create fountain resource
			filename = std::string(MATERIAL_DIRECTORY) + std::string("/fountain.obj");
			resourceManager.LoadResource(ResourceType::Mesh, "Fountain", filename.c_str());

			// Create shrine resources

			filename = std::string(MATERIAL_DIRECTORY) + std::string("/shrine1.obj");
			resourceManager.LoadResource(ResourceType::Mesh, "Shrine1", filename.c_str());

			filename = std::string(MATERIAL_DIRECTORY) + std::string("/shrine2.obj");
			resourceManager.LoadResource(ResourceType::Mesh, "Shrine2", filename.c_str());

			// Create gem resource
			filename = std::string(MATERIAL_DIRECTORY) + std::string("/gem.obj");
			resourceManager.LoadResource(ResourceType::Mesh, "Gem", filename.c_str());

			// Create cross resource
			filename = std::string(MATERIAL_DIRECTORY) + std::string("/cross.obj");
			resourceManager.LoadResource(ResourceType::Mesh, "Cross", filename.c_str());

			// Create grave resource
			filename = std::string(MATERIAL_DIRECTORY) + std::string("/grave.obj");
			resourceManager.LoadResource(ResourceType::Mesh, "Grave", filename.c_str());

			// Create duggrave resource
			filename = std::string(MATERIAL_DIRECTORY) + std::string("/duggrave.obj");
			resourceManager.LoadResource(ResourceType::Mesh, "DugGrave", filename.c_str());

			// Create bench resource
			filename = std::string(MATERIAL_DIRECTORY) + std::string("/bench.obj");
			resourceManager.LoadResource(ResourceType::Mesh, "Bench", filename.c_str());
		}

		// Materials/Shaders
		{
			// Load skybox shader
			filename = std::string(MATERIAL_DIRECTORY) + std::string("/skybox");
			resourceManager.LoadResource(ResourceType::Material, "SkyboxShader", filename.c_str());

			// Load textured shader (default)
			filename = std::string(MATERIAL_DIRECTORY) + std::string("/textured");
			resourceManager.LoadResource(ResourceType::Material, "TexturedShader", filename.c_str());

			// Load maze shader
			filename = std::string(MATERIAL_DIRECTORY) + std::string("/maze");
			resourceManager.LoadResource(ResourceType::Material, "MazeShader", filename.c_str());

			// Load overlay shader
			filename = std::string(MATERIAL_DIRECTORY) + std::string("/overlay");
			resourceManager.LoadResource(ResourceType::Material, "OverlayShader", filename.c_str());

			// Load fountain shader
			filename = std::string(MATERIAL_DIRECTORY) + std::string("/fountain");
			resourceManager.LoadResource(ResourceType::Material, "FountainShader", filename.c_str());

			// Load shiny shader
			filename = std::string(MATERIAL_DIRECTORY) + std::string("/shiny");
			resourceManager.LoadResource(ResourceType::Material, "ShinyShader", filename.c_str());

			// Load water shader
			filename = std::string(MATERIAL_DIRECTORY) + std::string("/water");
			resourceManager.LoadResource(ResourceType::Material, "WaterShader", filename.c_str());

			// Load monster shader
			filename = std::string(MATERIAL_DIRECTORY) + std::string("/monster");
			resourceManager.LoadResource(ResourceType::Material, "MonsterShader", filename.c_str());

			// Load leaves shader
			filename = std::string(MATERIAL_DIRECTORY) + std::string("/leaf");
			resourceManager.LoadResource(ResourceType::Material, "LeavesShader", filename.c_str());

			// Load proximity shader
			filename = std::string(MATERIAL_DIRECTORY) + std::string("/monster_sse");
			resourceManager.LoadResource(ResourceType::Material, "ProximityShader", filename.c_str());

			// Load portal shader
			filename = std::string(MATERIAL_DIRECTORY) + std::string("/portal");
			resourceManager.LoadResource(ResourceType::Material, "PortalShader", filename.c_str());
		}

		// Textures
		{
			// Load title texture
			filename = std::string(MATERIAL_DIRECTORY) + std::string("/title.png");
			resourceManager.LoadResource(ResourceType::Texture, "TitleTexture", filename.c_str());

			// Load paused texture
			filename = std::string(MATERIAL_DIRECTORY) + std::string("/paused.png");
			resourceManager.LoadResource(ResourceType::Texture, "PausedTexture", filename.c_str());

			// Load win texture
			filename = std::string(MATERIAL_DIRECTORY) + std::string("/win.png");
			resourceManager.LoadResource(ResourceType::Texture, "WinTexture", filename.c_str());

			// Load loss texture
			filename = std::string(MATERIAL_DIRECTORY) + std::string("/loss.png");
			resourceManager.LoadResource(ResourceType::Texture, "LossTexture", filename.c_str());

			// Load terrain texture
			filename = std::string(MATERIAL_DIRECTORY) + std::string("/terrain.png");
			resourceManager.LoadResource(ResourceType::Texture, "TerrainTexture", filename.c_str());

			// Load maze texture
			filename = std::string(MATERIAL_DIRECTORY) + std::string("/maze.png");
			resourceManager.LoadResource(ResourceType::Texture, "MazeTexture", filename.c_str());

			// Load drop texture
			filename = std::string(MATERIAL_DIRECTORY) + std::string("/drop.png");
			resourceManager.LoadResource(ResourceType::Texture, "DropTexture", filename.c_str());

			// Load jewel texture
			filename = std::string(MATERIAL_DIRECTORY) + std::string("/jewel.png");
			resourceManager.LoadResource(ResourceType::Texture, "GemTexture", filename.c_str());

			// Load rock texture
			filename = std::string(MATERIAL_DIRECTORY) + std::string("/rock.png");
			resourceManager.LoadResource(ResourceType::Texture, "RockTexture", filename.c_str());

			// Load crow texture
			filename = std::string(MATERIAL_DIRECTORY) + std::string("/crow.png");
			resourceManager.LoadResource(ResourceType::Texture, "CrowTexture", filename.c_str());

			// Load wood texture
			filename = std::string(MATERIAL_DIRECTORY) + std::string("/wood.png");
			resourceManager.LoadResource(ResourceType::Texture, "WoodTexture", filename.c_str());

			// Load dirt texture
			filename = std::string(MATERIAL_DIRECTORY) + std::string("/dirt.png");
			resourceManager.LoadResource(ResourceType::Texture, "DirtTexture", filename.c_str());

			// Load marble texture
			filename = std::string(MATERIAL_DIRECTORY) + std::string("/marble.png");
			resourceManager.LoadResource(ResourceType::Texture, "MarbleTexture", filename.c_str());

			// Load monster texture
			filename = std::string(MATERIAL_DIRECTORY) + std::string("/monster.png");
			resourceManager.LoadResource(ResourceType::Texture, "MonsterTexture", filename.c_str());

			// Load portal texture
			filename = std::string(MATERIAL_DIRECTORY) + std::string("/star.png");
			resourceManager.LoadResource(ResourceType::Texture, "PortalTexture", filename.c_str());

			// Load leaf texture
			filename = std::string(MATERIAL_DIRECTORY) + std::string("/leaf.png");
			resourceManager.LoadResource(ResourceType::Texture, "LeafTexture", filename.c_str());
		}

		// Set up texture for screen space effects
		scene.SetupDrawToTexture();
	}

	void Game::SetupScene() {
		// Create terrain and maze
		CreateInstance("Terrain", "Terrain", "TexturedShader", "TerrainTexture");
		CreateInstance("Maze", "Maze", "MazeShader", "MazeTexture");

		CreateInstance("Skybox", "Skybox", "SkyboxShader", "SkyboxTexture");

		SceneNode* s;
		Collision c;
		Gem g;

		// Center tree
		CreateTree(5);

		c.isPoint = true;
		c.position = glm::vec2(55.0f, 55.0f);
		c.size = glm::vec3(1.0f, 0.0f, 0.0f);

		collisions.push_back(c);

		// Crow
		s = CreateCrow("Crow1", "CrowBody", "TexturedShader", "CrowTexture");
		s = CreateCrow("Crow2", "CrowBody", "TexturedShader", "CrowTexture");
		s = CreateCrow("Crow3", "CrowBody", "TexturedShader", "CrowTexture");

		// Enemy
		s = CreateInstance("Monster", "MonsterParticles", "MonsterShader", "MonsterTexture");
		s->SetPosition(glm::vec3(110.0f, 2.0f, 110.0f));

		// Leaves
		s = CreateInstance("Leaves", "LeafParticles", "LeavesShader", "LeafTexture");
		s->SetPosition(glm::vec3(55.0f, 13.0f, 55.0f));

		// Rocks
		{
			for (int i = 0; i < 20; i++) {
				float r1 = ((float)rand() / (float)RAND_MAX) * 2.0f * glm::pi<float>();
				float r2 = 7.0f + rand() % 16;
				int r3 = rand() % 3;

				if (r3 == 0) {
					s = CreateInstance("Rock", "Rock1", "TexturedShader", "RockTexture");
				} else if (r3 == 1) {
					s = CreateInstance("Rock", "Rock2", "TexturedShader", "RockTexture");
				} else {
					s = CreateInstance("Rock", "Rock3", "TexturedShader", "RockTexture");
				}

				glm::vec2 pos = glm::vec2(55.0f + glm::cos((float)r1) * r2, 55.0f + glm::sin((float)r1) * r2);

				s->SetPosition(glm::vec3(pos.x, resourceManager.GetTerrainHeightAt(pos.x, pos.y) - 0.1f, pos.y));
				s->Scale(glm::vec3(0.5f, 0.5f, 0.5f));

				c.isPoint = true;
				c.position = glm::vec2(pos.x, pos.y);
				c.size = glm::vec3(1.0f, 0.0f, 0.0f);

				collisions.push_back(c);
			}
		}

		// Graveyard
		{
			for (int i = 0; i < 6; i++) {
				for (int j = 0; j < 3; j++) {
					if (i == 3 && j == 1) {
						s = CreateInstance("Cross", "Cross", "TexturedShader", "MarbleTexture");
						s->SetPosition(glm::vec3(5.0f + i * 2.0f, 0.0f, 5.0f + j * 4.0f));
						s->Scale(glm::vec3(0.25f));
						s->Rotate(glm::angleAxis(-90.0f * glm::pi<float>() / 180.0f, glm::cross(glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f))));

						s = CreateInstance("Grave", "DugGrave", "TexturedShader", "DirtTexture");
						s->SetPosition(glm::vec3(5.0f + i * 2.0f, -0.02f, 6.0f + j * 4.0f));
						s->Scale(glm::vec3(0.4f));

						continue;
					}

					s = CreateInstance("Cross", "Cross", "TexturedShader", "MarbleTexture");
					s->SetPosition(glm::vec3(5.0f + i * 2.0f, 0.0f, 5.0f + j * 4.0f));
					s->Scale(glm::vec3(0.25f));

					c.isPoint = true;
					c.position = glm::vec2(s->GetPosition().x, s->GetPosition().z);
					c.size = glm::vec3(0.25f, 0.0f, 0.0f);

					collisions.push_back(c);

					s = CreateInstance("Grave", "Grave", "TexturedShader", "TerrainTexture");
					s->SetPosition(glm::vec3(5.0f + i * 2.0f, -0.02f, 6.0f + j * 4.0f));
					s->Scale(glm::vec3(0.4f));
				}
			}
		}

		// Fountain
		{
			for (int i = 0; i < 6; i++) {
				for (int j = 0; j < 6; j++) {
					if (!((i == 0 && j != 0) || (i != 0 && j == 0) || (i == 5 && j != 0) || (i != 0 && j == 5))) {
						continue;
					}

					if (i - j == -1 || i - j == -4) {
						s = CreateInstance("Pillar", "Pillar3", "TexturedShader", "MarbleTexture");
					} else if (i + j == 5) {
						s = CreateInstance("Pillar", "Pillar2", "TexturedShader", "MarbleTexture");
					} else {
						s = CreateInstance("Pillar", "Pillar1", "TexturedShader", "MarbleTexture");
					}

					s->SetPosition(glm::vec3(98.5f + (i - 2.5f) * 2.0f, 0.0f, 98.5f + (j - 2.5f) * 2.0f));
					s->Scale(glm::vec3(0.5f, 0.5f, 0.5f));

					c.isPoint = true;
					c.position = glm::vec2(s->GetPosition().x, s->GetPosition().z);
					c.size = glm::vec3(0.4f, 0.0f, 0.0f);

					collisions.push_back(c);
				}
			}

			s = CreateInstance("Fountain", "Fountain", "TexturedShader", "MarbleTexture");
			s->SetPosition(glm::vec3(98.5f, 0.0f, 98.5f));
			s->Scale(glm::vec3(0.5f, 0.5f, 0.5f));

			c.isPoint = true;
			c.position = glm::vec2(s->GetPosition().x, s->GetPosition().z);
			c.size = glm::vec3(2.0f, 0.0f, 0.0f);

			collisions.push_back(c);

			s = CreateInstance("FountainWater", "WaterHole", "WaterShader", "TerrainTexture");
			s->SetPosition(glm::vec3(98.5f, 0.0f, 98.5f));
			s->Scale(glm::vec3(1.85f, 1.0f, 1.85f));

			s = CreateInstance("FountainParticles", "FountainParticles", "FountainShader", "DropTexture");
			s->SetPosition(glm::vec3(98.5f, 0.0f, 98.5f));
		}

		// Theater
		{
			for (int i = 0; i < 4; i++) {
				for (int j = 0; j < 2; j++) {
					s = CreateInstance("Bench", "Bench", "TexturedShader", "WoodTexture");
					s->SetPosition(glm::vec3(10.0f + i * 2.0f, 0.0f, 100.0f + (j - 0.5) * 4.0f));
					s->Rotate(glm::normalize(glm::angleAxis(90.0f * glm::pi<float>() / 180.0f, glm::vec3(0.0f, 1.0f, 0.0f))));
					s->Scale(glm::vec3(0.3f, 0.3f, 0.3f));

					c.isPoint = false;
					c.position = glm::vec2(s->GetPosition().x, s->GetPosition().z);
					c.size = glm::vec3(0.4f, 1.2f, 1.0f);

					collisions.push_back(c);
				}
			}

			s = CreateInstance("Shrine", "Shrine1", "TexturedShader", "RockTexture");
			s->SetPosition(glm::vec3(10.0f, 0.0f, 94.0f));
			s->Scale(glm::vec3(0.5f, 0.5f, 0.5f));

			c.isPoint = false;
			c.position = glm::vec2(10.0f, 94.0f);
			c.size = glm::vec3(0.5f, 0.5f, 0.5f);

			collisions.push_back(c);

			s = CreateInstance("Shrine", "Shrine1", "TexturedShader", "RockTexture");
			s->SetPosition(glm::vec3(16.0f, 0.0f, 94.0f));
			s->Scale(glm::vec3(0.5f, 0.5f, 0.5f));

			c.isPoint = false;
			c.position = glm::vec2(16.0f, 94.0f);
			c.size = glm::vec3(0.5f, 0.5f, 0.5f);

			collisions.push_back(c);

			s = CreateInstance("Stage", "Stage", "TexturedShader", "WoodTexture");
			s->SetPosition(glm::vec3(5.0f, 0.0f, 100.0f));
			s->Scale(glm::vec3(0.5f, 0.5f, 0.5f));

			c.isPoint = false;
			c.position = glm::vec2(5.0f, 100.0f);
			c.size = glm::vec3(2.5f, 5.1f, 5.0f);

			collisions.push_back(c);
		}

		// Shrine
		{
			for (int i = 0; i < 10; i++) {
				int r = rand() % 3;

				if (r == 0) {
					s = CreateInstance("Rock", "Rock1", "TexturedShader", "RockTexture");
				} else {
					s = CreateInstance("Rock", "Rock2", "TexturedShader", "RockTexture");
				}

				s->SetPosition(glm::vec3(99.5f + glm::cos((i / 10.0f) * 2.0f * glm::pi<float>()) * 5.0f, -0.1f, 8.5f + glm::sin((i / 10.0f) * 2.0f * glm::pi<float>()) * 5.0f));
				s->Scale(glm::vec3(0.5f, 0.5f, 0.5f));

				c.isPoint = true;
				c.position = glm::vec2(s->GetPosition().x, s->GetPosition().z);
				c.size = glm::vec3(1.0f, 0.0f, 0.0f);

				collisions.push_back(c);
			}

			s = CreateInstance("Shrine", "Shrine2", "TexturedShader", "RockTexture");
			s->SetPosition(glm::vec3(99.5f, 0.0f, 8.5f));
			s->Scale(glm::vec3(0.5f, 0.5f, 0.5f));

			c.isPoint = false;
			c.position = glm::vec2(99.5f, 8.5f);
			c.size = glm::vec3(1.1f, 1.1f, 1.1f);

			collisions.push_back(c);
		}

		// Place gems throughout the maze
		for (int i = 0; i < 25; i++) {
			int x;
			int z;

			do {
				x = 1 + rand() % 54;
				z = 1 + rand() % 54;
			} while (x % 2 == 0 || z % 2 == 0 || (x > 14 && x < 55 - 15 && z > 14 && z < 55 - 15) || (x < 10 && z < 10) || (x > 55 - 11 && z > 55 - 11) || (x < 10 && z > 55 - 11) || (x > 55 - 11 && z < 10));

			std::string name = "Gem" + std::to_string(i);

			s = CreateInstance(name, "Gem", "ShinyShader", "GemTexture");
			s->SetPosition(glm::vec3(x * 2.0f, 0.5f, z * 2.0f));

			g.id = i;
			g.location = glm::vec2(x * 2.0f, z * 2.0f);

			gems.push_back(g);
		}

		int x;
		int z;

		do {
			x = 1 + rand() % 54;
			z = 1 + rand() % 54;
		} while (x % 2 == 0 || z % 2 == 0 || (x > 14 && x < 55 - 15 && z > 14 && z < 55 - 15) || (x < 10 && z < 10) || (x > 55 - 11 && z > 55 - 11) || (x < 10 && z > 55 - 11) || (x > 55 - 11 && z < 10));

		// Create exit portal
		s = CreateInstance("Portal", "Portal", "PortalShader", "PortalTexture");
		s->SetPosition(glm::vec3(x * 2.0f, 0.5f, z * 2.0f));
		s->SetScale(glm::vec3(0.75f, 0.75f, 0.75f));

		// Play music
		PlaySound(TEXT(MUSIC.c_str()), NULL, SND_ASYNC | SND_LOOP);
	}

	void Game::MainLoop() {
		while (!glfwWindowShouldClose(window)) {
			// Maintain consistent framerate
			if (1.0f / (float)FPS > glfwGetTime() - lastFrame) {
				continue;
			}

			lastFrame = glfwGetTime();

			// Crows
			{
				SceneNode* crow1 = scene.GetNode("Crow1");
				SceneNode* crow2 = scene.GetNode("Crow2");
				SceneNode* crow3 = scene.GetNode("Crow3");

				float crowPositionAngle1 = 10.0f + glfwGetTime() * 0.25f;
				float crowPositionAngle2 = 70.0f + glfwGetTime() * 0.23f;
				float crowPositionAngle3 = 30.0f + glfwGetTime() * 0.3f;

				crow1->SetPosition(glm::vec3(55.0f + glm::cos(crowPositionAngle1) * 25.0f, 25.0f, 55.0f + glm::sin(crowPositionAngle1) * 25.0f));
				crow2->SetPosition(glm::vec3(55.0f + -glm::cos(crowPositionAngle2) * 30.0f, 30.0f, 55.0f + glm::sin(crowPositionAngle2) * 30.0f));
				crow3->SetPosition(glm::vec3(55.0f + glm::cos(crowPositionAngle3) * 20.0f, 35.0f, 55.0f + glm::sin(crowPositionAngle3) * 20.0f));

				float crowViewAngle1 = glm::acos(cos(crowPositionAngle1)) * (glm::sin(crowPositionAngle1) > 0 ? -1 : 1);
				float crowViewAngle2 = glm::acos(cos(crowPositionAngle2)) * (glm::sin(crowPositionAngle2) > 0 ? -1 : 1);
				float crowViewAngle3 = glm::acos(cos(crowPositionAngle3)) * (glm::sin(crowPositionAngle3) > 0 ? -1 : 1);

				glm::quat base = (glm::normalize(glm::angleAxis(glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f))));

				crow1->SetOrientation(glm::normalize(glm::angleAxis(crowViewAngle1, CAMERA_UP)));
				crow1->Rotate(base);

				crow2->SetOrientation(glm::normalize(glm::angleAxis(-crowViewAngle2, CAMERA_UP)));
				crow2->Rotate(base);

				crow3->SetOrientation(glm::normalize(glm::angleAxis(crowViewAngle3, CAMERA_UP)));
				crow3->Rotate(base);
			}

			// Gems

			glm::quat gemOrientation = glm::normalize(glm::angleAxis((float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f)));

			for (int i = 0; i < gems.size(); i++) {
				std::string name = "Gem" + std::to_string(i);

				SceneNode* s = scene.GetNode(name);

				// Rotate gem
				s->SetOrientation(gemOrientation);
			}

			// Start screen
			if (phase == 0) {
				// Orbit the camera around the center of the map for the start screen

				float positionAngle = glfwGetTime() * 0.1f;

				camera.SetPosition(glm::vec3(55.0f + glm::cos(positionAngle) * 55.0f, 15.0f, 55.0f + glm::sin(positionAngle) * 55.0f));

				float viewAngle = glm::acos(cos(positionAngle)) * (glm::sin(positionAngle) > 0 ? -1 : 1);

				camera.SetOrientation(glm::angleAxis(viewAngle + 1.25f * glm::pi<float>(), CAMERA_UP));

				// Move skybox to player position
				scene.GetNode("Skybox")->SetPosition(camera.GetPosition());

				// Draw title text
				scene.DrawToTexture(&camera);
				scene.DisplayTexture(resourceManager.GetResource("OverlayShader")->GetResource(), 0.0f, resourceManager.GetResource("TitleTexture")->GetResource());

				// Gameplay
			} else if (phase == 1) {
				// Have monster track player, and activate screenspace effect if nearby

				SceneNode* monster = scene.GetNode("Monster");

				glm::vec3 direction = camera.GetPosition() - monster->GetPosition();

				float distance = glm::distance(camera.GetPosition(), monster->GetPosition());

				// Move monster
				monster->SetPosition(monster->GetPosition() + glm::normalize(direction) * PLAYER_MOVE_SPEED * 0.4f);
				monster->SetPosition(glm::vec3(monster->GetPosition().x, 1.0f + resourceManager.GetTerrainHeightAt(monster->GetPosition().x, monster->GetPosition().z), monster->GetPosition().z));

				// Check proximity and change game state to game over if it gets too close
				if (distance <= 1.0f) {
					phase = 2;
				}

				// Hide enemy
				if (DISABLE_ENEMY) {
					monster->SetPosition(glm::vec3(-100.0f, -100.0f, -100.0f));
				}

				// Used to calculate collisions
				glm::vec3 nextPosition = camera.GetPosition();

				// Movement
				if (wPressed) {
					nextPosition -= camera.GetForward() * PLAYER_MOVE_SPEED;
				} else if (aPressed) {
					nextPosition -= camera.GetSide() * PLAYER_MOVE_SPEED;
				} else if (sPressed) {
					nextPosition += camera.GetForward() * PLAYER_MOVE_SPEED;
				} else if (dPressed) {
					nextPosition += camera.GetSide() * PLAYER_MOVE_SPEED;
				}

				// Check for collisions
				if (CheckCollisions(glm::vec2(nextPosition.x, nextPosition.z))) {
					nextPosition = camera.GetPosition();
				} else {
					CheckGems(glm::vec2(nextPosition.x, nextPosition.z));

					// If the player is at the portal they win
					if (glm::distance(camera.GetPosition(), scene.GetNode("Portal")->GetPosition()) < 1.5f) {
						phase = 3;
					}
				}

				// Set player height
				nextPosition.y = 1.0f + resourceManager.GetTerrainHeightAt(nextPosition.x, nextPosition.z);

				// Move player
				camera.SetPosition(nextPosition);

				// Move skybox to player position
				scene.GetNode("Skybox")->SetPosition(camera.GetPosition());

				// Render scene
				scene.DrawToTexture(&camera);
				scene.DisplayTexture(resourceManager.GetResource("ProximityShader")->GetResource(), distance);

				// Loss screen
			} else if (phase == 2) {
				// Move skybox to player position
				scene.GetNode("Skybox")->SetPosition(camera.GetPosition());

				// Draw paused text
				scene.DrawToTexture(&camera);
				scene.DisplayTexture(resourceManager.GetResource("OverlayShader")->GetResource(), 0.0f, resourceManager.GetResource("LossTexture")->GetResource());

				// Win screen
			} else if (phase == 3) {
				// Move skybox to player position
				scene.GetNode("Skybox")->SetPosition(camera.GetPosition());

				// Draw paused text
				scene.DrawToTexture(&camera);
				scene.DisplayTexture(resourceManager.GetResource("OverlayShader")->GetResource(), 0.0f, resourceManager.GetResource("WinTexture")->GetResource());

				// Paused
			} else {
				// Move skybox to player position
				scene.GetNode("Skybox")->SetPosition(camera.GetPosition());

				// Draw paused text
				scene.DrawToTexture(&camera);
				scene.DisplayTexture(resourceManager.GetResource("OverlayShader")->GetResource(), 0.0f, resourceManager.GetResource("PausedTexture")->GetResource());
			}

			// Push buffer drawn in the background onto the display
			glfwSwapBuffers(window);

			// Update other events like input handling
			glfwPollEvents();
		}
	}

	void Game::InitializeWindow() {
		// Initialize the GLFW library
		if (!glfwInit()) {
			throw(std::string("Could not initialize the GLFW library"));
		}

		// Initialize the window
		if (WINDOW_FULL_SCREEN) {
			window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE.c_str(), glfwGetPrimaryMonitor(), NULL);
		} else {
			window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE.c_str(), NULL, NULL);
		}

		if (!window) {
			glfwTerminate();

			throw(std::string("Could not create window"));
		}

		glfwMakeContextCurrent(window);

		glewExperimental = GL_TRUE;

		GLenum error = glewInit();

		if (error != GLEW_OK) {
			throw(std::string("Could not initialize the GLEW library: ") + std::string((const char*)glewGetErrorString(error)));
		}
	}

	void Game::InitializeView() {
		// Set up z-buffer

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);

		int width, height;

		// Set viewport

		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);

		// Set camera view
		camera.SetView(CAMERA_POSITION, CAMERA_FORWARD, CAMERA_UP);
		// Set camera projection matrix
		camera.SetProjection(CAMERA_FOV, CAMERA_NEAR_PLANE, CAMERA_FAR_PLANE, width, height);
	}

	void Game::InitializeEventHandlers() {
		// Set event callbacks

		glfwSetKeyCallback(window, KeyCallback);
		glfwSetCursorPosCallback(window, CursorCallback);
		glfwSetFramebufferSizeCallback(window, ResizeCallback);

		// Set pointer to game object for callbacks
		glfwSetWindowUserPointer(window, this);
	}

	void Game::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
		// Get user data with a pointer to the game class
		Game* game = (Game*)glfwGetWindowUserPointer(window);

		// Quit game if escape key is pressed
		if (key == GLFW_KEY_ESCAPE) {
			glfwSetWindowShouldClose(window, true);
		}

		// Start screen
		if (game->phase == 0) {
			// Start game if space key is pressed
			if (key == GLFW_KEY_SPACE) {
				game->phase = 1;

				// Reset camera for gameplay
				game->camera.SetView(CAMERA_POSITION, CAMERA_FORWARD, CAMERA_UP);
			}

			return;

			// Gameplay
		} else if (game->phase == 1) {
			if (key == GLFW_KEY_P && glfwGetTime() - game->pauseBuffer > 0.5f) {
				game->phase = 4;
				game->pauseBuffer = glfwGetTime();
			}

			// Paused
		} else if (game->phase == 4) {
			if (key == GLFW_KEY_P && glfwGetTime() - game->pauseBuffer > 0.5f) {
				game->phase = 1;
				game->pauseBuffer = glfwGetTime();

				// Reset keys
				game->wPressed = false;
				game->aPressed = false;
				game->sPressed = false;
				game->dPressed = false;
			}

			return;
		}

		// WASD movement

		if (key == GLFW_KEY_W) {
			if (action == GLFW_PRESS) {
				game->wPressed = true;
			} else if (action == GLFW_RELEASE) {
				game->wPressed = false;
			}
		}

		if (key == GLFW_KEY_A) {
			if (action == GLFW_PRESS) {
				game->aPressed = true;
			} else if (action == GLFW_RELEASE) {
				game->aPressed = false;
			}
		}

		if (key == GLFW_KEY_S) {
			if (action == GLFW_PRESS) {
				game->sPressed = true;
			} else if (action == GLFW_RELEASE) {
				game->sPressed = false;
			}
		}

		if (key == GLFW_KEY_D) {
			if (action == GLFW_PRESS) {
				game->dPressed = true;
			} else if (action == GLFW_RELEASE) {
				game->dPressed = false;
			}
		}
	}

	void Game::CursorCallback(GLFWwindow* window, double x, double y) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		// Get user data with a pointer to the game class
		Game* game = (Game*)glfwGetWindowUserPointer(window);

		// Ignore input if not in gameplay phase
		if (game->phase != 1) {
			return;
		}

		int width, height;

		glfwGetWindowSize(window, &width, &height);

		// Rotate player camera
		game->camera.Look(x, y, width, height);
	}

	void Game::ResizeCallback(GLFWwindow* window, int width, int height) {
		// Set up viewport and camera projection based on new window size
		glViewport(0, 0, width, height);

		// Get user data with a pointer to the game class
		Game* game = (Game*)glfwGetWindowUserPointer(window);

		// Set camera projection matrix
		game->camera.SetProjection(CAMERA_FOV, CAMERA_NEAR_PLANE, CAMERA_FAR_PLANE, width, height);
	}

	SceneNode* Game::CreateInstance(std::string entityName, std::string objectName, std::string materialName, std::string textureName) {
		Resource* geometry = resourceManager.GetResource(objectName);

		if (!geometry) {
			throw(std::string("Could not find resource \"") + objectName + std::string("\""));
		}

		Resource* material = resourceManager.GetResource(materialName);

		if (!material) {
			throw(std::string("Could not find resource \"") + materialName + std::string("\""));
		}

		Resource* texture = NULL;

		bool isSkybox = false;

		if (textureName != "") {
			if (textureName == "SkyboxTexture") {
				isSkybox = true;
			}

			texture = resourceManager.GetResource(textureName);

			if (!texture) {
				throw(std::string("Could not find resource \"") + textureName + std::string("\""));
			}
		}

		return scene.CreateNode(entityName, geometry, material, texture, isSkybox);
	}

	void Game::CreateTree(int i) {
		CreateTree(i, NULL);
	}

	void Game::CreateTree(int i, SceneNode* parent) {
		// End recursion
		if (i < 1) {
			return;
		}

		// Trunk
		if (parent == NULL) {
			SceneNode* branch = CreateBranchInstance("Trunk", "Cylinder", "TexturedShader");

			branch->SetPosition(glm::vec3(55.0f, resourceManager.GetTerrainHeightAt(55.0f, 55.0f), 55.0f));
			branch->SetOrientation(glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
			branch->SetScale(glm::vec3(1.0f, 4.0f, 1.0f));

			// Only add trunk node to scene graph
			scene.AddNode(branch);

			CreateTree(i - 1, branch);
		}

		// Branches
		else {
			// 2 Branches
			for (int j = -1; j < 2; j += 2) {
				SceneNode* branch = CreateBranchInstance("Branch", "Cylinder", "TexturedShader");

				branch->SetParent(parent);

				branch->SetPosition(glm::vec3(-1 * j * parent->GetScale()[0] * 1.5f, parent->GetScale()[1] * 1.25f, 0.0f));
				branch->SetOrientation(glm::angleAxis(j * glm::radians((i % 2 == 0) ? 35.0f : 45.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
				branch->SetScale(parent->GetScale() * 0.6f);

				parent->AddChild(branch);

				CreateTree(i - 1, branch);
			}
		}
	}

	SceneNode* Game::CreateBranchInstance(std::string entityName, std::string objectName, std::string materialName) {
		Resource* geometry = resourceManager.GetResource(objectName);

		if (!geometry) {
			throw(std::string("Could not find resource \"") + objectName + std::string("\""));
		}

		Resource* material = resourceManager.GetResource(materialName);

		if (!material) {
			throw(std::string("Could not find resource \"") + materialName + std::string("\""));
		}

		Resource* texture = resourceManager.GetResource("WoodTexture");

		// Create branch instance
		SceneNode* branch = new SceneNode(entityName, geometry, material, texture);

		return branch;
	}

	SceneNode* Game::CreateCrow(std::string entityName, std::string objectName, std::string materialName, std::string textureName) {
		Resource* geometry = resourceManager.GetResource(objectName);

		if (!geometry) {
			throw(std::string("Could not find resource \"") + objectName + std::string("\""));
		}

		Resource* material = resourceManager.GetResource(materialName);

		if (!material) {
			throw(std::string("Could not find resource \"") + materialName + std::string("\""));
		}

		Resource* texture = NULL;

		if (textureName != "") {
			texture = resourceManager.GetResource(textureName);

			if (!texture) {
				throw(std::string("Could not find resource \"") + textureName + std::string("\""));
			}
		}

		// Create base then attach 2 wings as children
		SceneNode* body = scene.CreateNode(entityName, geometry, material, texture);

		body->SetOrientation(glm::normalize(glm::angleAxis(glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f))));
		body->SetScale(glm::vec3(0.5f, 0.5f, 0.5f));

		// Left wing
		SceneNode* left = new SceneNode("LeftWing", geometry, material, texture);

		glm::quat left_orientation = glm::normalize(glm::angleAxis(glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)));

		left->SetPosition(glm::vec3(body->GetScale()[0] * 0.5f, 0.0f, 0.0f));
		left->SetOrientation(left_orientation);
		left->SetScale(body->GetScale() * 0.8f);

		left->SetParent(body);
		body->AddChild(left);

		// Right wing
		SceneNode* right = new SceneNode("RightWing", geometry, material, texture);

		glm::quat right_orientation = glm::normalize(glm::angleAxis(glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f)));

		right->SetPosition(glm::vec3(-body->GetScale()[0] * 0.5f, 0.0f, 0.0f));
		right->SetOrientation(right_orientation);
		right->SetScale(body->GetScale() * 0.8f);

		right->SetParent(body);
		body->AddChild(right);

		return body;
	}

	bool Game::CheckCollisions(glm::vec2 position) {
		glm::vec2 mazeCheck = glm::vec2(glm::round(camera.GetPosition().x / 2.0f), glm::round(camera.GetPosition().z / 2.0f));
		glm::vec3 mazePosition = glm::vec3(position.x, 0.0f, position.y);

		if (!DISABLE_MAZE_COLLISIONS) {
			if (resourceManager.GetMazeCollisions(mazeCheck.x - 1, mazeCheck.y, mazePosition)) {
				return true;
			} else if (resourceManager.GetMazeCollisions(mazeCheck.x + 1, mazeCheck.y, mazePosition)) {
				return true;
			} else if (resourceManager.GetMazeCollisions(mazeCheck.x, mazeCheck.y - 1, mazePosition)) {
				return true;
			} else if (resourceManager.GetMazeCollisions(mazeCheck.x, mazeCheck.y + 1, mazePosition)) {
				return true;
			} else if (resourceManager.GetMazeCollisions(mazeCheck.x - 1, mazeCheck.y - 1, mazePosition)) {
				return true;
			} else if (resourceManager.GetMazeCollisions(mazeCheck.x - 1, mazeCheck.y + 1, mazePosition)) {
				return true;
			} else if (resourceManager.GetMazeCollisions(mazeCheck.x + 1, mazeCheck.y - 1, mazePosition)) {
				return true;
			} else if (resourceManager.GetMazeCollisions(mazeCheck.x + 1, mazeCheck.y + 1, mazePosition)) {
				return true;
			}
		}

		for (int i = 0; i < collisions.size(); i++) {
			Collision c = collisions[i];

			if (c.isPoint && glm::distance(position, c.position) < c.size.x) {
				return true;
			} else if (position.x < c.position.x + c.size.x && position.x > c.position.x - c.size.x && position.y < c.position.y + c.size.y && position.y > c.position.y - c.size.y) {
				return true;
			}
		}

		return false;
	}

	void Game::CheckGems(glm::vec2 position) {
		for (int i = 0; i < gems.size(); i++) {
			if (glm::distance(position, gems[i].location) < 1.0f) {
				std::string name = "Gem" + std::to_string(gems[i].id);

				SceneNode* s = scene.GetNode(name);

				// Hide gem outside of bounds
				s->SetPosition(glm::vec3(-10.0f, -10.0f, -10.0f));

				return;
			}
		}
	}
}