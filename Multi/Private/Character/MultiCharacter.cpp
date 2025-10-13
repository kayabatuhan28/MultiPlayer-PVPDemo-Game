// Copyright Epic Games, Inc. All Rights Reserved.

#include "Character/MultiCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/TextBlock.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Widget/Crosshair.h"
#include "Widget/MainOverlay.h"
#include "Widget/OnHeadWidget.h"
#include "Widget/SkillSlot.h"

AMultiCharacter::AMultiCharacter()
{
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	
	TurnRateGamepad = 50.f;
	
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	
	GetCharacterMovement()->bOrientRotationToMovement = true; 
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); 
	
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; 
	CameraBoom->bUsePawnControlRotation = true;
	
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); 
	FollowCamera->bUsePawnControlRotation = false;

	OnHeadWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("OnHeadWidget"));
	OnHeadWidgetComp->AttachToComponent(GetMesh(),FAttachmentTransformRules::KeepRelativeTransform);

	DamageWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("DamageWidget"));
	DamageWidgetComp->AttachToComponent(GetMesh(),FAttachmentTransformRules::KeepRelativeTransform);
	
	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(GetMesh());

	
}

void AMultiCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("Move Forward / Backward", this, &AMultiCharacter::MoveForward);
	PlayerInputComponent->BindAxis("Move Right / Left", this, &AMultiCharacter::MoveRight);
	
	PlayerInputComponent->BindAxis("Turn Right / Left Mouse", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("Turn Right / Left Gamepad", this, &AMultiCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("Look Up / Down Mouse", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Look Up / Down Gamepad", this, &AMultiCharacter::LookUpAtRate);
	
	PlayerInputComponent->BindTouch(IE_Pressed, this, &AMultiCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &AMultiCharacter::TouchStopped);

	
	// Custom Input Functions
	PlayerInputComponent->BindAction("LeftMouseAttack", IE_Pressed, this, &AMultiCharacter::LeftMousePressed);

	PlayerInputComponent->BindAction("Dodge", IE_Pressed, this, &AMultiCharacter::Dodge);

	PlayerInputComponent->BindAction<FKeyPressedDelegate>("Skill1", IE_Pressed, this,&AMultiCharacter::CastSkill, 0);
	
	PlayerInputComponent->BindAction<FKeyPressedDelegate>("Skill2", IE_Pressed, this,&AMultiCharacter::CastSkill, 1);
	
	PlayerInputComponent->BindAction<FKeyPressedDelegate>("Skill3", IE_Pressed, this,&AMultiCharacter::CastSkill, 2);
	PlayerInputComponent->BindAction<FKeyReleasedDelegate>("Skill3", IE_Released, this,&AMultiCharacter::CastSkillReleased, 2);

	PlayerInputComponent->BindAction<FKeyPressedDelegate>("Skill4", IE_Pressed, this,&AMultiCharacter::CastSkill, 3);

	PlayerInputComponent->BindAction<FKeyPressedDelegate>("Skill5", IE_Pressed, this,&AMultiCharacter::CastSkill, 4);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AMultiCharacter::SprintPressed);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AMultiCharacter::SprintReleased);

	PlayerInputComponent->BindAction("Block", IE_Pressed, this, &AMultiCharacter::BlockPressed);
	PlayerInputComponent->BindAction("Block", IE_Released, this, &AMultiCharacter::BlockReleased);

	
}

