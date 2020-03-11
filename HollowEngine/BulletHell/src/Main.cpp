#include <Hollow.h>
#include "Hollow/Common.h"
#include "Hollow/Core/EntryPoint.h"

#include "Hollow/Managers/GameObjectManager.h"
#include "Hollow/Managers/MemoryManager.h"
#include "Hollow/Managers/SceneManager.h"
#include "Hollow/Managers/RenderManager.h"
#include "Hollow/Managers/ResourceManager.h"
#include "Hollow/Managers/EventManager.h"
#include "Hollow/Managers/ScriptingManager.h"

#include "Components/Health.h"
#include "Components/ParentOffset.h"

#include "GameMetaData/GameObjectType.h"
#include "GameMetaData/GameEventType.h"
#include "Hollow/Core/GameMetaData.h"

#include "DungeonGeneration/DungeonManager.h"
#include "Systems/HandSystem.h"

#include "Hollow/Components/Body.h"
#include "Hollow/Components/UITransform.h"
#include "GameLogic/GameLogicManager.h"

void Hollow::GameMetaData::Init()
{
	{
#define GAMEOBJECT_TYPE(name) mMapOfGameObjectTypes[#name] = (int)BulletHell::GameObjectType::name;
#include "GameMetaData/GameObjectType.enum"
#undef GAMEOBJECT_TYPE
	}

	{
#define GAME_EVENT(name) mMapOfGameEventTypes[#name] = (int)BulletHell::GameEventType::name;
#include "GameMetaData/GameEvents.enum"
#undef GAME_EVENT
	}
}

class GameLayer : public Hollow::Layer
{
	void OnUpdate(float dt)
	{
		if (BulletHell::GameLogicManager::hasGameStarted)
		{
			// Update Game managers here
			Hollow::ScriptingManager::Instance().RunScript("RoomLogic");
		}
	}
};

class BulletHellGame : public Hollow::Application
{
public:
	BulletHellGame()
	{
		Hollow::GameMetaData::Instance().Init();
		// Engine Initialization
		Application::Init("Resources/Settings.json");

		PushLayer(new GameLayer());
		BulletHell::GameLogicManager::Instance().Init();
	}

	~BulletHellGame()
	{
		HW_TRACE("BulletHell Prototype Closing");
	}

};


Hollow::Application* Hollow::CreateApplication()
{
	return new BulletHellGame();
}