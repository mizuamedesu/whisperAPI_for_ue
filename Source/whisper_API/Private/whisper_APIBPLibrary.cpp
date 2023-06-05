#include "whisper_APIBPLibrary.h"
#include "Runtime/Online/HTTP/Public/Http.h"

void Uwhisper_APIBPLibrary::SendWhisperAPIRequestAsync(FString Token, FString FilePath, FWhisperAPIResponseDelegate ResponseDelegate)
{
    FHttpModule* Http = &FHttpModule::Get();

    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();
    Request->SetVerb("POST");
    Request->SetURL("https://api.openai.com/v1/audio/transcriptions");
    Request->SetHeader("Authorization", FString::Printf(TEXT("Bearer %s"), *Token));

    TArray<uint8> FileContent;
    if (!FFileHelper::LoadFileToArray(FileContent, *FilePath))
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load file: %s"), *FilePath);
        ResponseDelegate.ExecuteIfBound(false, FString());
        return;
    }

    FString Boundary = "----WebKitFormBoundary7MA4YWxkTrZu0gW";
    FString PreFileBoundary = FString::Printf(TEXT("--%s\r\nContent-Disposition: form-data; name=\"file\"; filename=\"openai.mp3\"\r\nContent-Type: audio/mpeg\r\n\r\n"), *Boundary);
    FString PostFileBoundary = FString::Printf(TEXT("\r\n--%s--\r\n"), *Boundary);
    FString ModelBoundary = FString::Printf(TEXT("\r\n--%s\r\nContent-Disposition: form-data; name=\"model\"\r\n\r\nwhisper-1\r\n--%s--\r\n"), *Boundary, *Boundary);
    Request->SetHeader("Content-Type", FString::Printf(TEXT("multipart/form-data; boundary=%s"), *Boundary));

    TArray<uint8> RequestContent;
    RequestContent.Append((uint8*)TCHAR_TO_UTF8(*PreFileBoundary), PreFileBoundary.Len());
    RequestContent.Append(FileContent);
    RequestContent.Append((uint8*)TCHAR_TO_UTF8(*ModelBoundary), ModelBoundary.Len());
    Request->SetContent(RequestContent);

    Request->OnProcessRequestComplete().BindLambda([ResponseDelegate](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccessful)
        {
            if (!bSuccessful || !Response.IsValid())
            {
                UE_LOG(LogTemp, Error, TEXT("Whisper API request failed"));
                ResponseDelegate.ExecuteIfBound(false, FString());
            }
            else
            {
                FString ResponseContent = Response->GetContentAsString();
                ResponseDelegate.ExecuteIfBound(true, ResponseContent);
            }
        });

    Request->ProcessRequest();
}