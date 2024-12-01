#pragma once

#include "player.h"
#include "object_pool.h"

class Rock : public GameObject
{
public:
	double radius;
	double angle_speed;

	int nbBounces;

	virtual void Init() {
		GameObject::Init();
		forces.x = 0;
		forces.y = 0;
		aabb = Vector2D(2 * radius, 2 * radius);
		angle = 0;
		angle_speed = 5.0;

		nbBounces = 4;
	}

	virtual void Integrate() {
		GameObject::Integrate();
		angle -= angle_speed;
		if (angle < -180.0)
			angle += 360.0;
	}

	virtual void Receive(Message m) {
		if (m == COLLIDE_BOTTOM) {
			nbBounces--;
			if (nbBounces < 0)
				enabled = false;
			else {
				position.y = old_position.y;
				ApplyForce(Vector2D(0, -100));
			}
		}
		else if (m == COLLIDE_WATER) {
			enabled = false;
		}
	}
};

class RockCollideComponent : public Component
{
	std::vector<Rock * > * rocks;
public:
	virtual void Create(GameObject * player, std::vector<Rock * >* rocks) {
		go = player;
		this->rocks = rocks;
	}

	virtual void Update() {
		for (auto r = rocks->begin(); r != rocks->end(); r++) {
			if ((*r)->enabled) {

				Rock * rock = (*r);
				//Can't make it more efficient ! ;) only work with big enough rocks tho
				if ((go->position.x + go->aabb.x / 2 > rock->position.x - rock->radius) &&
					(go->position.x - go->aabb.x / 2 < rock->position.x + rock->radius) &&
					(go->position.y + go->aabb.y / 2 > rock->position.y - rock->radius) &&
					(go->position.y - go->aabb.y / 2 < rock->position.y + rock->radius))
				{
					
					Vector2D p;
					if (go->position.x + go->aabb.x / 2 < rock->position.x) p.x = go->position.x + go->aabb.x / 2;
					else if (go->position.x - go->aabb.x / 2 > rock->position.x) p.x = go->position.x - go->aabb.x / 2;
					else p.x = rock->position.x;

					if (go->position.y + go->aabb.y / 2 < rock->position.y) p.y = go->position.y + go->aabb.y / 2;
					else if (go->position.y - go->aabb.y / 2 > rock->position.y) p.y = go->position.y - go->aabb.y / 2;
					else p.y = rock->position.y;

					if ((rock->position - p).dotProduct(rock->position - p) < rock->radius * rock->radius) {
						rock->Receive(COLLIDE_BOTTOM);
						go->Receive(HIT);
					}
				}
			}
		}
	}
};

class RockRenderComponent : public RenderComponent
{
	double radius;
public :
	virtual void Create(GameObject * go, Sprite * sprite, double radius)
	{
		RenderComponent::Create(go, sprite);
		this->radius = radius;
	}

	virtual void Render(float i)
	{
		if (go->enabled) {
			Vector2D speed = (go->position - go->old_position);
			Vector2D rPos = go->position + (speed*i) - speed;
			int x = int(rPos.x);
			int y = int(rPos.y);

			SDL_Rect rect = { 0, 0, int(2 * radius), int(2 * radius) };
			if (sprite)
				GetSprite()->draw(x, y, NULL, &rect, go->angle, SDL_FLIP_NONE);
		}
	}

	virtual void Destroy() {
		if (sprite) {
			sprite->destroy();
			delete sprite;
			sprite = NULL;
		}
	}
};


class RockThrowComponent : public Component
{
	float timer;
	float lastThrowTime;
	Engine * engine;
	ObjectPool<Rock> * rock_pool;
	Vector2D initForce;
public:
	virtual void Create(GameObject * go, Engine * engine, float t, ObjectPool<Rock> * pool, Vector2D f)
	{
		Component::Create(go);
		this->rock_pool = pool;
		this->engine = engine;
		timer = t;
		initForce = f;
	}

	virtual void Init() {
		lastThrowTime = engine->getElapsedTime();
	}

	virtual void Update() {
		float time = engine->getElapsedTime();
		if (time > lastThrowTime + timer)
		{
			lastThrowTime = time;
			Rock * rock = rock_pool->FirstAvailable();
			if (rock != NULL) {
				rock->init_position = go->position;
				rock->Init();
				rock->ApplyForce(initForce);
			}
		}
	}
};

class Volcano : public GameObject
{
	ObjectPool<Rock> * pool;
public : 
	virtual void Create(ObjectPool<Rock> * pool)
	{
		this->pool = pool;
	}
};