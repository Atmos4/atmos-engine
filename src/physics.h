#pragma once
#include "game_object.h"


const double GRAVITY = 9.8; // Units ??
const double FRICTION = 0.5;

//Model for gravity force, would probably need some tweaks
class GravityComponent : public Component
{
	bool enabled = true;
public:

	virtual void Update() {
		if (enabled)
			go->ApplyForce(Vector2D(0.0,GRAVITY) * go->mass);
	}

	virtual void Receive(Message m)
	{
		switch (m) {
		case COLLIDE_WATER:
			enabled = false;
			break;
		case COLLIDE_NONE:
			enabled = true;
			break;
		default:
			break;
		}

	}
};

//Modelize air friction force
class FluidFrictionComponent : public Component
{
	double fluid_coef = 1;
public:
	virtual void Update() {
		go->ApplyForce((go->position - go->old_position)* (-FRICTION*fluid_coef));
	}

	virtual void Receive(Message m)
	{
		switch (m) {
		case COLLIDE_WATER:
			fluid_coef = 10;
			break;
		case COLLIDE_SURFACE:
			fluid_coef = 10;
			break;
		case COLLIDE_NONE:
			fluid_coef = 1;
			break;
		default:
			break;
		}

	}
};


//Not necessary, will have to be remove eventually
class InBoundsComponent : public Component
{
	Engine * engine;
public:
	virtual void Create(Engine * engine_, GameObject * go) {
		Component::Create(go);
		this->engine = engine_;
	}

	virtual void Update()
	{
		if (go->position.y > engine->getLevelHeight() - go->aabb.y / 2) {
			go->position.y = engine->getLevelHeight() - go->aabb.y / 2;
			go->Receive(COLLIDE_BOTTOM);
		}
		else go->Receive(COLLIDE_NONE);

		if (go->position.x > (engine->getLevelWidth() - go->aabb.x / 2)) {
			go->position.x = engine->getLevelWidth() - go->aabb.x / 2;
		}
		else if (go->position.x < go->aabb.y / 2) {
			go->position.x = go->aabb.y / 2;
		}
	}
};