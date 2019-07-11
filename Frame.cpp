#include "Frame.h"

Frame::Frame()
{
}

Frame::Frame(Joint2D body[])
{
	for (int i = 0; i < JointDirectory_Count; i++)
	{
		this->JointInSkeleton[i] = body[i];
	}
}

Frame::~Frame()
{
}

Joint2D * Frame::getFrame()
{
	return JointInSkeleton;
}