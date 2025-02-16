#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TetrisGameMode.h"
#include "TetrisBlock.h"
#include "TetrisPlayerController.generated.h"

UCLASS()
class TETRIS3D_API ATetrisPlayerController : public APlayerController
{
    GENERATED_BODY()

protected:
    virtual void BeginPlay() override;
    virtual void SetupInputComponent() override;

public:
    void MoveLeft();
    void MoveRight();
    void MoveDown();
    void Rotate();

	void StartSoftDrop();
	void StopSoftDrop();
	void HardDrop();

    // �Q�[�����[�h�ւ̎Q�Ɓi�u���b�N�𑀍삷�邽�߁j
    class ATetrisGameMode* TetrisGameMode;
};
