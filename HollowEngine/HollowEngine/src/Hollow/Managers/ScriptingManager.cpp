#include <hollowpch.h>
#include "ScriptingManager.h"

#include "Hollow/Components/Body.h"
#include "Hollow/Components/Camera.h"
#include "Hollow/Components/Transform.h"
#include "Hollow/Components/Material.h"
#include "Hollow/Components/Light.h"
#include "Hollow/Components/Script.h"
#include "Hollow/Components/Collider.h"
#include "Hollow/Components/UITransform.h"

#include "Hollow/Physics/Broadphase/Shape.h"

#include "Hollow/Managers/ResourceManager.h"
#include "Hollow/Managers/InputManager.h"
#include "Hollow/Managers/PhysicsManager.h"
#include "Hollow/Managers/AudioManager.h"
#include "Hollow/Managers/FrameRateController.h"
#include "Hollow/Systems/ParticleSystem.h"
#include "Hollow/Managers/RenderManager.h"
#include "Hollow/Components/UIImage.h"

namespace Hollow
{
	void ScriptingManager::RunScript(std::string name, GameObject* pGameObject, std::string folderName)
	{
		lua["gameObject"] = pGameObject;
		lua.script_file(rootPath + folderName + name + ext);
	}

	void ScriptingManager::Init(rapidjson::Value::Object& data)
	{
		rootPath = data["FilePath"].GetString();
		ext = data["FileExtension"].GetString();

		lua.open_libraries(sol::lib::base, sol::lib::math, sol::lib::bit32);

		// Controller
		lua["CONTROLLER"] = lua.create_table_with(
			"A", SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_A,
			"B", SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_B,
			"X", SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_X,
			"Y", SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_Y,
			"Back", SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_BACK,
			"Home", SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_GUIDE,
			"Start", SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_START,
			"LS", SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_LEFTSTICK,
			"RS", SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_RIGHTSTICK,
			"LB", SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_LEFTSHOULDER,
			"RB", SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_RIGHTSHOULDER,
			"DUP", SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_DPAD_UP,
			"DDOWN", SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_DPAD_DOWN,
			"DLEFT", SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_DPAD_LEFT,
			"DRIGHT", SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_DPAD_RIGHT,
			"LX",SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_LEFTX,
			"LY",SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_LEFTY,
			"RX",SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_RIGHTX,
			"RY",SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_RIGHTY,
			"LT",SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_TRIGGERLEFT,
			"RT",SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_TRIGGERRIGHT
		);

		// COMMON DATA TYPES
		auto mult_overloads = sol::overload(
			[](const glm::vec3& v1, const glm::vec3& v2) -> glm::vec3 { return v1 * v2; },
			[](const glm::vec3& v1, float f) -> glm::vec3 { return v1 * f; },
			[](float f, const glm::vec3& v1) -> glm::vec3 { return f * v1; }
		);
		
		lua.new_usertype<glm::vec3>("vec3",
			sol::constructors<glm::vec3(), glm::vec3(float), glm::vec3(float, float, float)>(),
			"x", &glm::vec3::x,
			"y", &glm::vec3::y,
			"z", &glm::vec3::z,
			"__add", [](const glm::vec3& l, const glm::vec3& r) { return glm::vec3(l.x + r.x, l.y + r.y, l.z + r.z); },
			"__sub", [](const glm::vec3& l, const glm::vec3& r) { return glm::vec3(l.x - r.x, l.y - r.y, l.z - r.z); },
			sol::meta_function::multiplication, mult_overloads
			);

		auto mult_overloads4 = sol::overload(
			[](const glm::vec4& v1, const glm::vec4& v2) -> glm::vec4 { return v1 * v2; },
			[](const glm::vec4& v1, float f) -> glm::vec4 { return v1 * f; },
			[](float f, const glm::vec4& v1) -> glm::vec4 { return f * v1; }
		);

		lua.new_usertype<glm::vec4>("vec4",
			sol::constructors<glm::vec4(), glm::vec4(float), glm::vec4(float, float, float, float)>(),
			"x", &glm::vec4::x,
			"y", &glm::vec4::y,
			"z", &glm::vec4::z,
			"w", &glm::vec4::w,
			"__add", [](const glm::vec4& l, const glm::vec4& r) { return glm::vec4(l.x + r.x, l.y + r.y, l.z + r.z, l.w + r.w); },
			"__sub", [](const glm::vec4& l, const glm::vec4& r) { return glm::vec4(l.x - r.x, l.y - r.y, l.z - r.z, l.z - r.z); },
			sol::meta_function::multiplication, mult_overloads4
			);

		auto mult_overloads2 = sol::overload(
			[](const glm::vec2& v1, const glm::vec2& v2) -> glm::vec2 { return v1 * v2; },
			[](const glm::vec2& v1, float f) -> glm::vec2 { return v1 * f; },
			[](float f, const glm::vec2& v1) -> glm::vec2 { return f * v1; }
		);

		lua.new_usertype<glm::vec2>("vec2",
			sol::constructors<glm::vec2(), glm::vec2(float), glm::vec2(float, float)>(),
			"x", &glm::vec2::x,
			"y", &glm::vec2::y,
			"__add", [](const glm::vec2& l, const glm::vec2& r) { return glm::vec2(l.x + r.x, l.y + r.y); },
			"__sub", [](const glm::vec2& l, const glm::vec2& r) { return glm::vec2(l.x - r.x, l.y - r.y); },
			sol::meta_function::multiplication, mult_overloads2
			);

		auto mult_overloads3 = sol::overload(
			[](const glm::ivec2& v1, const glm::ivec2& v2) -> glm::ivec2 { return v1 * v2; },
			[](const glm::ivec2& v1, int f) -> glm::ivec2 { return v1 * f; },
			[](int f, const glm::ivec2& v1) -> glm::ivec2 { return f * v1; }
		);

		lua.new_usertype<glm::ivec2>("ivec2",
			sol::constructors<glm::ivec2(), glm::ivec2(int), glm::ivec2(int, int)>(),
			"x", &glm::ivec2::x,
			"y", &glm::ivec2::y,
			"__add", [](const glm::ivec2& l, const glm::ivec2& r) { return glm::ivec2(l.x + r.x, l.y + r.y); },
			"__sub", [](const glm::ivec2& l, const glm::ivec2& r) { return glm::ivec2(l.x - r.x, l.y - r.y); },
			sol::meta_function::multiplication, mult_overloads3
		);

		// GLM functions
		lua.set_function("VecCross", [](const glm::vec3& v1, const glm::vec3& v2) -> glm::vec3 { return glm::cross(v1,v2); });
		lua.set_function("VecNormalize", [](const glm::vec3& v) -> glm::vec3 { return glm::normalize(v); });
		lua.set_function("VecLength", [](const glm::vec3& v) -> float { return glm::length(v); });
		
		// COMPONENTS
		lua.new_usertype<Body>("RigidBody",
			sol::constructors<Body()>(),
			"velocity", &Body::mVelocity,
			"angularVelocity", &Body::mAngularVelocity,
			"position", &Body::mPosition,
			"RotateBody", &Body::Rotate,
            "useGravity", &Body::mUseGravity
		);

		lua.new_usertype<Transform>("Transform",
			sol::constructors<Transform()>(),
			"position", &Transform::mPosition,
            "rotation", &Transform::mRotation,
            "forward", &Transform::GetForward,
			"Rotate", &Transform::Rotate,
			"scale", &Transform::mScale
			);

		lua.new_usertype<Camera>("Camera",
			sol::constructors<Camera()>(),
			"frontDirection", &Camera::mFront,
			"rightDirection", &Camera::mRight,
			"yaw", &Camera::mYaw,
			"pitch", &Camera::mPitch,
			"LERPFactor", &Camera::mLERPFactor,
			"PreviousPosition", &Camera::mPreviousPosition,
			"FocusPositions", &Camera::mFocusPositions
			);

		lua.new_usertype<Material>("Material",
			sol::constructors<Material()>(),
			"diffuse", &Material::mDiffuseColor,
			"alphaValue", &Material::mAlphaValue
			);

		lua.new_usertype<Light>("Light",
			sol::constructors<Light()>(),
			"lookAt", &Light::mLookAtPoint,
			"color", &Light::mColor,
			"radius", &Light::mRadius,
			"position", &Light::mPosition
			);

		lua.new_usertype<UITransform>("UITransform",
			sol::constructors<UITransform()>(),
			"position", &UITransform::mPosition,
			"scale", &UITransform::mScale,
			"layer", &UITransform::mLayer,
			"rotation", &UITransform::mRotation,
			"tilt", &UITransform::mTilt
			);

		lua.new_usertype<UIImage>("UIImage",
			sol::constructors<UIImage()>(),
			"imagePath", &UIImage::TexturePath,
			"alpha", &UIImage::mAlpha,
			"color", &UIImage::mColor,
			"isVisible", &UIImage::mIsVisible,
			"SetTexture", &UIImage::SetTexture
			);

		// GAMEOBJECT
        mGameObjectType = lua.new_usertype<GameObject>("GameObject",
            sol::constructors<GameObject()>(),
            "GetBody", &GameObject::GetComponent<Body>,
            "GetTransform", &GameObject::GetComponent<Transform>,
            "GetMaterial", &GameObject::GetComponent<Material>,
            "GetScript", &GameObject::GetComponent<Script>,
			"GetCollider", &GameObject::GetComponent<Collider>,
            "isActive", &GameObject::mActive,
			"GetCamera", &GameObject::GetComponent<Camera>,
			"GetLight", &GameObject::GetComponent<Light>,
			"GetUITransform", &GameObject::GetComponent<UITransform>,
			"GetUIImage", &GameObject::GetComponent<UIImage>,
			"tag", &GameObject::mTag,
            "type", &GameObject::mType,
			"id", &GameObject::mID
			);

		lua.set_function("CreateGameObject", &ResourceManager::LoadGameObjectFromFile, std::ref(ResourceManager::Instance()));
		lua.set_function("CreatePrefabAtPosition", &ResourceManager::LoadPrefabAtPosition, std::ref(ResourceManager::Instance()));
		lua.set_function("CreateScaledPrefabAtPosition", &ResourceManager::LoadScaledPrefabAtPosition, std::ref(ResourceManager::Instance()));

		// PHYSICS
		lua.set_function("ApplyLinearImpulse", &PhysicsManager::ApplyLinearImpulse, std::ref(PhysicsManager::Instance()));
		lua.set_function("ApplyAngularImpulse", &PhysicsManager::ApplyAngularImpulse, std::ref(PhysicsManager::Instance()));
		lua.set_function("ChangeRadius", &PhysicsManager::UpdateScale, std::ref(PhysicsManager::Instance()));
		lua.set_function("ClearParticleMemory", &RenderManager::ClearParticleMemory, std::ref(RenderManager::Instance()));

		// AUDIO
		lua.set_function("PlaySFX", &AudioManager::PlayEffect, std::ref(AudioManager::Instance()));
		lua.set_function("Mute", &AudioManager::Mute, std::ref(AudioManager::Instance()));

		// INPUT
		lua.set_function("IsKeyPressed", &InputManager::IsKeyPressed, std::ref(InputManager::Instance()));
		lua.set_function("IsKeyTriggered", &InputManager::IsKeyTriggered, std::ref(InputManager::Instance()));
		lua.set_function("IsKeyReleased", &InputManager::IsKeyReleased, std::ref(InputManager::Instance()));
		lua.set_function("GetAxis", &InputManager::GetAxisValue, std::ref(InputManager::Instance()));
		lua.set_function("IsControllerButtonPressed", &InputManager::IsControllerButtonPressed, std::ref(InputManager::Instance()));
		lua.set_function("IsControllerButtonTriggered", &InputManager::IsControllerButtonTriggered, std::ref(InputManager::Instance()));
		lua.set_function("IsControllerButtonReleased", &InputManager::IsControllerButtonReleased, std::ref(InputManager::Instance()));
		lua.set_function("IsControllerTriggerPressed", &InputManager::IsControllerTriggerPressed, std::ref(InputManager::Instance()));
		lua.set_function("IsControllerTriggerTriggered", &InputManager::IsControllerTriggerTriggered, std::ref(InputManager::Instance()));
		lua.set_function("IsControllerTriggerReleased", &InputManager::IsControllerTriggerReleased, std::ref(InputManager::Instance()));

		//FRAMERATE CONTROLLER
		lua.set_function("GetFrameTime", &FrameRateController::GetFrameTime, std::ref(FrameRateController::Instance()));

		//Particle System
		lua.set_function("ChangeParticleShader", &ParticleSystem::ChangeComputeShader, std::ref(*static_cast<ParticleSystem*>(SystemManager::Instance().GetSystem<ParticleSystem>())));

	}

}
