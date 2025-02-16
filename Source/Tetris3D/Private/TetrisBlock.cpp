#include "TetrisBlock.h"
#include "TetrisGrid.h"
#include "TetrisGameMode.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"

// コンストラクタ
ATetrisBlock::ATetrisBlock()
{
	PrimaryActorTick.bCanEverTick = true;
	BlockMesh = CreateDefaultSubobject<USceneComponent>(TEXT("BlockMesh"));
	RootComponent = BlockMesh;
}

// 初期化処理
void ATetrisBlock::BeginPlay()
{
	Super::BeginPlay();

	// グリッドの参照を取得
	ATetrisGameMode* GameMode = Cast<ATetrisGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GameMode)
	{
		Grid = GameMode->Grid;

		if (Grid)
		{
			UE_LOG(LogTemp, Warning, TEXT("TetrisBlock: Successfully linked to Grid at (%f, %f, %f)"),
				Grid->GetActorLocation().X, Grid->GetActorLocation().Y, Grid->GetActorLocation().Z);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("TetrisBlock: Failed to get Grid instance!"));
		}
	}
}

// 毎フレームの処理（デバッグ用）
void ATetrisBlock::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

int32 ATetrisBlock::GetRotationIndex() const
{
	// 現在の回転をクォータニオンで取得
	FQuat RotationQuat = GetActorQuat();

	// クォータニオンを Yaw, Pitch, Roll の回転角に変換
	FRotator RotationEuler = RotationQuat.Rotator();

	UE_LOG(LogTemp, Log, TEXT("RotationEuler: %s"), *RotationEuler.ToString());

	// Pitch 成分（Y軸回転）の角度を取得
	float PitchDegrees = RotationEuler.Pitch;

	// 180度の時も0度になってしまうので
	if (175.0f <= RotationEuler.Yaw && RotationEuler.Yaw <= 180.0f &&
		175.0f <= RotationEuler.Roll && RotationEuler.Roll <= 180.0f)
		PitchDegrees = 180.0f;

	// 近い90度の倍数に丸めて、RotationIndex を決定
	int32 RotationIndex = (FMath::RoundToInt(PitchDegrees / 90.0f) % 4 + 4) % 4; // 0,1,2,3 のループ

	UE_LOG(LogTemp, Log, TEXT("RotationIndex: %d (Pitch: %f)"), RotationIndex, PitchDegrees);

	return RotationIndex;
}

