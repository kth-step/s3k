#include "cap_table.h"
#include "error.h"

err_t cap_monitor_suspend(cte_t mon, uint64_t pid);
err_t cap_monitor_resume(cte_t mon, uint64_t pid);
err_t cap_monitor_reg_read(cte_t mon, uint64_t pid, uint64_t reg, uint64_t *val);
err_t cap_monitor_reg_write(cte_t mon, uint64_t pid, uint64_t reg, uint64_t val);
err_t cap_monitor_cap_read(cte_t mon, cte_t src, cap_t *cap);
err_t cap_monitor_cap_move(cte_t mon, cte_t src, cte_t dst, cap_t *cap);
err_t cap_monitor_pmp_load(cte_t mon, cte_t pmp, uint64_t pmpslot);
err_t cap_monitor_pmp_unload(cte_t mon, cte_t pmp);
