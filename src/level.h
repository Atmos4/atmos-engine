#pragma once
#include "parallax.h"
#include "ui.h"
#include "tilemap.h"
#include "rock.h"
#include "game.h"

class Game;

class Level : public Scene
{
	//Core
	Engine * engine;
	Game * game;

	//Various objects
	Player * player;
	std::vector<GameObject *> objs;
	ObjectPool<Rock> rock_pool;

	//TileMap
	int ** tilemap;
	Sprite * tileset;

	//Countdown
	float timeInGame;

	//Music
	Music *loseMusic, *winMusic;

	//Score
	unsigned int score;

	bool game_over;

public:
	virtual ~Level();
	virtual void Create(Engine * engine, Game * game);
	virtual void Receive(Message m);
	virtual void Init();
	virtual void Destroy();

	bool Load(const char * setfile, const char * mapfile);
};