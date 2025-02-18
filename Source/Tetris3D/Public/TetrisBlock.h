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

    void StartSoftDrop();  // Sキーを押したときの処理
    void StopSoftDrop();   // Sキーを離したときの処理
    void HardDrop();       // Wキーを押したときの処理

    TArray<FVector2D> GetBlockCells() const;
	TMap<FVector2D, UStaticMeshComponent*> GetCellMeshMap() const;

    void UpdateCellPosition(const TArray<int32>& ClearedLines);
    void RemoveCell(FVector2D Cell);


private:
    bool bIsLocked = false;  // ブロックが固定されたかどうか

    bool bIsSoftDropping = false;  // Sキーを押しているかのフラグ
    FTimerHandle SoftDropTimerHandle; // Sキーの高速落下用タイマー

    TArray<FVector> WallKickOffsets;  // 壁キック用のオフセット

    UPROPERTY()
    TMap<FVector2D, UStaticMeshComponent*> GhostBlockMeshes; // ゴーストブロックの各セル用マップ
    UPROPERTY()
    TSoftObjectPtr<UMaterial> GhostMaterial; // ゴーストブロック用のマテリアル
	UPROPERTY()
	TSoftObjectPtr<UStaticMesh> GhostMesh; // ブロックのメッシュアセット

    bool IsGhostMesh(UStaticMeshComponent* MeshComp) const;
	void InitializeGhostBlocks(); // ゴーストブロックの初期化
    void UpdateGhostPosition(); // ゴーストブロックの位置更新
	void ClearGhostBlocks(); // ゴーストブロックの削除
};
