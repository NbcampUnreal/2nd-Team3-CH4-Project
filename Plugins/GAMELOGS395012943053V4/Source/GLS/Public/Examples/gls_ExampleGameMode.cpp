#include "gls_ExampleGameMode.h"

#include <GLSMacroses.h>
#include <GLSSubsystem.h>
#include <GLSOverlaySubsystem.h>

DEFINE_LOG_CATEGORY_STATIC(GLS_LOG_CATEGORY, Log, All);

DEFINE_LOG_CATEGORY_STATIC(GLS_LOG_MESSAGE_CATEGORY, Log, All);

DEFINE_LOG_CATEGORY_STATIC(GLS_LOG_WITH_TAGS_CATEGORY, Log, All);

DEFINE_LOG_CATEGORY_STATIC(GLS_LOG_MESSAGE_WITH_TAGS_CATEGORY, Log, All);

class FNonUObjectClass
{
public:
	void PrintToGLS()
	{
		// printing to GLS without UObject context
		GLS_LOG(GLS_LOG_CATEGORY, Log, TEXT("GLS test log '%d' using GLS_LOG - FNonUObjectClass::PrintToGLS() macros"), 12345);
	}

	void PrintToGLSConst() const
	{
		// printing to GLS without UObject context from const function
		GLS_LOG(GLS_LOG_CATEGORY, Log, TEXT("GLS test log '%d' using GLS_LOG - FNonUObjectClass::PrintToGLS() const macros"), 12345);
	}

	static void PrintToGLSStatic()
	{
		// There is no way to use GLS macro from inside a static function, please use the standard one UE_LOG
		//GLS_LOG(GLS_LOG_CATEGORY, Log, TEXT("GLS test log '%d' using GLS_LOG - FNonUObjectClass::PrintToGLSStatic() const macros"), 12345);
	}
};

Agls_ExampleGameMode::Agls_ExampleGameMode()
	: Super()
{
	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.bCanEverTick = true;
}

void Agls_ExampleGameMode::BeginPlay()
{
	Super::BeginPlay();

	PrintToGLS();

	// TIMER for correct getting PIE instances to add to log tags
	FTimerHandle Handle;
	GetWorld()->GetTimerManager().SetTimer(
		Handle, FTimerDelegate::CreateWeakLambda(this, [this]()
		{
			FNonUObjectClass TestObject{};
			TestObject.PrintToGLS();
			TestObject.PrintToGLSConst();
			TestObject.PrintToGLSStatic();

			PrintToGLS();
			PrintWithoutContextConst();
		}), 1.f, true, 1.5); // FOR DEBUGING

	FTimerHandle Handle2;
	GetWorld()->GetTimerManager().SetTimer(Handle2, [this]()
	{
		if (auto Subsystem = UGLSOverlaySubsystem::Get(this))
		{
			//Subsystem->DisplayOverlay();
		}
	}, 1.5f, false);
}

void Agls_ExampleGameMode::PrintWithoutContextConst() const
{
	GLS_LOG(GLS_LOG_CATEGORY, Warning, TEXT("GLS test log '%d' using GLS_LOG - PrintWithoutContext() macros"), 12345);
}

void Agls_ExampleGameMode::PrintToGLS()
{
	const int32 LogCreateNum = 10;

	// EXAMPLES FOR TESTING WORK WITH GLS
	// printing to gls overlay and to console output

	// for C++
	{
		for (int32 i = 0; i < LogCreateNum; i++)
		{
			UE_LOG(GLS_LOG_CATEGORY, Display, TEXT("PrintToGLS()")); // testing duplicates by standard UE LOG
		}

		for (int32 i = 0; i < LogCreateNum; i++)
		{
			GLS_LOG(GLS_LOG_CATEGORY, Warning, TEXT("GLS test log '%d' using GLS_LOG macros"), i);
			GLS_LOG_CONTEXT(this, GLS_LOG_CATEGORY, Warning, TEXT("GLS test log '%d' using GLS_LOG_CONTEXT macros"), i);
		}

		for (int32 i = 0; i < LogCreateNum; i++)
		{
			GLS_LOG_STRING(GLS_LOG_MESSAGE_CATEGORY, Warning, FString::Printf(TEXT("GLS test log '%d' using GLS_LOG_STRING macros"), i));
			GLS_LOG_STRING_CONTEXT(this, GLS_LOG_MESSAGE_CATEGORY, Warning, FString::Printf(TEXT("GLS test log '%d' using GLS_LOG_STRING_CONTEXT macros"), i));

			GLS_LOG_STRING(GLS_LOG_MESSAGE_CATEGORY, Warning, TEXT("GLS test log using GLS_LOG_STRING macros"));
			GLS_LOG_STRING_CONTEXT(this, GLS_LOG_MESSAGE_CATEGORY, Warning, TEXT("GLS test log using GLS_LOG_STRING_CONTEXT macros"));
		}

		for (int32 i = 0; i < LogCreateNum; i++)
		{
			GLS_LOG_WITH_TAGS(this, GLS_LOG_WITH_TAGS_CATEGORY, Warning, TArray<FString>({TEXT("gls-tag-1"), TEXT("gls-tag-2")}), TEXT("GLS test log '%d' using GLS_LOG_WITH_TAGS macros"), i);
		}

		for (int32 i = 0; i < LogCreateNum; i++)
		{
			GLS_LOG_STRING_WITH_TAGS(this, GLS_LOG_MESSAGE_WITH_TAGS_CATEGORY, Warning, FString::Printf(TEXT("GLS test log '%d' using GLS_LOG_MESSAGE_WITH_TAGS macros"), i), TArray<FString>({TEXT("gls-tag-1"), TEXT("gls-tag-2")}));
			GLS_LOG_STRING_WITH_TAGS(this, GLS_LOG_MESSAGE_WITH_TAGS_CATEGORY, Warning, TEXT("GLS test log using GLS_LOG_MESSAGE_WITH_TAGS macros"), TArray<FString>({TEXT("gls-tag-1"), TEXT("gls-tag-2")}));
		}
	}

	// for BP
	{
		for (int32 i = 0; i < LogCreateNum; i++)
		{
			UGLSSubsystem::PrintStringToGLS(this, FString::Printf(TEXT("GLS test log '%d' using UGLSSubsystem::PrintStringToGLS function"), i), TEXT("GLS_CUSTOM_BLUEPRINT_CATEGORY"), EGLSLogType::Display, false);
		}

		for (int32 i = 0; i < LogCreateNum; i++)
		{
			UGLSSubsystem::PrintStringToGLSWithTags(this, {TEXT("gls-bp-tag-1"), TEXT("gls-bp-tag-2")}, FString::Printf(TEXT("GLS test log '%d' using UGLSSubsystem::PrintStringToGLS function"), i), TEXT("GLS_CUSTOM_BLUEPRINT_CATEGORY"),
			                                        EGLSLogType::Log, false);
		}
	}
}

void Agls_ExampleGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// Crazy printing for test perfomance

	//PrintToGLS();
	//PrintWithoutContextConst();
}