void AMultiCharacter::BeginPlay()
{
	Super::BeginPlay();

	OnHeadWidget = Cast<UOnHeadWidget>(OnHeadWidgetComp->GetUserWidgetObject());
	if (OnHeadWidget)
	{
		OnHeadWidget->SetVisibility(ESlateVisibility::Collapsed);
	}

	DamageWidget = Cast<UDamageWidget>(DamageWidgetComp->GetUserWidgetObject());
	if (!DamageWidget)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Damage Widget Cast Edilemedi!"));	
	}

	if(MainOverlayClass)
	{
		MainOverlayWidget = Cast<UMainOverlay>(CreateWidget(GetWorld(),MainOverlayClass));
		if(MainOverlayWidget != nullptr)
		{
			MainOverlayWidget->AddToViewport();
			MainOverlayWidget->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	if(CrossHairWidgetClass)
	{
		CrossHairWidget = Cast<UCrosshair>(CreateWidget(GetWorld(),CrossHairWidgetClass));
		if(CrossHairWidget != nullptr)
		{
			CrossHairWidget->AddToViewport();
			CrossHairWidget->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	GetWorld()->GetTimerManager().SetTimer(StaminaRegenHandle, this, &AMultiCharacter::StaminaRegen, 0.1f, true);

	
}

// Set Character Data
// ---------------------------------------------------------------------------------------------------------------------------------------------------
void AMultiCharacter::Server_SetCharacterData_Implementation(UPlayerDataAsset* DataAsset)
{
	Multicast_SetCharacterData(DataAsset);
}

void AMultiCharacter::Multicast_SetCharacterData_Implementation(UPlayerDataAsset* DataAsset)
{
	if (DataAsset->WeaponType == EWeaponType::GreatSword)
	{
		PlayerWeaponType = DataAsset->WeaponType;
		WeaponMesh->SetStaticMesh(DataAsset->WeaponMesh);
		WeaponMesh->SetWorldScale3D(FVector(0.9,0.9,0.9f));
		WeaponMesh->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, DataAsset->WeaponSocket);
	}
	else if (DataAsset->WeaponType == EWeaponType::Archer)
	{
		PlayerWeaponType = DataAsset->WeaponType;
		WeaponMesh->SetStaticMesh(DataAsset->WeaponMesh);
		WeaponMesh->SetWorldScale3D(FVector(1.f,1.f,1.f));
		WeaponMesh->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, DataAsset->WeaponSocket);
	}
	
	OnHeadWidget->SetVisibility(ESlateVisibility::Visible);
	CurrentHealth = DataAsset->CurrentHealth;
	MaxHealth = DataAsset->MaxHealth;
	CurrentStamina = DataAsset->CurrentStamina;
	MaxStamina = DataAsset->MaxStamina;
	
	OnHeadWidget->UpdateHealth(CurrentHealth, MaxHealth);
	OnHeadWidget->UpdateStamina(CurrentStamina, MaxStamina);

	DodgeCd = DataAsset->DodgeCd;
	RunAttackCd = DataAsset->SprintAttackCd;

	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;

	M_RunAttack = DataAsset->M_SprintAttack;

	for (int i = 0; i < DataAsset->M_Dodge.Num(); i++)
	{
		M_Dodge[i] = DataAsset->M_Dodge[i];
	}

	for (int i = 0; i < DataAsset->M_BasicAttack.Num(); i++)
	{
		M_BasicAttack[i] = DataAsset->M_BasicAttack[i];
	}

	for (int i = 0; i < DataAsset->M_SkillAttack.Num(); i++)
	{
		M_SkillAttack[i] = DataAsset->M_SkillAttack[i];
	}

	for (int i = 0; i < DataAsset->M_SkillImage.Num(); i++)
	{
		M_SkillImage[i] = DataAsset->M_SkillImage[i];
	}

	for (int i = 0; i < DataAsset->M_SkillCd.Num(); i++)
	{
		M_SkillCd[i] = DataAsset->M_SkillCd[i];
	}

	for (int i = 0; i < DataAsset->M_HitMontage.Num(); i++)
	{
		M_HitMontage[i] = DataAsset->M_HitMontage[i];
	}

	for (int i = 0; i < DataAsset->M_BlockHit.Num(); i++)
	{
		M_BlockHit[i] = DataAsset->M_BlockHit[i];
	}
}



// Fill Image
// ---------------------------------------------------------------------------------------------------------------------------------------------------
void AMultiCharacter::FillImage(UPlayerDataAsset* InDataAsset)
{
	if (InDataAsset == nullptr)
		return;
	
	MainOverlayWidget->SetVisibility(ESlateVisibility::Visible);
	if (InDataAsset->WeaponType == EWeaponType::Archer)
	{
		CrossHairWidget->SetVisibility(ESlateVisibility::Visible);
	}
	else if (InDataAsset->WeaponType == EWeaponType::GreatSword)
	{
		GetFollowCamera()->SetRelativeLocation(FVector(-60.f, 0, 40.f));
	}
	
	
	M_SkillImage[0] = InDataAsset->M_SkillImage[0];
	M_SkillImage[1] = InDataAsset->M_SkillImage[1];
	M_SkillImage[2] = InDataAsset->M_SkillImage[2];
	M_SkillImage[3] = InDataAsset->M_SkillImage[3];
	M_SkillImage[4] = InDataAsset->M_SkillImage[4];
	
	MainOverlayWidget->Skill1->SetProgressBarSkillIcon(InDataAsset->M_SkillImage[0]);
	MainOverlayWidget->Skill1->SkillKeyText->SetText(FText::FromString("1"));
	
	MainOverlayWidget->Skill2->SetProgressBarSkillIcon(InDataAsset->M_SkillImage[1]);
	MainOverlayWidget->Skill2->SkillKeyText->SetText(FText::FromString("2"));
	
	MainOverlayWidget->Skill3->SetProgressBarSkillIcon(InDataAsset->M_SkillImage[2]);
	MainOverlayWidget->Skill3->SkillKeyText->SetText(FText::FromString("3"));
	
	MainOverlayWidget->Skill4->SetProgressBarSkillIcon(InDataAsset->M_SkillImage[3]);
	MainOverlayWidget->Skill4->SkillKeyText->SetText(FText::FromString("4"));

	MainOverlayWidget->Skill5->SetProgressBarSkillIcon(InDataAsset->M_SkillImage[4]);
	MainOverlayWidget->Skill5->SkillKeyText->SetText(FText::FromString("5"));
}

// Calculate Direction
// ---------------------------------------------------------------------------------------------------------------------------------------------------
float AMultiCharacter::CalculateDirection()
{
	if (ForwardValue == 1)
	{
		if (RightValue == 1) // W+D
			{
			return 45.f;
			}
		if (RightValue == -1) // W+A
			{
			return -45.f;
			}
		return 0.f; // W
	}

	if (ForwardValue == -1)
	{
		if (RightValue == 1) // S+D
			{
			return 135.f;
			}
		if (RightValue == -1) // S+A
			{
			return -135.f;
			}
		return 180.f;
	}

	if (RightValue == 1) 
	{
		if (ForwardValue == 1) // D+W
			{
			return -45.f;
			}
		if (ForwardValue == -1) // D+S
			{
			return -135.f;
			}
		return 90.f;
	}
	
	if (RightValue == -1) 
	{
		if (ForwardValue == 1) // A+W
			{
			return 45.f;
			}
		if (ForwardValue == -1) // A+S
			{
			return  135.f;
			}
		if (ForwardValue == 0) // A
			{
			return -90.f;
			}
	}

	return 0;
}


// Replicated Play Montage
// ---------------------------------------------------------------------------------------------------------------------------------------------------
void AMultiCharacter::Server_PlayMontage_Implementation(UAnimMontage* Montage)
{
	Multicast_PlayMontage(Montage);
}

void AMultiCharacter::Multicast_PlayMontage_Implementation(UAnimMontage* Montage)
{
	PlayAnimMontage(Montage);
}


// Replicated Spawn Skill
// ---------------------------------------------------------------------------------------------------------------------------------------------------
void AMultiCharacter::Server_SpawnSkill_Implementation(TSubclassOf<AActor> Class, FTransform SpawnTransform,
	AActor* SkillOwner, ESkillType InSkillType, FName InAttachSocket)
{
	Multicast_SpawnSkill(Class, SpawnTransform, SkillOwner, InSkillType, InAttachSocket);
}

void AMultiCharacter::Multicast_SpawnSkill_Implementation(TSubclassOf<AActor> Class, FTransform SpawnTransform,
	AActor* SkillOwner, ESkillType InSkillType, FName InAttachSocket)
{
	if (HasAuthority())
	{
		if (InSkillType == ESkillType::Projectile)
		{
			FVector LookedLoc = TraceLoc();
			FRotator NewRot = UKismetMathLibrary::FindLookAtRotation(SpawnTransform.GetLocation(), LookedLoc);
			SpawnTransform.SetRotation(NewRot.Quaternion());
		}
		
		
		FActorSpawnParameters SpawnInfo;
		SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnInfo.Owner = SkillOwner;
		SpawnInfo.Instigator = this;
		
		AActor* SkillAct = GetWorld()->SpawnActor<AActor>(Class, SpawnTransform, SpawnInfo);
		if (InSkillType == ESkillType::Attached)
		{
			SkillAct->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, InAttachSocket);
		}
	}
}

