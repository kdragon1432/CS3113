#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL.h>
#include <SDL_opengl.h>



SDL_Window* displayWindow;
bool gameIsRunning = true;


void initialize() {
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("HW2", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
	glewInit();
#endif

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); /// background: white 

}

void process_input() {
	SDL_Event event;
	while (SDL_PollEvent(&event)){
		// NO INPUT
		if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
			gameIsRunning = false;
		}
	}

}

void update() {

}

void render() {
	glClear(GL_COLOR_BUFFER_BIT);
	SDL_GL_SwapWindow(displayWindow);

}

void shutdown() {
	SDL_Quit();
}

int main(int argc, char* argv[]) {
	initialize();

	while (gameIsRunning) {
		process_input();
		update();
		render();
	}

	shutdown();
	return 0;
}