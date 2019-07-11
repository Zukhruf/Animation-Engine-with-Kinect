#pragma once
#include "Joint2D.h"
class Frame
{
public:
	Frame();
	Frame(Joint2D body[]);
	~Frame();

	Joint2D * getFrame();
private:
	Joint2D JointInSkeleton[JointDirectory_Count];
};