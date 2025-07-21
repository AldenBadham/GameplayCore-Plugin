// // Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagAssetInterface.h"
#include "GameFramework/Actor.h"
#include "InteractableActor.generated.h"

UCLASS()
class INTERACTIONSYSTEMCORE_API AInteractableActor : public AActor, public IGameplayTagAssetInterface
{
	GENERATED_BODY()
	
public:	

	AInteractableActor(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// IGameplayTagAssetInterface
	virtual void GetOwnedGameplayTags(FGameplayTagContainer &TagContainer) const override
	{
		TagContainer.Reset();
		TagContainer.AppendTags(GameplayTags);
	}
	// ~IGameplayTagAssetInterface
	
public:
	
protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GameplayTags")
	FGameplayTagContainer GameplayTags;
	
};
