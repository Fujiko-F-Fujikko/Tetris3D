#include "TetrisPlayerController.h"
#include "Kismet/GameplayStatics.h"

void ATetrisPlayerController::BeginPlay()
{
    Super::BeginPlay();

    // TetrisGameMode を取得
    TetrisGameMode = Cast<ATetrisGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
}

void ATetrisPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    InputComponent->BindAction("MoveLeft", IE_Pressed, this, &ATetrisPlayerController::MoveLeft);
    InputComponent->BindAction("MoveRight", IE_Pressed, this, &ATetrisPlayerController::MoveRight);
    InputComponent->BindAction("MoveDown", IE_Pressed, this, &ATetrisPlayerController::MoveDown);
    InputComponent->BindAction("Rotate", IE_Pressed, this, &ATetrisPlayerController::Rotate);

    // Sキーで高速落下
    InputComponent->BindAction("SoftDrop", IE_Pressed, this, &ATetrisPlayerController::StartSoftDrop);
    InputComponent->BindAction("SoftDrop", IE_Released, this, &ATetrisPlayerController::StopSoftDrop);

    // Wキーでハードドロップ
    InputComponent->BindAction("HardDrop", IE_Pressed, this, &ATetrisPlayerController::HardDrop);

}

void ATetrisPlayerController::MoveLeft()
{
    if (TetrisGameMode && TetrisGameMode->CurrentBlock)
    {
        TetrisGameMode->CurrentBlock->MoveLeft();
    }
}

void ATetrisPlayerController::MoveRight()
{
    if (TetrisGameMode && TetrisGameMode->CurrentBlock)
    {
        TetrisGameMode->CurrentBlock->MoveRight();
    }
}

void ATetrisPlayerController::MoveDown()
{
    if (TetrisGameMode && TetrisGameMode->CurrentBlock)
    {
        TetrisGameMode->CurrentBlock->MoveDown();
    }
}

void ATetrisPlayerController::Rotate()
{
    if (TetrisGameMode && TetrisGameMode->CurrentBlock)
    {
        TetrisGameMode->CurrentBlock->Rotate();
    }
}

void ATetrisPlayerController::StartSoftDrop()
{
	if (TetrisGameMode && TetrisGameMode->CurrentBlock)
	{
		TetrisGameMode->CurrentBlock->StartSoftDrop();
	}
}

void ATetrisPlayerController::StopSoftDrop()
{
	if (TetrisGameMode && TetrisGameMode->CurrentBlock)
	{
		TetrisGameMode->CurrentBlock->StopSoftDrop();
	}
}

void ATetrisPlayerController::HardDrop()
{
	if (TetrisGameMode && TetrisGameMode->CurrentBlock)
	{
		TetrisGameMode->CurrentBlock->HardDrop();
	}
}
