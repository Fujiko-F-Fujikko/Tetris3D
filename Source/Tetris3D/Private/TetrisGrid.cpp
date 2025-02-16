#include "TetrisGrid.h"
#include <Kismet/GameplayStatics.h>

ATetrisGrid::ATetrisGrid()
{
    PrimaryActorTick.bCanEverTick = false;

    // SceneComponent を作成して RootComponent に設定
    RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
    RootComponent = RootScene;
}

void ATetrisGrid::BeginPlay()
{
    Super::BeginPlay();
    InitializeGrid();
}

void ATetrisGrid::InitializeGrid()
{
    // 1次元配列として GridWidth * GridHeight のサイズを確保
    GridData.SetNum(GridWidth * GridHeight);

    for (int i = 0; i < GridWidth * GridHeight; i++)
    {
        GridData[i] = nullptr; // すべてのセルを初期化
    }
}

void ATetrisGrid::RegisterBlock(const TArray<FVector2D>& BlockCells, ATetrisBlock* BlockInstance)
{
    for (const FVector2D& Cell : BlockCells)
    {
        int32 CellX = static_cast<int32>(Cell.X);
        int32 CellY = static_cast<int32>(Cell.Y);
        int32 Index = CellY * GridWidth + CellX; // 2D座標を1D配列のインデックスに変換

        if (CellX >= 0 && CellX < GridWidth && CellY >= 0 && CellY < GridHeight)
        {
            GridData[Index] = BlockInstance; // 正しくブロックを登録
            UE_LOG(LogTemp, Warning, TEXT("Block Registered at Grid[%d][%d]"), CellX, CellY);
        }
    }
}

void ATetrisGrid::UnregisterBlock(const TArray<FVector2D>& BlockCells)
{
    for (const FVector2D& Cell : BlockCells)
    {
        int32 CellX = static_cast<int32>(Cell.X);
        int32 CellY = static_cast<int32>(Cell.Y);
		int32 Index = CellY * GridWidth + CellX; // 2D座標を1D配列のインデックスに変換

        if (CellX >= 0 && CellX < GridWidth && CellY >= 0 && CellY < GridHeight)
        {
            GridData[Index] = nullptr;
            UE_LOG(LogTemp, Warning, TEXT("Block Unregistered at Grid[%d][%d]"), CellX, CellY);
        }
    }
}

bool ATetrisGrid::IsCellEmpty(int32 X, int32 Y)
{
    if (X < 0 || X >= GridWidth || Y < 0 || Y >= GridHeight)
    {
        UE_LOG(LogTemp, Warning, TEXT("IsCellEmpty: [%d, %d] is out of bounds!"), X, Y);
        return false; // 範囲外なら「空いていない」と判定
    }

    int32 Index = Y * GridWidth + X; // 2D座標を1D配列のインデックスに変換

    bool bIsEmpty = GridData[Index] == nullptr;

    UE_LOG(LogTemp, Warning, TEXT("IsCellEmpty: [%d, %d] = %s"), X, Y, bIsEmpty ? TEXT("true") : TEXT("false"));

    return bIsEmpty;
}

void ATetrisGrid::ClearFullLines()
{
    TArray<int32> LinesToClear;

    // 1. 消すべきラインをリストアップ
    for (int32 Y = 0; Y < GridHeight; Y++)
    {
        bool bIsFullLine = true;
        for (int32 X = 0; X < GridWidth; X++)
        {
            if (!GridData[Y * GridWidth + X]) // 空のセルがある場合、ラインは揃っていない
            {
                bIsFullLine = false;
                break;
            }
        }
        if (bIsFullLine)
        {
            LinesToClear.Add(Y);
        }
    }

    if (LinesToClear.Num() == 0) return; // 消すラインがなければ処理不要

    // 2. 各行のセルを削除（ブロック全体を消さずに、対象セルだけを消す）
    for (int32 Y : LinesToClear)
    {
        for (int32 X = 0; X < GridWidth; X++)
        {
            ATetrisBlock* Block = GridData[Y * GridWidth + X];
            if (Block)
            {
                // 消すセルの座標をブロックに通知し、削除処理を委託
                Block->RemoveCell(FVector2D(X, Y));

                // グリッドデータのセル情報をクリア
                GridData[Y * GridWidth + X] = nullptr;
            }
        }
    }

    // 3. 上にあるブロックを下に移動
    TSet<ATetrisBlock*> ProcessedBlocks; // 既に処理したブロックを記録

    for (int32 Y = LinesToClear.Last() + 1; Y < GridHeight; Y++)
    {
        for (int32 X = 0; X < GridWidth; X++)
        {
            ATetrisBlock* Block = GridData[Y * GridWidth + X];
            if (Block)
            {
                int32 NewY = Y - LinesToClear.Num();
                if (NewY >= 0)
                {
                    // セルの移動
                    GridData[NewY * GridWidth + X] = Block;
                    GridData[Y * GridWidth + X] = nullptr;
                }

                if (!ProcessedBlocks.Contains(Block))  // まだ処理していないブロックなら
                {
                    int32 OffsetY = LinesToClear.Num();
                    ProcessedBlocks.Add(Block);  // 処理済みとして記録

                    // ブロック内のすべてのセルの位置を更新
                    Block->UpdateCellPosition(LinesToClear);
                }
            }
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("ClearFullLines: Successfully cleared %d lines."), LinesToClear.Num());
}


// ワールド座標をグリッド座標に変換
FVector2D ATetrisGrid::WorldToGrid(FVector WorldLocation)
{

    FVector GridOrigin = GetActorLocation();
    int32 GridX = FMath::RoundToInt((WorldLocation.X - GridOrigin.X) / 100.f);
    int32 GridY = FMath::RoundToInt((WorldLocation.Z - GridOrigin.Z) / 100.f);

    //UE_LOG(LogTemp, Warning, TEXT("WorldToGrid: WorldLocation (%f, %f, %f) -> GridPos (%d, %d)"),
    //    WorldLocation.X, WorldLocation.Y, WorldLocation.Z, GridX, GridY);

    return FVector2D(GridX, GridY);
}

// グリッド座標をワールド座標に変換
FVector ATetrisGrid::GridToWorld(FVector2D GridPosition)
{
    FVector GridOrigin = GetActorLocation();
    return FVector(GridOrigin.X + GridPosition.X * 100.f, 0.f, GridOrigin.Z + GridPosition.Y * 100.f);
}
