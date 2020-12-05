#include "game/state/luagamestate.h"
#include "framework/configfile.h"
#include "framework/framework.h"
#include "framework/luaframework.h"
#include "game/state/gamestate.h"
#include "game/state/luagamestate_support.h"
#include "game/state/luagamestate_support_generated.h"
#include <sol/sol.hpp>

namespace OpenApoc
{

class LuaGameState::LuaGameStateImpl
{
};

LuaGameState::LuaGameState() : _impl(new LuaGameStateImpl{}) {}

LuaGameState::~LuaGameState() { delete _impl; }

void LuaGameState::init(GameState &state) {}

bool LuaGameState::runScript(const UString &scriptPath) { return false; }

} // namespace OpenApoc
