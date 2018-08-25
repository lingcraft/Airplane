#pragma once

#include "Definitions.h"

class WelcomeScene : public Layer
{
public:
	static Scene* createScene();
	CREATE_FUNC(WelcomeScene);
	bool init();
};