#pragma once
#include "Hollow/Systems/System.h"

namespace BulletHell
{
	class HandSystem : public Hollow::System
	{
		REGISTERSYSTEM(HandSystem, 4);
	public:
		void Init() override;
		void OnSceneInit() override;
		void Update() override;
		void AddGameObject(Hollow::GameObject* pGameObject) override;

        void OnDeleteAllGameObjects() override;
		void HandleBroadcastEvent(Hollow::GameEvent& event) override;
        static Hollow::GameObject* CreateUIObject(const glm::vec2& scale, const glm::vec2& position, const std::string& texturePath);
		static Hollow::GameObject* CreateCooldownUIObject(const glm::vec2& scale, const glm::vec2& position, const std::string& texturePath);
	    
	private:
		void CreateUIObjects();
		void SubscribeToEvents();

		void CheckHandUI();

		void CycleSpell(Hollow::GameEvent& event);

	private:
		Hollow::GameObject* mpLeftHandUI;
		Hollow::GameObject* mpRightHandUI;
		Hollow::GameObject* mpCombinedHandUI;
		Hollow::GameObject* mpCombinedCooldownUI;

		Hollow::GameObject* mpLeftHand;
		Hollow::GameObject* mpRightHand;

		Hollow::GameObject* mpPlayerObject;
	};
}