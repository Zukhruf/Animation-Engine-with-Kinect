#include "AnimationEngine.h"

#pragma region Kinect Variable
IKinectSensor* sensor;
IMultiSourceFrameReader* multiSourceReader;
ICoordinateMapper* mapper;
Joint joints[21];
ColorSpacePoint colorPoint;
CameraSpacePoint cameraCoordinate;
CameraSpacePoint csc[JointDirectory_Count];
ColorSpacePoint csp[JointDirectory_Count];
BOOLEAN tracked;
#pragma endregion

static bool playImportedKeyframe = false;
static bool playExisting = false;
static bool renderHead = false;
static bool renderTorso = false;
static bool renderLeftLeg = false;
static bool renderRightLeg = false;
static bool renderLeftArm = false;
static bool renderRightArm = false;
static bool renderJoint = false;

static float f_head = 0.5f;
static float f_torso = 0.5f;
static float f_right_arm = 0.5f;
static float f_left_arm = 0.5f;
static float f_right_leg = 0.5f;
static float f_left_leg = 0.5f;

#pragma region All Function
AnimationEngine::AnimationEngine()
{
}

AnimationEngine::~AnimationEngine()
{
}

//Initialize
void AnimationEngine::Init()
{
	//Initialize Kinect and Joint
	{
		initKinect();

		//Init Joint From Kinect
		for (int i = 0; i < JointDirectory_Count; i++)
		{
			JointFromKinect[i].initJoint();
		}

		//Init Joint For Animation
		for (int i = 0; i < JointDirectory_Count; i++)
		{
			JointForAnimation[i].initJoint();
		}

		//Init Joint From Txt File
		for (int i = 0; i < JointDirectory_Count; i++)
		{
			JointFromTxt[i].initJoint();
		}

		//Init Bone
		for (int i = 0; i < (JointDirectory_Count - 1); i++)
		{
			ConnectingBone[i].buildBone();
		}
	}
	
	//Initialize GUI
	{
		initGui();
	}

	//Initialize 2D Component
	{
		kepala.initComponen("head.png");
		tubuh.initComponen("torso.png");
		tangan_kanan.initComponen("left_arm.png");
		tangan_kiri.initComponen("left_arm.png");
		kaki_kanan.initComponen("left_leg.png");
		kaki_kiri.initComponen("left_leg.png");
	}
}

//Rendering
void AnimationEngine::Render()
{
	makeGui();

	renderGui();

	SDL_GL_MakeCurrent(window, glContext);

	//Setting Viewport
	glViewport(0, 0, width, height);
	//Clear the color and depth buffer
	glClear(GL_COLOR_BUFFER_BIT);

	//Set the background color
	glClearColor(0.8f, 0.8f, 0.8f, 1.0f);

	//Render Component
	renderComponen();

	toggleKinect(status);

	playImportKeyframe(playImportedKeyframe);

	playExistingKeyframe(playExisting);

	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

//Update
void AnimationEngine::Update(float deltaTime)
{
}

//The Keyboard Controller
void AnimationEngine::Input()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		ImGui_ImplSDL2_ProcessEvent(&event);                                                                                                    
		switch (event.type)
		{
		case SDL_QUIT:
			state = State::EXIT;
			break;
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym)
			{
			case SDLK_SPACE:
				saveFrameInVector();
				break;
			case SDLK_ESCAPE:
				state = State::EXIT;
				break;
			case SDLK_RETURN:
				/*saveCoordinateInNote();*/
				readCoordinate("KoordinatSetiapSendi.txt");
				break;
			case SDLK_i:
				status = KinectStat::ACTIVE;
				break;
			case SDLK_o:
				displaySingleFrame(ExistingKeyframe);
				break;
			case SDLK_p:
				status = KinectStat::DEACTIVE;
				break;
			}
			break;
		}
	}
}

//Activate Kinect
void AnimationEngine::initKinect()
{
	GetDefaultKinectSensor(&sensor);
	if (sensor != NULL)
	{
		sensor->Open();
		sensor->get_CoordinateMapper(&mapper);
		sensor->OpenMultiSourceFrameReader(
			FrameSourceTypes_Color |
			FrameSourceTypes_Body,
			&multiSourceReader
		);
	}
	else
	{
		printf("Kinect Not Available!\n");
	}
}

