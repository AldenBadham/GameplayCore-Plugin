// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InventorySystemInterface.generated.h"

class UInventorySystemComponent;

/**
 * @class UInventorySystemInterface
 * @see IInventorySystemInterface
 * @brief Interface class for the Unreal Engine's interface system
 * @details This is the UObject interface declaration required by Unreal Engine.
 * Do not modify this class as it's only used for UObject system registration.
 */
UINTERFACE(MinimalAPI, BlueprintType, meta = (CannotImplementInterfaceInBlueprint))
class UInventorySystemInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * @class IInventorySystemInterface
 * @see UInventorySystemComponent
 * @brief Interface providing access to an actor's inventory system component
 * @details This interface should be implemented by actors that need to expose
 * their inventory system functionality. The component can be owned by the implementing
 * actor or by another actor (e.g., PlayerState owning inventory for its Pawn).
 */
class INVENTORYSYSTEMCORE_API IInventorySystemInterface
{
	GENERATED_BODY()

public:
	/**
	 * Retrieves the inventory system component associated with this actor
	 * @return The inventory system component to use. May be located on another actor
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	virtual UInventorySystemComponent* GetInventorySystemComponent() const = 0;
};