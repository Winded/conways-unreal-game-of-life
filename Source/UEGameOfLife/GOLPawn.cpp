// Fill out your copyright notice in the Description page of Project Settings.

#include "UEGameOfLife.h"
#include "GOLPawn.h"

// Sets default values
AGOLPawn::AGOLPawn()
    : mLifeSim(0), mMoving(false), mTargetPos(FVector::ZeroVector), mTargetRot(FRotator::ZeroRotator),
      mLocked(false), mHorizontalMovement(0.0f), mVerticalMovement(0.0f), mLockDistance(0.0f)
{
    // Defaults
    LockPoint = FVector::ZeroVector;
    InitialRotation = FRotator(-30.0f, 45.0f, 0.0f);
    MinimumLockedPitch = -80.0f;
    MaximumLockedPitch = -10.0f;
    MinimumDistance = 2000.0f;
    MaximumDistance = 10000.0f;
    ZoomInterval = 100.0f;
    Speed = 10.0f;

 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    // Initialize components
    CameraRoot = CreateDefaultSubobject<USceneComponent>(TEXT("CameraRoot"));
    RootComponent = CameraRoot;
    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    Camera->AttachTo(CameraRoot);
}

// Called when the game starts or when spawned
void AGOLPawn::BeginPlay()
{
    Super::BeginPlay();

    // Find a life
    TActorIterator<ALifeSimulator> it(GetWorld());
    if(it) {
        mLifeSim = *it;
    }

    mLockDistance = MinimumDistance;
}

// Called every frame
void AGOLPawn::Tick( float DeltaTime )
{
    Super::Tick(DeltaTime);
    APlayerController *pc = Cast<APlayerController>(GetController());
    if(!pc) {
        return;
    }

    if(mLocked) {
        // Rotate camera by mouse delta
        if(mMoving) {
            float mouseX, mouseY;
            pc->GetInputMouseDelta(mouseX, mouseY);
            FRotator rot = CameraRoot->GetComponentRotation();
            rot.Pitch = FMath::Clamp(rot.Pitch + mouseY, MinimumLockedPitch, MaximumLockedPitch);
            rot.Yaw = rot.Yaw + mouseX;
            rot.Roll = 0.0f;
            CameraRoot->SetWorldRotation(rot);
        }

        // Apply distance to target pos
        mTargetPos = FVector(-mLockDistance, 0.0f, 0.0f);
    }
    else {
        if(mMoving) {
            //Rotate camera by mouse delta
            float mouseX, mouseY;
            pc->GetInputMouseDelta(mouseX, mouseY);
            FRotator rot = CameraRoot->GetComponentRotation();
            rot.Pitch = rot.Pitch + mouseY;
            rot.Yaw = rot.Yaw + mouseX;
            CameraRoot->SetWorldRotation(rot);

            // Process free movement
            FVector pos = CameraRoot->GetComponentLocation();
            pos += CameraRoot->GetForwardVector() * Speed * mVerticalMovement;
            pos += CameraRoot->GetRightVector() * Speed * mHorizontalMovement;
            CameraRoot->SetWorldLocation(pos);
        }
    }

    // Blend camera torwards target position and rotation
    Camera->SetRelativeLocation(FMath::Lerp(Camera->RelativeLocation, mTargetPos, 1.f / 4.f));
    Camera->SetRelativeRotation(FMath::Lerp(Camera->RelativeRotation, mTargetRot, 1.f / 4.f));
}

