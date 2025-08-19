#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "SelectedColor.generated.h"

USTRUCT(BlueprintType)
struct FSelectedColor : public FTableRowBase
{
	GENERATED_BODY()

public:
	//1������ x���� ���� ������ �� x�� ��
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SelectedColor")
	float R;
	// ����Ϳ��� ���� ����� �� ����Ǵ� �ð��� ��
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SelectedColor")
	float G;
	//ǳ���� �ӵ�
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SelectedColor")
	float B;
};

/*
 1. ������ (Red): (R: 1, G: 0, B: 0)
   2. ��� (Green): (R: 0, G: 1, B: 0)
   3. �Ķ��� (Blue): (R: 0, G: 0, B: 1)
   4. ����� (Yellow): (R: 1, G: 1, B: 0)
   5. û�ϻ� (Cyan): (R: 0, G: 1, B: 1)
   6. ��ȫ�� (Magenta): (R: 1, G: 0, B: 1)
   7. ��� (White): (R: 1, G: 1, B: 1)
   8. ������ (Black): (R: 0, G: 0, B: 0)
   9. ��Ȳ�� (Orange): (R: 1, G: 0.5, B: 0)
   10. ����� (Purple): (R: 0.5, G: 0, B: 1)
*/