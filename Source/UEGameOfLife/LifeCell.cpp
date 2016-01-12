#include "UEGameOfLife.h"
#include "LifeCell.h"

ALifeCell::ALifeCell()
    : mHighlighted(false)
{
    GridX = 0;
    GridY = 0;

    PrimaryActorTick.bCanEverTick = false;
}

void ALifeCell::BeginPlay()
{
    Super::BeginPlay();
    GetStaticMeshComponent()->SetMaterial(0, NormalMaterial);
}

bool ALifeCell::IsHighlighted()
{
    return mHighlighted;
}

void ALifeCell::SetHighlighted(bool highlighted)
{
    GetStaticMeshComponent()->SetMaterial(0, highlighted ? ActiveMaterial : NormalMaterial);
    Light->SetVisibility(highlighted);
    mHighlighted = highlighted;
}