// Called to bind functionality to input
void AGOLPawn::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
    Super::SetupPlayerInputComponent(InputComponent);

    InputComponent->BindAction("ChangeCameraMode", IE_Pressed, this, &AGOLPawn::ChangeCameraMode);

    InputComponent->BindAction("MoveCamera", IE_Pressed, this, &AGOLPawn::MoveCameraBegin);
    InputComponent->BindAction("MoveCamera", IE_Released, this, &AGOLPawn::MoveCameraEnd);

    InputComponent->BindAction("MoveCameraFurther", IE_Pressed, this, &AGOLPawn::MoveCameraFurther);
    InputComponent->BindAction("MoveCameraCloser", IE_Pressed, this, &AGOLPawn::MoveCameraCloser);

    InputComponent->BindAction("MoveForward", IE_Pressed, this, &AGOLPawn::MoveForward);
    InputComponent->BindAction("MoveForward", IE_Released, this, &AGOLPawn::ClearVerticalMovement);
    InputComponent->BindAction("MoveBackward", IE_Pressed, this, &AGOLPawn::MoveBackward);
    InputComponent->BindAction("MoveBackward", IE_Released, this, &AGOLPawn::ClearVerticalMovement);
    InputComponent->BindAction("MoveRight", IE_Pressed, this, &AGOLPawn::MoveRight);
    InputComponent->BindAction("MoveRight", IE_Released, this, &AGOLPawn::ClearHorizontalMovement);
    InputComponent->BindAction("MoveLeft", IE_Pressed, this, &AGOLPawn::MoveLeft);
    InputComponent->BindAction("MoveLeft", IE_Released, this, &AGOLPawn::ClearHorizontalMovement);

    InputComponent->BindAction("ToggleSimulation", IE_Pressed, this, &AGOLPawn::ToggleSimulation);

    InputComponent->BindAction("ActivateCell", IE_Pressed, this, &AGOLPawn::ActivateCell);
    InputComponent->BindAction("ActivateCell", IE_Released, this, &AGOLPawn::ClearActivationStatus);
    InputComponent->BindAction("DeactivateCell", IE_Pressed, this, &AGOLPawn::DeactivateCell);
    InputComponent->BindAction("DeactivateCell", IE_Released, this, &AGOLPawn::ClearActivationStatus);
}

bool AGOLPawn::IsLocked()
{
    return mLocked;
}

void AGOLPawn::SetLocked(bool locked)
{
    FVector camPos = Camera->GetComponentLocation();
    FRotator camRot = Camera->GetComponentRotation();
    if(locked) {
        CameraRoot->SetWorldLocation(LockPoint);
        CameraRoot->SetWorldRotation(InitialRotation);
        Camera->SetWorldLocation(camPos);
        Camera->SetWorldRotation(camRot);
    }
    else {
        mTargetPos = FVector::ZeroVector;
        mTargetRot = FRotator::ZeroRotator;
        Camera->SetRelativeLocationAndRotation(mTargetPos, mTargetRot);
        CameraRoot->SetWorldLocation(camPos);
        CameraRoot->SetWorldRotation(camRot);
    }
    mLocked = locked;
}

void AGOLPawn::ChangeCameraMode()
{
    SetLocked(!mLocked);
}

void AGOLPawn::MoveCameraBegin()
{
    APlayerController *pc = Cast<APlayerController>(GetController());
    if(pc) {
        pc->bShowMouseCursor = false;
        mMoving = true;
    }
}

void AGOLPawn::MoveCameraEnd()
{
    APlayerController *pc = Cast<APlayerController>(GetController());
    if(pc) {
        pc->bShowMouseCursor = true;
        mMoving = false;
    }
}

void AGOLPawn::MoveCameraFurther()
{
    if(!mLocked)
        return;

    mLockDistance = FMath::Clamp(mLockDistance + ZoomInterval, MinimumDistance, MaximumDistance);
}

void AGOLPawn::MoveCameraCloser()
{
    if(!mLocked)
        return;

    mLockDistance = FMath::Clamp(mLockDistance - ZoomInterval, MinimumDistance, MaximumDistance);
}

void AGOLPawn::MoveForward()
{
    mVerticalMovement = 1.0f;
}

void AGOLPawn::MoveBackward()
{
    mVerticalMovement = -1.0f;
}

void AGOLPawn::MoveRight()
{
    mHorizontalMovement = 1.0f;
}

void AGOLPawn::MoveLeft()
{
    mHorizontalMovement = -1.0f;
}

void AGOLPawn::ClearVerticalMovement()
{
    mVerticalMovement = 0.0f;
}

void AGOLPawn::ClearHorizontalMovement()
{
    mHorizontalMovement = 0.0f;
}

void AGOLPawn::ToggleSimulation()
{
    // TODO
}

void AGOLPawn::ActivateCell()
{
    // TODO
}

void AGOLPawn::DeactivateCell()
{
    // TODO
}

void AGOLPawn::ClearActivationStatus()
{
    // TODO
}

