#pragma once
#include "Core.h"
class VideoWindow :
	public Core
{
public:
	VideoWindow();
	~VideoWindow();
	virtual void Init();
	virtual void Render();
	virtual void Update(float deltaTime);
	virtual void Input();
};

