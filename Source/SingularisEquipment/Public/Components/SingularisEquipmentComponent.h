#pragma once

#include <CoreMinimal.h>
#include <Components/ActorComponent.h>

#include "SingularisEquipmentComponent.generated.h"

class UArrowComponent;

UCLASS(
	Blueprintable,
	BlueprintType,
	ClassGroup = ("Singularis"),
	meta = (BlueprintSpawnableComponent, DisplayName = "引力奇点装备组件")
)
class SINGULARISEQUIPMENT_API USingularisEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:
#pragma region Parameter

	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category = "SingularisEquipment|引力奇点装备|引用",
		meta = (DisplayName = "箭头组件引用", UseComponentPicker, AllowedClasses = "/Script/Engine.ArrowComponent")
	)
	FComponentReference ComponentReference{};

#pragma endregion

private:
#pragma region Internal Variable

	TWeakObjectPtr<ACharacter> OwnerCharacter = nullptr;
	TWeakObjectPtr<UArrowComponent> CachedArrowComponent = nullptr;
	TWeakObjectPtr<AActor> EquipActor = nullptr;

#pragma endregion

public:
#pragma region Constructors

	USingularisEquipmentComponent();

#pragma endregion

#pragma region ActorComponent Interface

	virtual void BeginPlay() override;
	virtual void TickComponent(
		float DeltaTime,
		ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction
	) override;

#pragma endregion

#pragma region State

	UFUNCTION(
		BlueprintPure,
		BlueprintCallable,
		Category = "SingularisInventory|引力奇点装备|State",
		meta = (DisplayName = "Equipped")
	)
	bool Equipped() const { return EquipActor.IsValid(); }

	UFUNCTION(
		BlueprintPure,
		BlueprintCallable,
		Category = "SingularisInventory|引力奇点装备|State",
		meta = (DisplayName = "GetEquip")
	)
	AActor* GetEquip() const { return EquipActor.Get(); }

#pragma endregion

#pragma region API

	UFUNCTION(
		BlueprintCallable,
		BlueprintAuthorityOnly,
		Category = "SingularisEquipment|引力奇点装备|API",
		meta = (DisplayName = "装备")
	)
	void Equip(AActor* Actor);

	UFUNCTION(
		BlueprintCallable,
		BlueprintAuthorityOnly,
		Category = "SingularisEquipment|引力奇点装备|API",
		meta = (DisplayName = "卸下")
	)
	void Deprive();

#pragma endregion

private:
#pragma region Internal Function

	void SetEquipment(AActor* Actor);
	void ApplyEquipment() const;
	void DepriveEquipment(AActor* Actor) const;

#pragma endregion
};
