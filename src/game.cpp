#include "game.h"

void Game::Create(Engine * engine)
{
	SDL_Log("Game::Create");
	GameObject::Create();

	this->engine = engine;

	//This prevents re-writing existing code, although it can be a little bit confusing. See SceneTransition() function
	transitionMessage = LEVEL_EXIT;
	SceneTransition();
}

void Game::Init()
{
	GameObject::Init();
	scene->Init();
	enabled = true;
}

void Game::Update() {
	SceneTransition();

	if (engine->isPressed(SDLK_q))running = false;

	if (enabled) {
		engine->UpdateFrame();
		for (auto in = inputs.begin(); in != inputs.end(); in++)
		{
			(*in)->Update();
		}
		
		for (auto ph = forces.begin(); ph != forces.end(); ph++)
		{
			(*ph)->Update();
		}

		for (auto obj = objects.begin(); obj != objects.end(); obj++)
		{
			if ((*obj)->enabled)
				(*obj)->Integrate();
		}
		
		for (auto cs = constraints.begin(); cs != constraints.end(); cs++)
		{
			(*cs)->Update();
		}
	}
	if (scene->ui)
		scene->ui->Update();
}

void Game::Render(float interpolation) {
	if (running) {
		for (auto gr = graphics.begin(); gr != graphics.end(); gr++)
		{
			if (enabled) //To disable interpolation, comment this 'if' block
				(*gr)->Render(interpolation);
			else
				(*gr)->Render(0.0f);
		}
	}
	engine->displayFps();
	engine->swapBuffers();
	engine->clearWindow();
}

void Game::Receive(Message m) {
	if (m == LEVEL_EXIT || m == LEVEL_START) {
		transitionMessage = m;
	}
	if (m == GAME_EXIT) {
		running = false;
		enabled = false;
	}
}

//This is dirty, but it works :)
void Game::SceneTransition() {
	if (transitionMessage != NONE) {
		enabled = false;

		engine->clearWindow();
		engine->drawCenteredMenuText(engine->getCamera()->h / 2, "LOADING...");
		engine->swapBuffers();

		if (scene)
			this->Clear();

		if (transitionMessage == LEVEL_EXIT) {
			scene = new MainMenu();
			scene->Create();

			MainMenuUIComponent * mmui = new MainMenuUIComponent();
			mmui->Create(engine, this, Sprite::createFromFile(engine, "assets/ui/alpha.bmp"), Sprite::createFromFile(engine, "assets/ui/wood_txr.bmp"));

			scene->ui = mmui;
			scene->AddComponent(mmui);

			ParallaxRenderComponent * background = new ParallaxRenderComponent();
			background->Create(engine, this, "assets/background/jungle/4.bmp", 0.0f, 1.2f);
			scene->AddComponent(background);


			graphics.push_back(background);
			graphics.push_back(mmui);
		}
		else if (transitionMessage == LEVEL_START) {
			Level * level = new Level();
			level->Create(engine, this);

			//To implement more levels, this would need a little refactoring
			level->Load("assets/map/jungle_tileset2.bmp", "assets/map/level1.csv");
			level->AddReceiver(this);
			scene = level;
		}

		transitionMessage = NONE;
		scene->Init();
		enabled = true;
		engine->clearWindow();
	}
	
}

void Game::Clear() {
	this->Destroy();

	inputs.clear();
	forces.clear();
	objects.clear();
	constraints.clear();
	graphics.clear();

	delete scene;
	scene = NULL;
}

void Game::Destroy()
{
	SDL_Log("Game::Destroy");
	for (auto gr = graphics.begin(); gr != graphics.end(); gr++)
	{
			(*gr)->Destroy();
	}
	scene->Destroy();

}

Game::~Game()
{
	if (scene)
		delete scene;
}