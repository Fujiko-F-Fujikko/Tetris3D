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

	// **ゴーストブロックのマテリアルを事前ロード**
	GhostMaterial = TSoftObjectPtr<UMaterial>(FSoftObjectPath(TEXT("Material'/Game/Materials/M_GhostBlock.M_GhostBlock'")));

	// **ゴーストブロックのメッシュを事前ロード**
	GhostMesh = TSoftObjectPtr<UStaticMesh>(FSoftObjectPath(TEXT("Engine.StaticMesh'/Engine/BasicShapes/Cube.Cube'")));
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

	// ゴーストブロックの初期化
	InitializeGhostBlocks();
	UpdateGhostPosition();

}

// 毎フレームの処理（デバッグ用）
void ATetrisBlock::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool ATetrisBlock::IsGhostMesh(UStaticMeshComponent* MeshComp) const
{
	for (const auto& Pair : GhostBlockMeshes) // TMapをループで走査
	{
		if (Pair.Value == MeshComp)  // 値 (UStaticMeshComponent*) が一致するか確認
		{
			return true;  // GhostBlock に含まれていたら true を返す
		}
	}
	return false;  // 見つからなければ false
}

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

		// ゴーストブロックのメッシュなら無視
		if (IsGhostMesh(MeshComp)) continue;

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

		// ゴーストブロックのメッシュなら無視
		if (IsGhostMesh(MeshComp)) continue;

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

void ATetrisBlock::InitializeGhostBlocks()
{
	if (!Grid) return; // グリッドが無ければ処理しない

	// **ゴーストブロックのマテリアルをロード**
	if (!GhostMaterial.IsValid())
	{
		GhostMaterial.LoadSynchronous();
	}

	// **ゴーストブロックのメッシュをロード**
	if (!GhostMesh.IsValid())
	{
		GhostMesh.LoadSynchronous();
	}

	// 初期状態のブロックセルを取得
	TArray<FVector2D> InitialCells = GetBlockCells();

	// ゴーストブロックのメッシュも 4 つ作成（セルごとに対応）
	for (int i = 0; i < InitialCells.Num(); i++)
	{
		// 新しい `UStaticMeshComponent` を作成
		FString GhostMeshName = FString::Printf(TEXT("GhostBlockMesh%d"), i);
		UStaticMeshComponent* GhostComp = NewObject<UStaticMeshComponent>(this, *GhostMeshName);

		GhostComp->bEditableWhenInherited = true;
		GhostComp->SetMobility(EComponentMobility::Movable);
		// ゴーストブロックは衝突判定を無効化し、可視化
		GhostComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GhostComp->SetMaterial(0, GhostMaterial.Get());
		GhostComp->SetStaticMesh(GhostMesh.Get());
		GhostComp->SetVisibility(true);

		this->AddInstanceComponent(GhostComp);
		GhostComp->SetupAttachment(RootComponent);
		GhostComp->RegisterComponent(); // UEのコンポーネントシステムに登録

		// 各セルの座標とメッシュを対応付け
		GhostBlockMeshes.Add(InitialCells[i], GhostComp);
	}
}

void ATetrisBlock::UpdateGhostPosition()
{
	if (!Grid) return; // グリッドが無い場合は何もしない

	// 現在のブロックのセル情報を取得（通常ブロックの位置）
	TArray<FVector2D> BlockCells = GetBlockCells();
	TArray<FVector2D> GhostCells = BlockCells; // ゴーストブロックの基準

	// ゴーストブロックの最下位置を計算
	bool CanMove = true;
	while (CanMove)
	{
		for (FVector2D& Cell : GhostCells)
		{
			int32 NewY = Cell.Y - 1; // 下に1マス移動

			// 壁または他のブロックに衝突したら停止
			if (!Grid->IsCellEmpty(Cell.X, NewY))
			{
				CanMove = false;
				break;
			}
		}

		// まだ下に移動できるなら全てのセルを下に1マス移動
		if (CanMove)
		{
			for (FVector2D& Cell : GhostCells)
			{
				Cell.Y -= 1;
			}
		}
	}

	// ゴーストブロックの各メッシュを更新
	TMap<FVector2D, UStaticMeshComponent*> NewGhostBlockMeshes;
	int i = 0;
	for (const auto& Pair : GhostBlockMeshes)
	{
		UStaticMeshComponent* MeshComp = Pair.Value;
		if (!MeshComp) continue;

		if (i < GhostCells.Num()) // インデックス範囲をチェック
		{
			FVector2D NewGridPos = GhostCells[i]; // 新しいセルの位置
			FVector WorldPos = Grid->GridToWorld(NewGridPos);

			// ゴーストブロックのメッシュ位置を更新
			MeshComp->SetWorldLocation(WorldPos);

			// 新しいゴーストブロックマップに追加
			NewGhostBlockMeshes.Add(NewGridPos, MeshComp);
		}
		i++;
	}

	// ゴーストブロックのデータを更新
	GhostBlockMeshes = MoveTemp(NewGhostBlockMeshes);
}

void ATetrisBlock::ClearGhostBlocks()
{
	for (auto& Pair : GhostBlockMeshes)
	{
		if (Pair.Value) // nullptr でないことを確認
		{
			Pair.Value->DestroyComponent(); // StaticMeshComponent を削除
		}
	}

	GhostBlockMeshes.Empty(); // マップをクリア
}

void ATetrisBlock::UpdateCellPosition(const TArray<int32>& ClearedLines)
{
	TMap<FVector2D, UStaticMeshComponent*> CellMeshMap = GetCellMeshMap();

	for (auto& CellMeshPair : CellMeshMap)
	{
		FVector2D& CellPos = CellMeshPair.Key;
		UStaticMeshComponent* MeshComp = CellMeshPair.Value;

		// このセルが削除された行より上にあるか判定
		int32 OffsetY = 0;
		for (int32 ClearedY : ClearedLines)
		{
			if (CellPos.Y > ClearedY)  // 上にあるセルだけ移動
			{
				OffsetY++;
			}
		}

		if (OffsetY > 0)
		{
			// Y座標をOffsetY分下に移動
			CellPos.Y -= OffsetY;

			// 視覚的な位置を更新
			FVector NewWorldPos = Grid->GridToWorld(CellPos);
			MeshComp->SetWorldLocation(NewWorldPos);
		}
	}
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
			// ゴーストブロックを削除
			ClearGhostBlocks();

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

	// ゴーストブロックの位置も更新
	UpdateGhostPosition();
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

	// ゴーストブロックの位置も更新
	UpdateGhostPosition();
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

	// ゴーストブロックの位置も更新
	UpdateGhostPosition();
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
		AddActorLocalRotation(FRotator(-90.f, 0.f, 0.f));
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
			AddActorLocalRotation(FRotator(90, 0, 0)); // Y軸回転
		}
		else
		{
			// 成功した場合、ゴーストブロックの位置も更新
			UpdateGhostPosition();
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
