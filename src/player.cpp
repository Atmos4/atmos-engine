#include "player.h"

void PlayerBehaviourComponent::Create(Engine* engine, GameObject * go)
{
	Component::Create(go);
	this->engine = engine;
}

void PlayerBehaviourComponent::Init()
{
	speed = PLAYER_SPEED;
	state = GROUND;

	go->aabb.x = PLAYER_WIDTH;
	go->aabb.y = PLAYER_HEIGHT;

	is_dying = false;
}

void PlayerBehaviourComponent::Update()
{

	if (is_dying) go->old_position = go->position;
	else{
		double factor;
		if (state == WATER || state == SURFACE) factor = 0.5;
		else factor = 1.0;

		Vector2D new_speed = go->position - go->old_position;

		if (engine->isPressed(SDLK_UP)) {
			if (state == GROUND) {
				new_speed.y -= speed * factor*1.5;
				state = AIR;
				go->Receive(JUMP);
			}
			else if (state == WATER)
				new_speed.y -= speed * factor;
		}
		else if (engine->isPressed(SDLK_DOWN)) {
			if (state == WATER || state == SURFACE)
				new_speed.y += speed * factor;
		}

		if (engine->isPressed(SDLK_RIGHT))
			new_speed.x += speed * factor;
		else if (engine->isPressed(SDLK_LEFT))
			new_speed.x += -speed * factor;

		if (engine->isReleased(SDLK_RIGHT) && engine->isReleased(SDLK_LEFT))new_speed.x = 0;

		if (new_speed.x > H_SPEED_CAP) new_speed.x = H_SPEED_CAP;
		if (new_speed.y > V_SPEED_CAP) new_speed.y = V_SPEED_CAP;
		if (new_speed.x < -H_SPEED_CAP) new_speed.x = -H_SPEED_CAP;
		if (new_speed.y < -V_SPEED_CAP) new_speed.y = -V_SPEED_CAP;

		go->old_position = go->position - new_speed;
	}

}

void PlayerBehaviourComponent::Receive(Message m)
{
	if (state != ROPE) {
		switch (m) {
		case COLLIDE_BOTTOM:
			if (state!=WATER)
				state = GROUND;
			break;
		case COLLIDE_WATER:
			state = WATER;
			break;
		case COLLIDE_SURFACE:
			state = SURFACE;
			break;
		case ROPE_LINKED:
			state = ROPE;
			break;
		case COLLIDE_NONE:
			if (state != ROPE)
				state = AIR;
			break;
		case HIT:
			is_dying = true;
			break;
		case DEAD:
			is_dying = false;
			break;
		default:
			break;
		}
	}
	if (m == ROPE_FREE) {
		state = AIR;
	}
}

void PlayerGraphicsComponent::Create(Engine * engine, GameObject * go, Sprite * sprite_)
{
	RenderComponent::Create( go,sprite_);
	this->engine = engine;
	int w = GetSprite()->getWidth()/8;
	int h = GetSprite()->getHeight()/12;
	for (int i = 0; i < 12; i++) {
		for (int j = 0; j < 8; j++) {
			clips[i*8 + j].x = w * j;
			clips[i*8 + j].y = h * i;
			clips[i*8 + j].w = w;
			clips[i*8 + j].h = h;
		}
	}
	flip = SDL_FLIP_NONE;
	state = GROUND;

	die_anim = -1;
}