//Deactivate Kinect
void AnimationEngine::deactivateKinect()
{
	sensor->Close();
}

//Get All Kinect Data Stream
void AnimationEngine::getAllKinectData()
{
	IMultiSourceFrame* frame = NULL;
	if (SUCCEEDED(multiSourceReader->AcquireLatestFrame(&frame)))
	{
		getBodyData(frame);
	}
}

//Toggle Kinect
void AnimationEngine::toggleKinect(KinectStat stat)
{
	switch (stat)
	{
	case KinectStat::ACTIVE:
		sensor->Open();
		getAllKinectData();
		break;
	case KinectStat::DEACTIVE:
		sensor->Close();
		break;
	}
}

//Get Skeleton Data from Kinect
void AnimationEngine::getBodyData(IMultiSourceFrame * frame)
{
	IBodyFrame* bodyFrame;
	IBodyFrameReference* bodyFrameRef = NULL;
	frame->get_BodyFrameReference(&bodyFrameRef);
	bodyFrameRef->AcquireFrame(&bodyFrame);

	if (bodyFrameRef) bodyFrameRef->Release();
	if (!bodyFrame) return;

	IBody* bodies[BODY_COUNT] = { 0 };
	bodyFrame->GetAndRefreshBodyData(BODY_COUNT, bodies);
	/*glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);*/
	for (int i = 0; i < BODY_COUNT; i++)
	{
		bodies[i]->get_IsTracked(&tracked);
		if (tracked)
		{
			bodies[i]->GetJoints(JointType_Count, joints);

			//Mapping Joint
			{
				csc[Head] = joints[JointType_Head].Position;
				csc[Neck] = joints[JointType_SpineShoulder].Position;
				csc[ShoulderLeft] = joints[JointType_ShoulderLeft].Position;
				csc[ElbowLeft] = joints[JointType_ElbowLeft].Position;
				csc[HandLeft] = joints[JointType_HandLeft].Position;
				csc[ShoulderRight] = joints[JointType_ShoulderRight].Position;
				csc[ElbowRight] = joints[JointType_ElbowRight].Position;
				csc[HandRight] = joints[JointType_HandRight].Position;
				csc[SpineBase] = joints[JointType_SpineBase].Position;
				csc[HipLeft] = joints[JointType_HipLeft].Position;
				csc[KneeLeft] = joints[JointType_KneeLeft].Position;
				csc[AnkleLeft] = joints[JointType_AnkleLeft].Position;
				csc[HipRight] = joints[JointType_HipRight].Position;
				csc[KneeRight] = joints[JointType_KneeRight].Position;
				csc[AnkleRight] = joints[JointType_AnkleRight].Position;
			}

			//Draw Skeleton
			{
				//Draw Joint
				for (int i = 0; i < JointDirectory_Count; i++)
				{
					mapper->MapCameraPointToColorSpace(csc[i], &csp[i]);

					if (renderJoint)
					{
						JointFromKinect[i].drawJoint(csp[i]);
					}
				}
			}
			//Draw Componen at The Origin
			renderComponen();

			//Head
			{
				kepala.setPos(csp[0].X + 15.0f, csp[0].Y);
				kepala.setSize(vec4(f_head, f_head, 0.0f, 0.0f));
			}

			//Torso
			{
				tubuh.setPos(((csp[1].X + csp[8].X) / 2), ((csp[1].Y + csp[8].Y) / 2) - 44.5f);
				tubuh.setSize(vec4(f_torso, f_torso, 0.0f, 0.0f));
			}

			//Right_arm
			{
				tangan_kanan.setPos(csp[6].X, csp[6].Y);
				tangan_kanan.setSize(vec4(f_right_arm, f_right_arm, 0.0f, 0.0f));
				/*right_arm.rotateComponen(vec4(0.0f, 0.0f, 1.0f, 0.0f), -90.0f + angleVector[0]);*/
			}

			//Left_arm
			{
				tangan_kiri.setPos(csp[3].X, csp[3].Y);
				tangan_kiri.setSize(vec4(f_left_arm, f_left_arm, 0.0f, 0.0f));
			}

			//Right_leg
			{
				kaki_kanan.setPos(csp[13].X, csp[13].Y);
				kaki_kanan.setSize(vec4(f_right_leg, f_right_leg, 0.0f, 0.0f));
			}

			//Left_leg
			{
				kaki_kiri.setPos(csp[10].X, csp[10].Y);
				kaki_kiri.setSize(vec4(f_left_leg, f_left_leg, 0.0f, 0.0f));
			}
		}
	}
	if (bodyFrame) bodyFrame->Release();
}

