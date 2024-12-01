#pragma once

#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"
#include "SDL2/SDL_mixer.h"
#include <map>
#include <string>


class Engine
{
public:
	// Destroys the avancez library instance
	void destroy();

	// Destroys the avancez library instance and exits
	//void quit();

	// Creates the main window. Returns true on success.
	bool init(int width, int height);


	// Clears the screen and draws all sprites and texts which have been drawn
	// since the last update call.
	// If update returns false, the application should terminate.
	void processInput();

	void swapBuffers();

	void clearWindow();


	// Draws the given text.
	void drawText(int x, int y, const char* msg);

	void drawMenuText(int x,int y, const char* msg);
	void drawCenteredMenuText(int y, const char* msg);

	void drawHugeText(int x, int y, const char * msg);
	void drawCenteredHugeText(int y, const char * msg);


	// Return the total time spent in the game, in seconds.
	float getElapsedTime();


	//true if the given key is pressed
	bool isPressed(int keyCode);


	//true if the given key is released
	bool isReleased(int keyCode);

	//display fps rate on screen
	void displayFps();

	void UpdateFrame() {
		if (getElapsedTime() - lastFrameTime > 0.05f) {
			currentFrame++;
			lastFrameTime = getElapsedTime();
			if (currentFrame >= 120) {
				currentFrame = 0;
			}
		}
	}
	int GetFrame() { return currentFrame; }

	void setCameraPos(int x, int y) {
		camera.x = x - (int(camera.w/(2*zoomFactor)));
		camera.y = y - (int(camera.h/(2*zoomFactor)));

		//Keep the camera in bounds 
		if( camera.x < 0 ) { camera.x = 0; }
		else if (camera.x > levelWidth - int(camera.w / zoomFactor)) { camera.x = levelWidth - int(camera.w / zoomFactor); }
		if( camera.y < 0 ) { camera.y = 0; } 
		else if( camera.y > levelHeight - int(camera.h/zoomFactor )) { camera.y = levelHeight - int(camera.h/zoomFactor); }
	}
	SDL_Rect * getCamera() {
		return &camera;
	}

	void setLevelDims(int width, int height) {
		levelWidth = width;
		levelHeight = height;
	}
	int getLevelWidth() {
		return levelWidth;
	}
	int getLevelHeight() {
		return levelHeight;
	}

	float getZoom() { return zoomFactor; }
	void setZoom(float zoom) { zoomFactor = zoom; }

	SDL_Renderer * getRenderer() { return renderer; }


private:
	SDL_Rect camera;
	float zoomFactor;

	int levelHeight, levelWidth;

	SDL_Window * window;
	SDL_Renderer * renderer;

	TTF_Font* font;
	TTF_Font* big_font;
	TTF_Font* huge_font;

	std::map<int, bool> keyStates;


	int currentFrame;
	float lastFrameTime;


	//FPS calculation
	Uint32 lastSecond;
	int fps;
	int frames;
};


class Sprite
{
	Engine * engine;
	SDL_Renderer * renderer;
	SDL_Texture * texture;

public:

	static Sprite * createFromFile(Engine * engine,const char * path);

	Sprite(Engine * engine, SDL_Renderer * renderer, SDL_Texture * texture) {
		this->engine = engine;
		this->renderer = renderer;
		this->texture = texture;
	}

	// Destroys the sprite instance
	void destroy();

	// Draw the sprite at the given position.
	// Valid coordinates are between (0,0) (upper left) and (width-32, height-32) (lower right).
	// (All sprites are 32*32 pixels, clipping is not supported)
	void draw(int x, int y, SDL_Rect * clip, SDL_RendererFlip flip);
	void draw(int x, int y, SDL_Rect * clip, SDL_Rect * dest, double angle, SDL_RendererFlip flip);

	void drawAbsolute(int x, int y, float multiplier);

	void drawResized(int x, int y, int width, int height);


	int getWidth() {
		int w;
		SDL_QueryTexture(texture, NULL, NULL, &w, NULL);
		return w;
	}
	int getHeight() {
		int h;
		SDL_QueryTexture(texture, NULL, NULL, NULL, &h);
		return h;
	}
};

class Sound
{
	Mix_Chunk * fx;
public:
	Sound(Mix_Chunk * ch) {
		fx = ch;
	}

	static Sound * createFromFile(const char * path) {
		Mix_Chunk *s = Mix_LoadWAV(path);
		if (s == NULL) {
			printf("Failed to load sound! SDL_mixer Error: %s\n", Mix_GetError());
			return NULL;
		}
		else {
			Mix_VolumeChunk(s, 20);
			return new Sound(s);
		}
	}

	void play() {
		Mix_PlayChannel(-1, fx, 0);
	}

	void destroy() {
		if (fx != NULL) {
			Mix_FreeChunk(fx);
			fx = NULL;
		}
	}
};

class Music
{
	Mix_Music * song;
public:
	Music(Mix_Music * s) {
		song = s;
	}

	static Music * createFromFile(const char * path) {
		Mix_Music *s = Mix_LoadMUS(path);
		if (s == NULL) {
			printf("Failed to load music! SDL_mixer Error: %s\n", Mix_GetError());
			return NULL;
		}
		else
			return new Music(s);
	}

	static void pause() {
		Mix_PauseMusic();
	}
	static void resume() {
		Mix_ResumeMusic();
	}

	void play(int loops) {
		Mix_PlayMusic(song, loops);
	}

	void destroy() {
		if (song != NULL) {
			Mix_FreeMusic(song);
			song = NULL;
		}
	}
};