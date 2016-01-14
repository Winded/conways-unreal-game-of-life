// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Pawn.h"
#include "Camera/CameraComponent.h"
#include "LifeSimulator.h"
#include <UserWidget.h>
#include "GOLPawn.generated.h"

UCLASS()
class AGOLPawn : public APawn
{
        GENERATED_BODY()

    public:
        AGOLPawn();

        UPROPERTY(BlueprintReadOnly, Category="Components")
        USceneComponent *CameraRoot;
        UPROPERTY(BlueprintReadOnly, Category="Components")
        UCameraComponent *Camera;

        UPROPERTY(EditAnywhere, Category="GUI")
        TSubclassOf<UUserWidget> HUDClass;

        UPROPERTY(BlueprintReadOnly, Category="Game Of Life")
        ALifeSimulator *LifeSimulator;

        UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Game Of Life")
        FVector LockPoint;
        UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Game Of Life")
        FRotator InitialRotation;

        UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Game Of Life")
        float MinimumLockedPitch;
        UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Game Of Life")
        float MaximumLockedPitch;

        UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Game Of Life")
        float MinimumDistance;
        UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Game Of Life")
        float MaximumDistance;

        /** How much do we change distance per scroll */
        UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Game Of Life")
        float ZoomInterval;
        UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Game Of Life")
        float Speed;
        UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Game Of Life")
        float FasterSpeed;

        // Called when the game starts or when spawned
        virtual void BeginPlay() override;

        // Called every frame
        virtual void Tick( float DeltaSeconds ) override;

        // Called to bind functionality to input
        virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;

        UFUNCTION(BlueprintCallable, BlueprintPure, Category="Game Of Life")
        bool IsLocked();
        UFUNCTION(BlueprintCallable, Category="Game Of Life")
        void SetLocked(bool locked);

        /** Set player's cursor position to screen center */
        UFUNCTION(BlueprintCallable, Category="Game Of Life")
        void CenterMousePosition();

        /** Lock camera and adjust distance to cell grid's size */
        UFUNCTION(BlueprintCallable, Category="Game Of Life")
        void RepositionCamera();

    protected:
        void ChangeCameraMode();

        void MoveCameraBegin();
        void MoveCameraEnd();

        void MoveCameraFurther();
        void MoveCameraCloser();

        void MoveFaster();
        void MoveSlower();

        void ToggleSimulation();

        void ActivateCell();
        void DeactivateCell();
        void ClearActivationStatus();

        void MoveVertical(float delta);
        void MoveHorizontal(float delta);

        void RotateVertical(float delta);
        void RotateHorizontal(float delta);

    private:
        UUserWidget *mHUD;

        bool mMoving;
        bool mMovingFaster;
        FVector mTargetPos;
        FRotator mTargetRot;

        bool mLocked;

        float mHorizontalMovement;
        float mVerticalMovement;

        float mRotateVertical;
        float mRotateHorizontal;

        float mLockDistance;
};
