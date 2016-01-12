
#pragma once

#include <Engine/StaticMeshActor.h>
#include <Components/PointLightComponent.h>
#include <Materials/MaterialInstance.h>
#include "LifeSimulator.h"
#include "LifeCell.generated.h"

UCLASS()
class ALifeCell : public AStaticMeshActor
{
        GENERATED_BODY()

    public:
        ALifeCell();

        UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="Components")
        UPointLightComponent *Light;

        UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Life Cell")
        UMaterialInstance *NormalMaterial;
        UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Life Cell")
        UMaterialInstance *ActiveMaterial;

        UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="Life Cell")
        ALifeSimulator *Simulator;

        UPROPERTY(BlueprintReadOnly, Category="Life Cell")
        int32 GridX;
        UPROPERTY(BlueprintReadOnly, Category="Life Cell")
        int32 GridY;

        virtual void BeginPlay() override;

        UFUNCTION(BlueprintCallable, BlueprintPure, Category="Life Cell")
        bool IsHighlighted();
        UFUNCTION(BlueprintCallable, Category="Life Cell")
        void SetHighlighted(bool highlighted);

    private:
        bool mHighlighted;
};
