
#pragma once
#include "../interrupts.h"
#include "../../stdio/stdio.h"

__attribute__((interrupt)) void Syscall0(interrupt_frame *frame);
__attribute__((interrupt)) void Syscall1(interrupt_frame *frame);
__attribute__((interrupt)) void Syscall2(interrupt_frame *frame);
__attribute__((interrupt)) void Syscall3(interrupt_frame *frame);
__attribute__((interrupt)) void Syscall4(interrupt_frame *frame);
__attribute__((interrupt)) void Syscall5(interrupt_frame *frame);
__attribute__((interrupt)) void Syscall6(interrupt_frame *frame);