// Get Trace Hit Location from camera trace
// ---------------------------------------------------------------------------------------------------------------------------------------------------
FVector AMultiCharacter::TraceLoc()
{
	ECollisionChannel TraceChannelProperty = ECC_Visibility;
	
	FHitResult Hit;
	
	FVector TraceStartLoc = GetFollowCamera()->GetComponentLocation();
	FVector Direction = UKismetMathLibrary::GetForwardVector(GetFollowCamera()->GetComponentRotation());
	FVector TraceEndLoc = (Direction * 9000.f) + TraceStartLoc;
	
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	
	GetWorld()->LineTraceSingleByChannel(Hit, TraceStartLoc, TraceEndLoc, TraceChannelProperty, QueryParams);
	//DrawDebugLine(GetWorld(), TraceStartLoc, TraceEndLoc, Hit.bBlockingHit ? FColor::Blue : FColor::Red, true, 1.0f, 0, 1.0f);
	return Hit.Location;
}


// Block
// ---------------------------------------------------------------------------------------------------------------------------------------------------
void AMultiCharacter::Server_SetBlock_Implementation(bool IsActive)
{
	Multicast_SetBlock(IsActive);
}

void AMultiCharacter::Multicast_SetBlock_Implementation(bool IsActive)
{
	Actions.bIsBlocking = IsActive;
}


