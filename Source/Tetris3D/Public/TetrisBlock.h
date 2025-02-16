#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TetrisBlock.generated.h"

class ATetrisGrid;
class ATetrisGameMode;

UENUM(BlueprintType)
enum class EBlockType : uint8
{
    I UMETA(DisplayName = "I Block"),
    T UMETA(DisplayName = "T Block"),
    L UMETA(DisplayName = "L Block"),
    S UMETA(DisplayName = "S Block"),
    O UMETA(DisplayName = "O Block")
};

UCLASS()
class TETRIS3D_API ATetrisBlock : public AActor
{
    GENERATED_BODY()

public:
    ATetrisBlock();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Block", meta = (AllowPrivateAccess = "true"))
    USceneComponent* BlockMesh;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tetris")
    EBlockType BlockType = EBlockType::O;

    UPROPERTY()
    ATetrisGrid* Grid;

    void MoveLeft();
    void MoveRight();
    void MoveDown();
    void Rotate();

    void StartSoftDrop();  // S�L�[���������Ƃ��̏���
    void StopSoftDrop();   // S�L�[�𗣂����Ƃ��̏���
    void HardDrop();       // W�L�[���������Ƃ��̏���

    int32 GetRotationIndex() const;

    TArray<FVector2D> GetBlockCells() const;
	TMap<FVector2D, UStaticMeshComponent*> GetCellMeshMap() const;

    void UpdateCellPosition(const TArray<int32>& ClearedLines);
    void RemoveCell(FVector2D Cell);

private:
    bool bIsLocked = false;  // �u���b�N���Œ肳�ꂽ���ǂ���

    bool bIsSoftDropping = false;  // S�L�[�������Ă��邩�̃t���O
    FTimerHandle SoftDropTimerHandle; // S�L�[�̍��������p�^�C�}�[

    TArray<FVector> WallKickOffsets;  // �ǃL�b�N�p�̃I�t�Z�b�g

};
