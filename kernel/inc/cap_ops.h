#pragma once

#include "cap_table.h"
#include "error.h"

err_t cap_read(cte_t cte, cap_t *cap);
err_t cap_move(cte_t src, cte_t dst, cap_t *cap);
err_t cap_delete(cte_t cte);
void cap_reclaim(cte_t parent, cap_t parent_cap, cte_t child, cap_t child_cap);
err_t cap_reset(cte_t cte);
err_t cap_derive(cte_t src, cte_t dst, cap_t new_cap);
