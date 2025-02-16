#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TetrisGrid.h"
#include "TetrisGameMode.generated.h"

class ATetrisGrid;
class ATetrisBlock;

UCLASS()
class TETRIS3D_API ATetrisGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    ATetrisGameMode();

protected:
    virtual void BeginPlay() override;

public:
    UPROPERTY()
    ATetrisGrid* Grid;

    UPROPERTY(EditAnywhere, Category = "Tetris")
    TArray<TSubclassOf<ATetrisBlock>> BlockTypes;

    UPROPERTY()
    ATetrisBlock* CurrentBlock; // åªç›ÇÃÉuÉçÉbÉNÇä«óù

    void SpawnNewBlock();
};
