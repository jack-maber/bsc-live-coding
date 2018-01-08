//main.cpp - defines the entry point of the application

#include "main.h"


#pragma region "Initilisation"
int main(int argc, char* args[])
{
	//Initialises the SDL Library, passing in SDL_INIT_VIDEO to only initialise the video subsystems
	//https://wiki.libsdl.org/SDL_Init
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		//Display an error message box
		//https://wiki.libsdl.org/SDL_ShowSimpleMessageBox
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, SDL_GetError(), "SDL_Init failed", NULL);
		return 1;
	}

	//Create an SDL2 Window
	//https://wiki.libsdl.org/SDL_CreateWindow
	SDL_Window* window = SDL_CreateWindow("SDL2 Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 640, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
	//Checks to see if the window has been created, the pointer will have a value of some kind
	if (window == nullptr)
	{
		//Shows error if creation of Window Fails
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, SDL_GetError(), "SDL_CreateWindow failed", NULL);
		//Close the SDL Library
		//https://wiki.libsdl.org/SDL_Quit
		SDL_Quit();
		return 1;
	}

	//Gets 3.2 version of OPENGL
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	//SDL Context/Window is created here
	SDL_GLContext GL_Context = SDL_GL_CreateContext(window);
	if (GL_Context == nullptr)
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, SDL_GetError(), "SDL GL Create Context failed", NULL);
		SDL_DestroyWindow(window);
		SDL_Quit();
		return 1;
	}
	
	//Initialize GLEW
	glewExperimental = GL_TRUE;
	GLenum glewError = glewInit();
	if (glewError != GLEW_OK)
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, (char*)glewGetErrorString(glewError), "GLEW Init Failed", NULL);
	}

#pragma endregion

#pragma region "Variables"
	//Sets Up camera variables 
	vec3 cameraPosition = vec3(10.0f, 0.0f, 0.0f);
	vec3 cameraTarget = vec3(0.0f, 0.0f, 0.0f);
	vec3 cameraUp = vec3(0.0f, 1.0f, 0.0f);
	vec3 cameraDirection = vec3(0.0f);
	vec3 FPScameraPos = vec3(0.0f);
	float CameraX = 0.0f;
	float CameraY = 0.0f;
	float CameraDistance = (float)(cameraTarget - cameraPosition).length();

	//Sets Matrixes for Camera
	mat4 viewMatrix = lookAt(cameraPosition, cameraTarget, cameraUp);
	mat4 projectionMatrix = perspective(radians(90.0f), float(800 / 600), 0.1f, 100.0f);
	
	
	//Light Variables
	vec4 ambientLightColour = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	vec3 lightDirection = vec3(0.0f, 0.0f, -5.0f);
	vec4 diffuseLightColour = vec4(2.0f, 2.0f, 2.0f, 2.0f);
	vec4 specularLightColour = vec4(2.0f, 2.0f, 2.0f, 2.0f);

#pragma endregion 
	
#pragma region "GameObjects"	
	//Creates ObjectList for desired Rendered Objects to reside in 
	std::vector<GameObject*> gameObjectList;


	//Create GameObjects
	GameObject * pCar = new GameObject();
	pCar->setPosition(vec3(0.5f, 0.0f, 0.0f));
	pCar->loadMeshesFromFile("armoredrecon.fbx");
	pCar->loadDiffuseTextureFromFile("armoredrecon_diff.png");
	pCar->loadShaderProgram("textureVert.glsl", "textureFrag.glsl");
	gameObjectList.push_back(pCar);

	GameObject * Tank1 = new GameObject();
	Tank1->setPosition(vec3(-5.0f, 0.0f, 0.0f));
	Tank1->loadMeshesFromFile("Tank1.FBX");
	Tank1->loadDiffuseTextureFromFile("Tank1DF.png");
	Tank1->loadShaderProgram("lightingVert.glsl", "lightingFrag.glsl");
	Tank1->setRotation(vec3(0.0f, 1.6f, 0.0f));
	gameObjectList.push_back(Tank1);



#pragma endregion	
	
