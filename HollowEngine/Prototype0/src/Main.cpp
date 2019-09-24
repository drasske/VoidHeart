#include <Hollow.h>

#include "PrototypeComponent.h"

#include "Hollow/Core/EntryPoint.h"
#include "Hollow/Core/GameObject.h"

#include "Hollow/Managers/GameObjectManager.h"
#include "Hollow/Managers/MemoryManager.h"
#include "Hollow/Managers/RenderManager.h"

#include "Hollow/Components/TestComponent.h"
#include "Hollow/Components/Shape.h"
#include "Hollow/Components/Material.h"

// Quoting theCherno: "A layer basically handles events and draws stuff"
// The game layer could draw the entire 3d scene
class GameLayer : public Hollow::Layer
{
	void OnUpdate(float dt) override
	{
		
	}

	void OnEvent(Hollow::Event& e) override
	{
		HW_TRACE("{0}", e);
	}
};

// UI layer draws the UI
class UILayer : public Hollow::Layer
{
	void OnUpdate(float dt) override
	{

	}

	void OnEvent(Hollow::Event& e) override
	{
		// the UI layer handled the mouse button pressed event so it is not
		// propagated further to the game layer
		if(e.GetEventType() == Hollow::EventType::MouseButtonPressed)
		{
			e.Handled = true;
		}
		HW_TRACE("{0}", e);
	}
};

class Prototype0 : public Hollow::Application
{
public:
	Prototype0()
	{
		PushLayer(new GameLayer());
		PushOverlay(new UILayer());
		
		Hollow::GameObject* obj1 = Hollow::MemoryManager::Instance().NewGameObject();
		Hollow::GameObject* obj2 = Hollow::MemoryManager::Instance().NewGameObject();
		Hollow::TestComponent* test = static_cast<Hollow::TestComponent*>(Hollow::MemoryManager::Instance().NewComponent("TestComponent"));
		PrototypeComponent* proto = static_cast<PrototypeComponent*>(Hollow::MemoryManager::Instance().NewComponent("PrototypeComponent"));

		obj1->AddComponent(test);
		obj2->AddComponent(proto);
		Hollow::GameObjectManager::Instance().AddGameObject(obj1);
		Hollow::GameObjectManager::Instance().AddGameObject(obj2);

		// TESTING ImGui Debug
		Hollow::GameObject* obj3 = Hollow::MemoryManager::Instance().NewGameObject();
		Hollow::Shape* shape = static_cast<Hollow::Shape*>(Hollow::MemoryManager::Instance().NewComponent("Shape"));
		Hollow::Material* mat = static_cast<Hollow::Material*>(Hollow::MemoryManager::Instance().NewComponent("Material"));
		obj3->AddComponent(shape);
		obj3->AddComponent(mat);
		Hollow::GameObjectManager::Instance().AddGameObject(obj3);
		Hollow::RenderManager::Instance().mShapes.push_back(shape);
		Hollow::RenderManager::Instance().mMaterials.push_back(mat);
	}
	
	~Prototype0()
	{
	}
};


Hollow::Application* Hollow::CreateApplication()
{
	return new Prototype0();
}