void PlayerGraphicsComponent::Render(float interpolation)
{

	int currentFrame = engine->GetFrame();

	if (!go->enabled)
		return;
	Vector2D speed = (go->position - go->old_position);
	Vector2D rPos = go->position + (speed*interpolation) - speed;
	int x = int(rPos.x);
	int y = int(rPos.y);

	engine->setCameraPos(x,y);

	if (speed.x > 0) {
		flip = SDL_FLIP_NONE;
	}
	else if (speed.x < 0) {
		flip = SDL_FLIP_HORIZONTAL;
	}



	if (GetSprite()) {
		if (die_anim >= 0) {
			if (((120 + currentFrame-die_anim)/2)%12 < 6)
				GetSprite()->draw(x, y, &clips[((120 + currentFrame - die_anim)/2) % 6 + 63], flip);
			else {
				go->Receive(DEAD);
				die_anim = -1;
			}

		}
		else if (state == GROUND) {
			if (speed.x)
				GetSprite()->draw(x, y, &clips[currentFrame % 6 + 8], flip);
			else
				GetSprite()->draw(x, y, &clips[(currentFrame/3) % 4], flip);
		}
		else if (state == AIR) {
			if (speed.y < 0) {
				GetSprite()->draw(x, y, &clips[17], flip);
			}
			else if (speed.y == 0) {
				GetSprite()->draw(x, y, &clips[22], flip);
			}
			else {
				GetSprite()->draw(x, y, &clips[23], flip);
			}
		}
		else if (state == ROPE) {
			SDL_Rect rect = { 0,0,clips[0].w, clips[0].h };
			if (engine->isPressed(SDLK_UP) || engine->isPressed(SDLK_DOWN))
				GetSprite()->draw(x - 10, y, &clips[91+ currentFrame%4], &rect, go->angle, SDL_FLIP_NONE);
			else
				GetSprite()->draw(x-10, y, &clips[91],&rect, go->angle, SDL_FLIP_NONE);
		}
		else if (state == WATER) {
			if (speed.dotProduct(speed) < 1.0  || (speed.x == 0 && engine->isReleased(SDLK_UP) && engine->isReleased(SDLK_DOWN)))
				GetSprite()->draw(x, y, &clips[81 + (currentFrame / 2) % 6], flip);
			else {
				SDL_Rect rect = { 0,0,clips[0].w, clips[0].h };
				double angle = atan2( speed.y,speed.x) * 180 / M_PI;
				if (angle > 90 || angle < -90)
					GetSprite()->draw(x, y, &clips[87 + (currentFrame / 2) % 4], &rect, angle, SDL_FLIP_VERTICAL);
				else
					GetSprite()->draw(x, y, &clips[87 + (currentFrame / 2) % 4], &rect, angle, SDL_FLIP_NONE);
			}
				
		}
	}
}


void PlayerGraphicsComponent::Receive(Message m)
{
	if (state != ROPE) {
		switch (m) {
		case COLLIDE_BOTTOM:
			if (state != WATER)
				state = GROUND;
			break;
		case COLLIDE_WATER:
			state = WATER;
			break;
		case ROPE_LINKED:
			state = ROPE;
			break;
		case COLLIDE_NONE:
			if (state != ROPE)
				state = AIR;
			break;
		default:
			break;
		}
	}
	if (m == HIT) {
		if (die_anim == -1)
			die_anim = engine->GetFrame();
	}
	if (m == ROPE_FREE) {
		state = AIR;
	}
}

void PlayerGraphicsComponent::Destroy()
{
	if (sprite != NULL) {
		sprite->destroy();
		delete sprite;
	}
	sprite = NULL;
}



Player::~Player()
{
	SDL_Log("Player::~Player");
}

void Player::Init()
{
	mass = 1;
	respawnPoint = init_position;
	SDL_Log("Player::Init");
	GameObject::Init();
	lives = NUM_LIVES;
}

void Player::Receive(Message m)
{
	if (m == HIT && god) {
		return;
	}
	if (m == DEAD) {
		if (linked) {
			linked = false;
			for (auto it = components.begin(); it != components.end(); it++)
				(*it)->Receive(ROPE_FREE);
		}
		lives--;
		if (lives >= 0) {
			position = respawnPoint;
			old_position = respawnPoint;
		}
		else Send(GAME_OVER);
	}
	for (auto it = components.begin(); it != components.end(); it++)
		(*it)->Receive(m);

	if (m == COLLIDE_BOTTOM || m == ROPE_LINKED)
		Send(ROPE_RESET);
}