// Dodge
// ---------------------------------------------------------------------------------------------------------------------------------------------------
void AMultiCharacter::ClearDodgeCd()
{
	Actions.bCanDodge = true;
}


// Clear Can Run Attack
// ---------------------------------------------------------------------------------------------------------------------------------------------------
void AMultiCharacter::ClearRunAttackCd()
{
	Actions.bCanRunAttack = true;
}


// Take Hit
// ---------------------------------------------------------------------------------------------------------------------------------------------------
void AMultiCharacter::TakeHit(float TakingDamage, int HitMontageIndex, int BlockMontageIndex)
{
	if (bIsDead == true)
		return;
	
	if (Actions.bIsBlocking == true && CurrentStamina - TakingDamage >= 0.f)
	{
		if (BlockMontageIndex > 0)
		{
			Server_PlayMontage(M_BlockHit[BlockMontageIndex]);
		}
		CurrentStamina -= TakingDamage;
		OnHeadWidget->UpdateStamina(CurrentStamina, MaxStamina);
		TakingDamage = 0.f;
	}
	else
	{
		if (HitMontageIndex > 0 && Actions.bCanHitReaction == true)
		{
			Server_PlayMontage(M_HitMontage[HitMontageIndex]);
		}
	}

	if (CurrentHealth - TakingDamage <= 0.f)
	{
		bIsDead = true;
		CurrentHealth = 0.f;
		OnHeadWidget->UpdateHealth(CurrentHealth, MaxHealth);
		DamageWidget->SetDamageText(TakingDamage);
		OnDead();
	}
	else
	{
		CurrentHealth -= TakingDamage;
		OnHeadWidget->UpdateHealth(CurrentHealth, MaxHealth);
		DamageWidget->SetDamageText(TakingDamage);
	}
	
	
	
}


// Basic Attack
// ---------------------------------------------------------------------------------------------------------------------------------------------------
void AMultiCharacter::LeftMousePressed()
{
	if (ActiveState != Empty || !bIsDataSetted || GetCharacterMovement()->IsFalling() == true)
	{
		return;
	}

	if (Actions.bIsSprinting == true)
	{
		Server_PlayMontage(M_RunAttack);
		//Actions.bCanRunAttack = false;
	
		//FTimerHandle Handle;
		//GetWorld()->GetTimerManager().SetTimer(Handle, this, &ARageCharacter::ClearRunAttackCd, RunAttackCd, false);
		
		return;
	}

	if (LightAttackCount == 0)
	{
		Server_PlayMontage(M_BasicAttack[0]);
		LightAttackCount++;
	}
	else if (LightAttackCount == 1)
	{
		Server_PlayMontage(M_BasicAttack[1]);
		LightAttackCount++;
	}
	else
	{
		Server_PlayMontage(M_BasicAttack[2]);
		LightAttackCount = 0;
	}
}


// Sprint
// ---------------------------------------------------------------------------------------------------------------------------------------------------
void AMultiCharacter::SprintPressed()
{
	Actions.bIsSprinting = true;
}