#pragma region Buffer and Screen Declerations
	//Colour Buffer Texture
	GLuint colourBufferID = createTexture(800, 640);

	//Create Depth Buffer
	GLuint depthRenderBufferID;
	glGenRenderbuffers(1, &depthRenderBufferID);
	glBindRenderbuffer(GL_RENDERBUFFER, depthRenderBufferID);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, 800, 640);

	//Create Framebuffer
	GLuint frameBufferID;
	glGenFramebuffers(1, &frameBufferID);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderBufferID);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, colourBufferID, 0);

	//Error checking for Frame Buffer for PP
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Unable to create frame buffer for post processing", "Frame Buffer Error", NULL);
	}

	//Create Screen Sized Texture for Post Processing 
	GLfloat screenVerts[] =
	{
		-1, -1,
		1, -1,
		-1, 1,
		1, 1
	};

	//Creates Screen Quad for PP
	GLuint screenQuadVBOID;
	glGenBuffers(1, &screenQuadVBOID);
	glBindBuffer(GL_ARRAY_BUFFER, screenQuadVBOID);
	glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), screenVerts, GL_STATIC_DRAW);

	//Creates Vertex Array for PP
	GLuint screenVAO;
	glGenVertexArrays(1, &screenVAO);
	glBindVertexArray(screenVAO);
	glBindBuffer(GL_ARRAY_BUFFER, screenQuadVBOID);

	//Enable Vertex Array
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);

	//Loads Post Proccesing Shaders and Texture it is loaded onto 
	GLuint postProcessingProgramID = LoadShaders("passThroughVert.glsl", "postBlackAndWhite.glsl");
	GLint texture0Location = glGetUniformLocation(postProcessingProgramID, "texture0");
#pragma endregion	
#pragma region Physics

	//Initalise Bullet

	///collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
	btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();

	///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
	btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);

	///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
	btBroadphaseInterface* overlappingPairCache = new btDbvtBroadphase();

	///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
	btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver;

	btDiscreteDynamicsWorld* dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);

	//Sets 
	dynamicsWorld->setGravity(btVector3(0, -2, 0));

	//Creates Floor
	btCollisionShape* groundShape = new btBoxShape(btVector3(btScalar(50.), btScalar(2.), btScalar(50.)));

	btTransform groundTransform;
	groundTransform.setIdentity();
	groundTransform.setOrigin(btVector3(0, -10, 0));

	//Having Zero Mass means the object will not move when hit
	btScalar mass(0.);

	btVector3 localInertia(0, 0, 0);

	//using motionstate is optional, it provides interpolation capabilities, and only synchronizes 'active' objects
	btDefaultMotionState* myMotionState = new btDefaultMotionState(groundTransform);
	//Constructs Rigidbody on info passed
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, groundShape, localInertia);
	btRigidBody* GroundRigidbody = new btRigidBody(rbInfo);

	//add the body to the dynamics world, dynamicworld holds all the simulations
	dynamicsWorld->addRigidBody(GroundRigidbody);

	//Creates Hitbox for Car
	btCollisionShape* carCollisionShape = new btBoxShape(btVector3(2, 2, 2));

	//Sets car position to current Model position for Physics 
	glm::vec3 carPosition = pCar->getPosition();


	/// Create Dynamic Objects
	btTransform carTransform;
	carTransform.setIdentity();
	//Sets hitbox position to same as the model location in the world space 
	carTransform.setOrigin(btVector3(carPosition.x, carPosition.y, carPosition.z));
	btVector3 carInertia(0, 0, 0);
	btScalar carMass(0.7f);
	carCollisionShape->calculateLocalInertia(carMass, carInertia);

	//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
	btDefaultMotionState* carMotionState = new btDefaultMotionState(carTransform);
	btRigidBody::btRigidBodyConstructionInfo carRbInfo(carMass, carMotionState, carCollisionShape, carInertia);
	btRigidBody* carRigidbody = new btRigidBody(carRbInfo);
	carRigidbody->setActivationState(DISABLE_DEACTIVATION);

	//Adds Car Ridgidbody to Dynamicworld 
	dynamicsWorld->addRigidBody(carRigidbody);

	//Sets Impulse Direction 
	int InvertGravity = -10;
	pCar->SetRigidbody(carRigidbody);
	pCar->SetCollision(carCollisionShape);

	
	
#pragma endregion

#pragma region Raycast
	//Raycast code comes from :http://www.opengl-tutorial.org/miscellaneous/clicking-on-objects/picking-with-a-physics-library/

	// The ray Start and End positions, in Normalized Device Coordinates (Have you read Tutorial 4 ?)
	glm::vec4 lRayStart_NDC(
		((float)CameraX / (float)640 - 0.5f) * 2.0f, // [0,1024] -> [-1,1]
		((float)CameraY / (float)800 - 0.5f) * 2.0f, // [0, 768] -> [-1,1]
		-1.0, // The near plane maps to Z=-1 in Normalized Device Coordinates
		1.0f
	);
	glm::vec4 lRayEnd_NDC(
		((float)CameraX / (float)640 - 0.5f) * 2.0f,
		((float)CameraY / (float)800 - 0.5f) * 2.0f,
		0.0,
		1.0f
	);

	// The Projection matrix goes from Camera Space to NDC.
	// So inverse(ProjectionMatrix) goes from NDC to Camera Space.
	glm::mat4 InverseProjectionMatrix = glm::inverse(projectionMatrix);

	// The View Matrix goes from World Space to Camera Space.
	// So inverse(ViewMatrix) goes from Camera Space to World Space.
	glm::mat4 InverseViewMatrix = glm::inverse(viewMatrix);

	glm::vec4 lRayStart_camera = InverseProjectionMatrix * lRayStart_NDC;    lRayStart_camera /= lRayStart_camera.w;
	glm::vec4 lRayStart_world = InverseViewMatrix       * lRayStart_camera; lRayStart_world /= lRayStart_world.w;
	glm::vec4 lRayEnd_camera = InverseProjectionMatrix * lRayEnd_NDC;      lRayEnd_camera /= lRayEnd_camera.w;
	glm::vec4 lRayEnd_world = InverseViewMatrix       * lRayEnd_camera;   lRayEnd_world /= lRayEnd_world.w;

	glm::vec3 lRayDir_world(lRayEnd_world - lRayStart_world);
	lRayDir_world = glm::normalize(lRayDir_world);
