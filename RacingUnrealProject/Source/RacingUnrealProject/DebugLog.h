// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class RACINGUNREALPROJECT_API DebugLog
{
public:
	DebugLog();
	~DebugLog();
	//my deseg ---
public:
	/**
	 * @brief 
	 * @param Sender the sender of the message (actor or actor component in most cases)
	 * @param Message 
	 */
	static void DebugNormal(UObject* Sender, FString Message);
};
