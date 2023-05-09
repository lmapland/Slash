#pragma once

UENUM(BlueprintType)
enum class EObjectiveType : uint8
{
	EOT_EnemyKilled,
	EOT_ItemAdded,
	EOT_ItemEquipped,
	EOT_ItemUsed,
	EOT_AttributeUpdated,
	EOT_LeveledUp
};