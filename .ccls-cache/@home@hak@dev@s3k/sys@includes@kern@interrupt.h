#pragma once

#include "kern/proc.h"
#include "kern/types.h"

Proc *InterruptHandler(Proc *proc, Word mcause, Word mtval);
