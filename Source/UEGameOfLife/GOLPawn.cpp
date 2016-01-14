// Fill out your copyright notice in the Description page of Project Settings.

#include "UEGameOfLife.h"
#include "GOLPawn.h"

// Sets default values
AGOLPawn::AGOLPawn()
    : LifeSimulator(0), mMoving(false), mMovingFaster(false), mHUD(0),
      mTargetPos(FVector::ZeroVector), mTargetRot(FRotator::ZeroRotator),
      mLocked(false), mHorizontalMovement(0.0f), mVerticalMovement(0.0f),
      mRotateVertical(0.0f), mRotateHorizontal(0.0f), mLockDistance(0.0f)
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
    FasterSpeed = 25.0f;

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

    APlayerController *pc = Cast<APlayerController>(GetController());
    pc->bShowMouseCursor = true;
    if(HUDClass) {
        mHUD = CreateWidget<UUserWidget>(pc, HUDClass);
        if(mHUD)
            mHUD->AddToViewport();
    }

    // Find a life
    TActorIterator<ALifeSimulator> it(GetWorld());
    if(it) {
        LifeSimulator = *it;
    }

    mLockDistance = MinimumDistance;

    LifeSimulator->GenerateGrid(5);
    SetLocked(true);
}

// Called every frame
void AGOLPawn::Tick( float DeltaTime )
{
    Super::Tick(DeltaTime);

    if(mLocked) {
        // Rotate camera by mouse delta
        if(mMoving) {
            FRotator rot = CameraRoot->GetComponentRotation();
            rot.Pitch = FMath::Clamp(rot.Pitch + mRotateVertical, MinimumLockedPitch, MaximumLockedPitch);
            rot.Yaw = rot.Yaw + mRotateHorizontal;
            rot.Roll = 0.0f;
            CameraRoot->SetWorldRotation(rot);
            CenterMousePosition();
        }

        // Apply distance to target pos
        mTargetPos = FVector(-mLockDistance, 0.0f, 0.0f);
    }
    else {
        if(mMoving) {
            //Rotate camera by mouse delta
            FRotator rot = CameraRoot->GetComponentRotation();
            rot.Pitch = rot.Pitch + mRotateVertical;
            rot.Yaw = rot.Yaw + mRotateHorizontal;
            CameraRoot->SetWorldRotation(rot);
            CenterMousePosition();

            // Process free movement
            FVector pos = CameraRoot->GetComponentLocation();
            float speed = mMovingFaster ? FasterSpeed : Speed;
            pos += CameraRoot->GetForwardVector() * speed * mVerticalMovement;
            pos += CameraRoot->GetRightVector() * speed * mHorizontalMovement;
            CameraRoot->SetWorldLocation(pos);
        }
    }

    // Blend camera torwards target position and rotation
    Camera->SetRelativeLocation(FMath::Lerp(Camera->RelativeLocation, mTargetPos, 1.f / 4.f));
    Camera->SetRelativeRotation(FMath::Lerp(Camera->RelativeRotation, mTargetRot, 1.f / 4.f));
}

// Called to bind functionality to input
void AGOLPawn::SetupPlayerInputComponent(UInputComponent* InputComponent)
{
    Super::SetupPlayerInputComponent(InputComponent);

    InputComponent->BindAction("ChangeCameraMode", IE_Pressed, this, &AGOLPawn::ChangeCameraMode);

    InputComponent->BindAction("MoveCamera", IE_Pressed, this, &AGOLPawn::MoveCameraBegin);
    InputComponent->BindAction("MoveCamera", IE_Released, this, &AGOLPawn::MoveCameraEnd);

    InputComponent->BindAction("MoveCameraFurther", IE_Pressed, this, &AGOLPawn::MoveCameraFurther);
    InputComponent->BindAction("MoveCameraCloser", IE_Pressed, this, &AGOLPawn::MoveCameraCloser);

    InputComponent->BindAction("MoveFaster", IE_Pressed, this, &AGOLPawn::MoveFaster);
    InputComponent->BindAction("MoveFaster", IE_Released, this, &AGOLPawn::MoveSlower);

    InputComponent->BindAction("ToggleSimulation", IE_Pressed, this, &AGOLPawn::ToggleSimulation);

    InputComponent->BindAction("ActivateCell", IE_Pressed, this, &AGOLPawn::ActivateCell);
    InputComponent->BindAction("ActivateCell", IE_Released, this, &AGOLPawn::ClearActivationStatus);
    InputComponent->BindAction("DeactivateCell", IE_Pressed, this, &AGOLPawn::DeactivateCell);
    InputComponent->BindAction("DeactivateCell", IE_Released, this, &AGOLPawn::ClearActivationStatus);

    InputComponent->BindAxis("MoveCameraVertical", this, &AGOLPawn::MoveVertical);
    InputComponent->BindAxis("MoveCameraHorizontal", this, &AGOLPawn::MoveHorizontal);

    InputComponent->BindAxis("RotateCameraVertical", this, &AGOLPawn::RotateVertical);
    InputComponent->BindAxis("RotateCameraHorizontal", this, &AGOLPawn::RotateHorizontal);
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

void AGOLPawn::CenterMousePosition()
{
    APlayerController *pc = Cast<APlayerController>(GetController());
    if(!pc) return;
    FViewport *vp = pc->GetLocalPlayer()->ViewportClient->Viewport;
    FIntPoint size = vp->GetSizeXY();
    vp->SetMouse(size.X / 2, size.Y / 2);
}

void AGOLPawn::RepositionCamera()
{
    SetLocked(true);
    if(LifeSimulator) {
        int size = LifeSimulator->GetSize();
        mLockDistance = FMath::Clamp(150.0f * size, MinimumDistance, MaximumDistance);
    }
}

void AGOLPawn::ChangeCameraMode()
{
    SetLocked(!mLocked);
}

void AGOLPawn::MoveCameraBegin()
{
    APlayerController *pc = CastChecked<APlayerController>(GetController());
    pc->bShowMouseCursor = false;
    mMoving = true;
}

void AGOLPawn::MoveCameraEnd()
{
    APlayerController *pc = CastChecked<APlayerController>(GetController());
    pc->bShowMouseCursor = true;
    mMoving = false;
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

void AGOLPawn::MoveFaster()
{
    mMovingFaster = true;
}

void AGOLPawn::MoveSlower()
{
    mMovingFaster = false;
}

void AGOLPawn::ToggleSimulation()
{
    LifeSimulator->Simulating = !LifeSimulator->Simulating;
}

void AGOLPawn::ActivateCell()
{
    LifeSimulator->ActivationStatus = AS_Activating;
}

void AGOLPawn::DeactivateCell()
{
    LifeSimulator->ActivationStatus = AS_Deactivating;
}

void AGOLPawn::ClearActivationStatus()
{
    LifeSimulator->ActivationStatus = AS_None;
}

void AGOLPawn::MoveVertical(float delta)
{
    mVerticalMovement = delta;
}

void AGOLPawn::MoveHorizontal(float delta)
{
    mHorizontalMovement = delta;
}

void AGOLPawn::RotateVertical(float delta)
{
    mRotateVertical = delta;
}

void AGOLPawn::RotateHorizontal(float delta)
{
    mRotateHorizontal = delta;
}
