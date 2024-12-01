#pragma once

#include <set>
#include "engine.h"

class GameObject;

enum Message {
	NONE, COLLIDE_NONE, COLLIDE_WATER, COLLIDE_SURFACE, COLLIDE_BOTTOM,
	ROPE_LINKED, ROPE_FREE, ROPE_RESET, CHECKPOINT, JUMP, DEAD, HIT,
	LEVEL_PAUSE,LEVEL_RESUME, LEVEL_EXIT,	LEVEL_COMPLETE, LEVEL_START, 
	GAME_MENU, GAME_OVER, GAME_EXIT
};


class Component
{
protected:
	GameObject * go;		// the game object this component is part of

public:
	virtual ~Component() {}

	virtual void Create(GameObject * go) {
		this->go = go;
	}

	virtual void Init() {}
	virtual void Update() = 0;
	virtual void Receive(Message m) {}
	virtual void Destroy() {}
};


class RenderComponent : public Component
{
protected:
	Sprite* sprite;


	void DestroySprite(Sprite * s) {
		if (s != NULL) {
			s->destroy();
			delete s;
		}
	}

public:

	virtual void Create(GameObject * go, Sprite * sprite) {
		this->go = go, this->sprite = sprite;
	}
	virtual void Update(){}
	virtual void Render(float interpolator)=0;

	Sprite * GetSprite() { return sprite; }

};

class SoundComponent : public Component
{
protected:
	Sound * sound;
public:
	virtual void Create(Sound * s) {
		sound = s;
	}

	virtual void Update() {}

	virtual void Destroy() {
		if (sound) {
			sound->destroy();
			delete sound;
			sound = NULL;
		}
	}
};

