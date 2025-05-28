// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "InventorySystemCore/Public/Components/InventorySystemComponent.h"
#include "InventorySystemCore/Public/Containers/InventoryContainer.h"
#include "InventorySystemCore/Public/Instances/ItemInstance.h"
#include "InventorySystemCore/Public/Definitions/ItemDefinition.h"
#include "InventorySystemCore/Public/Data/InventorySet.h"
#include "Engine/World.h"
#include "Tests/AutomationEditorCommon.h"
#include "Tests/Definitions/TestItemDefinition.h"
#include "Tests/Definitions/TestItemDefinition_Unique.h"


#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FInventory_AddValidItemTest, "InventorySystem.Add.ValidItem", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FInventory_AddInvalidItemTest, "InventorySystem.Add.InvalidItem", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FInventory_MoveItemBetweenContainersTest, "InventorySystem.Move.BetweenContainers", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FInventory_HandleValidationTest, "InventorySystem.Handle.IsValid", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FInventory_SetInjectionTest, "InventorySystem.Set.ApplyDefaultInventorySet", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FInventory_AddValidItemTest::RunTest(const FString& Parameters)
{
	// Create test world
	UWorld* World = FAutomationEditorCommonUtils::CreateNewMap();
	TestNotNull(TEXT("World should be valid"), World);

	// Create an actor with the inventory component
	AActor* TestActor = World->SpawnActor<AActor>();
	TestNotNull(TEXT("Test Actor should be valid"), TestActor);

	UInventorySystemComponent* InventoryComponent = NewObject<UInventorySystemComponent>(TestActor);
	TestNotNull(TEXT("Inventory Component should be valid"), InventoryComponent);

	TestActor->AddOwnedComponent(InventoryComponent);
	InventoryComponent->RegisterComponent();
	InventoryComponent->InitializeComponent();

	// Test error cases
	{
		AddInfo(TEXT("Testing error cases"));

		// Simple add test
		{
			AddInfo(TEXT("Testing basic item addition"));

			const TSubclassOf<UItemDefinition> TestItemDef = UTestItemDefinition::StaticClass();
			const FInventoryResult Result = InventoryComponent->TryAddItemDefinition(TestItemDef, 1);

			if (!Result.Succeeded())
			{
				AddInfo(FString::Printf(TEXT("Tried add a simple item once (%s)"), *Result.FailureReason.ToString()));
			}
			TestNotNull(TEXT("Created instance should be valid"), Result.Instances[0]);
			TestTrue(TEXT("Result should have succeed"), Result.Succeeded());
		}

		// Add test with stack
		{
			AddInfo(TEXT("Testing basic item addition with stack"));

			// Clean inventory
			InventoryComponent->Empty();

			const TSubclassOf<UItemDefinition> TestItemDef = UTestItemDefinition::StaticClass();
			constexpr int32 AddedCount = 15;

			AddInfo(FString::Printf(TEXT("Try add %d stackable item in the inventory"), AddedCount));
			const FInventoryResult Result = InventoryComponent->TryAddItemDefinition(TestItemDef, AddedCount);

			// Check that items are correctly stacked
			const UItemFragment_Storable* StorableFragment = TestItemDef.GetDefaultObject()->FindFragmentByClass<UItemFragment_Storable>();

			TestNotNull(TEXT("Storable fragment should exist"), StorableFragment);
			TestTrue(TEXT("Item should be stackable"), StorableFragment->CanStack());


			// Should create 2 stacks: one of 10 and one of 5
			TestEqual(TEXT("Should create correct number of stacks"), Result.Num(), 2);

			const int32 StackCount = InventoryComponent->GetStackCountByDefinition(TestItemDef);
			TestEqual(TEXT("Total stack count should match requested amount"), StackCount, 2);
			const int32 TotalCount = InventoryComponent->GetTotalCountByDefinition(TestItemDef);
			AddInfo(FString::Printf(TEXT("Total quantity in inventory : %d in %d stacks"), TotalCount, StackCount));
		}
		// Add test with stack on existing stacks from the previous test
		{
			AddInfo(TEXT("Testing stacking behavior over previous test"));

			const TSubclassOf<UItemDefinition> TestItemDef = UTestItemDefinition::StaticClass();
			constexpr int32 AddedCount = 8;
			const int32 PreviousCount = InventoryComponent->GetStackCountByDefinition(TestItemDef);

			AddInfo(FString::Printf(TEXT("Try add %d stackable item in the inventory"), AddedCount));
			const FInventoryResult Result = InventoryComponent->TryAddItemDefinition(TestItemDef, AddedCount);

			// Check that items are correctly stacked
			const UItemFragment_Storable* StorableFragment = TestItemDef.GetDefaultObject()->FindFragmentByClass<UItemFragment_Storable>();

			TestNotNull(TEXT("Storable fragment should exist"), StorableFragment);
			TestTrue(TEXT("Item should be stackable"), StorableFragment->CanStack());

			// Should create one other stack: one existing of 10, one new one of 10 and a newly created one of 3
			TestEqual(TEXT("Should modify correct number of stacks"), Result.Num(), 2);

			const int32 NewStackCount = InventoryComponent->GetStackCountByDefinition(TestItemDef);
			TestEqual(TEXT("Total stack count should match requested amount"), NewStackCount, 3);

			const int32 TotalCount = InventoryComponent->GetTotalCountByDefinition(TestItemDef);
			AddInfo(FString::Printf(TEXT("Total quantity in inventory : %d in %d stacks"), TotalCount, AddedCount + PreviousCount));
		}
		// Unique item additions
		{
			AddInfo(TEXT("Testing unique item behavior"));

			// Clean inventory
			InventoryComponent->Empty();

			FInventoryResult Result;

			const TSubclassOf<UItemDefinition> TestItemDef = UTestItemDefinition_Unique::StaticClass();

			// Check that the Unique flag is set correctly
			const UItemFragment_Storable* StorableFragment = TestItemDef.GetDefaultObject()->FindFragmentByClass<UItemFragment_Storable>();
			TestTrue(TEXT("Item should be unique"), StorableFragment->IsUnique());

			// First addition should be successful
			AddInfo(FString::Printf(TEXT("Try to add 2 unique items")));
			Result = InventoryComponent->TryAddItemDefinition(TestItemDef, 2);
			TestEqual(TEXT("Only one item should have been added"), Result.Num(), 1);

			Result = {};

			// Second addition should fail
			AddInfo(FString::Printf(TEXT("Try to add an other unique item")));
			Result = InventoryComponent->TryAddItemDefinition(TestItemDef, 1);
			TestEqual(TEXT("Second unique item should not be added"), Result.Num(), 0);
		}
	}

	// Cleaning
	World->DestroyWorld(false);

	return true;
}

