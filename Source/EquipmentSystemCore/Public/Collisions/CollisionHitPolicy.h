// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#pragma once

#include "CoreMinimal.h"

#include "CollisionHitPolicy.generated.h"

/**
 * EHitCollisionPolicy
 *
 * Defines how to handle collision on a weapon actor
 */
UENUM(BlueprintType)
enum class ECollisionHitPolicy : uint8
{
	OneHitOnly UMETA(DisplayName = "One Hit Only", ToolTip = "One hit for the whole window"),
	OneHitPerActor UMETA(DisplayName = "One Hit Per Actor", ToolTip = "One hit is allowed per actor on the whole window"),
	OneHitPerSuccessiveActor UMETA(DisplayName = "One Hit Per Successive Actor", ToolTip = "Only hits actor if different from previous actor"),
	Count UMETA(Hidden)
};