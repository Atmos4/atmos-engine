#include "level.h"
#include "object_factory.h"
#include <fstream>
#include <iterator>
#include <sstream>
#include <string>


Level::~Level()
{
	for (auto obj = objs.begin(); obj != objs.end(); obj++)
		delete (*obj);

	delete winMusic;
	delete loseMusic;
}

void Level::Create(Engine * engine_, Game * g)
{
	GameObject::Create();
	this->engine = engine_;
	this->game = g;

	rock_pool.Create(10);

	//TODO : move all of this in a load() function

	engine->setLevelDims(800, 400);

	//Music
	loseMusic = Music::createFromFile("assets/music/lose.wav");
	winMusic = Music::createFromFile("assets/music/win.wav");

}

bool Level::Load(const char * setfile, const char * mapfile) {

	//Init the tileset
	tileset = Sprite::createFromFile(engine, setfile);
	if (tileset == NULL)return false;

	//Create parallax background
	ParallaxRenderComponent * background1 = new ParallaxRenderComponent();
	background1->Create(engine, this, "assets/background/jungle/1.bmp", 0.0f,1.4f);
	ParallaxRenderComponent * background2 = new ParallaxRenderComponent();
	background2->Create(engine, this, "assets/background/jungle/2.bmp", 0.5f, 1.4f);
	ParallaxRenderComponent * background3 = new ParallaxRenderComponent();
	background3->Create(engine, this, "assets/background/jungle/3.bmp", 1.0f, 1.4f);
	ParallaxRenderComponent * background4 = new ParallaxRenderComponent();
	background4->Create(engine, this, "assets/background/jungle/4.bmp", 1.5f, 1.4f);
	ParallaxRenderComponent * background5 = new ParallaxRenderComponent();
	background5->Create(engine, this, "assets/background/jungle/5.bmp", 2.0f, 1.4f);

	AddComponent(background1);
	AddComponent(background2);
	AddComponent(background3);
	AddComponent(background4);
	AddComponent(background5);

	game->graphics.push_back(background1);
	game->graphics.push_back(background2);
	game->graphics.push_back(background3);
	game->graphics.push_back(background4);
	game->graphics.push_back(background5);


	//Create player (input, physics, graphics, sounds, ui)
	player = new Player();
	PlayerBehaviourComponent * player_behaviour = new PlayerBehaviourComponent();
	player_behaviour->Create(engine, player);
	GravityComponent * gravity = new GravityComponent();
	gravity->Create(player);
	FluidFrictionComponent * fluids = new FluidFrictionComponent();
	fluids->Create(player);
	InBoundsComponent * inbounds = new InBoundsComponent();
	inbounds->Create(engine, player);
	PlayerGraphicsComponent * player_graphics = new PlayerGraphicsComponent();
	player_graphics->Create(engine, player, Sprite::createFromFile(engine, "assets/sprites/player_sheet.bmp"));
	PlayerHitSoundComponent * hit_sound = new PlayerHitSoundComponent();
	hit_sound->Create(Sound::createFromFile("assets/music/hit.wav"));
	PlayerJumpSoundComponent * jump_sound = new PlayerJumpSoundComponent();
	jump_sound->Create(Sound::createFromFile("assets/music/jump.wav"));
	PlayerCheckpointSoundComponent * checkpoint_sound = new PlayerCheckpointSoundComponent();
	checkpoint_sound->Create(Sound::createFromFile("assets/music/checkpoint.wav"));
	WaterCountdownComponent * water = new WaterCountdownComponent();
	water->Create(engine,player, Sprite::createFromFile(engine, "assets/ui/breath_bar.bmp"), Sprite::createFromFile(engine, "assets/ui/bar.bmp"));

	player->Create();
	player->AddComponent(player_behaviour);
	player->AddComponent(gravity);
	player->AddComponent(fluids);
	player->AddComponent(inbounds);
	player->AddComponent(player_graphics);
	player->AddComponent(water);
	player->AddComponent(hit_sound);
	player->AddComponent(jump_sound);
	player->AddComponent(checkpoint_sound);
	player->AddReceiver(this);

	game->inputs.push_back(player_behaviour);
	game->forces.push_back(gravity);
	game->forces.push_back(fluids);
	game->constraints.push_back(inbounds);
	game->constraints.push_back(water);
	game->graphics.push_back(player_graphics);
	game->graphics.push_back(water);
	game->objects.push_back(player);

	objs.push_back(player);

	// Create Tile Map
	std::ifstream file(mapfile);
	std::string line;
	int count = 0, row = 0, col = 0;

	while (std::getline(file, line)) {
		col = 0;
		std::string temp;
		std::stringstream ss(line);
		while (std::getline(ss, temp, ',')) {

			int id = std::stoi(temp);
			if (id >= 0) count++;
			col++;
		}
		row++;
	}
	SDL_Log("count: %d",count);
	SDL_Log("col: %d",col);
	SDL_Log("row: %d",row);
	file.clear();
	file.seekg(0, file.beg);

	//Init the tilemap
	tilemap = new int*[col];
	for (int c = 0; c < col; c++) {
		tilemap[c] = new int[row];
		for (int r = 0; r < row; r++) {
			tilemap[c][r] = -1;
		}
	}

	engine->setLevelDims(col * TILE_WIDTH, row * TILE_HEIGHT);
	row = 0;
	col = 0;
	count = 0;
	while (std::getline(file, line)) {
		col = 0;
		std::string temp;
		std::stringstream ss(line);
		while (std::getline(ss, temp, ',')) {
			int id = std::stoi(temp);
			if (id == 0) {
				player->init_position = Vector2D(col * TILE_WIDTH, row * TILE_HEIGHT);
				tilemap[col][row] = id;
			}
			else if (id == 13) {
				GameObject * rope = ObjectFactory::CreateRope(engine, game, player);
				rope->init_position = Vector2D(col * TILE_WIDTH + TILE_WIDTH / 2, row * TILE_HEIGHT + TILE_WIDTH / 2);
				objs.push_back(rope);
			}
			else if (id == 14) {
				GameObject * skeleton = ObjectFactory::CreateSkeleton(engine, game, player);
				skeleton->init_position = Vector2D(col * TILE_WIDTH + TILE_WIDTH / 2, (row+1) * TILE_HEIGHT - skeleton->aabb.y / 2);
				objs.push_back(skeleton);

			}
			else if (id == 669) {
				GameObject * volcano = ObjectFactory::CreateVolcano(engine, game, player, Vector2D(-40, -70), &rock_pool);
				volcano->init_position = Vector2D(col * TILE_WIDTH + TILE_WIDTH / 2, row * TILE_HEIGHT - TILE_WIDTH / 2);
				objs.push_back(volcano);
				tilemap[col][row] = id;
			}
			else if (id >= 2) {
				if (id == 12) {
					GameObject * fish = ObjectFactory::CreateFish(engine, game, player);
					fish->init_position = Vector2D(col * TILE_WIDTH + TILE_WIDTH / 2, (row + 1) * TILE_HEIGHT - fish->aabb.y / 2);
					objs.push_back(fish);
					id = 3;
				}

				tilemap[col][row] = id;
			}
			col++;
		}
		row++;
	}

	TileMapComponent * tmc = new TileMapComponent();
	tmc->Create(engine, player, tileset, tilemap, col, row);
	tmc->setCollideObjs(&rock_pool.pool);
	player->AddComponent(tmc);

	game->constraints.push_back(tmc);
	game->graphics.push_back(tmc);


	//UI
	GameUIComponent * gui = new GameUIComponent();
	gui->Create(engine, this, Sprite::createFromFile(engine, "assets/ui/heart.bmp"),Sprite::createFromFile(engine, "assets/ui/alpha.bmp"), player);
	this->ui = gui;
	this->AddComponent(ui);

	game->graphics.push_back(ui);


	return true;
}


void Level::Init()
{
	for (auto obj = objs.begin(); obj != objs.end(); obj++)
		(*obj)->Init();
	GameObject::Init();
}

void Level::Destroy()
{
	GameObject::Destroy();
	for (auto obj = objs.begin(); obj != objs.end(); obj++)
		(*obj)->Destroy();

	if (tileset != NULL) {
		tileset->destroy();
		delete tileset;
		tileset = NULL;
	}

	loseMusic->destroy();
	winMusic->destroy();
}

void Level::Receive(Message m)
{
	if (m == LEVEL_COMPLETE) {
		for (auto c = components.begin(); c != components.end(); c++)
			(*c)->Receive(m);
		winMusic->play(1);
		game->enabled = false;
		game_over = true;
	}
	if (m == GAME_OVER) {
		for (auto c = components.begin(); c != components.end(); c++)
			(*c)->Receive(m);
		loseMusic->play(1);
		game->enabled = false;
		game_over = true;
	}
	if (m == LEVEL_RESUME) {
		if (game_over)
			this->Init();
		game->enabled = true;
		game_over = false;
	}
	if (m == LEVEL_PAUSE) {
		game->enabled = false;
	}
	if (m == LEVEL_EXIT) {
		game->Receive(m);
	}
}