bool FInventory_AddInvalidItemTest::RunTest(const FString& Parameters)
{
	// Create test world
	UWorld* World = FAutomationEditorCommonUtils::CreateNewMap();
	TestNotNull(TEXT("World should be valid"), World);

	// Create an actor with the inventory component
	AActor* TestActor = World->SpawnActor<AActor>();
	TestNotNull(TEXT("Test Actor should be valid"), TestActor);

	UInventorySystemComponent* InventoryComponent = NewObject<UInventorySystemComponent>(TestActor);
	TestNotNull(TEXT("Inventory Component should be valid"), InventoryComponent);

	TestActor->AddOwnedComponent(InventoryComponent);
	InventoryComponent->RegisterComponent();
	InventoryComponent->InitializeComponent();

	// Test error cases
	{
		AddInfo(TEXT("Testing error cases"));

		// Test with an invalid definition
		{
			const TSubclassOf<UItemDefinition> InvalidDef = nullptr;

			const FInventoryResult Result = InventoryComponent->TryAddItemDefinition(InvalidDef, 1);
			if (!Result.Succeeded())
			{
				AddInfo(FString::Printf(TEXT("Tried add an invalid definition (%s)"), *Result.FailureReason.ToString()));
			}
			TestEqual(TEXT("Should not add item with invalid definition"), Result.Instances.Num(), 0);
		}

		// Test with negative count
		{
			const TSubclassOf<UTestItemDefinition> TestItemDef = UTestItemDefinition::StaticClass();

			const FInventoryResult Result = InventoryComponent->TryAddItemDefinition(TestItemDef, -1);
			if (!Result.Succeeded())
			{
				AddInfo(FString::Printf(TEXT("Tried add with a negative count (%s)"), *Result.FailureReason.ToString()));
			}
			TestEqual(TEXT("Should not add item with negative count"), Result.Instances.Num(), 0);
		}

		// Test with a count of zero
		{
			const TSubclassOf<UTestItemDefinition> TestItemDef = UTestItemDefinition::StaticClass();

			const FInventoryResult Result = InventoryComponent->TryAddItemDefinition(TestItemDef, 0);
			if (!Result.Succeeded())
			{
				AddInfo(FString::Printf(TEXT("Tried add with a zero count (%s)"), *Result.FailureReason.ToString()));
			}
			TestEqual(TEXT("Should not add item with zero count"), Result.Instances.Num(), 0);
		}
	}

	// Cleaning
	World->DestroyWorld(false);

	return true;
}

