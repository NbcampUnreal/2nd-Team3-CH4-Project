#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerStatusWidget.generated.h"

class UTextBlock;
class UProgressBar;
class UImage;
class UTexture2D;

/** 플레이어 상태 정보 구조체 */
USTRUCT(BlueprintType)
struct FPlayerStatusInfo
{
    GENERATED_BODY()
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Status")
    FString PlayerName;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Status")
    float CurrentHealth;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Status")
    float MaxHealth;
    
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Status")
    UTexture2D* PlayerThumbnailTexture;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Status")
    int32 TeamID;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Status")
    int32 CharacterID;
    
    // 생성자로 기본값 설정
    FPlayerStatusInfo()
        : PlayerName(TEXT("Unknown")),
          CurrentHealth(100.0f),
          MaxHealth(100.0f),
          PlayerThumbnailTexture(nullptr),
          TeamID(0),
          CharacterID(-1)
    {
    }
};

UCLASS()
class WEAPONMASTER_API UPlayerStatusWidget : public UUserWidget
{
    GENERATED_BODY()
public:
    virtual void NativeConstruct() override;
    
    /** 플레이어 상태 정보를 한번에 업데이트 */
    UFUNCTION(BlueprintCallable, Category = "PlayerStatus")
    void UpdatePlayerStatus(const FPlayerStatusInfo& StatusInfo);
    
    /** 체력 업데이트 */
    UFUNCTION(BlueprintCallable, Category = "PlayerStatus")
    void UpdateHealth(float CurrentHealth, float MaxHealth);
    
    /** 플레이어 이름 설정 */
    UFUNCTION(BlueprintCallable, Category = "PlayerStatus")
    void SetPlayerName(const FString& Name);
    
    /** 썸네일 이미지 설정 */
    UFUNCTION(BlueprintCallable, Category = "PlayerStatus")
    void SetThumbnail(UTexture2D* Thumbnail);
    
    /** 플레이어 ID 설정 */
    UFUNCTION(BlueprintCallable, Category = "PlayerStatus")
    void SetID(int32 NewID);
    
    /** 캐릭터 ID 반환 */
    UFUNCTION(BlueprintCallable, Category = "PlayerStatus")
    int32 GetCharacterID() const { return CharacterID; }
    
    /** 팀 ID 설정 */
    UFUNCTION(BlueprintCallable, Category = "PlayerStatus")
    void SetTeamID(int32 NewTeamID);
    
    /** 팀 ID 반환 */
    UFUNCTION(BlueprintCallable, Category = "PlayerStatus")
    int32 GetTeamID() const { return TeamID; }
    
protected:
    // UI 구성요소
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> PlayerNameText;
    
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UProgressBar> HealthBar;
    
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UImage> PlayerThumbnail;
    
    // 플레이어 정보
    UPROPERTY(BlueprintReadOnly, Category = "PlayerStatus")
    int32 CharacterID;
    
    UPROPERTY(BlueprintReadOnly, Category = "PlayerStatus")
    int32 TeamID;
};