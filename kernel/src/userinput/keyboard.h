#pragma once
#include <stdint.h>
#include "kbScancodeTranslation.h"
#include "../BasicRenderer.h"
#include "../memory/heap.h"
#include "../memory.h"

bool IsShift();
bool IsCaps();

void PrepareKeyboard();
void HandleKeyboard(uint8_t scancode);
uint8_t KeyboardGetScancode();
char KeyboardGetChar();
char *KeyboardGetStr();