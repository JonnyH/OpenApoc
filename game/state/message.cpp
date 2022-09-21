#include "game/state/message.h"

#include <utility>

#include "battle/battle.h"
#include "battle/battleunit.h"
#include "city/building.h"
#include "city/vehicle.h"
#include "game/state/gamestate.h"
#include <utility>

namespace OpenApoc
{

const Vec3<int> EventMessage::NO_LOCATION = {-1, -1, -1};

EventMessage::EventMessage(GameTime time, UString text, Vec3<int> location)
    : time(time), text(std::move((text))), location(location)
{
}

EventMessage::EventMessage(GameTime time, UString text)
    : EventMessage(time, std::move(text), NO_LOCATION)
{
}

}; // namespace OpenApoc