//Animating Frame
void AnimationEngine::AnimateFrame(vector<Frame> a)
{
	deactivateKinect();

	int vectorSize = a.size();
	int ProsesLerp = vectorSize - 1;

	static int n = 0;

	if (n < ProsesLerp)
	{
		Joint2D * temp = a[n].getFrame();
		Joint2D * temp2 = a[n + 1].getFrame();

		static float lerpValue = 0.00f;
		lerpValue += 0.01f;
		if (lerpValue < 1.00f)
		{
			//Clear the color and depth buffer
			glClear(GL_COLOR_BUFFER_BIT);

			for (int j = 0; j < JointDirectory_Count; j++)
			{
				lerpJoint[j] = mix(temp[j].getJointVector(), temp2[j].getJointVector(), lerpValue);
				if (renderJoint)
				{
					JointForAnimation[j].drawJoint(lerpJoint[j]);
				}
			}
			renderComponen();
	
			////Draw Component at The Origin
			//Head
			{
				kepala.setPos(lerpJoint[0].x + 15.0f, lerpJoint[0].y);
				kepala.setSize(vec4(f_head, f_head, 0.0f, 0.0f));
			}

			//Torso
			{
				tubuh.setPos(((lerpJoint[1].x + lerpJoint[8].x) / 2), ((lerpJoint[1].y + lerpJoint[8].y) / 2) - 44.5f);
				tubuh.setSize(vec4(f_torso, f_torso, 0.0f, 0.0f));
			}

			//Right_arm
			{
				GLfloat rotateAtShoulder1 = angleFromArbitraryOrigin(temp[5].getJointVector(), temp[6].getJointVector(), lerpJoint[5], lerpJoint[6]);
				tangan_kanan.setPos(lerpJoint[6].x, lerpJoint[6].y);
				tangan_kanan.rotateComponen(-rotateAtShoulder1 + (-90.0f));
				tangan_kanan.setSize(vec4(f_right_arm, f_right_arm, 0.0f, 0.0f));
			}

			//Left_arm
			{
				GLfloat rotateAtShoulder = angleFromArbitraryOrigin(temp[2].getJointVector(), temp[3].getJointVector(), lerpJoint[2], lerpJoint[3]);
				tangan_kiri.setPos(lerpJoint[3].x, lerpJoint[3].y);
				tangan_kiri.rotateComponen(rotateAtShoulder + (90.0f));
				tangan_kiri.setSize(vec4(f_left_arm, f_left_arm, 0.0f, 0.0f));
			}

			//Right_leg
			{
				kaki_kanan.setPos(lerpJoint[13].x, lerpJoint[13].y);
				kaki_kanan.setSize(vec4(f_right_leg, f_right_leg, 0.0f, 0.0f));
			}

			//Left_leg
			{
				kaki_kiri.setPos(lerpJoint[10].x, lerpJoint[10].y);
				kaki_kiri.setSize(vec4(f_left_leg, f_left_leg, 0.0f, 0.0f));
			}

			if (lerpValue > 0.99f && lerpValue < 1.0f)
			{
				n = n + 1;
				lerpValue = 0.00f;
			}
		}
	}
}

