#pragma once
#include "game_object.h"

const double PLAYER_SPEED = 8.0;
const int NUM_LIVES = 3;

const int PLAYER_WIDTH = 20;
const int PLAYER_HEIGHT = 30;

const double H_SPEED_CAP = 6.0;
const double V_SPEED_CAP = 20.0;

const float MAX_TIME_UNDERWATER = 3.0f;


enum State{GROUND, AIR, WATER, SURFACE, ROPE};

class PlayerBehaviourComponent : public Component {
public:
	virtual ~PlayerBehaviourComponent() {}

	virtual void Create(Engine* engine, GameObject * go);

	virtual void Init();
	virtual void Update();
	virtual void Receive(Message m);

private:
	Engine * engine;
	State state;
	double speed;
	bool is_dying;


};

class WaterCountdownComponent : public RenderComponent
{
	Engine * engine;
	float timeEnteredWater, timeUnderWater;
	float maxTime;
	Sprite * bar;


public :
	virtual void Create(Engine * engine, GameObject * go, Sprite * sprite, Sprite * bar) {
		RenderComponent::Create(go, sprite);
		this->bar = bar;
		this->engine = engine;
		timeEnteredWater = 0;
		timeUnderWater = 0;
		maxTime = MAX_TIME_UNDERWATER;
	}

	virtual void Update() {
		if (timeEnteredWater != 0) {
			timeUnderWater = engine->getElapsedTime();
			if (timeUnderWater - timeEnteredWater > maxTime)
			{
				go->Receive(HIT);
			}
		}
	}

	virtual void Render(float i) {
		if (bar && sprite) {
			if (timeEnteredWater != 0) {
				if (timeUnderWater - timeEnteredWater < maxTime)
					sprite->drawResized(engine->getCamera()->w/2 - 39, 6, int(100 * (maxTime - timeUnderWater + timeEnteredWater) / maxTime), -1);
				bar->drawResized(engine->getCamera()->w / 2 - 40, 5, 100, -1);
			}
		}
	}

	virtual void Destroy() {
		if (sprite) {
			sprite->destroy();
			delete sprite;
			sprite = NULL;
		}
		if (bar) {
			bar->destroy();
			delete bar;
			bar = NULL;
		}
	}

	virtual void Receive(Message m) {
		if (m== COLLIDE_SURFACE || m == DEAD){
			if (timeEnteredWater != 0)
				timeEnteredWater = 0;
		}
		else if (m == COLLIDE_WATER) {
			if (timeEnteredWater == 0) {
				timeEnteredWater = engine->getElapsedTime();
				timeUnderWater = timeEnteredWater;
			}
		}
	}
};


class PlayerGraphicsComponent : public RenderComponent
{
	Engine * engine;
public:
	virtual ~PlayerGraphicsComponent() {}

	virtual void Create(Engine * engine, GameObject * go, Sprite * sprite);
	virtual void Render(float interpolation);
	virtual void Receive(Message m);
	virtual void Destroy();


private:
	SDL_Rect clips[100];
	SDL_RendererFlip flip;

	State state;

	int die_anim;
};

class PlayerHitSoundComponent : public SoundComponent {
	bool dying = false;
public:
	virtual void Receive(Message m) {
		if (m == HIT && !dying) {
			dying = true;
			sound->play();
		}
		if (m == DEAD) {
			dying = false;
		}

	}
};

class PlayerJumpSoundComponent : public SoundComponent {
public:
	virtual void Receive(Message m) {
		if (m == JUMP) {
			sound->play();
		}
	}
};

class PlayerCheckpointSoundComponent : public SoundComponent {
public:
	virtual void Receive(Message m) {
		if (m == CHECKPOINT) {
			sound->play();
		}
	}
};

class Player : public GameObject {

public:
	int lives;

	bool linked;

	Vector2D respawnPoint;

	bool god = false;



	virtual ~Player();
	virtual void Init();
	virtual void Receive(Message m);

	void RemoveLife() {
		lives--;
		SDL_Log("remaining lives %d", lives);
	}
};