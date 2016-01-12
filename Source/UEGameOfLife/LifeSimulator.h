
#pragma once

#include <GameFramework/Actor.h>
#include <EnumAsByte.h>
#include "LifeSimulator.generated.h"

class ALifeCell;

enum EActivationStatus
{
    AS_None,
    AS_Activating,
    AS_Deactivating
};

UCLASS()
class ALifeSimulator : public AActor
{
        GENERATED_BODY()

    public:
        ALifeSimulator();
        ~ALifeSimulator();

        UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category="Life Simulator")
        bool Simulating;

        UPROPERTY(EditAnywhere, Category="Life Simulator")
        TSubclassOf<ALifeCell> CellClass;

        UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Life Simulator")
        float Spacing;

        UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Life Simulator")
        float SimulationInterval;

        UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Life Simulator")
        bool Repeating;

        UPROPERTY(EditAnywhere, Category="Life Simulator")
        TEnumAsByte<ETraceTypeQuery> TraceChannel;

        EActivationStatus ActivationStatus;

        virtual void BeginPlay() override;
        virtual void Tick(float DeltaSeconds) override;

        UFUNCTION(BlueprintCallable, Category="Life Simulator")
        void GenerateGrid(int32 size);

        UFUNCTION(BlueprintCallable, BlueprintPure, Category="Life Simulator")
        int32 GetSize();

    private:
        void SimulationTick();
        void AddCell(ALifeCell *cell);
        bool GetCellValue(int32 gridX, int32 gridY);
        void SetCellValue(int32 gridX, int32 gridY, bool value);
        void SetCellValue(ALifeCell *cell, bool value);

        int GetLivingNeighbors(int gridX, int gridY);
        bool IsNeighborAlive(int gridX, int gridY);

        bool mGenerated;

        bool **mGrid;
        bool **mGridNextState;
        int mSize;

        TArray<ALifeCell*> mCells;

        float mLastSimulation;
};
