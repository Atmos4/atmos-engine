#include "engine.h"

// Creates the main window. Returns true on success.
bool Engine::init(int width, int height)
{
	SDL_Log("Initializing the engine...\n");

	if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL failed the initialization: %s\n", SDL_GetError());
		return false;
	}

	//Create window
	window = SDL_CreateWindow("AtmostEngine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, -0);
	if (window == NULL)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Window could not be created! SDL_Error: %s\n", SDL_GetError());
		return false;
	}

	//Create renderer for window
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (renderer == NULL)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Renderer could not be created! SDL Error: %s\n", SDL_GetError());
		return false;
	}

	TTF_Init();
	font = TTF_OpenFont("assets/ui/space_invaders.ttf", 12); //this opens a font style and sets a size
	if (font == NULL)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "font cannot be created! SDL_Error: %s\n", SDL_GetError());
		return false;
	}

	big_font = TTF_OpenFont("assets/ui/space_invaders.ttf", 24); 
	if (big_font == NULL)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "font cannot be created! SDL_Error: %s\n", SDL_GetError());
		return false;
	}

	huge_font = TTF_OpenFont("assets/ui/space_invaders.ttf", 80);
	if (huge_font == NULL)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "font cannot be created! SDL_Error: %s\n", SDL_GetError());
		return false;
	}

	//Initialize SDL_mixer
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
	{
		printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
		return false;
	}
	Mix_VolumeMusic(20);

	//Initialize renderer color
	SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);

	//Clear screen
	SDL_RenderClear(renderer);

	camera.x = 0;
	camera.y = 0;
	camera.w = width;
	camera.h = height;

	zoomFactor = 2.0f;


	fps = 0;
	frames = 0;
	lastSecond = SDL_GetTicks();

	SDL_Log("Engine up and running...\n");
	return true;
}


// Destroys the engine instance
void Engine::destroy()
{
	SDL_Log("Shutting down the engine\n");

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	TTF_CloseFont(font);
	TTF_CloseFont(big_font);
	TTF_CloseFont(huge_font);

	//Quit SDL subsystems
	Mix_Quit();
	TTF_Quit();
	SDL_Quit();
}


void Engine::processInput()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
			case SDL_KEYDOWN:
				keyStates[event.key.keysym.sym] = true;
				break;
			case SDL_KEYUP:
				keyStates[event.key.keysym.sym] = false;
				break;
			case SDL_WINDOWEVENT:
				switch (event.window.event) {
					case SDL_WINDOWEVENT_CLOSE:   // exit game
						// This will need some refactoring, it's very ugly
						keyStates[SDLK_q] = true;
						break;
					default:
						break;
				}
			break;
		}
	}
}

void Engine::swapBuffers() {
	//Update screen
	SDL_RenderPresent(renderer);
}

void Engine::clearWindow() {
	//Clear screen
	SDL_SetRenderDrawColor(renderer,0, 102, 0, 255);
	SDL_RenderClear(renderer);
}


void Engine::drawText(int x, int y, const char * msg)
{
	SDL_Color yellow = { 255, 204, 0 };  // this is the color in rgb format, maxing out all would give you the color white, and it will be your text's color
	
	SDL_Surface* surf = TTF_RenderText_Solid(font, msg, yellow); // as TTF_RenderText_Solid could only be used on SDL_Surface then you have to create the surface first

	SDL_Texture* msg_texture = SDL_CreateTextureFromSurface(renderer, surf); //now you can convert it into a texture

	int w = 0;
	int h = 0;
	SDL_QueryTexture(msg_texture, NULL, NULL, &w, &h);
	SDL_Rect dst_rect = { x, y, w, h };

	SDL_RenderCopy(renderer, msg_texture, NULL, &dst_rect);

	SDL_DestroyTexture(msg_texture);
	SDL_FreeSurface(surf);
}

void Engine::drawMenuText(int x, int y, const char * msg)
{
	SDL_Color yellow = { 255, 204, 0 };
	SDL_Surface* surf = TTF_RenderText_Solid(big_font, msg, yellow);
	SDL_Texture* msg_texture = SDL_CreateTextureFromSurface(renderer, surf);

	int w = 0;
	int h = 0;
	SDL_QueryTexture(msg_texture, NULL, NULL, &w, &h);
	SDL_Rect dst_rect = { x, y, w, h };

	SDL_RenderCopy(renderer, msg_texture, NULL, &dst_rect);

	SDL_DestroyTexture(msg_texture);
	SDL_FreeSurface(surf);
}

void Engine::drawCenteredMenuText(int y, const char * msg)
{
	SDL_Color yellow = { 255, 204, 0 };
	SDL_Surface* surf = TTF_RenderText_Solid(big_font, msg, yellow);
	SDL_Texture* msg_texture = SDL_CreateTextureFromSurface(renderer, surf);

	int w = 0;
	int h = 0;
	SDL_QueryTexture(msg_texture, NULL, NULL, &w, &h);
	SDL_Rect dst_rect = { camera.w / 2 - w / 2, y, w, h };

	SDL_RenderCopy(renderer, msg_texture, NULL, &dst_rect);

	SDL_DestroyTexture(msg_texture);
	SDL_FreeSurface(surf);
}

