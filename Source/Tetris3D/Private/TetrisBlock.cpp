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

	// **�S�[�X�g�u���b�N�̃}�e���A�������O���[�h**
	GhostMaterial = TSoftObjectPtr<UMaterial>(FSoftObjectPath(TEXT("Material'/Game/Materials/M_GhostBlock.M_GhostBlock'")));

	// **�S�[�X�g�u���b�N�̃��b�V�������O���[�h**
	GhostMesh = TSoftObjectPtr<UStaticMesh>(FSoftObjectPath(TEXT("Engine.StaticMesh'/Engine/BasicShapes/Cube.Cube'")));
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

	// �S�[�X�g�u���b�N�̏�����
	InitializeGhostBlocks();
	UpdateGhostPosition();

}

// ���t���[���̏����i�f�o�b�O�p�j
void ATetrisBlock::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool ATetrisBlock::IsGhostMesh(UStaticMeshComponent* MeshComp) const
{
	for (const auto& Pair : GhostBlockMeshes) // TMap�����[�v�ő���
	{
		if (Pair.Value == MeshComp)  // �l (UStaticMeshComponent*) ����v���邩�m�F
		{
			return true;  // GhostBlock �Ɋ܂܂�Ă����� true ��Ԃ�
		}
	}
	return false;  // ������Ȃ���� false
}

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

		// �S�[�X�g�u���b�N�̃��b�V���Ȃ疳��
		if (IsGhostMesh(MeshComp)) continue;

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

		// �S�[�X�g�u���b�N�̃��b�V���Ȃ疳��
		if (IsGhostMesh(MeshComp)) continue;

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

void ATetrisBlock::InitializeGhostBlocks()
{
	if (!Grid) return; // �O���b�h��������Ώ������Ȃ�

	// **�S�[�X�g�u���b�N�̃}�e���A�������[�h**
	if (!GhostMaterial.IsValid())
	{
		GhostMaterial.LoadSynchronous();
	}

	// **�S�[�X�g�u���b�N�̃��b�V�������[�h**
	if (!GhostMesh.IsValid())
	{
		GhostMesh.LoadSynchronous();
	}

	// ������Ԃ̃u���b�N�Z�����擾
	TArray<FVector2D> InitialCells = GetBlockCells();

	// �S�[�X�g�u���b�N�̃��b�V���� 4 �쐬�i�Z�����ƂɑΉ��j
	for (int i = 0; i < InitialCells.Num(); i++)
	{
		// �V���� `UStaticMeshComponent` ���쐬
		FString GhostMeshName = FString::Printf(TEXT("GhostBlockMesh%d"), i);
		UStaticMeshComponent* GhostComp = NewObject<UStaticMeshComponent>(this, *GhostMeshName);

		GhostComp->bEditableWhenInherited = true;
		GhostComp->SetMobility(EComponentMobility::Movable);
		// �S�[�X�g�u���b�N�͏Փ˔���𖳌������A����
		GhostComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GhostComp->SetMaterial(0, GhostMaterial.Get());
		GhostComp->SetStaticMesh(GhostMesh.Get());
		GhostComp->SetVisibility(true);

		this->AddInstanceComponent(GhostComp);
		GhostComp->SetupAttachment(RootComponent);
		GhostComp->RegisterComponent(); // UE�̃R���|�[�l���g�V�X�e���ɓo�^

		// �e�Z���̍��W�ƃ��b�V����Ή��t��
		GhostBlockMeshes.Add(InitialCells[i], GhostComp);
	}
}