bool FInventory_MoveItemBetweenContainersTest::RunTest(const FString& Parameters)
{
	UWorld* World = FAutomationEditorCommonUtils::CreateNewMap();
	AActor* TestActor = World->SpawnActor<AActor>();
	auto* Inventory = NewObject<UInventorySystemComponent>(TestActor);
	Inventory->RegisterComponent();

	const FGameplayTag ContainerTagA = InventorySystemGameplayTags::TAG_Inventory_Container_Default;
	const FGameplayTag ContainerTagB = InventorySystemGameplayTags::TAG_Inventory_Container_Bag;

	UInventoryContainer* ContainerA = NewObject<UInventoryContainer>(Inventory);
	UInventoryContainer* ContainerB = NewObject<UInventoryContainer>(Inventory);
	Inventory->RegisterContainer(ContainerTagA, ContainerA);
	Inventory->RegisterContainer(ContainerTagB, ContainerB);

	const TSubclassOf<UTestItemDefinition> TestItemDef = UTestItemDefinition::StaticClass();
	FInventoryResult AddResult = Inventory->TryAddItemDefinitionIn(ContainerTagA, TestItemDef, 1);
	TestTrue("Item was added to ContainerA", AddResult.Succeeded());

	const FInventoryEntryHandle Handle = Inventory->FindHandleFromInstance(AddResult.Instances[0]);
	const FInventoryResult MoveResult = Inventory->TryMoveByHandle(Handle, ContainerB);
	TestTrue("Item moved to ContainerB", MoveResult.Succeeded());

	// Cleaning
	World->DestroyWorld(false);

	return true;
}

bool FInventory_HandleValidationTest::RunTest(const FString& Parameters)
{
	const FInventoryEntryHandle InvalidHandle;
	TestFalse("Handle should be invalid", InvalidHandle.IsHandleValid());

	auto* DummyInstance = NewObject<UItemInstance>();
	const FInventoryEntryHandle ValidHandle(0, DummyInstance, 1, NewObject<UInventoryContainer>());
	TestTrue("Handle should be valid", ValidHandle.IsHandleValid());

	return true;
}

bool FInventory_SetInjectionTest::RunTest(const FString& Parameters)
{
	UInventorySystemComponent* Component = NewObject<UInventorySystemComponent>();
	Component->RegisterComponent();

	UInventoryContainer* Container = NewObject<UInventoryContainer>(Component);
	Component->RegisterContainer(FGameplayTag::RequestGameplayTag("Inventory.Default"), Container);

	UInventorySet* Set = LoadObject<UInventorySet>(nullptr, TEXT("/Game/Data/Sets/TestInventorySet.TestInventorySet"));
	TestNotNull("Inventory Set loaded", Set);

	Set->GiveToInventorySystem(Component);

	const TArray<FInventoryEntryHandle> Stacks = Component->GetAllStacks();
	TestTrue("Inventory has at least one item", Stacks.Num() > 0);

	return true;
}

#endif
