#pragma once

#include <CoreMinimal.h>
#include <Components/ActorComponent.h>

#include "SingularisEquipmentComponent.generated.h"

class UArrowComponent;

#pragma region 委托签名

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEquipmentEquippedSignature, AActor*, Equipment);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEquipmentDeprivedSignature, AActor*, OldEquipment);

#pragma endregion

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

#pragma region 事件分发器

	UPROPERTY(
		BlueprintAssignable,
		Category = "SingularisEquipment|引力奇点装备|事件分发器",
		meta = (DisplayName = "装备时")
	)
	FOnEquipmentEquippedSignature OnEquipmentEquipped{};

	UPROPERTY(
		BlueprintAssignable,
		Category = "SingularisEquipment|引力奇点装备|事件分发器",
		meta = (DisplayName = "卸下时")
	)
	FOnEquipmentDeprivedSignature OnEquipmentDeprived{};

#pragma endregion

private:
#pragma region Internal Variable

	TWeakObjectPtr<ACharacter> OwnerCharacter = nullptr;
	TWeakObjectPtr<UArrowComponent> CachedArrowComponent = nullptr;

	UPROPERTY(ReplicatedUsing = OnRep_EquipmentActor)
	TWeakObjectPtr<AActor> EquipmentActor = nullptr;

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
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

#pragma endregion

#pragma region State

	UFUNCTION(
		BlueprintPure,
		BlueprintCallable,
		Category = "SingularisEquipment|引力奇点装备|State",
		meta = (DisplayName = "Equipped")
	)
	bool Equipped() const { return EquipmentActor.IsValid(); }

	UFUNCTION(
		BlueprintPure,
		BlueprintCallable,
		Category = "SingularisEquipment|引力奇点装备|State",
		meta = (DisplayName = "GetEquip")
	)
	AActor* GetEquip() const { return EquipmentActor.Get(); }

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
#pragma region Response

	UFUNCTION()
	void OnRep_EquipmentActor(TWeakObjectPtr<AActor> OldEquipment) const;

#pragma endregion

#pragma region Internal Function

	void SetEquipment(AActor* Actor);
	void ApplyEquipment(AActor* OldEquipment) const;

#pragma endregion
};
