// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "EquipmentSystemInterface.generated.h"

class UEquipmentSystemComponent;

/**
 * @class UEquipmentSystemInterface
 * @see IEquipmentSystemInterface
 * @brief Interface class for the Unreal Engine's interface system
 * @details This is the UObject interface declaration required by Unreal Engine.
 * Do not modify this class as it's only used for UObject system registration.
 */
UINTERFACE(MinimalAPI, BlueprintType, meta = (CannotImplementInterfaceInBlueprint))
class UEquipmentSystemInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * @class IEquipmentSystemInterface
 * @see UEquipmentSystemComponent
 * @brief Interface providing access to an actor's equipment system component
 * @details This interface should be implemented by actors that need to expose
 * their equipment system functionality. The component can be owned by the implementing
 * actor or by another actor (e.g., PlayerState owning equipment for its Pawn).
 */
class EQUIPMENTSYSTEMCORE_API IEquipmentSystemInterface
{
	GENERATED_BODY()

public:
	/**
	 * Retrieves the equipment system component associated with this actor
	 * @return The equipment system component to use. May be located on another actor
	 */
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	virtual UEquipmentSystemComponent* GetEquipmentSystemComponent() const = 0;
};