void Engine::drawHugeText(int x, int y, const char * msg) {
	SDL_Color yellow = { 255, 204, 0 }; 
	SDL_Surface* surf = TTF_RenderText_Solid(huge_font, msg, yellow);
	SDL_Texture* msg_texture = SDL_CreateTextureFromSurface(renderer, surf); 

	int w = 0;
	int h = 0;
	SDL_QueryTexture(msg_texture, NULL, NULL, &w, &h);
	SDL_Rect dst_rect = { x, y, w, h };

	SDL_RenderCopy(renderer, msg_texture, NULL, &dst_rect);

	SDL_DestroyTexture(msg_texture);
	SDL_FreeSurface(surf);
}

void Engine::drawCenteredHugeText(int y, const char * msg) {
	SDL_Color yellow = { 255, 204, 0 };
	SDL_Surface* surf = TTF_RenderText_Solid(huge_font, msg, yellow);
	SDL_Texture* msg_texture = SDL_CreateTextureFromSurface(renderer, surf);

	int w = 0;
	int h = 0;
	SDL_QueryTexture(msg_texture, NULL, NULL, &w, &h);
	SDL_Rect dst_rect = { camera.w / 2 - w / 2, y, w, h };

	SDL_RenderCopy(renderer, msg_texture, NULL, &dst_rect);

	SDL_DestroyTexture(msg_texture);
	SDL_FreeSurface(surf);
}

float Engine::getElapsedTime()
{
	return SDL_GetTicks() / 1000.f;
}

bool Engine::isPressed(int keyCode) {
	return keyStates[keyCode];
}

bool Engine::isReleased(int keyCode) {
	return !keyStates[keyCode];
}


void Engine::displayFps() {
	if (fps != 0)
		drawText(10, 10, std::to_string(fps).c_str());
	frames++;
	if (SDL_GetTicks() - lastSecond > 1000) {
		fps = frames;
		frames = 0;
		lastSecond = SDL_GetTicks();
	}
}

Sprite * Sprite::createFromFile(Engine * engine, const char * path) {

	SDL_Surface* surf = SDL_LoadBMP(path);
	if (surf == NULL)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unable to load image %s! SDL_image Error: %s\n", path, SDL_GetError());
		return NULL;
	}

	//Create texture from surface pixels
	SDL_Texture * texture = SDL_CreateTextureFromSurface(engine->getRenderer(), surf);
	if (texture == NULL)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unable to create texture from %s! SDL Error: %s\n", path, SDL_GetError());
		return NULL;
	}

	//Get rid of old loaded surface
	SDL_FreeSurface(surf);
	return new Sprite(engine, engine->getRenderer(), texture);

}

void Sprite::draw(int x, int y, SDL_Rect * clip, SDL_RendererFlip flip)
{
	SDL_Rect rect;

	if (clip == NULL)
		SDL_QueryTexture(texture, NULL, NULL, &(rect.w), &(rect.h));
	else {
		rect.w = clip->w;
		rect.h = clip->h;
	}

	rect.x = int((x - engine->getCamera()->x - rect.w/2) * engine->getZoom());
	rect.y = int((y - engine->getCamera()->y - rect.h/2) * engine->getZoom());


	rect.w = int(engine->getZoom() * rect.w);
	rect.h = int(engine->getZoom() * rect.h);


	//Render texture to screen
	SDL_RenderCopyEx(renderer, texture, clip, &rect, 0, NULL, flip);
}

void Sprite::drawAbsolute(int x, int y, float multiplier)
{
	SDL_Rect rect;

	rect.x = x;
	rect.y = y;

	SDL_QueryTexture(texture, NULL, NULL, &(rect.w), &(rect.h));

	rect.w = int(multiplier*rect.w);
	rect.h = int(multiplier*rect.h);

	SDL_RenderCopy(renderer, texture, NULL, &rect);


}

void Sprite::drawResized(int x, int y, int width, int height) {
	SDL_Rect rect;
	rect.x = x;
	rect.y = y;
	SDL_QueryTexture(texture, NULL, NULL, &(rect.w), &(rect.h));
	if (width >= 0)
		rect.w = width;
	if (height >= 0)
		rect.h = height;

	SDL_RenderCopy(renderer, texture, NULL, &rect);
}

void Sprite::draw(int x, int y, SDL_Rect * clip,SDL_Rect * dest, double angle, SDL_RendererFlip flip)
{

	dest->x = int((x - engine->getCamera()->x - dest->w / 2) * engine->getZoom());
	dest->y = int((y - engine->getCamera()->y - dest->h / 2) * engine->getZoom());


	dest->w = int(engine->getZoom() * dest->w);
	dest->h = int(engine->getZoom() * dest->h);


	//Render texture to screen
	SDL_RenderCopyEx(renderer, texture, clip, dest, angle, NULL, flip);
}


void Sprite::destroy()
{
	if (texture != NULL) {
		SDL_DestroyTexture(texture);
		texture = NULL;
	}
}