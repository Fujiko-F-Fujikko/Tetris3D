#include "TetrisGrid.h"
#include <Kismet/GameplayStatics.h>

ATetrisGrid::ATetrisGrid()
{
    PrimaryActorTick.bCanEverTick = false;

    // SceneComponent ���쐬���� RootComponent �ɐݒ�
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
    // 1�����z��Ƃ��� GridWidth * GridHeight �̃T�C�Y���m��
    GridData.SetNum(GridWidth * GridHeight);

    for (int i = 0; i < GridWidth * GridHeight; i++)
    {
        GridData[i] = nullptr; // ���ׂẴZ����������
    }
}

void ATetrisGrid::RegisterBlock(const TArray<FVector2D>& BlockCells, ATetrisBlock* BlockInstance)
{
    for (const FVector2D& Cell : BlockCells)
    {
        int32 CellX = static_cast<int32>(Cell.X);
        int32 CellY = static_cast<int32>(Cell.Y);
        int32 Index = CellY * GridWidth + CellX; // 2D���W��1D�z��̃C���f�b�N�X�ɕϊ�

        if (CellX >= 0 && CellX < GridWidth && CellY >= 0 && CellY < GridHeight)
        {
            GridData[Index] = BlockInstance; // �������u���b�N��o�^
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
		int32 Index = CellY * GridWidth + CellX; // 2D���W��1D�z��̃C���f�b�N�X�ɕϊ�

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
        return false; // �͈͊O�Ȃ�u�󂢂Ă��Ȃ��v�Ɣ���
    }

    int32 Index = Y * GridWidth + X; // 2D���W��1D�z��̃C���f�b�N�X�ɕϊ�

    bool bIsEmpty = GridData[Index] == nullptr;

    UE_LOG(LogTemp, Warning, TEXT("IsCellEmpty: [%d, %d] = %s"), X, Y, bIsEmpty ? TEXT("true") : TEXT("false"));

    return bIsEmpty;
}

void ATetrisGrid::ClearFullLines()
{
    TArray<int32> LinesToClear;

    // 1. �����ׂ����C�������X�g�A�b�v
    for (int32 Y = 0; Y < GridHeight; Y++)
    {
        bool bIsFullLine = true;
        for (int32 X = 0; X < GridWidth; X++)
        {
            if (!GridData[Y * GridWidth + X]) // ��̃Z��������ꍇ�A���C���͑����Ă��Ȃ�
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

    if (LinesToClear.Num() == 0) return; // �������C�����Ȃ���Ώ����s�v

    // 2. �e�s�̃Z�����폜�i�u���b�N�S�̂��������ɁA�ΏۃZ�������������j
    for (int32 Y : LinesToClear)
    {
        for (int32 X = 0; X < GridWidth; X++)
        {
            ATetrisBlock* Block = GridData[Y * GridWidth + X];
            if (Block)
            {
                // �����Z���̍��W���u���b�N�ɒʒm���A�폜�������ϑ�
                Block->RemoveCell(FVector2D(X, Y));

                // �O���b�h�f�[�^�̃Z�������N���A
                GridData[Y * GridWidth + X] = nullptr;
            }
        }
    }

    // 3. ��ɂ���u���b�N�����Ɉړ�
    TSet<ATetrisBlock*> ProcessedBlocks; // ���ɏ��������u���b�N���L�^

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
                    // �Z���̈ړ�
                    GridData[NewY * GridWidth + X] = Block;
                    GridData[Y * GridWidth + X] = nullptr;
                }

                if (!ProcessedBlocks.Contains(Block))  // �܂��������Ă��Ȃ��u���b�N�Ȃ�
                {
                    int32 OffsetY = LinesToClear.Num();
                    ProcessedBlocks.Add(Block);  // �����ς݂Ƃ��ċL�^

                    // �u���b�N���̂��ׂẴZ���̈ʒu���X�V
                    Block->UpdateCellPosition(LinesToClear);
                }
            }
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("ClearFullLines: Successfully cleared %d lines."), LinesToClear.Num());
}


// ���[���h���W���O���b�h���W�ɕϊ�
FVector2D ATetrisGrid::WorldToGrid(FVector WorldLocation)
{

    FVector GridOrigin = GetActorLocation();
    int32 GridX = FMath::RoundToInt((WorldLocation.X - GridOrigin.X) / 100.f);
    int32 GridY = FMath::RoundToInt((WorldLocation.Z - GridOrigin.Z) / 100.f);

    //UE_LOG(LogTemp, Warning, TEXT("WorldToGrid: WorldLocation (%f, %f, %f) -> GridPos (%d, %d)"),
    //    WorldLocation.X, WorldLocation.Y, WorldLocation.Z, GridX, GridY);

    return FVector2D(GridX, GridY);
}

// �O���b�h���W�����[���h���W�ɕϊ�
FVector ATetrisGrid::GridToWorld(FVector2D GridPosition)
{
    FVector GridOrigin = GetActorLocation();
    return FVector(GridOrigin.X + GridPosition.X * 100.f, 0.f, GridOrigin.Z + GridPosition.Y * 100.f);
}