/*
// 現在のブロックの形状を取得
TArray<FVector2D> ATetrisBlock::GetBlockCells() const
{
	TArray<FVector2D> BlockCells;
	FVector2D GridPos = Grid->WorldToGrid(GetActorLocation());
	int32 GridX = FMath::RoundToInt(GridPos.X);
	int32 GridY = FMath::RoundToInt(GridPos.Y);

	int32 RotationIndex = GetRotationIndex();

	switch (BlockType)
	{
	case EBlockType::I:
		if (RotationIndex == 0)
		{
			// 縦向き
			BlockCells.Add(FVector2D(GridX, GridY + 1));
			BlockCells.Add(FVector2D(GridX, GridY));
			BlockCells.Add(FVector2D(GridX, GridY - 1));
			BlockCells.Add(FVector2D(GridX, GridY - 2));
		}
		else if (RotationIndex == 1)
		{
			//横向き
			BlockCells.Add(FVector2D(GridX - 1, GridY));
			BlockCells.Add(FVector2D(GridX, GridY));
			BlockCells.Add(FVector2D(GridX + 1, GridY));
			BlockCells.Add(FVector2D(GridX + 2, GridY));

		}
		else if (RotationIndex == 2)
		{
			// 縦向き
			BlockCells.Add(FVector2D(GridX, GridY - 1));
			BlockCells.Add(FVector2D(GridX, GridY));
			BlockCells.Add(FVector2D(GridX, GridY + 1));
			BlockCells.Add(FVector2D(GridX, GridY + 2));
		}
		else
		{
			// 横向き
			BlockCells.Add(FVector2D(GridX + 1, GridY));
			BlockCells.Add(FVector2D(GridX, GridY));
			BlockCells.Add(FVector2D(GridX - 1, GridY));
			BlockCells.Add(FVector2D(GridX - 2, GridY));
		}
		break;

	case EBlockType::T:
		if (RotationIndex == 0)
		{
			BlockCells.Add(FVector2D(GridX - 1, GridY));
			BlockCells.Add(FVector2D(GridX, GridY));
			BlockCells.Add(FVector2D(GridX + 1, GridY));
			BlockCells.Add(FVector2D(GridX, GridY + 1));
		}
		else if (RotationIndex == 1)
		{
			BlockCells.Add(FVector2D(GridX, GridY - 1));
			BlockCells.Add(FVector2D(GridX, GridY));
			BlockCells.Add(FVector2D(GridX - 1, GridY));
			BlockCells.Add(FVector2D(GridX, GridY + 1));
		}
		else if (RotationIndex == 2)
		{
			BlockCells.Add(FVector2D(GridX - 1, GridY));
			BlockCells.Add(FVector2D(GridX, GridY));
			BlockCells.Add(FVector2D(GridX + 1, GridY));
			BlockCells.Add(FVector2D(GridX, GridY - 1));
		}
		else
		{
			BlockCells.Add(FVector2D(GridX, GridY - 1));
			BlockCells.Add(FVector2D(GridX, GridY));
			BlockCells.Add(FVector2D(GridX + 1, GridY));
			BlockCells.Add(FVector2D(GridX, GridY + 1));
		}
		break;

	case EBlockType::L:
		if (RotationIndex == 0)
		{
			BlockCells.Add(FVector2D(GridX, GridY + 1));
			BlockCells.Add(FVector2D(GridX, GridY));
			BlockCells.Add(FVector2D(GridX, GridY - 1));
			BlockCells.Add(FVector2D(GridX + 1, GridY - 1));
		}
		else if (RotationIndex == 1)
		{
			BlockCells.Add(FVector2D(GridX - 1, GridY));
			BlockCells.Add(FVector2D(GridX, GridY));
			BlockCells.Add(FVector2D(GridX + 1, GridY));
			BlockCells.Add(FVector2D(GridX + 1, GridY + 1));
		}
		else if (RotationIndex == 2)
		{
			BlockCells.Add(FVector2D(GridX, GridY + 1));
			BlockCells.Add(FVector2D(GridX, GridY));
			BlockCells.Add(FVector2D(GridX - 1, GridY + 1));
			BlockCells.Add(FVector2D(GridX, GridY - 1));
		}
		else
		{
			BlockCells.Add(FVector2D(GridX - 1, GridY));
			BlockCells.Add(FVector2D(GridX, GridY));
			BlockCells.Add(FVector2D(GridX + 1, GridY));
			BlockCells.Add(FVector2D(GridX - 1, GridY - 1));
		}
		break;

	case EBlockType::S:
		if (RotationIndex == 0)
		{
			// 横向き
			BlockCells.Add(FVector2D(GridX, GridY + 1));
			BlockCells.Add(FVector2D(GridX, GridY));
			BlockCells.Add(FVector2D(GridX - 1, GridY + 1));
			BlockCells.Add(FVector2D(GridX + 1, GridY));
		}
		else if (RotationIndex == 1)
		{
			// 縦向き
			BlockCells.Add(FVector2D(GridX, GridY));
			BlockCells.Add(FVector2D(GridX, GridY + 1));
			BlockCells.Add(FVector2D(GridX - 1, GridY));
			BlockCells.Add(FVector2D(GridX - 1, GridY - 1));
		}
		else if (RotationIndex == 2)
		{
			// 横向き
			BlockCells.Add(FVector2D(GridX, GridY - 1));
			BlockCells.Add(FVector2D(GridX, GridY));
			BlockCells.Add(FVector2D(GridX - 1, GridY));
			BlockCells.Add(FVector2D(GridX + 1, GridY - 1));
		}
		else
		{
			// 縦向き
			BlockCells.Add(FVector2D(GridX, GridY));
			BlockCells.Add(FVector2D(GridX + 1, GridY));
			BlockCells.Add(FVector2D(GridX + 1, GridY + 1));
			BlockCells.Add(FVector2D(GridX, GridY - 1));
		}
		break;

	case EBlockType::O:
		BlockCells.Add(FVector2D(GridX, GridY));
		BlockCells.Add(FVector2D(GridX + 1, GridY));
		BlockCells.Add(FVector2D(GridX, GridY + 1));
		BlockCells.Add(FVector2D(GridX + 1, GridY + 1));
		break;
	}

	return BlockCells;
}
*/

