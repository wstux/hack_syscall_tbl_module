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

#ifndef HIDE_PROC_H
#define HIDE_PROC_H

/*
 * @breif   Change module visibility status.
 * @param   pid - process idto which the signal was sent.
 * @param   sig - signal number.
 */
void ch_visibility_status(pid_t pid, int sig);

#endif // HIDE_PROC_H