//Saving coordinate in notepad using ofstream
void AnimationEngine::saveCoordinateInNote(SaveMode savemode)
{
	int vectorSize = ExistingKeyframe.size();
	Joint2D totalJoint[JointDirectory_Count];

	//Overwrite File TXT
	switch (savemode)
	{
	case SaveMode::OVERWRITE:
		for (int i = 0; i < vectorSize; i++)
		{
			Joint2D * temp = ExistingKeyframe[i].getFrame();
			//Tulis di txt
			ofstream output_file;
			output_file.open("KoordinatSetiapSendi.txt", ios::trunc);
			for (int j = 0; j < JointDirectory_Count; j++)
			{
				output_file << temp[j].getJointVector().x << " " << temp[j].getJointVector().y << "\n";
			}
			output_file << "\n";
			output_file.close();
		}
		break;
	case SaveMode::SAVE_EXISTING:
		for (int i = 0; i < vectorSize; i++)
		{
			Joint2D * temp = ExistingKeyframe[i].getFrame();
			//Tulis di txt
			ofstream output_file;
			output_file.open("KoordinatSetiapSendi.txt", ios::app);
			for (int j = 0; j < JointDirectory_Count; j++)
			{
				output_file << temp[j].getJointVector().x << " " << temp[j].getJointVector().y << "\n";
			}
			output_file << "\n";
			output_file.close();
		}
		break;
	}
}

//Saving coordinate in vector (Like dynamic array)
void AnimationEngine::saveFrameInVector()
{
	//Simpan dengan vector
	frames = Frame(JointFromKinect);
	ExistingKeyframe.push_back(frames);
	cout << "Vector Frame : " << totalVector++ << endl;
}

//Display Single Frame
void AnimationEngine::displaySingleFrame(vector<Frame> a)
{
	int vectorSize = a.size();
	for (int i = 0; i < vectorSize; i++)
	{
		//Clear the color and depth buffer
		glClear(GL_COLOR_BUFFER_BIT);

		//Set the background color
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

		Joint2D * temp = a[i].getFrame();
		for (int j = 0; j < JointDirectory_Count; j++)
		{
			temp[i].drawJoint(temp[j].getJointVector());
		}
	}
}

//Read Coordinate of Joint from .txt file
void AnimationEngine::readCoordinate(const char * fileName)
{
	ifstream input_file(fileName);
	path = fileName;
	string line;
	int i = 0;
	while (!input_file.eof())
	{
		while (getline(input_file, line))
		{
			istringstream iss(line);
			/*if (!(iss >> temporary[i].x >> temporary[i].y))
			{
				break;
			}*/
			iss >> temporary[i].x >> temporary[i].y;
			JointFromTxt[i].setJointVector((GLfloat)temporary[i].x, (GLfloat)temporary[i].y);
			i++;
			if (line == "")
			{
				/*cout << "Test Spasi" << endl;*/
				keyframesFromText = Frame(JointFromTxt);
				keyframeFromText.push_back(keyframesFromText);
			}
			if (i == 16)
			{
				i = 0;
			}
		}
	}
}

//Play Animation From Imported Keyframe
void AnimationEngine::playImportKeyframe(bool play)
{
	if (play)
	{
		AnimateFrame(keyframeFromText);
	}
	else {
		status = KinectStat::ACTIVE;
	}
}

//Play Animation Directly
void AnimationEngine::playExistingKeyframe(bool play)
{
	if (play)
	{
		AnimateFrame(ExistingKeyframe);
	}
	else {
		status = KinectStat::ACTIVE;
	}
}

//Rendering Component
void AnimationEngine::renderComponen()
{
	/*glClear(GL_COLOR_BUFFER_BIT);*/
	if (renderRightLeg)
	{
		kaki_kanan.drawComponen();
	}

	if (renderRightArm)
	{
		tangan_kanan.drawComponen();
	}

	if (renderTorso)
	{
		tubuh.drawComponen();
	}

	if (renderHead)
	{
		kepala.drawComponen();
	}

	if (renderLeftArm)
	{
		tangan_kiri.drawComponen();
	}

	if (renderLeftLeg)
	{
		kaki_kiri.drawComponen();
	}
}

//Initialize GUI
void AnimationEngine::initGui()
{
	//Make GUI
	const char* glsl_version = "#version 440 core";

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls

	// Setup Dear ImGui style
	/*ImGui::StyleColorsDark();*/
	ImGui::StyleColorsClassic();

	// Setup Platform/Renderer bindings
	ImGui_ImplSDL2_InitForOpenGL(window, glContext);
	ImGui_ImplOpenGL3_Init(glsl_version);
}

