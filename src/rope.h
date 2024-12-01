#pragma once
#include "game_object.h"
#include "physics.h"
#include "game.h"


class RopeConstraintComponent : public Component 
{
	GameObject * linked;
	double length;
	bool first;
public:
	virtual ~RopeConstraintComponent() {}

	virtual void Create(GameObject * go, GameObject * go0, double length, bool first);
	virtual void Init();
	virtual void Update();
};

class RopeRenderComponent : public RenderComponent
{
	GameObject * linked;
	double length;
public:
	virtual ~RopeRenderComponent(){}

	virtual void Create( GameObject * go, Sprite * sprite, GameObject * go0, double length);
	virtual void Render(float interpolation);
};

class RopeLink : public GameObject
{
public:
	virtual ~RopeLink() {}

};

class Rope : public GameObject
{
	Engine * engine;

	Sprite * rope_sprite;

	RopeLink ** rope;
	int size;
	double length;

public :

	virtual ~Rope();
	virtual void Create(Engine * engine, int size, double length, const char * rope_file);
	virtual void Destroy();
	virtual void Init();


	virtual void Receive(Message m) {
		for (auto it = components.begin(); it != components.end(); it++)
			(*it)->Receive(m);
	}

	double GetRopeLength() {
		return length * size;
	}

	RopeLink** GetRope() { return rope; }
	double Length() { return length; }
	int Size() { return size; }
	Sprite * GetSprite() { return rope_sprite; }

};

class RopeCollider : public Component

{
	int linked;
	bool canSpace;
	bool enabled;

	Engine * engine;
	Player * player;
	Rope * base;
	RopeLink** rope;
	int size;
	double length;

public:

	virtual void Create(Engine * engine,Player * go, Rope * rope_base, RopeLink ** rope_, int size, double length);
	virtual void Update();
	virtual void Receive(Message m) {
		if (m == ROPE_RESET && !enabled) {
			SDL_Log("Rope reset");
			enabled = true;
		}
	}
};