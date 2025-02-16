#include "TetrisGameMode.h"
#include "TetrisGrid.h"
#include "TetrisBlock.h"
#include "Kismet/GameplayStatics.h"

ATetrisGameMode::ATetrisGameMode()
{
    PrimaryActorTick.bCanEverTick = false;
}

void ATetrisGameMode::BeginPlay()
{
    Super::BeginPlay();

    FVector GridLocation = FVector(0.f, 0.f, -900.f);  // グリッドの底辺を設定
    Grid = GetWorld()->SpawnActor<ATetrisGrid>(ATetrisGrid::StaticClass(), GridLocation, FRotator::ZeroRotator);

    SpawnNewBlock();
}

void ATetrisGameMode::SpawnNewBlock()
{
    if (BlockTypes.Num() > 0)
    {
        int32 RandomIndex = FMath::RandRange(0, BlockTypes.Num() - 1);
        TSubclassOf<ATetrisBlock> SelectedBlock = BlockTypes[RandomIndex];

        FVector2D SpawnGridPos = FVector2D(4, Grid->GridHeight - 3); // X=4 にして余裕を持たせる
        FVector SpawnLocation = Grid->GridToWorld(SpawnGridPos);
        FRotator SpawnRotation = FRotator::ZeroRotator;
        CurrentBlock = GetWorld()->SpawnActor<ATetrisBlock>(SelectedBlock, SpawnLocation, SpawnRotation);
        // スポーン後にブロックがはみ出していないかチェック
        if (CurrentBlock)
        {
            UE_LOG(LogTemp, Warning, TEXT("Spawned new block: %s at (%.2f, %.2f, %.2f)"),
                *CurrentBlock->GetName(), SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z);

            TArray<FVector2D> BlockCells = CurrentBlock->GetBlockCells();

            for (FVector2D& Cell : BlockCells)
            {
                if (Cell.X < 0)
                {
                    UE_LOG(LogTemp, Warning, TEXT("Block is out of bounds (left), shifting right"));
                    CurrentBlock->SetActorLocation(CurrentBlock->GetActorLocation() + FVector(100.f, 0.f, 0.f));
                }
                else if (Cell.X >= Grid->GridWidth)
                {
                    UE_LOG(LogTemp, Warning, TEXT("Block is out of bounds (right), shifting left"));
                    CurrentBlock->SetActorLocation(CurrentBlock->GetActorLocation() - FVector(100.f, 0.f, 0.f));
                }
            }
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to spawn new block!"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("BlockTypes array is empty!"));
    }
}
