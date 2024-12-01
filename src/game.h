#pragma once

#include <set>
#include "level.h"

class Level;

class Game : public GameObject
{

	Engine* engine;
	Scene  * scene;
	bool running = true;
	int loops;
	Message transitionMessage;

public:
	Uint32 lastTime;
	std::vector<Component*> inputs; //All the inputcomponent
	std::vector<Component*> forces; //All the forces
	std::vector<GameObject*> objects; //All the objects
	std::vector<Component*> constraints; //All the contraints
	std::vector<RenderComponent*> graphics; //All the graphics - called separately

	virtual ~Game();
	virtual void Create(Engine* engine);
	virtual void Init();
	virtual void Update();
	virtual void Render(float interpolation);
	virtual void Receive(Message m);
	void SceneTransition();
	void Clear();
	bool Running() {
		return running;
	}
	virtual void Destroy();
};