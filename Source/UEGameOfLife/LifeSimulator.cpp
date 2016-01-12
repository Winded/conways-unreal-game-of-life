#include "UEGameOfLife.h"
#include "LifeSimulator.h"
#include "LifeCell.h"

ALifeSimulator::ALifeSimulator()
    : mGenerated(false), mGrid(0), mGridNextState(0), mSize(0), mLastSimulation(0.0f)
{
    Simulating = false;
    Spacing = 10.0f;
    SimulationInterval = 0.1f;
    ActivationStatus = AS_None;
    Repeating = false;

    PrimaryActorTick.bCanEverTick = true;
}

ALifeSimulator::~ALifeSimulator()
{
    if(mGenerated) {
        for (int i = 0; i < mSize; i++) {
            delete[] mGrid[i];
        }
        delete[] mGrid;
        mGrid = 0;
        for (int i = 0; i < mSize; i++) {
            delete[] mGridNextState[i];
        }
        delete[] mGridNextState;
        mGridNextState = 0;
    }
}

void ALifeSimulator::BeginPlay()
{
    Super::BeginPlay();
}

void ALifeSimulator::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    if(Simulating) {
        float t = GetWorld()->GetGameState()->GetServerWorldTimeSeconds();
        if(t - mLastSimulation >= SimulationInterval) {
            SimulationTick();
            mLastSimulation = t;
        }
    }
    else {
        APlayerController *pc = *(GetWorld()->GetPlayerControllerIterator());
        FHitResult hit;
        pc->GetHitResultUnderCursorByChannel(TraceChannel.GetValue(), true, hit);
        ALifeCell *cell = Cast<ALifeCell>(hit.Actor.Get());
        if(cell) {
            switch(ActivationStatus) {
            case AS_Activating:
                SetCellValue(cell, true);
                break;
            case AS_Deactivating:
                SetCellValue(cell, false);
                break;
            }
        }
    }
}

void ALifeSimulator::GenerateGrid(int32 size)
{
    if(mGenerated) {
        for (int i = 0; i < mSize; i++) {
            delete[] mGrid[i];
        }
        delete[] mGrid;
        mGrid = 0;
        for (int i = 0; i < mSize; i++) {
            delete[] mGridNextState[i];
        }
        delete[] mGridNextState;
        mGridNextState = 0;

        for(int32 i = 0; i < mCells.Num(); i++) {
            ALifeCell *cell = mCells[i];
            cell->Destroy();
        }
        mCells.Empty();
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

    for(int32 x = 1; x <= size; x++) {
        for(int32 y = 1; y <= size; y++) {
            ALifeCell *cell = (ALifeCell*)GetWorld()->SpawnActor(CellClass);
            cell->GridX = x;
            cell->GridY = y;
            AddCell(cell);
        }
    }

    mGenerated = true;
}

int32 ALifeSimulator::GetSize()
{
    return mSize;
}

void ALifeSimulator::SimulationTick()
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

    for(int i = 0; i < mCells.Num(); i++) {
        ALifeCell *cell = mCells[i];
        bool value = mGrid[cell->GridX - 1][cell->GridY - 1];
        cell->SetHighlighted(value);
    }
}

void ALifeSimulator::AddCell(ALifeCell *cell)
{
    cell->Simulator = this;
    FVector pos = GetActorLocation();
    pos.X += (float)(cell->GridX) * Spacing - (float)mSize / 2.f;
    pos.Y += (float)(cell->GridY) * Spacing - (float)mSize / 2.f;
    cell->SetActorLocation(pos);
    mCells.Add(cell);
}

bool ALifeSimulator::GetCellValue(int32 gridX, int32 gridY)
{
    if (!mGrid) return false;
    if (gridX > mSize || gridY > mSize ||
        gridX < 1 || gridY < 1)
        return false;

    return mGrid[gridX - 1][gridY - 1];
}

void ALifeSimulator::SetCellValue(int32 gridX, int32 gridY, bool value)
{
    if (!mGrid) return;
    if (gridX > mSize || gridY > mSize ||
        gridX < 1 || gridY < 1)
        return;

    mGrid[gridX - 1][gridY - 1] = value;
}

void ALifeSimulator::SetCellValue(ALifeCell *cell, bool value)
{
    SetCellValue(cell->GridX, cell->GridY, value);
    cell->SetHighlighted(value);
}

int ALifeSimulator::GetLivingNeighbors(int gridX, int gridY)
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

bool ALifeSimulator::IsNeighborAlive(int gridX, int gridY)
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

