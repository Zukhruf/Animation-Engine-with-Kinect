#include "AnimationEngine.h"

int main(int argc, char** argv) {
	AnimationEngine ae = AnimationEngine();
	ae.startCore("Skeletal Animation Engine V4", false, WindowFlag::WINDOWED);
	return 0;
}