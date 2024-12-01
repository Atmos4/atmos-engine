#include "parallax.h"

void ParallaxRenderComponent::Create(Engine * engine, GameObject * go, const char * sprite_name, float plxSpeed, float z)
{
	RenderComponent::Create(go, Sprite::createFromFile(engine, sprite_name));
	this->engine = engine;

	width = sprite->getWidth();
	height = sprite->getHeight();

	plx_speed = plxSpeed;
	zoom_factor = z;
}

void ParallaxRenderComponent::Render(float i) {
	if (!go->enabled)
		return;

	float zoom = engine->getZoom()*zoom_factor;
	int renderWidth = int((width*zoom));
	int scrollX = (int(( - engine->getCamera()->x) * plx_speed)) % renderWidth;
	int scrollY = int(( - engine->getCamera()->y) * plx_speed);

	if (sprite) {
		sprite->drawAbsolute(scrollX, scrollY, zoom);
		sprite->drawAbsolute(scrollX + renderWidth, scrollY, zoom);
	}
}

void ParallaxRenderComponent::Destroy()
{
	if (sprite != NULL) {
		sprite->destroy();
		delete sprite;
		sprite = NULL;
	}
}