void AMultiCharacter::SprintReleased()
{
	Actions.bIsSprinting = false;
}


// Dodge
// ---------------------------------------------------------------------------------------------------------------------------------------------------
void AMultiCharacter::Dodge()
{
	if (ActiveState != EActiveState::Empty ||
		!bIsDataSetted || Actions.bCanDodge == false ||
		GetCharacterMovement()->IsFalling() == true ||
		Tags.Contains("Slow"))
	{
		return;
	}
		

	Actions.bCanDodge = false;
	
	FTimerHandle Handle;
	GetWorld()->GetTimerManager().SetTimer(Handle, this, &AMultiCharacter::ClearDodgeCd, DodgeCd, false);
	
	if (ForwardValue == 1) // Forward Dash
		{
		Server_PlayMontage(M_Dodge[0]);
		}
	else if (ForwardValue == -1) // Backward Dash
		{
		Server_PlayMontage(M_Dodge[1]);
		}
	else if (RightValue == 1) // Right Dash
		{
		Server_PlayMontage(M_Dodge[2]);
		}
	else if (RightValue == -1) // Left Dash
		{
		Server_PlayMontage(M_Dodge[3]);
		}
	else // No Key Pressed Then Backward Dash
		{
		Server_PlayMontage(M_Dodge[1]);
		}
}


// Skill
// ---------------------------------------------------------------------------------------------------------------------------------------------------
void AMultiCharacter::CastSkill(int InSkillIndex)
{
	if (ActiveState != EActiveState::Empty || !bIsDataSetted || IsSkillCd[InSkillIndex] == true || Actions.bIsSilence == true)
		return;

	

	
	if (InSkillIndex == 0)
	{
		FTimerHandle Skill0Handle;
		GetWorld()->GetTimerManager().SetTimer(Skill0Handle, this, &AMultiCharacter::ResetSkill1Cd, M_SkillCd[0],false);
		SetSkillCooldownWidget(InSkillIndex);
		Server_PlayMontage(M_SkillAttack[InSkillIndex]);
		IsSkillCd[InSkillIndex] = true;
	}
	else if (InSkillIndex == 1)
	{
		FTimerHandle Skill1Handle;
		GetWorld()->GetTimerManager().SetTimer(Skill1Handle, this, &AMultiCharacter::ResetSkill2Cd, M_SkillCd[1],false);
		SetSkillCooldownWidget(InSkillIndex);
		Server_PlayMontage(M_SkillAttack[InSkillIndex]);
		IsSkillCd[InSkillIndex] = true;
	}
	else if (InSkillIndex == 2)
	{
		if (PlayerWeaponType == EWeaponType::Archer)
		{
			//RainArrow();
			TestRainArrow();
		}
		else
		{
			FTimerHandle Skill2Handle;
			GetWorld()->GetTimerManager().SetTimer(Skill2Handle, this, &AMultiCharacter::ResetSkill3Cd, M_SkillCd[2],false);
			SetSkillCooldownWidget(InSkillIndex);
			Server_PlayMontage(M_SkillAttack[InSkillIndex]);
			IsSkillCd[InSkillIndex] = true;
		}
		
	}
	else if (InSkillIndex == 3)
	{
		FTimerHandle Skill3Handle;
		GetWorld()->GetTimerManager().SetTimer(Skill3Handle, this, &AMultiCharacter::ResetSkill4Cd, M_SkillCd[3],false);
		SetSkillCooldownWidget(InSkillIndex);
		Server_PlayMontage(M_SkillAttack[InSkillIndex]);
		IsSkillCd[InSkillIndex] = true;
	}
	else if (InSkillIndex == 4)
	{
		FTimerHandle Skill4Handle;
		GetWorld()->GetTimerManager().SetTimer(Skill4Handle, this, &AMultiCharacter::ResetSkill5Cd, M_SkillCd[4],false);
		SetSkillCooldownWidget(InSkillIndex);
		Server_PlayMontage(M_SkillAttack[InSkillIndex]);
		IsSkillCd[InSkillIndex] = true;
	}


}

