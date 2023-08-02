#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL.h>
#include <SDL_opengl.h>

const int WINDOW_WIDTH=640, WINDOW_HEIGHT=480;
const float BG_RED=1.0f, BG_GREEN=1.0f, BG_BLUE=1.0f, BG_OPACITY=1.0f;

SDL_Window* displayWindow;
bool gameIsRunning = true;


void initialise()
{
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("HW5!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
	glewInit();
#endif

	glClearColor(BG_RED, BG_GREEN, BG_BLUE, BG_OPACITY);
}
void process_input()
{
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
			gameIsRunning = false;
		}
	}

}
void update() {}
void render() 
{
	glClear(GL_COLOR_BUFFER_BIT);
	SDL_GL_SwapWindow(displayWindow);
}

void shutdown() { SDL_Quit(); }

int main(int argc, char* argv[]) {
	initialise();

	SDL_Event event;
	while (gameIsRunning) {
		process_input();
		update();
		render();
	}

	shutdown();
	return 0;
}