void ATetrisBlock::UpdateGhostPosition()
{
	if (!Grid) return; // �O���b�h�������ꍇ�͉������Ȃ�

	// ���݂̃u���b�N�̃Z�������擾�i�ʏ�u���b�N�̈ʒu�j
	TArray<FVector2D> BlockCells = GetBlockCells();
	TArray<FVector2D> GhostCells = BlockCells; // �S�[�X�g�u���b�N�̊

	// �S�[�X�g�u���b�N�̍ŉ��ʒu���v�Z
	bool CanMove = true;
	while (CanMove)
	{
		for (FVector2D& Cell : GhostCells)
		{
			int32 NewY = Cell.Y - 1; // ����1�}�X�ړ�

			// �ǂ܂��͑��̃u���b�N�ɏՓ˂������~
			if (!Grid->IsCellEmpty(Cell.X, NewY))
			{
				CanMove = false;
				break;
			}
		}

		// �܂����Ɉړ��ł���Ȃ�S�ẴZ��������1�}�X�ړ�
		if (CanMove)
		{
			for (FVector2D& Cell : GhostCells)
			{
				Cell.Y -= 1;
			}
		}
	}

	// �S�[�X�g�u���b�N�̊e���b�V�����X�V
	TMap<FVector2D, UStaticMeshComponent*> NewGhostBlockMeshes;
	int i = 0;
	for (const auto& Pair : GhostBlockMeshes)
	{
		UStaticMeshComponent* MeshComp = Pair.Value;
		if (!MeshComp) continue;

		if (i < GhostCells.Num()) // �C���f�b�N�X�͈͂��`�F�b�N
		{
			FVector2D NewGridPos = GhostCells[i]; // �V�����Z���̈ʒu
			FVector WorldPos = Grid->GridToWorld(NewGridPos);

			// �S�[�X�g�u���b�N�̃��b�V���ʒu���X�V
			MeshComp->SetWorldLocation(WorldPos);

			// �V�����S�[�X�g�u���b�N�}�b�v�ɒǉ�
			NewGhostBlockMeshes.Add(NewGridPos, MeshComp);
		}
		i++;
	}

	// �S�[�X�g�u���b�N�̃f�[�^���X�V
	GhostBlockMeshes = MoveTemp(NewGhostBlockMeshes);
}

void ATetrisBlock::ClearGhostBlocks()
{
	for (auto& Pair : GhostBlockMeshes)
	{
		if (Pair.Value) // nullptr �łȂ����Ƃ��m�F
		{
			Pair.Value->DestroyComponent(); // StaticMeshComponent ���폜
		}
	}

	GhostBlockMeshes.Empty(); // �}�b�v���N���A
}

void ATetrisBlock::UpdateCellPosition(const TArray<int32>& ClearedLines)
{
	TMap<FVector2D, UStaticMeshComponent*> CellMeshMap = GetCellMeshMap();

	for (auto& CellMeshPair : CellMeshMap)
	{
		FVector2D& CellPos = CellMeshPair.Key;
		UStaticMeshComponent* MeshComp = CellMeshPair.Value;

		// ���̃Z�����폜���ꂽ�s����ɂ��邩����
		int32 OffsetY = 0;
		for (int32 ClearedY : ClearedLines)
		{
			if (CellPos.Y > ClearedY)  // ��ɂ���Z�������ړ�
			{
				OffsetY++;
			}
		}

		if (OffsetY > 0)
		{
			// Y���W��OffsetY�����Ɉړ�
			CellPos.Y -= OffsetY;

			// ���o�I�Ȉʒu���X�V
			FVector NewWorldPos = Grid->GridToWorld(CellPos);
			MeshComp->SetWorldLocation(NewWorldPos);
		}
	}
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
			// �S�[�X�g�u���b�N���폜
			ClearGhostBlocks();

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

	// �S�[�X�g�u���b�N�̈ʒu���X�V
	UpdateGhostPosition();
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

	// �S�[�X�g�u���b�N�̈ʒu���X�V
	UpdateGhostPosition();
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

	// �S�[�X�g�u���b�N�̈ʒu���X�V
	UpdateGhostPosition();
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
		AddActorLocalRotation(FRotator(-90.f, 0.f, 0.f));
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
			AddActorLocalRotation(FRotator(90, 0, 0)); // Y����]
		}
		else
		{
			// ���������ꍇ�A�S�[�X�g�u���b�N�̈ʒu���X�V
			UpdateGhostPosition();
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
