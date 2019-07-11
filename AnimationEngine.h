#pragma once
#include "Core.h"
#include "Component2D.h"
#include "Joint2D.h"
#include "Frame.h"
#include "Bone.h"

enum class KinectStat { ACTIVE, DEACTIVE };
enum class SaveMode { OVERWRITE, SAVE_EXISTING };

class AnimationEngine :
	public Core
{
public:
	AnimationEngine();
	~AnimationEngine();
	virtual void Init();
	virtual void Render();
	virtual void Update(float deltaTime);
	virtual void Input();

private:
	void initKinect();									//Initialize Kinect
	void deactivateKinect();							//Deactivate Kinect
	void getAllKinectData();							//Get All Data Stream From Kinect
	void toggleKinect(KinectStat stat);					//Toggle Kinect
	void getBodyData(IMultiSourceFrame* frame);			//Get Body Data From Stream
	void AnimateFrame(vector<Frame> a);					//Animating Keyframe
	void saveCoordinateInNote(SaveMode savemode);		//Save Coordinate To .TXT File
	void saveFrameInVector();							//Save Frame in Vector //Like Array List
	void displaySingleFrame(vector<Frame> a);			//Display Single Frame
	void readCoordinate(const char * fileName);			//Read Coordinate From .TXT File
	void playImportKeyframe(bool play);					//Play Imported Keyframe
	void playExistingKeyframe(bool play);				//Play Directly
	void renderComponen();								//Render Componen
	void initGui();										//Initialize GUI
	void makeGui();										//Make GUI
	void renderGui();									//Render GUI
	GLfloat angleFromArbitraryOrigin(vec4 a, vec4 b, vec4 aUpdate, vec4 bUpdate);	//Method to get Angle From Other Origin

	//User define object
	Component2D kepala, tubuh, kaki_kiri, kaki_kanan, tangan_kiri, tangan_kanan;
	Joint2D JointFromKinect[JointDirectory_Count];
	Joint2D JointForAnimation[JointDirectory_Count];
	Joint2D JointForSingleFrame[JointDirectory_Count];
	Joint2D JointFromTxt[JointDirectory_Count];
	Bone ConnectingBone[JointDirectory_Count - 1];
	Frame frames, keyframesFromText;
	string path;
	KinectStat status;

	//Make Vector Frame
	vector<Frame> ExistingKeyframe;
	vector<Frame> keyframeFromText;

	int totalVector = 0;

	vec4 lerpJoint[JointDirectory_Count];
	vec4 temporary[JointDirectory_Count];
	vec4 resultInterpolate[JointDirectory_Count];

	vec2 vectorAtoB_before[4];
	vec2 vectorAtoB_after[4];

	GLfloat lengthVector[4];

	GLfloat angleVector[8];
};