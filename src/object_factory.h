#pragma once
#include "rope.h"
#include "enemy.h"
#include "player.h"
#include "rock.h"

class ObjectFactory 
{
public:
	static GameObject * CreateRope(Engine * engine, Game * game, Player * player) {

		Rope * rope = new Rope();
		rope->Create(engine, 10, 10.0, "assets/sprites/rope.bmp");

		for (int i = 0; i < rope->Size(); i++) {
			rope->GetRope()[i] = new RopeLink();
			GravityComponent * rope_gravity = new GravityComponent();
			rope_gravity->Create(rope->GetRope()[i]);
			/*AirFrictionComponent * airfriction = new AirFrictionComponent();
			airfriction->Create(rope->GetRope()[i]);*/
			RopeConstraintComponent * base_constraint = new RopeConstraintComponent();
			base_constraint->Create(rope->GetRope()[i], rope, rope->Length()*(i + 2), true);

			GameObject * previous;
			if (i == 0)previous = rope;
			else previous = rope->GetRope()[i - 1];

			RopeConstraintComponent * constraint = new RopeConstraintComponent();
			constraint->Create(rope->GetRope()[i], previous, rope->Length(), (i == 0));
			RopeRenderComponent * rope_graphics = new RopeRenderComponent();
			rope_graphics->Create(rope->GetRope()[i], rope->GetSprite(), previous, rope->Length());
			rope->GetRope()[i]->Create();
			rope->GetRope()[i]->AddComponent(rope_gravity);
			rope->GetRope()[i]->AddComponent(base_constraint);
			//rope->GetRope()[i]->AddComponent(airfriction);
			rope->GetRope()[i]->AddComponent(constraint);
			rope->GetRope()[i]->AddComponent(rope_graphics);

			game->objects.push_back(rope->GetRope()[i]);
			game->forces.push_back(rope_gravity);
			game->constraints.push_back(base_constraint);
			game->constraints.push_back(constraint);
			game->graphics.push_back(rope_graphics);
		}

		game->objects.push_back(rope);

		RopeCollider * rope_collider = new RopeCollider();
		rope_collider->Create(engine, player, rope, rope->GetRope(), rope->Size(), rope->Length());
		rope->AddComponent(rope_collider);
		player->AddReceiver(rope);

		game->constraints.push_back(rope_collider);

		return rope;

	}

	static GameObject * CreateSkeleton(Engine * engine, Game * game, Player * player)
	{
		GameObject * obj = new GameObject();
		obj->Create();
		obj->aabb = Vector2D(22, 32);

		PatrolBehaviorComponent * patrol = new PatrolBehaviorComponent();
		patrol->Create(obj, Vector2D(40, 0));

		SkeletonRenderComponent * render = new SkeletonRenderComponent();
		render->Create(engine, obj, Sprite::createFromFile(engine, "assets/sprites/skeleton.bmp"));

		EnemyCollideComponent * collider = new EnemyCollideComponent();
		collider->Create(obj, player);

		obj->AddComponent(patrol);
		obj->AddComponent(collider);
		obj->AddComponent(render);

		game->inputs.push_back(patrol);
		game->constraints.push_back(collider);
		game->graphics.push_back(render);
		game->objects.push_back(obj);

		return obj;
	}

	static GameObject * CreateFish(Engine * engine, Game * game, Player * player)
	{
		GameObject * obj = new GameObject();
		obj->Create();
		obj->aabb = Vector2D(30, 17);

		PatrolBehaviorComponent * patrol = new PatrolBehaviorComponent();
		patrol->Create(obj, Vector2D(0, 32));

		FishRenderComponent * render = new FishRenderComponent();
		render->Create(engine, obj, Sprite::createFromFile(engine, "assets/sprites/fish.bmp"));

		EnemyCollideComponent * collider = new EnemyCollideComponent();
		collider->Create(obj, player);

		obj->AddComponent(patrol);
		obj->AddComponent(collider);
		obj->AddComponent(render);

		game->inputs.push_back(patrol);
		game->constraints.push_back(collider);
		game->graphics.push_back(render);
		game->objects.push_back(obj);

		return obj;
	}

	static GameObject * CreateVolcano(Engine * engine, Game * game, Player * player, Vector2D direction, ObjectPool<Rock> * rock_pool)
	{
		Volcano * obj = new Volcano();
		obj->Create(rock_pool);

		for (auto rock = rock_pool->pool.begin(); rock != rock_pool->pool.end(); rock++)
		{
			double radius = rand() % 8 + 6;
			GravityComponent * gravity = new GravityComponent();
			gravity->Create(*rock);
			RockRenderComponent * render = new RockRenderComponent();
			render->Create(*rock,Sprite::createFromFile(engine,"assets/sprites/rock.bmp"), radius);
			(*rock)->Create();
			(*rock)->radius = radius;
			(*rock)->AddComponent(gravity);
			(*rock)->AddComponent(render);
			(*rock)->AddReceiver(obj);

			game->forces.push_back(gravity);
			game->graphics.push_back(render);

			game->objects.push_back(*rock);
		}

		RockThrowComponent * rtc = new RockThrowComponent();
		rtc->Create(obj, engine, 2.0f, rock_pool, direction);
		RockCollideComponent * rcc = new RockCollideComponent();
		rcc->Create(player, &rock_pool->pool);

		obj->AddComponent(rtc);
		obj->AddComponent(rcc);

		game->inputs.push_back(rtc);
		game->constraints.push_back(rcc);


		return obj;
	}
};