#pragma endregion
	
	//Locks cursor to OPENGL screen 
	SDL_SetRelativeMouseMode(SDL_bool(SDL_ENABLE));

	
	
	//Timing Declarations
	int lastTicks = SDL_GetTicks();
	int currentTicks = SDL_GetTicks();


	//Event loop, we will loop until running is set to false, usually if escape has been pressed or window is closed
	bool running = true;
	//SDL Event structure, this will be checked in the while loop
	SDL_Event ev;
	while (running)
	{
		//Poll for the events which have happened in this frame
		//https://wiki.libsdl.org/SDL_PollEvent
		while (SDL_PollEvent(&ev))
		{
			//Switch case for every message we are intereted in
			switch (ev.type)
			{
				//QUIT Message, usually called when the window has been closed
			case SDL_QUIT:
				running = false;
				break;

			case SDL_MOUSEMOTION:
				// Get Mouse Direction
				CameraX += ev.motion.xrel / 200.0f;
				CameraY += -ev.motion.yrel / 200.0f;
				
				// Limits Camera View
				if (CameraY > 150.0f) CameraY = 150.0f; else if (CameraY < -150.0f) CameraY = -150.0f;
				
				// Calculate camera target 
				cameraTarget = cameraPosition + CameraDistance * vec3(cos(CameraX), tan(CameraY), sin(CameraX));
				
				// Normalises Camera Direction 
				cameraDirection = normalize(cameraTarget - cameraPosition);
				break;

				//KEYDOWN Message, called when a key has been pressed down
			case SDL_KEYDOWN:

				//Check the actual key code of the key that has been pressed
				switch (ev.key.keysym.sym)
				{
					// Keys
				case SDLK_ESCAPE:
					running = false;
					break;
				
					//Camera Movement Keys
				case SDLK_w:
					FPScameraPos = cameraDirection * 0.2f;
					break;
				case SDLK_s:
					FPScameraPos = -cameraDirection * 0.2f;
					break;
				case SDLK_a:
					FPScameraPos = -cross(cameraDirection, cameraUp) * 0.5f;
					break;
				case SDLK_d:
					FPScameraPos = cross(cameraDirection, cameraUp) * 0.5f;
					break;
				
				case SDLK_t:
					GameObject * pCar = new GameObject();
					pCar->setPosition(vec3(0.5f, 0.0f, 0.0f));
					pCar->loadMeshesFromFile("armoredrecon.fbx");
					pCar->loadDiffuseTextureFromFile("armoredrecon_diff.png");
					pCar->loadShaderProgram("textureVert.glsl", "textureFrag.glsl");
					gameObjectList.push_back(pCar);

					pCar->SetRigidbody;
					
					dynamicsWorld->addRigidBody(pCar->m_rigidBody);
				case SDLK_RIGHT:
					//Apply an impulse to car
					carRigidbody->applyCentralForce(btVector3(-5, 0, 0) * 50);
					break;
				
				case SDLK_LEFT:
					//Apply an impulse to car in key direction
					carRigidbody->applyCentralForce(btVector3(5, 0, 0) * 50);
					break;
				
				case SDLK_UP:
					//Apply an impulse to car in key direction
					carRigidbody->applyCentralForce(btVector3(0, 5, 0) * 50);
					break;
				
				case SDLK_DOWN:
					//Apply an impulse to car in key direction
					carRigidbody->applyCentralForce(btVector3(0, -5, 0) * 50);
					break;

				case SDLK_SPACE:
					//Invert gravity in simulation 
					InvertGravity *= -1;
					dynamicsWorld->setGravity(btVector3(0.0, InvertGravity, 0.0));
					break;
				

				case SDLK_LCTRL:
					//Raycast Controls
					glm::vec3 out_direction = cameraTarget - cameraPosition;
					out_direction = glm::normalize(out_direction);
					glm::vec3 out_end = cameraPosition + out_direction*1000.0f;

					btCollisionWorld::ClosestRayResultCallback RayCallback(
						btVector3(cameraPosition.x, cameraPosition.y, cameraPosition.z),
						btVector3(out_end.x, out_end.y, out_end.z)
					);
					dynamicsWorld->rayTest(
						btVector3(cameraPosition.x, cameraPosition.y, cameraPosition.z),
						btVector3(out_end.x, out_end.y, out_end.z),
						RayCallback
					);

					if (RayCallback.hasHit()) {
						printf("Hit Mesh %i", (int)RayCallback.m_collisionObject->getUserPointer());
						
					}
					else {
						printf("Not Hit");
						
					}
					break;
				}
				//Updates Camera Position 
				cameraPosition += FPScameraPos;
				cameraTarget += FPScameraPos;
			}
		}

		//Gets Tick and Steps through simulation
		currentTicks = SDL_GetTicks();
		float deltaTime = (float)(currentTicks - lastTicks) / 1000.0f;
		dynamicsWorld->stepSimulation(1.f / 60.f, 10);

		//Sets View Matrix
		viewMatrix = lookAt(cameraPosition, cameraTarget, cameraUp);

		//Iterates through game objects in the list then updates the render command 
		for (GameObject * pObj : gameObjectList)
		{
			pObj->update();
		}
	
		//Enables Depth Test and backface culling to save on processing 
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		
		//Changes Background Colour
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClearDepth(1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Passes through GameObject list and renders each of them 
		for (GameObject * pObj : gameObjectList)
		{
			pObj->preRender();
			GLuint currentProgramID = pObj->getShaderProgramID();

			//retrieve the shader values
			GLint viewMatrixLocation = glGetUniformLocation(currentProgramID, "viewMatrix");
			GLint projectionMatrixLocation = glGetUniformLocation(currentProgramID, "projectionMatrix");
			GLint lightDirectionLocation = glGetUniformLocation(currentProgramID, "lightDirection");
			GLint ambientLightColourLocation = glGetUniformLocation(currentProgramID, "ambientLightColour");
			GLint diffuseLightColourLocation = glGetUniformLocation(currentProgramID, "diffuseLightColour");
			GLint specularLightColourLocation = glGetUniformLocation(currentProgramID, "specularLightColour");

			//send shader values
			glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, value_ptr(viewMatrix));
			glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, value_ptr(projectionMatrix));

			glUniform3fv(lightDirectionLocation, 1, value_ptr(lightDirection));
			glUniform4fv(ambientLightColourLocation, 1, value_ptr(ambientLightColour));
			glUniform4fv(diffuseLightColourLocation, 1, value_ptr(diffuseLightColour));
			glUniform4fv(specularLightColourLocation, 1, value_ptr(specularLightColour));

			pObj->render();
		}


		//Swaps Window for next rendered window 
		SDL_GL_SwapWindow(window);
		
		//Updated Last tick to current ticks 
		lastTicks = currentTicks;
	}
	
