// Fill out your copyright notice in the Description page of Project Settings.


#include "DebugLog.h"

DebugLog::DebugLog()
{
}

DebugLog::~DebugLog()
{
}

void DebugLog::DebugNormal(UObject* Sender, FString Message)
{
	
	UE_LOG(LogTemp, Warning, TEXT("Sender(%s) -> %s"), *Sender->GetClass()->GetFName().ToString(), *Message);
}
