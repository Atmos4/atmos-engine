#pragma once

#include "component.h"
#include "game_object.h"
#include "player.h"

class ParallaxRenderComponent : public RenderComponent
{
	Engine * engine;
public:

	virtual void Create(Engine* engine, GameObject * go, const char * sprite_name, float plxSpeed, float zoom);
	virtual void Render(float i);
	virtual void Destroy();

private :
	int width;
	int height;
	float plx_speed;
	float zoom_factor;
};

