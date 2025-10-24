// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ErrorFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class GAMEPLAYCORE_API UErrorFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "Error")
	static void PopMessageBox(EAppMsgType::Type Type,  FText Message, FText Title);

	UFUNCTION(BlueprintCallable, Category = "Error")
	static void ShowNotificationInfo(const FText Text, const FText SubText);
};
