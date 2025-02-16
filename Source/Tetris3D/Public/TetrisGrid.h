#pragma once

#include "CoreMinimal.h"
#include "TetrisBlock.h"
#include "TetrisGrid.generated.h"

UCLASS()
class TETRIS3D_API ATetrisGrid : public AActor
{
    GENERATED_BODY()

public:
    ATetrisGrid();

protected:
    virtual void BeginPlay() override;

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
    int32 GridWidth = 10; // �� 10 �}�X

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
    int32 GridHeight = 20; // �c 20 �}�X

    UPROPERTY(VisibleAnywhere)
    USceneComponent* RootScene;  // SceneComponent ��ǉ�

    UPROPERTY()
    TArray<ATetrisBlock*> GridData; // 1�����z��ŊǗ�

    void InitializeGrid();
    void RegisterBlock(const TArray<FVector2D>& BlockCells, ATetrisBlock* BlockInstance);
    void UnregisterBlock(const TArray<FVector2D>& BlockCells);
    bool IsCellEmpty(int32 X, int32 Y);
    void ClearFullLines();

    FVector2D WorldToGrid(FVector WorldLocation);
    FVector GridToWorld(FVector2D GridPosition);
};
