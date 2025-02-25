#pragma once

#include "cap/table.h"
#include "error.h"

err_t cap_read(cte_t cte, cap_t *cap);
err_t cap_move(cte_t src, cte_t dst);
err_t cap_ipc_move(cte_t src, cte_t dst);
err_t cap_delete(cte_t cte);
err_t cap_revoke(cte_t parent);
err_t cap_derive(cte_t src, cte_t dst, cap_t new_cap);
