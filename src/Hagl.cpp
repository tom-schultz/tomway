// hagl.cpp : Defines the entry point for the application.
//

#include "Hagl.h"
#include "WindowSystem.h"
#include "RenderSystem.h"

int main(int argc, char* argv[])
{
	hagl::WindowSystem windowSystem(1024, 768);
	windowSystem.init();
	hagl::RenderSystem renderSystem(windowSystem);
	renderSystem.init();

	while (true) {
		windowSystem.handle_events();
	}

	return 0;
}