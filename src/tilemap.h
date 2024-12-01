#pragma once
#include "game_object.h"
#include "player.h"
#include "rock.h"

const int TILE_WIDTH = 16;
const int TILE_HEIGHT = 16;
const int TILESET_NROWS = 39;

class TileMapComponent : public RenderComponent
{
private :
	Engine * engine;
	Player * player;
	int** tilemap;
	int width, height;
	std::vector<Rock *> * objs;

	bool Collides(const Vector2D& c1, const Vector2D& dm1, const Vector2D& c2, const Vector2D& dm2)
	{
		return ((c1.x + dm1.x / 2 > c2.x) &&
			(c1.x - dm1.x / 2 < dm2.x) &&
			(c1.y + dm1.y / 2 > c2.y) &&
			(c1.y - dm1.y / 2 < dm2.y));
	}
public:

	virtual ~TileMapComponent() {
		for (int a = 0; a < width; a++) {
			delete[] tilemap[a];
		}
		delete[] tilemap;
	}

	virtual void Create(Engine * engine, Player * go, Sprite * sprite, int ** tm, int width, int height)
	{
		RenderComponent::Create(go, sprite);
		player = go;
		this->engine = engine;
		tilemap = tm;
		this->width = width;
		this->height = height;
	}

	void setCollideObjs(std::vector<Rock *> * objs)
	{
		this->objs = objs;
	}

	void Collision(GameObject *, bool resolve);

	virtual void Init();

	virtual void Update();


	virtual void Render(float);

};