#pragma once

#include "game_object.h"


class PatrolBehaviorComponent : public Component
{
	Vector2D direction, origin;
	double speed;
public:
	virtual void Create(GameObject * go, Vector2D direction)
	{
		Component::Create(go);
		this->direction = direction;
		speed = rand() % 10 + 21;
	}

	virtual void Init()
	{
		origin = go->position;
	}

	virtual void Update() {
		Vector2D v = go->position - origin;
		if (v.dotProduct(v) > direction.dotProduct(direction))direction= direction* -1;
		go->old_position = go->position - direction / speed;
	}
};


class SkeletonRenderComponent : public RenderComponent
{
	Engine * engine;
	SDL_Rect clips[13];
	int animFrame;

public:
	virtual void Create(Engine * engine, GameObject * go, Sprite * sprite)
	{
		RenderComponent::Create(go, sprite);
		this->engine = engine;
		for (int i = 0; i < 13; i++) {
			clips[i].x = i * 22;
			clips[i].y = 0;
			clips[i].w = 22;
			clips[i].h = 33;
		}

	}
	virtual void Render(float i)
	{
		if (!go->enabled)
			return;

		Vector2D speed = (go->position - go->old_position);
		Vector2D rPos = go->position + (speed*i) - speed;
		int x = int(rPos.x);
		int y = int(rPos.y);

		int currentFrame = engine->GetFrame();

		if (sprite) {
			if ((go->position - go->old_position).x > 0)
				sprite->draw(x, y, &clips[currentFrame%13], SDL_FLIP_NONE);
			else
				sprite->draw(x, y, &clips[currentFrame%13], SDL_FLIP_HORIZONTAL);
		}
	}

	virtual void Destroy()
	{
		if (sprite != NULL) {
			sprite->destroy();
			delete sprite;
			sprite = NULL;
		}
	}
};

class FishRenderComponent : public RenderComponent
{
	Engine * engine;
	SDL_Rect clips[12];
	int currentFrame;
	double lastFrameTime;

public:
	virtual void Create(Engine * engine, GameObject * go, Sprite * sprite)
	{
		RenderComponent::Create(go, sprite);
		this->engine = engine;
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 3; j++) {
				clips[j * 4 + i].x = i * 35;
				clips[j * 4 + i].y = j * 23;
				clips[j * 4 + i].w = 35;
				clips[j * 4 + i].h = 23;
			}
		}

	}
	virtual void Render(float i)
	{
		if (engine->getElapsedTime() - lastFrameTime > 0.05f) {
			currentFrame++;
			lastFrameTime = engine->getElapsedTime();
			if (currentFrame >= 12) {
				currentFrame = 0;
			}
		}

		if (sprite) {
			if ((go->position - go->old_position).x >= 0)
				sprite->draw(int(go->position.x), int(go->position.y), &clips[currentFrame], SDL_FLIP_NONE);
			else
				sprite->draw(int(go->position.x), int(go->position.y), &clips[currentFrame], SDL_FLIP_HORIZONTAL);
		}
	}

	virtual void Destroy()
	{
		if (sprite != NULL) {
			sprite->destroy();
			delete sprite;
			sprite = NULL;
		}
	}
};


class EnemyCollideComponent : public Component
{
	Player* player;

public:
	virtual void Create(GameObject * go, Player * player) {
		Component::Create(go);
		this->player = player;
	}
	virtual void Update()
	{
		if ((go->position.x + go->aabb.x / 2 > player->position.x - player->aabb.x / 2) &&
			(go->position.x - go->aabb.x / 2 < player->position.x + player->aabb.x / 2) &&
			(go->position.y + go->aabb.y / 2 > player->position.y - player->aabb.y / 2) &&
			(go->position.y - go->aabb.y / 2 < player->position.y + player->aabb.y / 2))
		{
			player->Receive(HIT);
		}
	}
};

