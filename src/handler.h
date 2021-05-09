#ifndef _PMON_HANDLER_H_
#define _PMON_HANDLER_H_

#include <stdbool.h>

/* 
 * handle a single process event
 */
int handle_proc_ev(int nl_sock, int target_pid, bool *need_exit);


#endif  /* _PMON_HANDLER_H_ */
