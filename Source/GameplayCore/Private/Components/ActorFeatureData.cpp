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