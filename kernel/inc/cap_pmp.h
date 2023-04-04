#pragma once

#include "cap_table.h"
#include "error.h"

err_t cap_pmp_load(cte_t cidx, uint64_t slot);
err_t cap_pmp_unload(cte_t cidx);
