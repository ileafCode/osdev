
#pragma once
#include "../interrupts.h"
#include "../../stdio/stdio.h"

__attribute__((interrupt)) void Syscall0x80(interrupt_frame *frame);