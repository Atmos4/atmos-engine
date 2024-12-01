#include <stdlib.h>

#include "engine.h"
#include "game.h"
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

const int TICKS_PER_SECOND = 25;
const int DELAY = 1000 / TICKS_PER_SECOND;
const int MAX_FRAMESKIP = 5;


Engine engine;
Game game;

static void mainloop(void){
	if (!game.Running()){
		//Clean up
		game.Destroy();
		engine.destroy();
		#ifdef __EMSCRIPTEN__
        emscripten_cancel_main_loop();  /* this should "kill" the app. */
        #else
        exit(0);
        #endif
	}
	engine.processInput();
	int loops = 0;
	while (SDL_GetTicks() > game.lastTime && loops < MAX_FRAMESKIP) {
		game.Update();
		game.lastTime += DELAY;
		loops++;
	}
	//Interpolation
	float interpolation = float(SDL_GetTicks() + DELAY - game.lastTime)/ float(DELAY);
	game.Render(interpolation);
}

int main(int argc, char** argv) {
	if (!engine.init(800, 480))return -1;
	game.Create(&engine);
	game.Init();
	game.lastTime = SDL_GetTicks();

	#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(mainloop, 0, 1);
    #else
    while (1) { mainloop(); }
    #endif
}