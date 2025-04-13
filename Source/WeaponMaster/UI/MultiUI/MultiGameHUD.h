#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MultiGameHUD.generated.h"

class USessionWidget;
class UPlayerNameWidget;
class USessionLobbyWidget;
class UPlayerStatusWidget;
class UWrapStatusWidget;
class UChatWidget;
class UOptionMenuWidget;
class AEOSPlayerController;

UCLASS()
class WEAPONMASTER_API AMultiGameHUD : public AHUD
{
	GENERATED_BODY()
public:
    virtual void BeginPlay() override;

    UFUNCTION(BlueprintCallable)
    void TestDummyModule();

    UFUNCTION(BlueprintCallable)
    void TestChatModule(FString TestString, int32 TargetCharacterID);

    UFUNCTION(BlueprintCallable)
    void SetHPModule(float NewHP, int32 TargetCharacterID);
    
    UFUNCTION(BlueprintCallable)
    void SetMenuWidget(bool bIsOpen);

	UFUNCTION(BlueprintCallable)
	void UpdatePlayerStatus(int32 TargetCharacterID, const FPlayerStatusInfo& StatusInfo);

	UFUNCTION(BlueprintCallable)
	void RefreshAllPlayerStatus();
	
	UFUNCTION(BlueprintCallable)
	UPlayerStatusWidget* GetPlayerStatusWidget(int32 CharacterID);
	
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UWrapStatusWidget> WrapStatusWidgetClass;
	UPROPERTY()
    TObjectPtr<UWrapStatusWidget> WrapStatusWidget;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UChatWidget> ChatWidgetClass;
	UPROPERTY()
    TObjectPtr<UChatWidget> ChatWidget;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UOptionMenuWidget> OptionMenuWidgetClass;
	UPROPERTY()
    TObjectPtr<UOptionMenuWidget> OptionMenuWidget;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> MapSelectWidgetClass;
	UPROPERTY()
	TObjectPtr<USessionWidget> MapSelectWidget;

    void LogMessage(const FString& Message);

	UFUNCTION()
	void TransferHUDBy(const EMapType MapType);

private:
    UPROPERTY(EditAnywhere, Category = "UI")
    float BaseX = 50.f;

    UPROPERTY(EditAnywhere, Category = "UI")
    float Spacing = 320.f;

    UPROPERTY(EditAnywhere, Category = "UI")
    float DefaultY = -50.f;

    UPROPERTY(EditAnywhere, Category = "UI")
    int TestInt = 4;
};
