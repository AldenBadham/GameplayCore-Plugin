// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#pragma once

#include "CoreMinimal.h"
#include "Data/InventoryList.h"
#include "UObject/Object.h"
#include "InventoryContainer.generated.h"

class UItemDefinition;
class UItemInstance;
class UInventorySystemComponent;
class UStoragePolicy;


/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class INVENTORYSYSTEMCORE_API UInventoryContainer : public UObject
{
	GENERATED_BODY()

public:
	UInventoryContainer(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// UObject
	virtual bool IsSupportedForNetworking() const override { return true; }
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	// ~UObject

	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags);


	UFUNCTION(BlueprintCallable, Category="Inventory|Container")
	FInventoryResult TryAddItemDefinition(TSubclassOf<UItemDefinition> Definition, int32 Count);

	UFUNCTION(BlueprintCallable, Category="Inventory|Container")
	FInventoryResult TryAddItemInstance(UItemInstance* Instance, int32 Count);

	UFUNCTION(BlueprintCallable, Category="Inventory|Container")
	bool TryRemoveItem(FInventoryEntryHandle& Handle, FGameplayTag& OutFailureReason);

	UFUNCTION(BlueprintCallable, Category="Inventory|Container")
	FInventoryResult TryMoveItemTo(FInventoryEntryHandle Handle, UInventoryContainer* TargetContainer);

	UFUNCTION(BlueprintCallable, Category="Inventory|Container")
	FInventoryEntryHandle FindHandle(UItemInstance* Instance) const;

	UFUNCTION(BlueprintCallable, Category="Inventory|Container")
	FInventoryList& GetInventoryList() { return InventoryList; }

	UFUNCTION(BlueprintCallable, Category="Inventory|Container")
	void SetOwnerComponent(UInventorySystemComponent* NewOwner);
	int32 GetStackCountByDefinition(const TSubclassOf<UItemDefinition>& DefinitionClass) const;
	int32 GetTotalCountByDefinition(const TSubclassOf<UItemDefinition>& DefinitionClass) const;

	UFUNCTION(BlueprintCallable, Category="Inventory|Container")
	void AddStoragePolicy(UStoragePolicy* Policy);

protected:
	bool ValidateStorage(UItemInstance* Instance, FGameplayTag& OutFailureReason) const;

	UPROPERTY()
	UInventorySystemComponent* OwnerComponent = nullptr;

	// Replicated list of items in this container
	UPROPERTY(Replicated)
	FInventoryList InventoryList;

	// Policies local to this container (not replicated)
	UPROPERTY()
	TArray<TObjectPtr<UStoragePolicy>> Policies;
};
