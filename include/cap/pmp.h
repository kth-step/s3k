#pragma once

#include "cap/table.h"
#include "error.h"

err_t cap_pmp_load(cte_t cidx, pmp_slot_t slot);
err_t cap_pmp_unload(cte_t cidx);
