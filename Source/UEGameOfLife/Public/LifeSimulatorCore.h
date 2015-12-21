// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "LifeSimulatorCore.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UEGAMEOFLIFE_API ULifeSimulatorCore : public UActorComponent
{
	GENERATED_BODY()

public:	
	ULifeSimulatorCore();

	// Called when the game starts
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

	/** Generates a new grid with the specified size. Deletes the old grid. */
	UFUNCTION(BlueprintCallable, Category="Life Simulator Core")
	void GenerateGrid(int32 size);

	/** Processes one cycle of game of life, if a grid has been generated. */
	UFUNCTION(BlueprintCallable, Category = "Life Simulator Core")
	void CycleGrid();

	/** Get the value of the specified grid cell. */
	UFUNCTION(BlueprintCallable, Category = "Life Simulator Core")
	bool GetCellValue(int32 gridX, int32 gridY);

	/** Set the value of the specified grid cell. */
	UFUNCTION(BlueprintCallable, Category = "Life Simulator Core")
	void SetCellValue(int32 gridX, int32 gridY, bool value);

private:
	int GetLivingNeighbors(int gridX, int gridY);

	bool **mGrid;
	bool **mGridNextState;
	int mSize;
	
};
