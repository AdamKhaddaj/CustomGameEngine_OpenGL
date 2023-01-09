# Custom_GameEngine_OpenGL
A short game created in a custom game engine made with OpenGL. This game implements lighting, dynamic textures, procedural object generation, screenspace effects, particle systems, all done with the engine. The code is scalable and can easily have further rendering methods implemented, such as shadow mapping and normal mapping.

To view an exampleof what the game looks like, here is a short gameplay video:
https://youtu.be/r-yxcTFkeb4

Build instructions:

*NOTE these instructions will currently not work unless you move all files in the 'modes_and_images' and 'shaders' directories into the source directory.

Change directory in path_config.h to be directory of where this project is installed.

Change library path directory in CMakeLists.txt line 26 to library directory you're using for glfw, glew, and SOIL libraries.

WASD to move, P to pause, MOUSE to look

Enjoy!

(darksouls 1 > demonsouls)
