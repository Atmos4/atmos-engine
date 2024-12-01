#include "rope.h"


//TODO : move in physics
bool collisionPointRect(Vector2D &p, Vector2D &r1, Vector2D &r2)
{
	return (p.x > r1.x && p.x< r2.x && p.y > r1.y && p.y < r2.y);
}

void RopeConstraintComponent::Create( GameObject* go, GameObject * go0, double length_, bool first)
{
	Component::Create(go);
	linked = go0;
	this->length = length_;
	this->first = first;
}

void RopeConstraintComponent::Init()
{
	go->mass = 0.5;
	go->position = linked->position+Vector2D(0,length);
	go->old_position = go->position;
}

void RopeConstraintComponent::Update()
{

	Vector2D dist = linked->position - go->position;
	double l = dist.length();

	double force = l - length;


	//String joint - to get a rod joint, just remove the condition
	if (l > length) {
		if (first) {
			go->position = go->position + (dist / l) * force*0.9;
		}
		else {
			//new position
			go->position = go->position + ((dist / l) * force*0.5);
			linked->position = linked->position - ((dist / l) * force*0.5);

		}
	}

}

void RopeRenderComponent::Create( GameObject* go,Sprite * sprite, GameObject * go0, double length_)
{
	RenderComponent::Create( go, sprite);
	this->linked = go0;
	this->length = length_;
}

void RopeRenderComponent::Render(float interpolation) 
{
	if (!go->enabled)
		return;

	Vector2D vect = linked->position - go->position;
	SDL_Rect dest = {0,0, 30, int(vect.length()+1)};
	double angle = atan2(-vect.x, vect.y)*180/M_PI;


	Vector2D interpolated = go->position + (go->position-go->old_position) * interpolation - (go->position - go->old_position);

	if (GetSprite())
		GetSprite()->draw(int(interpolated.x+vect.x/2), int(interpolated.y+vect.y/2), NULL,&dest,angle, SDL_FLIP_NONE);
}

void RopeCollider::Create(Engine * engine, Player * go, Rope * base_, RopeLink ** rope_,int size_, double length_)
{
	Component::Create( go);
	this->engine = engine;
	this->player = go;
	base = base_;
	rope = rope_;
	size = size_;
	length = length_;

	enabled = true;
}


void RopeCollider::Update() {
	//If the object is linked, update all movements following the link's movements
	if (player->linked == false) {
		linked = 0;
	}
	if (linked) {
		if (engine->isPressed(SDLK_DOWN)) {
			linked++;
			if (linked >= size) {
				linked = size -1;
			}
		}
		else if (engine->isPressed(SDLK_SPACE) && canSpace) {
			if (engine->isReleased(SDLK_UP)) {
				go->Receive(ROPE_FREE);
				go->old_position = rope[linked]->old_position;
				go->ApplyForce(Vector2D(0, -60));
				linked = 0;
				player->linked = false;
				canSpace = false;
				enabled = false;
				player->Receive(JUMP);
			}
		}
		else if (engine->isPressed(SDLK_UP)){
			linked--;
			if (linked < 1) {
				linked = 1;
			}
		}
		else {
			if (engine->isPressed(SDLK_LEFT))rope[linked]->ApplyForce(Vector2D(-5, 5));
			if (engine->isPressed(SDLK_RIGHT))rope[linked]->ApplyForce(Vector2D(5, 5));
		}
		if (linked) {
			go->position = rope[linked]->position;
			Vector2D vect = rope[linked - 1]->position - rope[linked]->position;
			double angle = atan2(vect.x, -vect.y) * 180 / M_PI;
			go->angle = angle;
		}
		
	}
	else
	{
		if (enabled) {
			Vector2D dist = rope[size / 2]->position - go->position;
			double rope_length = size * length;
			//Handle collision only if necessary
			if (dist.dotProduct(dist) < rope_length*rope_length) {
				for (int i = 1; i < size; i++) {
					Vector2D rect1 = (go->position - go->aabb / 2);
					Vector2D rect2 = (go->position + go->aabb / 2);
					if (collisionPointRect(rope[i]->position, rect1, rect2) && !player->linked)
					{
						linked = i;
						go->Receive(ROPE_LINKED);
						player->linked = true;
						canSpace = false;
					}
				}
			}
		}
	}

	if (!canSpace && engine->isReleased(SDLK_SPACE))
	{
		canSpace = true;
	}
}


void Rope::Create(Engine * engine, int size_, double length_, const char * rope_file)
{
	GameObject::Init();
	this->engine = engine;

	rope_sprite = Sprite::createFromFile(engine,rope_file);

	this->size = size_;
	this->length = length_;
	rope = new RopeLink*[size];

}

void Rope::Destroy()
{
	for (int i = 0; i < size; i++)
	{
		rope[i]->Destroy();
	}
	GameObject::Destroy();

	if (rope_sprite != nullptr) {
		rope_sprite->destroy();
		delete rope_sprite;
		rope_sprite = nullptr;
	}
}

Rope::~Rope()
{

	for (int i = 0; i < size; i++)
	{
		delete rope[i];
	}

	delete[] rope;
}

void Rope::Init()
{
	GameObject::Init();

	for (int i = 0; i < size; i++)
	{
		rope[i]->Init();
	}
}