TArray<FVector2D> ATetrisBlock::GetBlockCells() const
{
	TArray<FVector2D> BlockCells;

	// ブロック内の全ての StaticMeshComponent を取得
	TArray<UStaticMeshComponent*> MeshComponents;
	GetComponents<UStaticMeshComponent>(MeshComponents);

	// 各 StaticMeshComponent のワールド座標をグリッド座標に変換し、対応付ける
	for (UStaticMeshComponent* MeshComp : MeshComponents)
	{
		if (!MeshComp) continue;

		// StaticMeshComponent のワールド座標を取得
		FVector WorldLocation = MeshComp->GetComponentLocation();

		// ワールド座標をグリッド座標に変換
		FVector2D GridPos = Grid->WorldToGrid(WorldLocation);

		BlockCells.Add(GridPos);
	}

	return BlockCells;
}

TMap<FVector2D, UStaticMeshComponent*> ATetrisBlock::GetCellMeshMap() const
{
	TMap<FVector2D, UStaticMeshComponent*> CellMeshMap;

	// ブロック内の全ての StaticMeshComponent を取得
	TArray<UStaticMeshComponent*> MeshComponents;
	GetComponents<UStaticMeshComponent>(MeshComponents);

	// 各 StaticMeshComponent のワールド座標をグリッド座標に変換し、対応付ける
	for (UStaticMeshComponent* MeshComp : MeshComponents)
	{
		if (!MeshComp) continue;

		// StaticMeshComponent のワールド座標を取得
		FVector WorldLocation = MeshComp->GetComponentLocation();

		// ワールド座標をグリッド座標に変換
		FVector2D GridPos = Grid->WorldToGrid(WorldLocation);

		CellMeshMap.Add(GridPos, MeshComp);
	}

	return CellMeshMap;
}

void ATetrisBlock::RemoveCell(FVector2D Cell)
{
	// GetBlockCells() を使用して現在のブロックのセル情報を取得
	TArray<FVector2D> CurrentCells = GetBlockCells();

	// 指定されたセルが現在のブロックに含まれているかチェック
	if (!CurrentCells.Contains(Cell))
	{
		UE_LOG(LogTemp, Warning, TEXT("RemoveCell: Cell [%d, %d] is not part of this block"), (int32)Cell.X, (int32)Cell.Y);
		return;
	}

	// CellMeshMap から該当セルの StaticMeshComponent を取得し削除
	TMap<FVector2D, UStaticMeshComponent*> CellMeshMap = GetCellMeshMap();
	if (UStaticMeshComponent** MeshComp = CellMeshMap.Find(Cell))
	{
		if (*MeshComp)
		{
			(*MeshComp)->DestroyComponent();
		}
	}

	// すべてのセルが削除された場合、ブロック自体を削除
	if (GetCellMeshMap().Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Destroy: Block %s has been completely removed"), *GetActorLabel());
		Destroy();
	}
}

void ATetrisBlock::UpdateCellPosition(int32 OffsetY)
{
	TMap<FVector2D, UStaticMeshComponent*> CellMeshMap = GetCellMeshMap();

	// すべてのセルを移動
	for (auto& Elem : CellMeshMap)
	{
		FVector2D OldGridPos = Elem.Key;
		FVector2D NewGridPos = FVector2D(OldGridPos.X, OldGridPos.Y - OffsetY); // Y軸方向に移動

		// StaticMeshComponent の取得
		UStaticMeshComponent* MeshComp = Elem.Value;
		if (MeshComp)
		{
			FVector NewWorldPos = Grid->GridToWorld(NewGridPos);
			MeshComp->SetWorldLocation(NewWorldPos);
		}
	}
}

