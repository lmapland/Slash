#pragma once

UENUM(BlueprintType)
enum class EObjectiveType : uint8
{
	EOT_EnemyKilled,
	EOT_ItemAdded,
	EOT_ItemEquipped,
	EOT_ItemUsed,
	//EOT_ItemSold,
	//EOT_ItemPurchased,
	//EOT_ItemPickedUp,
	EOT_AttributeUpdated,
	EOT_LeveledUp
};