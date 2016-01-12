// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Pawn.h"
#include "Camera/CameraComponent.h"
#include "LifeSimulator.h"
#include "GOLPawn.generated.h"

UCLASS()
class AGOLPawn : public APawn
{
        GENERATED_BODY()

    public:
        AGOLPawn();

        UPROPERTY(BlueprintReadOnly, Category="Game Of Life")
        USceneComponent *CameraRoot;
        UPROPERTY(BlueprintReadOnly, Category="Game Of Life")
        UCameraComponent *Camera;

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

        UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Game Of Life")
        float ZoomInterval;
        UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Game Of Life")
        float Speed;

        // Called when the game starts or when spawned
        virtual void BeginPlay() override;

        // Called every frame
        virtual void Tick( float DeltaSeconds ) override;

        // Called to bind functionality to input
        virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

        UFUNCTION(BlueprintCallable, BlueprintPure, Category="Game Of Life")
        bool IsLocked();
        UFUNCTION(BlueprintCallable, Category="Game Of Life")
        void SetLocked(bool locked);

    protected:
        void ChangeCameraMode();

        void MoveCameraBegin();
        void MoveCameraEnd();

        void MoveCameraFurther();
        void MoveCameraCloser();

        void MoveForward();
        void MoveBackward();
        void MoveRight();
        void MoveLeft();
        void ClearVerticalMovement();
        void ClearHorizontalMovement();

        void ToggleSimulation();

        void ActivateCell();
        void DeactivateCell();
        void ClearActivationStatus();

    private:
        ALifeSimulator *mLifeSim;

        bool mMoving;
        FVector mTargetPos;
        FRotator mTargetRot;

        bool mLocked;

        float mHorizontalMovement;
        float mVerticalMovement;

        float mLockDistance;
};