FVector ATetrisBlock::CalculateNewBlockPosition(int32 OffsetY)
{
	FVector SumPosition = FVector::ZeroVector;
	TArray<FVector2D> Cells = GetBlockCells(); // ブロック内のセル情報を取得

	for (FVector2D Cell : Cells)
	{
		FVector WorldPos = Grid->GridToWorld(FVector2D(Cell.X, Cell.Y - OffsetY)); // 新しい位置
		SumPosition += WorldPos;
	}

	FVector NewPosition = SumPosition / Cells.Num(); // セルの重心を新しい位置に
	return NewPosition;
}


// 下移動
void ATetrisBlock::MoveDown()
{
	if (!Grid) return;

	// ブロックがすでにロックされている場合は何もしない
	if (bIsLocked) return;

	FVector2D GridPos = Grid->WorldToGrid(GetActorLocation());

	// `%d` → `%f` に修正
	UE_LOG(LogTemp, Warning, TEXT("MoveDown: Current GridPos [%.2f, %.2f] (WorldLocation: %.2f, %.2f, %.2f)"),
		GridPos.X, GridPos.Y, GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z);

	// ブロックのすべてのマスの現在の Grid 座標を取得
	TArray<FVector2D> BlockCells = GetBlockCells();

	// すべてのマスが 1マス下に移動可能かチェック
	for (const FVector2D& Cell : BlockCells)
	{
		int32 NextY = Cell.Y - 1;

		// 左右の範囲外のマス (`X < 0` or `X >= GridWidth`) は無視する
		if (Cell.X < 0 || Cell.X >= Grid->GridWidth)
		{
			continue;
		}

		if (!Grid->IsCellEmpty(Cell.X, NextY))
		{
			UE_LOG(LogTemp, Warning, TEXT("Checking MoveDown for Cell [%d, %d]"), (int32)Cell.X, (int32)Cell.Y);
			//  着地判定ログ追加
			UE_LOG(LogTemp, Warning, TEXT("Block Locked at [%d, %d]"), (int32)GridPos.X, (int32)GridPos.Y);

			// ブロックをロック
			bIsLocked = true;
			
			// ブロックをグリッドに登録
			Grid->RegisterBlock(GetBlockCells(), this);
			// ライン消去を実行
			Grid->ClearFullLines();

			//  新しいブロックをスポーン
			ATetrisGameMode* GameMode = Cast<ATetrisGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
			if (GameMode)
			{
				GameMode->SpawnNewBlock();
			}

			return;
		}
	}
	// すべてのマスが 1マス下に移動できるなら、移動を実行
	SetActorLocation(Grid->GridToWorld(FVector2D(GridPos.X, GridPos.Y - 1)));
}

// 左移動
void ATetrisBlock::MoveLeft()
{
	if (!Grid) return;

	TArray<FVector2D> BlockCells = GetBlockCells();

	// 左に移動できるかチェック
	for (const FVector2D& Cell : BlockCells)
	{
		int32 NextX = Cell.X - 1;

		// 左端 (`X < 0`) にぶつからないかチェック
		if (NextX < 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("MoveLeft: Block hit left wall at [%d, %d]"), (int32)Cell.X, (int32)Cell.Y);
			return;
		}

		if (!Grid->IsCellEmpty(NextX, Cell.Y))
		{
			UE_LOG(LogTemp, Warning, TEXT("MoveLeft: Block cannot move left due to another block at [%d, %d]"), NextX, Cell.Y);
			return;
		}
	}

	FVector2D GridPos = Grid->WorldToGrid(GetActorLocation());
	SetActorLocation(Grid->GridToWorld(FVector2D(GridPos.X - 1, GridPos.Y)));
}

