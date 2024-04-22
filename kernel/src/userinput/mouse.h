#pragma once
#include "../IO.h"
#include "../math.h"
#include "../BasicRenderer.h"

#define PS2Leftbutton 0b00000001
#define PS2Middlebutton 0b00000100
#define PS2Rightbutton 0b00000010
#define PS2XSign 0b00010000
#define PS2YSign 0b00100000
#define PS2XOverflow 0b01000000
#define PS2YOverflow 0b10000000

extern uint8_t MousePointer[];

void InitPS2Mouse();
void MouseWrite(uint8_t value);
uint8_t MouseRead();
void HandlePS2Mouse(uint8_t data);
void ProcessMousePacket();
extern Point MousePosition;
extern uint64_t mouseButtonData;