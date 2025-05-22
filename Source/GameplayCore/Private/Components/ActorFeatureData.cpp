// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#include "Components/ActorFeatureData.h"

#include "Components/ActorFeatureRegisteredDelegate.h"

FActorFeatureData::~FActorFeatureData()
{
	for (const TSharedRef<FActorFeatureRegisteredDelegate>& RegisteredDelegate : RegisteredDelegates)
	{
		// This ensures it will not execute if cleared during delegate iteration
		RegisteredDelegate->bRemoved = true;
	}
}