#pragma once

#include "kern/proc.h"
#include "kern/types.h"

Proc *SyscallHandler(Proc *proc);