//Make GUI Here <<---- ADD HERE
void AnimationEngine::makeGui()
{
	static int counter = 0;

	static bool show_about_window = false;
	static bool close_window = false;
	static bool load_txt_file = false;
	static bool save_txt_file = false;
	static bool open_keyframe_from_txt_file = false;
	static bool save_existing = false;
	static bool overwrite = false;
	static bool open_existing_keyframe = false;
	static bool show_head_componen_control = false;
	static bool show_torso_componen_control = false;
	static bool show_right_arm_componen_control = false;
	static bool show_left_arm_componen_control = false;
	static bool show_right_leg_componen_control = false;
	static bool show_left_leg_componen_control = false;

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(window);
	ImGui::NewFrame();

	
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("Menu"))
		{
			if (ImGui::MenuItem("Open", NULL, &open_keyframe_from_txt_file))
			{
				
			}
			if (ImGui::BeginMenu("Save to .txt File"))
			{
				if (ImGui::MenuItem("Save Existing", NULL, &save_existing))
				{
					saveCoordinateInNote(SaveMode::SAVE_EXISTING);
				}
				if (ImGui::MenuItem("Overwrite", NULL, &overwrite))
				{
					saveCoordinateInNote(SaveMode::OVERWRITE);
				}
				ImGui::EndMenu();
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Exit", NULL, &close_window))
			{
				state = State::EXIT;
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Componen"))
		{
			if (ImGui::MenuItem("Head", NULL, &show_head_componen_control))
			{
				
			}
			if (ImGui::MenuItem("Torso", NULL, &show_torso_componen_control))
			{
				
			}
			if (ImGui::MenuItem("Left Arm", NULL, &show_left_arm_componen_control))
			{
				
			}
			if (ImGui::MenuItem("Right Arm", NULL, &show_right_arm_componen_control))
			{
				
			}
			if (ImGui::MenuItem("Left Leg", NULL, &show_left_leg_componen_control))
			{
				
			}
			if (ImGui::MenuItem("Right Leg", NULL, &show_right_leg_componen_control))
			{
				
			}
			if (ImGui::MenuItem("Joint", NULL, &renderJoint))
			{

			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Help"))
		{
			ImGui::MenuItem("About Animation Engine", NULL, &show_about_window);
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

	ImGui::Begin("Main Control");
	{
		if (ImGui::Button("Activate Kinect"))
		{
			status = KinectStat::ACTIVE;
		}
		ImGui::SameLine();
		if (ImGui::Button("Deactivated Kinect"))
		{
			status = KinectStat::DEACTIVE;
		}

		//Button
		if (ImGui::Button("Save Keyframe"))
		{
			saveFrameInVector();
			counter++;
		}
		ImGui::SameLine();
		ImGui::Button("Load");
		ImGui::SameLine();
		if (ImGui::MenuItem("Open Existing Keyframe", NULL, &open_existing_keyframe))
		{

		}

		ImGui::Text("Keyframe : %d", counter);

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::Text("Frame In Vector : %d", totalVector);
		ImGui::End();
	}

	if (show_about_window)
	{
		ImGui::Begin("About", &show_about_window);
		ImGui::Text("Build With GLSL Version 4.40");
		ImGui::Text("Animation Engine Version 1.0");
		ImGui::Text("Zukhruf Mifta -2018");
		ImGui::Text("Email : zukhruf.m.f@gmail.com");
		ImGui::End();
	}

	if (open_keyframe_from_txt_file)
	{
		ImGui::Begin("Play Keyframe", &open_keyframe_from_txt_file);
		ImGui::Text("File Name : KoordinatSetiapSendi.txt");
		readCoordinate("KoordinatSetiapSendi.txt");
		if (ImGui::Button("Play Imported Keyframe"))
		{
			playImportedKeyframe = true;
		}
		ImGui::SameLine();
		if (ImGui::Button("Stop"))
		{
			playImportedKeyframe = false;
		}
		ImGui::End();
	}

	if (open_existing_keyframe)
	{
		ImGui::Begin("Play Existing Keyframe", &open_existing_keyframe);
		if (ImGui::Button("Play"))
		{
			playExisting = true;
		}
		ImGui::SameLine();
		if (ImGui::Button("Stop"))
		{
			playExisting = false;
		}
		ImGui::End();
	}

	if (show_head_componen_control)
	{
		ImGui::Begin("Head Componen Control", &show_head_componen_control);
		ImGui::SliderFloat("Size", &f_head, 0.0f, 1.0f, "ratio = %.3f");
		ImGui::End();
		renderHead = show_head_componen_control;
	}
	if (show_torso_componen_control)
	{
		ImGui::Begin("Torso Componen Control", &show_torso_componen_control);
		ImGui::SliderFloat("Size", &f_torso, 0.0f, 1.0f, "ratio = %.3f");
		ImGui::End();
		renderTorso = show_torso_componen_control;
	}
	if (show_right_arm_componen_control)
	{
		ImGui::Begin("Right Arm Componen Control", &show_right_arm_componen_control);
		ImGui::SliderFloat("Size", &f_right_arm, 0.0f, 1.0f, "ratio = %.3f");
		ImGui::End();
		renderRightArm = show_right_arm_componen_control;
	}
	if (show_left_arm_componen_control)
	{
		ImGui::Begin("Left Arm Componen Control", &show_left_arm_componen_control);
		ImGui::SliderFloat("Size", &f_left_arm, 0.0f, 1.0f, "ratio = %.3f");
		ImGui::End();
		renderLeftArm = show_left_arm_componen_control;
	}
	if (show_right_leg_componen_control)
	{
		ImGui::Begin("Right Leg Componen Control", &show_right_leg_componen_control);
		ImGui::SliderFloat("Size", &f_right_leg, 0.0f, 1.0f, "ratio = %.3f");
		ImGui::End();
		renderRightLeg = show_right_leg_componen_control;
	}
	if (show_left_leg_componen_control)
	{
		ImGui::Begin("Left Leg Componen Control", &show_left_leg_componen_control);
		ImGui::SliderFloat("Size", &f_left_leg, 0.0f, 1.0f, "ratio = %.3f");
		ImGui::End();
		renderLeftLeg = show_left_leg_componen_control;
	}
}

//Render GUI
void AnimationEngine::renderGui()
{
	// Rendering
	ImGui::Render();
}

//Rotate Arbitrary Origin
GLfloat AnimationEngine::angleFromArbitraryOrigin(vec4 a, vec4 b, vec4 aUpdate, vec4 bUpdate)
{
	//1. Cari dulu vector dari A -> B
	//2. Hitung angle dari A -> B yang belum terupdate dengan yang sudah terupdate
	//3. Cari angle pakai glm::angle
	//Vector A ke B

	/*GLfloat i_A_to_B = temp[6].getJointVector().x - temp[5].getJointVector().x;
	GLfloat j_A_to_B = temp[6].getJointVector().y - temp[5].getJointVector().y;*/

	GLfloat i_A_to_B = b.x - a.x;
	GLfloat j_A_to_B = b.y - a.y;

	GLfloat i_A_to_B_after_lerp = bUpdate.x - aUpdate.x;
	GLfloat j_A_to_B_after_lerp = bUpdate.y - aUpdate.y;

	/*vectorAtoB_before[0] = vec2(i_A_to_B, j_A_to_B);
	vectorAtoB_after[0] = vec2(i_A_to_B_after_lerp, j_A_to_B_after_lerp);*/

	vec2 vectorAtoB_before = vec2(i_A_to_B, j_A_to_B);
	vec2 vectorAtoB_after = vec2(i_A_to_B_after_lerp, j_A_to_B_after_lerp);

	//Panjang Vector A ke B
	GLfloat lengthVectorFromAtoB = length(vectorAtoB_before);
	GLfloat lengthVectorFromAtoBAfterLerp = length(vectorAtoB_after);

	//Perkalian Panjang Vector A B
	GLfloat vectorLengthMultiply = lengthVectorFromAtoB * lengthVectorFromAtoBAfterLerp;

	//Dot Product A B
	GLfloat DotProductAtoB = dot(vectorAtoB_before[0], vectorAtoB_after[0]);

	//Theta dari DotProduct dibagi Panjang Vector
	GLfloat theta = acos((DotProductAtoB) / vectorLengthMultiply);

	return theta;
}

#pragma endregion