// 右移動
void ATetrisBlock::MoveRight()
{
	if (!Grid) return;

	TArray<FVector2D> BlockCells = GetBlockCells();

	// 右に移動できるかチェック
	for (const FVector2D& Cell : BlockCells)
	{
		int32 NextX = Cell.X + 1;

		// 右端 (`X >= GridWidth`) にぶつからないかチェック
		if (NextX >= Grid->GridWidth)
		{
			UE_LOG(LogTemp, Warning, TEXT("MoveRight: Block hit right wall at [%d, %d]"), (int32)Cell.X, (int32)Cell.Y);
			return;
		}

		if (!Grid->IsCellEmpty(NextX, Cell.Y))
		{
			UE_LOG(LogTemp, Warning, TEXT("MoveRight: Block cannot move right due to another block at [%d, %d]"), NextX, Cell.Y);
			return;
		}
	}

	FVector2D GridPos = Grid->WorldToGrid(GetActorLocation());
	SetActorLocation(Grid->GridToWorld(FVector2D(GridPos.X + 1, GridPos.Y)));
}

void ATetrisBlock::Rotate()
{
	if (BlockType == EBlockType::O) return; // Oブロックは回転しない

	//1回目の90度回転がなぜかおかしいので4回空回転させておく
	for (int i = 0; i < 4; i++)
	{
		AddActorLocalRotation(FRotator(90, 0, 0)); // Y軸回転
	}

	// 回転の試行回数（通常の回転 + 壁キックの試行）
	TArray<FVector> RotationAttempts = {
		FVector(0, 0, 0),      // 元の位置
		FVector(100, 0, 0),    // 右に1マス
		FVector(-100, 0, 0),   // 左に1マス
		FVector(0, 0, 100),    // 上に1マス（下に空きがある場合）
		FVector(0, 0, -100)    // 下に1マス（上に空きがある場合）
	};

	for (const FVector& Offset : RotationAttempts)
	{
		// 一時的に回転を適用
		// 現在の回転を取得
		FRotator CurrentRotation = GetActorRotation();
		UE_LOG(LogTemp, Warning, TEXT("Current Rotation: P=%f Y=%f R=%f"), CurrentRotation.Pitch, CurrentRotation.Yaw, CurrentRotation.Roll);
		// Pitch のみを +90° 回転
		AddActorLocalRotation(FRotator(90.f, 0.f, 0.f));
		// 回転後の状態をログ出力
		FRotator NewRotation = GetActorRotation();
		UE_LOG(LogTemp, Warning, TEXT("New Rotation: P=%f Y=%f R=%f"), NewRotation.Pitch, NewRotation.Yaw, NewRotation.Roll);

		// オフセットを適用
		SetActorLocation(GetActorLocation() + Offset);

		// 回転後の位置を確認
		TArray<FVector2D> NewBlockCells = GetBlockCells();
		bool bCanRotate = true;
		for (const FVector2D& Cell : NewBlockCells)
		{
			if (!Grid->IsCellEmpty(Cell.X, Cell.Y))
			{
				bCanRotate = false;
				break;
			}
		}

		if (!bCanRotate)
		{
			// 失敗した場合、元の位置と回転に戻す
			SetActorLocation(GetActorLocation() - Offset);
			AddActorLocalRotation(FRotator(-90, 0, 0)); // Y軸回転
		}
	}
}

void ATetrisBlock::StartSoftDrop()
{
	if (!bIsSoftDropping)
	{
		bIsSoftDropping = true;

		// 0.1秒ごとに MoveDown() を実行（通常の落下より速くする）
		GetWorldTimerManager().SetTimer(SoftDropTimerHandle, this, &ATetrisBlock::MoveDown, 0.1f, true);
	}
}

void ATetrisBlock::StopSoftDrop()
{
	bIsSoftDropping = false;
	GetWorldTimerManager().ClearTimer(SoftDropTimerHandle);
}

void ATetrisBlock::HardDrop()
{
	while (!bIsLocked)  // ブロックがロックされるまで繰り返す
	{
		MoveDown();
	}
}
