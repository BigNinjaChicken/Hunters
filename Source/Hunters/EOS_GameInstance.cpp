// Fill out your copyright notice in the Description page of Project Settings.

#include "EOS_GameInstance.h"
#include "OnlineSubsystemUtils.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineIdentityInterface.h"

void UEOS_GameInstance::LoginWithEOS(FString ID, FString Token, FString LoginType)
{
    IOnlineSubsystem *SubsystemReference = Online::GetSubsystem(this->GetWorld());
    if (SubsystemReference)
    {
        IOnlineIdentityPtr identityPointerRef = SubsystemReference->GetIdentityInterface();
        if (identityPointerRef)
        {
            FOnlineAccountCredentials AccountDetails;
            AccountDetails.Id = ID;
            AccountDetails.Token = Token;
            AccountDetails.Type = LoginType;
            identityPointerRef->OnLoginCompleteDelegates->AddUObject(this, &UEOS_GameInstance::LoginWithEOS_Return);
            identityPointerRef->Login(0, AccountDetails);
        }
    }
}

void UEOS_GameInstance::LoginWithEOS_Return(int32 LocalUserNum, bool bWasSuccess, const FUniqueNetId &UserId, const FString &Error)
{
    if (bWasSuccess)
    {
        UE_LOG(LogTemp, Warning, TEXT("Login Success"))
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Login Fail Reason - %s"), *Error)
    }
}
