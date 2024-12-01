#pragma once

// GameObject represents objects which moves are drawn
#include <vector>
#include "component.h"
#include "vector2D.h"

class GameObject
{
protected:
	std::vector<GameObject*> receivers;
	std::vector<Component*> components;


public:

	Vector2D position;
	Vector2D old_position;

	Vector2D forces;
	double mass;

	Vector2D aabb;

	Vector2D init_position;
	double angle;

	bool enabled;

	virtual void Create()
	{
		enabled = false;
	}

	virtual void Init()
	{
		//Otherwise it crashes
		mass = 1;
		position = init_position;
		old_position = position;

		for (auto it = components.begin(); it != components.end(); it++)
			(*it)->Init();
		enabled = true;
	}

	virtual void Destroy()
	{
		for (auto it = components.begin(); it != components.end(); it++)
			(*it)->Destroy();
	}

	virtual ~GameObject()
	{
		for (auto it = components.begin(); it != components.end(); it++)
			delete (*it);
	}

	/** This is the main physics integration formula, using 2nd order Verlet integration. */
	virtual void Integrate()
	{
		Vector2D temp = position;
		position = position * 2 - old_position + forces / (mass * 10); // Verlet integration
		old_position = temp;
		forces.x = 0.0;
		forces.y = 0.0;
		
	}

	virtual void AddReceiver(GameObject *go){receivers.push_back(go);}

	virtual GameObject* AddComponent(Component * c) 
	{
		components.push_back(c); 
		return this; 
	}

	virtual void Receive(Message m) {}

	void Send(Message m)
	{
		for (unsigned int i = 0; i < receivers.size(); i++)
		{
			if (!receivers[i]->enabled)
				continue;

			receivers[i]->Receive(m);
		}
	}

	void ApplyForce(Vector2D f) {
		forces = forces + f;
	}

};