#pragma region "Delete"	
	//Remove Rigidbodys from simulation
	int NoOfCollisionObjects=dynamicsWorld->getNumCollisionObjects();

	for (int i = 0; i < NoOfCollisionObjects -1; i++)
	{
		btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[i];
		dynamicsWorld->removeCollisionObject(obj);
	}

	//Iterates through models and deletes them
	auto gameObjectIter = gameObjectList.begin();
	while (gameObjectIter != gameObjectList.end())
	{
		if ((*gameObjectIter))
		{
			(*gameObjectIter)->destroy();
			delete (*gameObjectIter);
			gameObjectIter = gameObjectList.erase(gameObjectIter);
		}
	}
	
	//All the deleting goes on down here 
	glDeleteProgram(postProcessingProgramID);
	glDeleteVertexArrays(1, &screenVAO);
	glDeleteBuffers(1, &screenQuadVBOID);
	glDeleteFramebuffers(1, &frameBufferID);
	glDeleteRenderbuffers(1, &depthRenderBufferID);
	glDeleteTextures(1, &colourBufferID);

	//Deletes GL_CONTEXT/Window
	SDL_GL_DeleteContext(GL_Context);
	
	// Delete Ground
		delete groundShape;
	if (GroundRigidbody->getMotionState() != NULL)
	{
		delete GroundRigidbody->getMotionState();
	}
	delete GroundRigidbody;

	//delete solver
	delete solver;

	//delete broadphase
	delete overlappingPairCache;

	//delete dispatcher
	delete dispatcher;

	delete collisionConfiguration;


	//Destroy the window and quit SDL2
	//https://wiki.libsdl.org/SDL_DestroyWindow
	SDL_DestroyWindow(window);
	//https://wiki.libsdl.org/SDL_Quit
	SDL_Quit();

	return 0;
}
#pragma endregion 