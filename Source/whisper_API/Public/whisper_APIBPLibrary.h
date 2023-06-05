#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "whisper_APIBPLibrary.generated.h"

DECLARE_DYNAMIC_DELEGATE_TwoParams(FWhisperAPIResponseDelegate, bool, bWasSuccessful, FString, ResponseContent);

UCLASS()
class WHISPER_API_API Uwhisper_APIBPLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Whisper API")
        static void SendWhisperAPIRequestAsync(FString Token, FString FilePath, FWhisperAPIResponseDelegate ResponseDelegate);
};