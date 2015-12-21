// Fill out your copyright notice in the Description page of Project Settings.

#include "UEGameOfLife.h"
#include "LifeSimulatorCore.h"


ULifeSimulatorCore::ULifeSimulatorCore()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	bWantsBeginPlay = true;
	PrimaryComponentTick.bCanEverTick = false;

	Repeating = false;
	mGrid = 0;
	mGridNextState = 0;
	mSize = 0;
}


// Called when the game starts
void ULifeSimulatorCore::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void ULifeSimulatorCore::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );
}

void ULifeSimulatorCore::GenerateGrid(int32 size)
{
	if (mGrid) {
		for (int i = 0; i < mSize; i++) {
			delete[] mGrid[i];
		}
		delete[] mGrid;
		mGrid = 0;
	}
	if (mGridNextState) {
		for (int i = 0; i < mSize; i++) {
			delete[] mGridNextState[i];
		}
		delete[] mGridNextState;
		mGridNextState = 0;
	}

	mSize = size;
	mGrid = new bool*[size];
	for (int i = 0; i < size; i++) {
		mGrid[i] = new bool[size];
		for (int ii = 0; ii < size; ii++)
			mGrid[i][ii] = false;
	}
	mGridNextState = new bool*[size];
	for (int i = 0; i < size; i++) {
		mGridNextState[i] = new bool[size];
		for (int ii = 0; ii < size; ii++)
			mGridNextState[i][ii] = false;
	}
}

void ULifeSimulatorCore::CycleGrid()
{
	if (!mGrid || !mGridNextState) return;

	for (int x = 1; x <= mSize; x++)
		for (int y = 1; y <= mSize; y++) {
			bool result = false;
			int count = GetLivingNeighbors(x, y);
			bool living = mGrid[x - 1][y - 1];

			if (living && (count == 2 || count == 3))
				result = true;
			else if (!living && count == 3)
				result = true;

			mGridNextState[x - 1][y - 1] = result;
		}
	
	for (int x = 1; x <= mSize; x++)
		for (int y = 1; y <= mSize; y++) {
			mGrid[x - 1][y - 1] = mGridNextState[x - 1][y - 1];
		}
}

bool ULifeSimulatorCore::GetCellValue(int32 gridX, int32 gridY)
{
	if (!mGrid) return false;
	if (gridX > mSize || gridY > mSize ||
		gridX < 1 || gridY < 1)
		return false;

	return mGrid[gridX - 1][gridY - 1];
}

void ULifeSimulatorCore::SetCellValue(int32 gridX, int32 gridY, bool value)
{
	if (!mGrid) return;
	if (gridX > mSize || gridY > mSize ||
		gridX < 1 || gridY < 1)
		return;

	mGrid[gridX - 1][gridY - 1] = value;
}

int ULifeSimulatorCore::GetLivingNeighbors(int gridX, int gridY)
{
	if (!mGrid) return 0;
	if (gridX > mSize || gridY > mSize ||
		gridX < 1 || gridY < 1)
		return 0;

	int count = 0;

	// Right side
	if (IsNeighborAlive(gridX + 1, gridY))
		count++;
	// Right Bottom side
	if (IsNeighborAlive(gridX + 1, gridY + 1))
		count++;
	// Bottom side
	if (IsNeighborAlive(gridX, gridY + 1))
		count++;
	// Left Bottom side
	if (IsNeighborAlive(gridX - 1, gridY + 1))
		count++;
	// Left side
	if (IsNeighborAlive(gridX - 1, gridY))
		count++;
	// Left Upper side
	if (IsNeighborAlive(gridX - 1, gridY - 1))
		count++;
	// Upper side
	if (IsNeighborAlive(gridX, gridY - 1))
		count++;
	// Right Upper side
	if (IsNeighborAlive(gridX + 1, gridY - 1))
		count++;

	return count;
}

bool ULifeSimulatorCore::IsNeighborAlive(int gridX, int gridY)
{
	if (Repeating && gridX > mSize)
		gridX = 1;
	else if (Repeating && gridX < 1)
		gridX = mSize;
	else if (!Repeating && (gridX < 1 || gridX > mSize))
		return false;

	if (Repeating && gridY > mSize)
		gridY = 1;
	else if (Repeating && gridY < 1)
		gridY = mSize;
	else if (!Repeating && (gridY < 1 || gridY > mSize))
		return false;

	return mGrid[gridX - 1][gridY - 1];
}

