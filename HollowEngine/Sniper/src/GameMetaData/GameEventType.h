#pragma once
enum class GameEventType
{
#define GAME_EVENT(name) name,
#include "GameEvents.enum"
#undef GAME_EVENT
	NUM
};
