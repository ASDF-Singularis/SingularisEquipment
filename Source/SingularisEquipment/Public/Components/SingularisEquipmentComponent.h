#pragma once

#include <CoreMinimal.h>
#include <Components/ActorComponent.h>

#include "SingularisEquipmentComponent.generated.h"

class UArrowComponent;

#pragma region 委托签名

/** 装备成功时广播，携带被装备的 Actor 引用 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEquipmentEquippedSignature, AActor*, Equipment);

/** 卸下装备时广播，携带被卸下的旧 Actor 引用 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEquipmentDeprivedSignature, AActor*, OldEquipment);

#pragma endregion

/**
 * 引力奇点装备组件。
 *
 * Server Authority State Synchronization and Reactive Side Effects 设计模式典范：
 * 服务器权威维护 EquipmentActor 复制状态，装备与卸下的物理副作用统一经 ApplyEquipment 响应式执行。
 */
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

	/**
	 * 箭头组件引用。
	 *
	 * 用于确定装备的附加父级与插槽；未解析到时回退至 Owner 根组件。
	 */
	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category = "引力奇点装备组件",
		meta = (DisplayName = "箭头组件引用", UseComponentPicker, AllowedClasses = "/Script/Engine.ArrowComponent")
	)
	FComponentReference ComponentReference{};

#pragma endregion

#pragma region Event Dispatcher

	/** 装备成功时广播 */
	UPROPERTY(
		BlueprintAssignable,
		Category = "引力奇点装备组件|事件分发器",
		meta = (DisplayName = "装备时")
	)
	FOnEquipmentEquippedSignature OnEquipmentEquipped{};

	/** 卸下装备时广播 */
	UPROPERTY(
		BlueprintAssignable,
		Category = "引力奇点装备组件|事件分发器",
		meta = (DisplayName = "卸下时")
	)
	FOnEquipmentDeprivedSignature OnEquipmentDeprived{};

#pragma endregion

private:
#pragma region State

	/** 缓存的拥有者角色 */
	TWeakObjectPtr<ACharacter> OwnerCharacter = nullptr;

	/** 缓存的箭头组件，由 ComponentReference 解析填充 */
	TWeakObjectPtr<UArrowComponent> CachedArrowComponent = nullptr;

	/** 当前装备的 Actor（复制） */
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

#pragma region API

	/** 是否已装备 */
	UFUNCTION(
		BlueprintPure,
		Category = "引力奇点装备组件|API",
		meta = (DisplayName = "Equipped")
	)
	bool Equipped() const { return EquipmentActor.IsValid(); }

	/** 获取当前装备的 Actor */
	UFUNCTION(
		BlueprintPure,
		Category = "引力奇点装备组件|API",
		meta = (DisplayName = "GetEquip")
	)
	AActor* GetEquip() const { return EquipmentActor.Get(); }

	/**
	 * 装备指定 Actor。
	 *
	 * 仅服务器权威有效；已装备或入参非法时静默忽略。
	 */
	UFUNCTION(
		BlueprintCallable,
		BlueprintAuthorityOnly,
		Category = "引力奇点装备组件|API",
		meta = (DisplayName = "装备")
	)
	void Equip(AActor* Actor);

	/** 卸下当前装备 */
	UFUNCTION(
		BlueprintCallable,
		BlueprintAuthorityOnly,
		Category = "引力奇点装备组件|API",
		meta = (DisplayName = "卸下")
	)
	void Deprive();

#pragma endregion

private:
#pragma region Response

	/** EquipmentActor 复制回调 */
	UFUNCTION()
	void OnRep_EquipmentActor(TWeakObjectPtr<AActor> OldEquipment) const;

#pragma endregion

#pragma region Internal Function

	/** 设置 EquipmentActor 复制状态并应用副作用 */
	void SetEquipment(AActor* Actor);

	/** 将装备状态变化应用到物理表现 */
	void ApplyEquipment(AActor* OldEquipment) const;

#pragma endregion
};
