#pragma once

#include "Components/ActorComponent.h"
#include "CoreMinimal.h"
#include "Data/EquipmentSlotSet.h"
#include "EquipmentSlotComponent.generated.h"

class UEquipmentSystemComponent;
class UEquipmentSlotMapData;

UCLASS(BlueprintType, meta = (BlueprintSpawnableComponent))
class EQUIPMENTSYSTEMCORE_API UEquipmentSlotComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UEquipmentSlotComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Equipment", Meta = (Categories = "Equipment.Slot"))
	void Server_EquipItemAtSlot(FGameplayTag SlotTag);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Meta = (Categories = "Equipment.Slot"))
	void AddItemToSlot(FGameplayTag SlotTag, UItemInstance* Item);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Meta = (Categories = "Equipment.Slot"))
	UItemInstance* RemoveItemFromSlot(FGameplayTag SlotTag);

protected:
	void EquipItemInSlot(FGameplayTag SlotTag);
	void UnequipItemInSlot(FGameplayTag SlotTag);

	FEquipmentSlotSet* FindInstanceSetForSlot(FGameplayTag SlotTag);
	bool IsValidSlot(FGameplayTag SlotTag) const;

	UEquipmentSystemComponent* FindEquipmentSystem() const;

	UFUNCTION()
	void OnRep_Slots();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UEquipmentSlotMapData> SlotMap;

	UPROPERTY(Replicated, ReplicatedUsing = OnRep_Slots)
	TArray<FEquipmentSlotSet> Slots = {};
};