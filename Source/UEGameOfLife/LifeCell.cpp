#include "UEGameOfLife.h"
#include "LifeCell.h"

ALifeCell::ALifeCell()
    : mHighlighted(false)
{
    GridX = 0;
    GridY = 0;

    Light = CreateDefaultSubobject<UPointLightComponent>(TEXT("Light"));
    Light->AttachTo(GetStaticMeshComponent());

    PrimaryActorTick.bCanEverTick = true;
}

void ALifeCell::BeginPlay()
{
    Super::BeginPlay();
    GetStaticMeshComponent()->SetMaterial(0, NormalMaterial);
    Light->SetVisibility(false);
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

