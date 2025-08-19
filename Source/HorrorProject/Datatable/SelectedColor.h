#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "SelectedColor.generated.h"

USTRUCT(BlueprintType)
struct FSelectedColor : public FTableRowBase
{
	GENERATED_BODY()

public:
	//1번부터 x번의 색이 등장할 때 x의 값
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SelectedColor")
	float R;
	// 모니터에서 색이 노출될 때 노출되는 시간의 값
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SelectedColor")
	float G;
	//풍선의 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SelectedColor")
	float B;
};

/*
 1. 빨간색 (Red): (R: 1, G: 0, B: 0)
   2. 녹색 (Green): (R: 0, G: 1, B: 0)
   3. 파란색 (Blue): (R: 0, G: 0, B: 1)
   4. 노란색 (Yellow): (R: 1, G: 1, B: 0)
   5. 청록색 (Cyan): (R: 0, G: 1, B: 1)
   6. 자홍색 (Magenta): (R: 1, G: 0, B: 1)
   7. 흰색 (White): (R: 1, G: 1, B: 1)
   8. 검은색 (Black): (R: 0, G: 0, B: 0)
   9. 주황색 (Orange): (R: 1, G: 0.5, B: 0)
   10. 보라색 (Purple): (R: 0.5, G: 0, B: 1)
*/