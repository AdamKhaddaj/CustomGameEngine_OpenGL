#include <iostream>
#include <exception>
#include "game.h"

// Main function that builds and runs the game
int main() {
	Game::Game game;

	try {
		game.Initialize();

		game.SetupResources();
		game.SetupScene();

		game.MainLoop();
	} catch (std::string exception) {
		std::cerr << exception << std::endl;
	}

	return 0;
}