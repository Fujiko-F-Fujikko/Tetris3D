#include "TetrisBlock.h"
#include "TetrisGrid.h"
#include "TetrisGameMode.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"

// �R���X�g���N�^
ATetrisBlock::ATetrisBlock()
{
	PrimaryActorTick.bCanEverTick = true;
	BlockMesh = CreateDefaultSubobject<USceneComponent>(TEXT("BlockMesh"));
	RootComponent = BlockMesh;
}

// ����������
void ATetrisBlock::BeginPlay()
{
	Super::BeginPlay();

	// �O���b�h�̎Q�Ƃ��擾
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

// ���t���[���̏����i�f�o�b�O�p�j
void ATetrisBlock::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

int32 ATetrisBlock::GetRotationIndex() const
{
	// ���݂̉�]���N�H�[�^�j�I���Ŏ擾
	FQuat RotationQuat = GetActorQuat();

	// �N�H�[�^�j�I���� Yaw, Pitch, Roll �̉�]�p�ɕϊ�
	FRotator RotationEuler = RotationQuat.Rotator();

	UE_LOG(LogTemp, Log, TEXT("RotationEuler: %s"), *RotationEuler.ToString());

	// Pitch �����iY����]�j�̊p�x���擾
	float PitchDegrees = RotationEuler.Pitch;

	// 180�x�̎���0�x�ɂȂ��Ă��܂��̂�
	if (175.0f <= RotationEuler.Yaw && RotationEuler.Yaw <= 180.0f &&
		175.0f <= RotationEuler.Roll && RotationEuler.Roll <= 180.0f)
		PitchDegrees = 180.0f;

	// �߂�90�x�̔{���Ɋۂ߂āARotationIndex ������
	int32 RotationIndex = (FMath::RoundToInt(PitchDegrees / 90.0f) % 4 + 4) % 4; // 0,1,2,3 �̃��[�v

	UE_LOG(LogTemp, Log, TEXT("RotationIndex: %d (Pitch: %f)"), RotationIndex, PitchDegrees);

	return RotationIndex;
}

/*
// ���݂̃u���b�N�̌`����擾
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
			// �c����
			BlockCells.Add(FVector2D(GridX, GridY + 1));
			BlockCells.Add(FVector2D(GridX, GridY));
			BlockCells.Add(FVector2D(GridX, GridY - 1));
			BlockCells.Add(FVector2D(GridX, GridY - 2));
		}
		else if (RotationIndex == 1)
		{
			//������
			BlockCells.Add(FVector2D(GridX - 1, GridY));
			BlockCells.Add(FVector2D(GridX, GridY));
			BlockCells.Add(FVector2D(GridX + 1, GridY));
			BlockCells.Add(FVector2D(GridX + 2, GridY));

		}
		else if (RotationIndex == 2)
		{
			// �c����
			BlockCells.Add(FVector2D(GridX, GridY - 1));
			BlockCells.Add(FVector2D(GridX, GridY));
			BlockCells.Add(FVector2D(GridX, GridY + 1));
			BlockCells.Add(FVector2D(GridX, GridY + 2));
		}
		else
		{
			// ������
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
			// ������
			BlockCells.Add(FVector2D(GridX, GridY + 1));
			BlockCells.Add(FVector2D(GridX, GridY));
			BlockCells.Add(FVector2D(GridX - 1, GridY + 1));
			BlockCells.Add(FVector2D(GridX + 1, GridY));
		}
		else if (RotationIndex == 1)
		{
			// �c����
			BlockCells.Add(FVector2D(GridX, GridY));
			BlockCells.Add(FVector2D(GridX, GridY + 1));
			BlockCells.Add(FVector2D(GridX - 1, GridY));
			BlockCells.Add(FVector2D(GridX - 1, GridY - 1));
		}
		else if (RotationIndex == 2)
		{
			// ������
			BlockCells.Add(FVector2D(GridX, GridY - 1));
			BlockCells.Add(FVector2D(GridX, GridY));
			BlockCells.Add(FVector2D(GridX - 1, GridY));
			BlockCells.Add(FVector2D(GridX + 1, GridY - 1));
		}
		else
		{
			// �c����
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

	// �u���b�N���̑S�Ă� StaticMeshComponent ���擾
	TArray<UStaticMeshComponent*> MeshComponents;
	GetComponents<UStaticMeshComponent>(MeshComponents);

	// �e StaticMeshComponent �̃��[���h���W���O���b�h���W�ɕϊ����A�Ή��t����
	for (UStaticMeshComponent* MeshComp : MeshComponents)
	{
		if (!MeshComp) continue;

		// StaticMeshComponent �̃��[���h���W���擾
		FVector WorldLocation = MeshComp->GetComponentLocation();

		// ���[���h���W���O���b�h���W�ɕϊ�
		FVector2D GridPos = Grid->WorldToGrid(WorldLocation);

		BlockCells.Add(GridPos);
	}

	return BlockCells;
}

TMap<FVector2D, UStaticMeshComponent*> ATetrisBlock::GetCellMeshMap() const
{
	TMap<FVector2D, UStaticMeshComponent*> CellMeshMap;

	// �u���b�N���̑S�Ă� StaticMeshComponent ���擾
	TArray<UStaticMeshComponent*> MeshComponents;
	GetComponents<UStaticMeshComponent>(MeshComponents);

	// �e StaticMeshComponent �̃��[���h���W���O���b�h���W�ɕϊ����A�Ή��t����
	for (UStaticMeshComponent* MeshComp : MeshComponents)
	{
		if (!MeshComp) continue;

		// StaticMeshComponent �̃��[���h���W���擾
		FVector WorldLocation = MeshComp->GetComponentLocation();

		// ���[���h���W���O���b�h���W�ɕϊ�
		FVector2D GridPos = Grid->WorldToGrid(WorldLocation);

		CellMeshMap.Add(GridPos, MeshComp);
	}

	return CellMeshMap;
}

void ATetrisBlock::RemoveCell(FVector2D Cell)
{
	// GetBlockCells() ���g�p���Č��݂̃u���b�N�̃Z�������擾
	TArray<FVector2D> CurrentCells = GetBlockCells();

	// �w�肳�ꂽ�Z�������݂̃u���b�N�Ɋ܂܂�Ă��邩�`�F�b�N
	if (!CurrentCells.Contains(Cell))
	{
		UE_LOG(LogTemp, Warning, TEXT("RemoveCell: Cell [%d, %d] is not part of this block"), (int32)Cell.X, (int32)Cell.Y);
		return;
	}

	// CellMeshMap ����Y���Z���� StaticMeshComponent ���擾���폜
	TMap<FVector2D, UStaticMeshComponent*> CellMeshMap = GetCellMeshMap();
	if (UStaticMeshComponent** MeshComp = CellMeshMap.Find(Cell))
	{
		if (*MeshComp)
		{
			(*MeshComp)->DestroyComponent();
		}
	}

	// ���ׂẴZ�����폜���ꂽ�ꍇ�A�u���b�N���̂��폜
	if (GetCellMeshMap().Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Destroy: Block %s has been completely removed"), *GetActorLabel());
		Destroy();
	}
}

void ATetrisBlock::UpdateCellPosition(int32 OffsetY)
{
	TMap<FVector2D, UStaticMeshComponent*> CellMeshMap = GetCellMeshMap();

	// ���ׂẴZ�����ړ�
	for (auto& Elem : CellMeshMap)
	{
		FVector2D OldGridPos = Elem.Key;
		FVector2D NewGridPos = FVector2D(OldGridPos.X, OldGridPos.Y - OffsetY); // Y�������Ɉړ�

		// StaticMeshComponent �̎擾
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
	TArray<FVector2D> Cells = GetBlockCells(); // �u���b�N���̃Z�������擾

	for (FVector2D Cell : Cells)
	{
		FVector WorldPos = Grid->GridToWorld(FVector2D(Cell.X, Cell.Y - OffsetY)); // �V�����ʒu
		SumPosition += WorldPos;
	}

	FVector NewPosition = SumPosition / Cells.Num(); // �Z���̏d�S��V�����ʒu��
	return NewPosition;
}


// ���ړ�
void ATetrisBlock::MoveDown()
{
	if (!Grid) return;

	// �u���b�N�����łɃ��b�N����Ă���ꍇ�͉������Ȃ�
	if (bIsLocked) return;

	FVector2D GridPos = Grid->WorldToGrid(GetActorLocation());

	// `%d` �� `%f` �ɏC��
	UE_LOG(LogTemp, Warning, TEXT("MoveDown: Current GridPos [%.2f, %.2f] (WorldLocation: %.2f, %.2f, %.2f)"),
		GridPos.X, GridPos.Y, GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z);

	// �u���b�N�̂��ׂẴ}�X�̌��݂� Grid ���W���擾
	TArray<FVector2D> BlockCells = GetBlockCells();

	// ���ׂẴ}�X�� 1�}�X���Ɉړ��\���`�F�b�N
	for (const FVector2D& Cell : BlockCells)
	{
		int32 NextY = Cell.Y - 1;

		// ���E�͈̔͊O�̃}�X (`X < 0` or `X >= GridWidth`) �͖�������
		if (Cell.X < 0 || Cell.X >= Grid->GridWidth)
		{
			continue;
		}

		if (!Grid->IsCellEmpty(Cell.X, NextY))
		{
			UE_LOG(LogTemp, Warning, TEXT("Checking MoveDown for Cell [%d, %d]"), (int32)Cell.X, (int32)Cell.Y);
			//  ���n���胍�O�ǉ�
			UE_LOG(LogTemp, Warning, TEXT("Block Locked at [%d, %d]"), (int32)GridPos.X, (int32)GridPos.Y);

			// �u���b�N�����b�N
			bIsLocked = true;
			
			// �u���b�N���O���b�h�ɓo�^
			Grid->RegisterBlock(GetBlockCells(), this);
			// ���C�����������s
			Grid->ClearFullLines();

			//  �V�����u���b�N���X�|�[��
			ATetrisGameMode* GameMode = Cast<ATetrisGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
			if (GameMode)
			{
				GameMode->SpawnNewBlock();
			}

			return;
		}
	}
	// ���ׂẴ}�X�� 1�}�X���Ɉړ��ł���Ȃ�A�ړ������s
	SetActorLocation(Grid->GridToWorld(FVector2D(GridPos.X, GridPos.Y - 1)));
}

// ���ړ�
void ATetrisBlock::MoveLeft()
{
	if (!Grid) return;

	TArray<FVector2D> BlockCells = GetBlockCells();

	// ���Ɉړ��ł��邩�`�F�b�N
	for (const FVector2D& Cell : BlockCells)
	{
		int32 NextX = Cell.X - 1;

		// ���[ (`X < 0`) �ɂԂ���Ȃ����`�F�b�N
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

// �E�ړ�
void ATetrisBlock::MoveRight()
{
	if (!Grid) return;

	TArray<FVector2D> BlockCells = GetBlockCells();

	// �E�Ɉړ��ł��邩�`�F�b�N
	for (const FVector2D& Cell : BlockCells)
	{
		int32 NextX = Cell.X + 1;

		// �E�[ (`X >= GridWidth`) �ɂԂ���Ȃ����`�F�b�N
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
	if (BlockType == EBlockType::O) return; // O�u���b�N�͉�]���Ȃ�

	//1��ڂ�90�x��]���Ȃ������������̂�4����]�����Ă���
	for (int i = 0; i < 4; i++)
	{
		AddActorLocalRotation(FRotator(90, 0, 0)); // Y����]
	}

	// ��]�̎��s�񐔁i�ʏ�̉�] + �ǃL�b�N�̎��s�j
	TArray<FVector> RotationAttempts = {
		FVector(0, 0, 0),      // ���̈ʒu
		FVector(100, 0, 0),    // �E��1�}�X
		FVector(-100, 0, 0),   // ����1�}�X
		FVector(0, 0, 100),    // ���1�}�X�i���ɋ󂫂�����ꍇ�j
		FVector(0, 0, -100)    // ����1�}�X�i��ɋ󂫂�����ꍇ�j
	};

	for (const FVector& Offset : RotationAttempts)
	{
		// �ꎞ�I�ɉ�]��K�p
		// ���݂̉�]���擾
		FRotator CurrentRotation = GetActorRotation();
		UE_LOG(LogTemp, Warning, TEXT("Current Rotation: P=%f Y=%f R=%f"), CurrentRotation.Pitch, CurrentRotation.Yaw, CurrentRotation.Roll);
		// Pitch �݂̂� +90�� ��]
		AddActorLocalRotation(FRotator(90.f, 0.f, 0.f));
		// ��]��̏�Ԃ����O�o��
		FRotator NewRotation = GetActorRotation();
		UE_LOG(LogTemp, Warning, TEXT("New Rotation: P=%f Y=%f R=%f"), NewRotation.Pitch, NewRotation.Yaw, NewRotation.Roll);

		// �I�t�Z�b�g��K�p
		SetActorLocation(GetActorLocation() + Offset);

		// ��]��̈ʒu���m�F
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
			// ���s�����ꍇ�A���̈ʒu�Ɖ�]�ɖ߂�
			SetActorLocation(GetActorLocation() - Offset);
			AddActorLocalRotation(FRotator(-90, 0, 0)); // Y����]
		}
	}
}

void ATetrisBlock::StartSoftDrop()
{
	if (!bIsSoftDropping)
	{
		bIsSoftDropping = true;

		// 0.1�b���Ƃ� MoveDown() �����s�i�ʏ�̗�����葬������j
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
	while (!bIsLocked)  // �u���b�N�����b�N�����܂ŌJ��Ԃ�
	{
		MoveDown();
	}
}
