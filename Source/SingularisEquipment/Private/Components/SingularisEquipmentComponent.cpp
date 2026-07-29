#include "Components/SingularisEquipmentComponent.h"

#include <Components/ArrowComponent.h>
#include <GameFramework/Character.h>
#include <Net/UnrealNetwork.h>

USingularisEquipmentComponent::USingularisEquipmentComponent()
{
	SetIsReplicatedByDefault(true);

	PrimaryComponentTick.bStartWithTickEnabled = true;
	PrimaryComponentTick.bCanEverTick = true;

	bAutoActivate = true;
}

void USingularisEquipmentComponent::BeginPlay()
{
	Super::BeginPlay();

	checkf(GetOwner()->IsA<ACharacter>(), TEXT("SingularisEquipComponent: Owner not is Character"));

	OwnerCharacter = Cast<ACharacter>(GetOwner());

	if (AActor* Owner = GetOwner())
		CachedArrowComponent = Cast<UArrowComponent>(ComponentReference.GetComponent(Owner));
}

void USingularisEquipmentComponent::TickComponent(
	const float DeltaTime,
	const ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction
)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void USingularisEquipmentComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USingularisEquipmentComponent, EquipmentActor);
}

void USingularisEquipmentComponent::Equip(AActor* Actor)
{
	if (Equipped() || !IsValid(Actor)) return;

	SetEquipment(Actor);
}

void USingularisEquipmentComponent::Deprive()
{
	if (!Equipped()) return;

	SetEquipment(nullptr);
}

void USingularisEquipmentComponent::OnRep_EquipmentActor(const TWeakObjectPtr<AActor> OldEquipment) const
{
	ApplyEquipment(OldEquipment.Get());
}

void USingularisEquipmentComponent::SetEquipment(AActor* Actor)
{
	// 1) 服务器权威检查
	if (!IsValid(GetOwner()) || !GetOwner()->HasAuthority()) return;

	// 2) 幂等性
	if (Actor == EquipmentActor) return;

	// 3) 设置复制副作用状态
	AActor* OldEquipActor = EquipmentActor.Get();
	EquipmentActor = Actor;

	// 4) 服务器应用副作用
	//    响应式编程
	ApplyEquipment(OldEquipActor);
}

void USingularisEquipmentComponent::ApplyEquipment(AActor* OldEquipment) const
{
	if (!OwnerCharacter.IsValid()) return;

	if (EquipmentActor.IsValid())
	{
		// 1) 获取根组件
		USceneComponent* Root = EquipmentActor->GetRootComponent();
		if (!IsValid(Root)) return;

		// 2) 转为图元
		UPrimitiveComponent* PrimComp = Cast<UPrimitiveComponent>(Root);
		if (!IsValid(PrimComp)) return;

		// 2) 关闭物理
		PrimComp->SetSimulatePhysics(false);
		PrimComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		// 4) 附加 - 根据目标箭头组件确定附加父级和插槽
		const FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);

		if (CachedArrowComponent.IsValid())
		{
			// 通过箭头组件的父级和插槽名进行附加
			// 有骨骼：箭头挂在 SkeletalMeshComponent 的某个 Socket/Bone 上 → 装备附加到同一骨骼
			// 无骨骼：箭头挂在普通 SceneComponent 上 → 装备附加到同一父级
			USceneComponent* AttachParent = CachedArrowComponent->GetAttachParent();
			const FName AttachSocket = CachedArrowComponent->GetAttachSocketName();

			if (IsValid(AttachParent))
			{
				EquipmentActor->AttachToComponent(AttachParent, AttachmentRules, AttachSocket);
				// 附加后匹配箭头相对父级的变换，使装备精确位于箭头标记的位置
				Root->SetRelativeTransform(CachedArrowComponent->GetRelativeTransform());
			}
		}
		else
		{
			// 无箭头组件时回退到附加至角色根组件
			EquipmentActor->AttachToComponent(OwnerCharacter->GetRootComponent(), AttachmentRules);
		}
	}
	else
	{
		if (!IsValid(OldEquipment)) return;

		// 1) 获取根组件
		USceneComponent* Root = OldEquipment->GetRootComponent();
		if (!IsValid(Root)) return;

		// 2) 转为图元
		UPrimitiveComponent* PrimComp = Cast<UPrimitiveComponent>(Root);
		if (!IsValid(PrimComp)) return;

		// 3) 开启物理
		PrimComp->SetMobility(EComponentMobility::Movable);
		PrimComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		PrimComp->SetSimulatePhysics(true);

		// 4) 分离
		const FDetachmentTransformRules DetachRules(EDetachmentRule::KeepWorld, true);
		OldEquipment->DetachFromActor(DetachRules);
	}
}
