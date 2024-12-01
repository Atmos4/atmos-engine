#pragma once

#include "player.h"

class UIComponent : public RenderComponent
{
protected:
	Engine * engine;
	bool click, selected;
public:
	virtual void Create(Engine * e, GameObject * go, Sprite * s)
	{
		RenderComponent::Create(go, s);
		this->engine = e;
		click = false;
		selected = false;
	}

	virtual void Update() {
		if (!click)
			if (engine->isReleased(SDLK_RETURN) && engine->isReleased(SDLK_UP) && engine->isReleased(SDLK_DOWN) && engine->isReleased(SDLK_ESCAPE))
				click = true;
	}

	virtual void Render(float i) = 0;
};

class GameUIComponent : public UIComponent
{
	Player * player;
	Sprite * alpha;

	const char * title;

	bool isPaused;

public:
	virtual void Create(Engine * engine, GameObject * go, Sprite * s,Sprite * s2, Player * p)
	{
		UIComponent::Create(engine,go, s);
		alpha = s2;
		this->player = p;
		isPaused = false;
		title = "PAUSE";
	}

	virtual void Update()
	{
		UIComponent::Update();

		//GODMODE
		if (engine->isPressed(SDLK_g)) player->god = true;
		else if (engine->isPressed(SDLK_n))player->god = false;


		if (engine->isPressed(SDLK_ESCAPE) && click) {
			click = false;
			if (isPaused && !strcmp(title,"PAUSE")) {
				isPaused = false;
				go->Receive(LEVEL_RESUME);
			}
			else {
				go->Receive(LEVEL_PAUSE);
				isPaused = true;
			}

		}
		if (isPaused && click) {
			if (engine->isPressed(SDLK_UP)) {
				click = false;
				selected = false;
			}
			if (engine->isPressed(SDLK_DOWN)) {
				click = false;
				selected = true;
			}
			if (engine->isPressed(SDLK_RETURN)) {
				click = false;
				isPaused = false;
				if (selected)
					go->Receive(LEVEL_EXIT);
				else {
					go->Receive(LEVEL_RESUME);
				}
				title = "PAUSE";
			}
		}


	}

	virtual void Render(float i) {
		for (int i = 0; i < player->lives; i++) {
			sprite->drawAbsolute(10 + i * 20, 30, 0.5);
		}
		if (player->god)engine->drawCenteredMenuText(60, "GODMODE");
		if (isPaused) {
			if (alpha)
				alpha->drawResized(0, 0, engine->getCamera()->w, engine->getCamera()->h);
			engine->drawCenteredMenuText(110, title);
			if (!strcmp(title,"PAUSE"))
				engine->drawCenteredMenuText(engine->getCamera()->h / 2 - 20, "RESUME");
			else
				engine->drawCenteredMenuText(engine->getCamera()->h / 2 - 20, "RESTART");

			engine->drawCenteredMenuText(engine->getCamera()->h / 2 + 20, "EXIT");
			engine->drawCenteredMenuText(engine->getCamera()->h / 2 + (selected ? 20 : -20), ">                       <");
		}
	}

	virtual void Destroy() {


		if (sprite != NULL) {
			sprite->destroy();
			delete sprite;
			sprite = NULL;
		}

		if (alpha) {
			alpha->destroy();
			delete alpha;
			alpha = NULL;
		}
	}

	virtual void Receive(Message m) {
		if (m == LEVEL_COMPLETE) {
			title = "YOU WIN !";
			isPaused = true;
		}
		if (m == GAME_OVER) {
			title = "GAME OVER !";
			isPaused = true;
		}
	}
};

class MainMenuUIComponent : public UIComponent
{
	Sprite * wood;
public:
	virtual void Create(Engine * engine, GameObject * go, Sprite * s, Sprite * w)
	{
		UIComponent::Create(engine, go, s);
		wood = w;
	}

	virtual void Update()
	{
		UIComponent::Update();

		if (click) {
			if (engine->isPressed(SDLK_UP)) {
				selected = false;
				click = false;
			}
			if (engine->isPressed(SDLK_DOWN)) {
				selected = true;
				click = false;
			}
			if (engine->isPressed(SDLK_RETURN)) {
				click = false;
				if (selected)go->Receive(GAME_EXIT);
				else go->Receive(LEVEL_START);
			}
		}
	}

	virtual void Render(float i) {
		sprite->drawResized(0, 0, engine->getCamera()->w, engine->getCamera()->h);

		wood->drawResized(engine->getCamera()->w / 2 - 165, 85, 120, 35);
		wood->drawResized(engine->getCamera()->w / 2 - 140, 115, 270, 110);
		wood->drawResized(engine->getCamera()->w / 2 - 15, 225, 30, 60);

		engine->drawMenuText(engine->getCamera()->w/2 - 160, 90, "JUNGLE");
		engine->drawCenteredHugeText(125, "HUNT");

		engine->drawCenteredMenuText(engine->getCamera()->h / 2 + 100, "START");
		engine->drawCenteredMenuText(engine->getCamera()->h / 2 + 140, "EXIT");
		engine->drawCenteredMenuText(engine->getCamera()->h / 2 + (selected ? 140 : 100), ">                       <");
	}

	virtual void Destroy() {
		if (sprite) {
			sprite->destroy();
			delete sprite;
			sprite = NULL;
		}
		if (wood) {
			wood->destroy();
			delete wood;
			wood = NULL;
		}
	}
};

class Scene : public GameObject
{

public:
	UIComponent * ui;

};

class MainMenu : public Scene
{
	Music * mainMusic;
public:
	virtual void Create() {
		mainMusic = Music::createFromFile("assets/music/loop.wav");
	}

	virtual void Init() {
		mainMusic->play(-1);
	}

	virtual void Destroy() {
		GameObject::Destroy();
		mainMusic->destroy();
	}

	virtual ~MainMenu() {
		delete mainMusic;
	}
};

//Not necessary right now, but can be useful if multithreaded
/*class LoadingUIComponent : public UIComponent
{
	AtmostEngine * engine;
public :
	virtual void Create(AtmostEngine * e) { engine = e; }
	virtual void Update() {}
	virtual void Render(float i) {
		engine->drawCenteredMenuText(engine->getCamera()->h / 2, "LOADING...");
	}
};*/