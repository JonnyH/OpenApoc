#pragma once

#include "library/strings.h"

namespace OpenApoc
{

class GameState;

class LuaGameState
{
  private:
	class LuaGameStateImpl;
	LuaGameStateImpl *_impl = nullptr;

  public:
	enum class Hook
	{
		ModLoad,
		NewGame,
		GameStart,
		EndTick,
		EndSecond,
		EndMinute,
		EndHour,
		EndDay,
		EndWeek,
		StartBattle,
		EndBattle,
	};
	LuaGameState();
	~LuaGameState();

	void init(GameState &game);

	bool addHook(Hook type, const UString &scriptPath);
	bool callHook(Hook type);
	bool runScript(const UString &scriptPath);
};
} // namespace OpenApoc
