// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

//refrence for this debugging system
//https://nerivec.github.io/old-ue4-wiki/pages/logs-printing-class-name-function-name-line-number-of-your-calling-code.html


//Current Class Name + Function Name where this is called!
#define DEBUG_CLASS_FUNC (FString(__FUNCTION__))

//Current Class where this is called!
#define DEBUG_CLASS (FString(__FUNCTION__).Left(FString(__FUNCTION__).Find(TEXT(":"))) )

//Current Function Name where this is called!
#define DEBUG_FUNCTION (FString(__FUNCTION__).Right(FString(__FUNCTION__).Len() - FString(__FUNCTION__).Find(TEXT("::")) - 2 ))

//Current line number where this macro was called
#define DEBUG_LINE (FString::FromInt(__LINE__))

/* some other macros
//Current Class and Line Number where this is called!
#define JOYSTR_CUR_CLASS_LINE (JOYSTR_CUR_CLASS + "(" + JOYSTR_CUR_LINE + ")")
  
//Current Function Signature where this is called!
#define JOYSTR_CUR_FUNCSIG (FString(__FUNCSIG__))
//Victory Screen Message
// 	Gives you the Class name and exact line number where you print a message to yourself!
#define VSCREENMSG(Param1) (GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, *(JOYSTR_CUR_CLASS_LINE + ": " + Param1)) )

#define VSCREENMSG2(Param1,Param2) (GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, *(JOYSTR_CUR_CLASS_LINE + ": " + Param1 + " " + Param2)) )

#define VSCREENMSGF(Param1,Param2) (GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, *(JOYSTR_CUR_CLASS_LINE + ": " + Param1 + " " + FString::SanitizeFloat(Param2))) )
 */


//Messages via UE_LOG

//DL = DebugLog
#define DL_NORMAL(Message) UE_LOG(LogTemp, Warning, TEXT("%s<%s> %s"), *DEBUG_CLASS, *DEBUG_LINE , *FString(Message))
// example DL_NORMAL("Hello world!!")

#define DL_ERROR(Message) UE_LOG(LogTemp, Error, TEXT("%s<%s> %s"), *DEBUG_CLASS, *DEBUG_LINE , *FString(Message))

#define DL_Grey(Message) UE_LOG(LogTemp, Display, TEXT("%s<%s> %s"), *DEBUG_CLASS, *DEBUG_LINE , *FString(Message))

/**
 * 
 */
class RACINGUNREALPROJECT_API DebugLog
{
public:
	DebugLog();
	~DebugLog();
	
};
