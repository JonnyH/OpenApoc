#include "game/state/battle/ai/aidecision.h"
#include "game/state/battle/battleunit.h"
#include "game/state/shared/aequipment.h"
#include "library/strings_format.h"

namespace OpenApoc
{

AIAction::AIAction() : weaponStatus(WeaponStatus::NotFiring) {}

AIMovement::AIMovement() : movementMode(MovementMode::Walking), kneelingMode(KneelingMode::None) {}

AIDecision::AIDecision(sp<AIAction> action, sp<AIMovement> movement)
    : action(action), movement(movement)
{
}

UString AIAction::getName()
{
	switch (type)
	{
		case AIAction::Type::AttackGrenade:
			return OpenApoc::format2("Attack {0} with grenade {1} ", targetUnit->id,
			                         item->type->id);
		case AIAction::Type::AttackWeaponTile:
			if (item)
			{
				return OpenApoc::format2("Attack {0} with weapon {1} ", targetLocation,
				                         item->type->id);
			}
			else
			{
				return OpenApoc::format2("Attack {0} with weapon(s)", targetLocation);
			}
		case AIAction::Type::AttackWeaponUnit:
			if (item)
			{
				return OpenApoc::format2("Attack {0} with weapon {1} ", targetUnit->id,
				                         item->type->id);
			}
			else
			{
				return OpenApoc::format2("Attack {0} with weapon(s)", targetUnit->id);
			}
		case AIAction::Type::AttackPsiMC:
			return OpenApoc::format2("Attack {0} with psi MC using {1} ", targetUnit->id,
			                         item->type->id);
		case AIAction::Type::AttackPsiStun:
			return OpenApoc::format2("Attack {0} with psi stun using {1} ", targetUnit->id,
			                         item->type->id);
		case AIAction::Type::AttackPsiPanic:
			return OpenApoc::format2("Attack {0} with psi panic using {1} ", targetUnit->id,
			                         item->type->id);
		case AIAction::Type::AttackBrainsucker:
			return OpenApoc::format2("Attack {0} with brainsucker", targetUnit->id);
		case AIAction::Type::AttackSuicide:
			return OpenApoc::format2("Attack (suicide) {0}  ", targetUnit->id);
	}
	LogError2("Unimplemented getName for AIAction {}", (int)type);
	return "";
}

bool AIAction::inProgressInternal(BattleUnit &u)
{
	switch (type)
	{
		case AIAction::Type::AttackGrenade:
			return !u.missions.empty() &&
			       u.missions.front()->type == BattleUnitMission::Type::ThrowItem;
		case AIAction::Type::AttackWeaponTile:
			return u.isAttacking() &&
			       (u.targetingMode == BattleUnit::TargetingMode::TileCenter ||
			        u.targetingMode == BattleUnit::TargetingMode::TileGround) &&
			       u.targetTile == targetLocation;
		case AIAction::Type::AttackWeaponUnit:
			return u.isAttacking() && u.targetingMode == BattleUnit::TargetingMode::Unit &&
			       u.targetUnit == targetUnit;
		case AIAction::Type::AttackPsiMC:
		case AIAction::Type::AttackPsiStun:
		case AIAction::Type::AttackPsiPanic:
			// Even though the attack might already be over, we won't check for it
			// This is intentional. Psi attack is considered "in progress" indefinitely
			// otherwise, especially in TB, when attack is instant, AI would spam attacks and
			// we would rather have them do attacks with a small delay
			// which we introduce via reThinkDelay value
			return u.agent->modified_stats.psi_energy < psiEnergySnapshot;
		case AIAction::Type::AttackBrainsucker:
			return u.current_movement_state == MovementState::Brainsuck ||
			       u.target_body_state == BodyState::Jumping;
		case AIAction::Type::AttackSuicide:
			// If you're not dead yet then you haven't popped yet :)))
			return false;
	}
	return false;
}

bool AIAction::inProgress(BattleUnit &u)
{
	if (inProgressInternal(u))
	{
		executed = true;
		return true;
	}
	return false;
}

bool AIAction::isFinished(BattleUnit &u) { return !inProgress(u) && executed; }

UString AIMovement::getName()
{
	switch (type)
	{
		case AIMovement::Type::Stop:
			return OpenApoc::format2("Stop");
		case AIMovement::Type::ChangeStance:
			return OpenApoc::format2("Change Stance");
		case AIMovement::Type::Patrol:
			return OpenApoc::format2("Move to {0}", targetLocation);
		case AIMovement::Type::Advance:
			return OpenApoc::format2("Advance on target to {0}", targetLocation);
		case AIMovement::Type::Pursue:
			return OpenApoc::format2("Pursue target to {0}", targetLocation);
		case AIMovement::Type::GetInRange:
			return OpenApoc::format2("Get in range, moving to {0}", targetLocation);
		case AIMovement::Type::Retreat:
			return OpenApoc::format2("Retreat to {0}", targetLocation);
		case AIMovement::Type::TakeCover:
			return OpenApoc::format2("Taking cover, moving to {0}", targetLocation);
		case AIMovement::Type::Turn:
			return OpenApoc::format2("Turn to {0}", targetLocation);
	}
	LogError2("Unimplemented getName for AIMovement {}", (int)type);
	return "";
}

bool AIMovement::inProgressInternal(BattleUnit &u)
{
	switch (type)
	{
		case AIMovement::Type::Advance:
		case AIMovement::Type::Pursue:
		case AIMovement::Type::GetInRange:
		case AIMovement::Type::Patrol:
		case AIMovement::Type::Retreat:
		case AIMovement::Type::TakeCover:
			// Must be moving with proper parameters but not yet there
			{
				if (movementMode != u.movement_mode || kneelingMode != u.kneeling_mode)
				{
					return false;
				}
				if (u.missions.empty())
				{
					return false;
				}
				auto &m = u.missions.front();
				if (m->type != BattleUnitMission::Type::GotoLocation)
				{
					return false;
				}
				if (std::abs(m->targetLocation.x - targetLocation.x) > 1 ||
				    std::abs(m->targetLocation.y - targetLocation.y) > 1 ||
				    std::abs(m->targetLocation.z - targetLocation.z) > 1)
				{
					return false;
				}
				return true;
			}
		case AIMovement::Type::Stop:
			// Must be still moving
			return u.isMoving();
		case AIMovement::Type::ChangeStance:
			// Must be still changing stance
			return movementMode != u.movement_mode || kneelingMode != u.kneeling_mode;
		case AIMovement::Type::Turn:
			// Must be turning towards target facing but not yet turned
			{
				if (u.missions.empty())
				{
					return false;
				}
				auto &m = u.missions.front();
				if (m->type != BattleUnitMission::Type::Turn)
				{
					return false;
				}
				return m->targetFacing == BattleUnitMission::getFacing(u, targetLocation);
			}
	}
	return false;
}

bool AIMovement::inProgress(BattleUnit &u)
{
	if (inProgressInternal(u))
	{
		executed = true;
		return true;
	}
	return false;
}

bool AIMovement::isFinished(BattleUnit &u) { return !inProgress(u) && executed; }

UString AIDecision::getName()
{
	return OpenApoc::format2("Action: [{0}] Movement: [{1}]", action ? action->getName() : "NULL",
	                         movement ? movement->getName() : "NULL");
}

bool AIDecision::isEmpty() { return !action && !movement; }
} // namespace OpenApoc