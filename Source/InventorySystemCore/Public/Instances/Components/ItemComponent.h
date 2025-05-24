// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ItemComponent.generated.h"

class UItemInstance;

/**
 * @class UItemComponent
 * @see UItemInstance
 * @brief Base class for modular components that can be added to item instances.
 * @details This class serves as the foundation for creating specialized components
 * that add specific functionalities to item instances. Components are attached to item
 * instances to extend their capabilities. Each component can implement its own replication
 * logic and functionality while maintaining a modular design approach.
 */
UCLASS(Abstract)
class INVENTORYSYSTEMCORE_API UItemComponent : public UObject
{
	GENERATED_BODY()

public:
	// UObject
	/**
	 * Sets up property replication for the component
	 * @param OutLifetimeProps Array of lifetime properties to be filled with replication properties
	 */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override { Super::GetLifetimeReplicatedProps(OutLifetimeProps); };
	// ~UObject

	virtual void Initialize(UItemInstance* InInstance);

	/**
	 * Returns the item instance that owns this fragment
	 * @return The owning item instance, or nullptr if not attached
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure)
	UItemInstance* GetOwningInstance();

protected:
	/** The item instance that owns this fragment instance */
	UPROPERTY(Transient)
	TObjectPtr<UItemInstance> OwningInstance = nullptr;
};