#ifndef __KEY_H
#define __KEY_H
#include "stm32f10x.h"                  // Device header

void Key_Init(void);
void KeyIT_Init(void);
void KeyIT_SwiInit(FunctionalState State);

#endif