void AMultiCharacter::CastSkillReleased(int InSkillIndex)
{
	if (PlayerWeaponType == EWeaponType::Archer && InSkillIndex == 2 && IsSkillCd[InSkillIndex] == false)
	{
		if (TargetArea)
		{
			TargetArea->K2_DestroyActor();
			TargetArea = nullptr;
		}
		
		GetWorld()->GetTimerManager().ClearTimer(SetTargetAreaLocHandle);
		
		IsSkillCd[InSkillIndex] = true;
		FTimerHandle Skill2Handle;
		GetWorld()->GetTimerManager().SetTimer(Skill2Handle, this, &AMultiCharacter::ResetSkill3Cd, M_SkillCd[2],false);
		SetSkillCooldownWidget(InSkillIndex);
		Server_PlayMontage(M_SkillAttack[InSkillIndex]);
	}
}

void AMultiCharacter::TestRainArrow()
{
	FVector SpawnLoc = TraceLoc();
	FTransform Transform(FRotator::ZeroRotator, SpawnLoc, FVector(1,1,1));
	
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnInfo.Owner = this;
	SpawnInfo.Instigator = this;

	AActor* HoldedAct = GetWorld()->SpawnActor<AActor>(TargetAreaClass, Transform, SpawnInfo);
	if (!HoldedAct)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("HoldedAct Not Valid!!"));
	}
	
	TargetArea = HoldedAct;
	if (TargetArea)
	{
		GetWorld()->GetTimerManager().SetTimer(SetTargetAreaLocHandle, this, &AMultiCharacter::SetTargetAreaLoc, 0.01f, true);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Target Area Not Valid!!"));
	}
}

void AMultiCharacter::SetTargetAreaLoc()
{
	if (TargetArea)
	{
		FVector NewLoc = TraceLoc();
		NewLoc.Z = 0.f;
		TargetArea->SetActorLocation(NewLoc);
	}
}

void AMultiCharacter::SetSkillCooldownWidget(int InSkillIndex)
{
	if (InSkillIndex == 0)
	{
		MainOverlayWidget->Skill1->SetSkillCooldown(M_SkillCd[0]);
	}
	else if (InSkillIndex == 1)
	{
		MainOverlayWidget->Skill2->SetSkillCooldown(M_SkillCd[1]);
	}
	else if (InSkillIndex == 2)
	{
		MainOverlayWidget->Skill3->SetSkillCooldown(M_SkillCd[2]);
	}
	else if (InSkillIndex == 3)
	{
		MainOverlayWidget->Skill4->SetSkillCooldown(M_SkillCd[3]);
	}
	else if (InSkillIndex == 4)
	{
		MainOverlayWidget->Skill5->SetSkillCooldown(M_SkillCd[4]);
	}
}



void AMultiCharacter::ResetSkill1Cd()
{
	IsSkillCd[0] = false;
}

void AMultiCharacter::ResetSkill2Cd()
{
	IsSkillCd[1] = false;
}

void AMultiCharacter::ResetSkill3Cd()
{
	IsSkillCd[2] = false;
}

void AMultiCharacter::ResetSkill4Cd()
{
	IsSkillCd[3] = false;
}

void AMultiCharacter::ResetSkill5Cd()
{
	IsSkillCd[4] = false;
}


// Block
// ---------------------------------------------------------------------------------------------------------------------------------------------------
void AMultiCharacter::BlockPressed()
{
	Server_SetBlock(true);
	GetCharacterMovement()->MaxWalkSpeed = 0.f;
}

void AMultiCharacter::BlockReleased()
{
	Server_SetBlock(false);
	GetCharacterMovement()->MaxWalkSpeed = 400.f;
}

void AMultiCharacter::StaminaRegen()
{
	if (CurrentStamina + 1 >= MaxStamina)
	{
		CurrentStamina = MaxStamina;
	}
	else
	{
		CurrentStamina++;
	}

	if (OnHeadWidget)
	{
		OnHeadWidget->UpdateStamina(CurrentStamina, MaxStamina);
	}
	
}

void AMultiCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	Jump();
}

void AMultiCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	StopJumping();
}



void AMultiCharacter::TurnAtRate(float Rate)
{
	AddControllerYawInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void AMultiCharacter::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
	
}

void AMultiCharacter::MoveForward(float Value)
{

	if (Actions.bIsBlocking == true)
		return;
	
	ForwardValue = Value;
	
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AMultiCharacter::MoveRight(float Value)
{

	if (Actions.bIsBlocking == true)
		return;
	
	RightValue = Value;
	
	if ( (Controller != nullptr) && (Value != 0.0f) )
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
	}
}
