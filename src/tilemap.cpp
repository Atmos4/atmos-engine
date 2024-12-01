#include "tilemap.h"

void TileMapComponent::Init() {
	for (int j = 0; j < height; j++) {
		for (int i = 0; i < width; i++) {
			if (tilemap[i][j] == 0 || tilemap[i][j] == 1)
				tilemap[i][j] = 2;
		}
	}
}

void TileMapComponent::Update()
{
	Collision(go, true);
	if (objs != NULL)
	{
		for (auto i = objs->begin(); i != objs->end(); i++) {
			if ((*i)->enabled)
				Collision(*i, false);
		}
	}
}

void TileMapComponent::Collision(GameObject * go, bool resolve)
{
	Vector2D newPos = go->position;

	Vector2D topleft = go->position - go->aabb / 2;
	Vector2D bottomright = go->position + go->aabb / 2;

	int x1 = int(floor(topleft.x / TILE_WIDTH));
	int x2 = int(floor(bottomright.x / TILE_WIDTH));
	int y1 = int(floor(topleft.y / TILE_HEIGHT));
	int y2 = int(floor(bottomright.y / TILE_HEIGHT));

	if (x1 < 0)x1 = 0;
	if (x2 >= width)x2 = width - 1;
	if (y1 < 0)y1 = 0;
	if (y2 >= height)y2 = height - 1;

	bool no_collision = true;

	for (int j = y1; j <= y2; j++) {
		for (int i = x1; i <= x2; i++) {
			int id = tilemap[i][j];
			if (id >= 0) {
				Vector2D p1(i * TILE_WIDTH, j * TILE_HEIGHT);
				Vector2D p2((i + 1) * TILE_WIDTH, (j + 1) * TILE_HEIGHT);

				if (Collides(go->position, go->aabb, p1, p2)) {
					no_collision = false;
					if (id == 2) {
						player->respawnPoint = p1;
						tilemap[i][j] = 0;
						player->Receive(CHECKPOINT);
					}
					else if (id == 3) {
						go->Receive(COLLIDE_WATER);
					}
					else if (id >= 4 && id <= 9){
						if (id == 4) {
							if (engine->isPressed(SDLK_UP))
								go->ApplyForce(Vector2D(0, -20));
						}
						go->Receive(COLLIDE_SURFACE);
						
					}
					else if (id == 663 || id == 664 || id == 702 || id == 703) {
						go->Send(LEVEL_COMPLETE);
						return;
					}
					else if (id >= 704 && id <= 706) {
						go->Receive(HIT);
					}
					else if (id > 14) {
						if (Collides(newPos, go->aabb, p1, p2)) {

							if (Collides(Vector2D(go->old_position.x, go->position.y), go->aabb, p1, p2))
							{
								if (go->position.y > go->old_position.y)
								{
									newPos.y = p1.y - go->aabb.y / 2;
									go->Receive(COLLIDE_BOTTOM);
								}
								else
									newPos.y = p2.y + go->aabb.y / 2;
							}
							if (Collides(Vector2D(go->position.x, go->old_position.y), go->aabb, p1, p2)) {
								if (go->position.x > go->old_position.x)
									newPos.x = p1.x - go->aabb.x / 2;
								else
									newPos.x = p2.x + go->aabb.x / 2;
							}//Eventually, add more messages
						}
					}
				}
			}
		}
	}
	if (resolve)
		go->position = newPos;
	if (no_collision)
		go->Receive(COLLIDE_NONE);
}

void TileMapComponent::Render(float i) 
{
	SDL_Rect clip = { 0,0,TILE_WIDTH, TILE_HEIGHT };
	for (int j = 0; j < height; j++) {
		for (int i = 0; i < width; i++) {
			if (tilemap[i][j] >= 0) {
				if (tilemap[i][j] == 0) {
					if (engine->GetFrame() % 4 >= 2)
						tilemap[i][j] = 1;
				}

				else if (tilemap[i][j] == 1) {
					if (engine->GetFrame() % 4 < 2)
						tilemap[i][j] = 0;
				}
				if (sprite) {
					clip.x = (tilemap[i][j] % TILESET_NROWS) * TILE_WIDTH;
					clip.y = int(std::floor(tilemap[i][j] / TILESET_NROWS) * TILE_HEIGHT);
					sprite->draw((i*TILE_WIDTH + TILE_WIDTH / 2), (j*TILE_HEIGHT + TILE_HEIGHT / 2), &clip, SDL_FLIP_NONE);
				}
			}
		}
	}
}