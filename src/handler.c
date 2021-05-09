/**
 * 
 * In user space     -  In  kernel space
 *
 * parent process ID  =  parent->tgid
 * parent thread  ID  =  parent->pid
 * child  process ID  =  child->tgid
 * child  thread  ID  =  child->pid
 * 
 */

#include "sock.h"
#include "psutil.h"
#include <errno.h>      /* error, EINTR */
#include <stdbool.h>    /* bool */
#include <stdio.h>      /* printf */


static inline bool
filter_target_pid(struct nlcn_recv_msg *m, int target_pid)
{
    if (target_pid != -1 && target_pid != m->proc_ev.event_data.exit.process_tgid) {
        return true;
    }
    return false;
}


static inline void
handle_fork_ev(struct nlcn_recv_msg *m, int target_pid)
{
    if (filter_target_pid(m, target_pid))
        return;
    
    int pid = m->proc_ev.event_data.fork.parent_tgid;
    struct proc p = get_process_info_bypid(pid);

    printf("fork -> {name: %s, pid: %d, tid: %d, cpid: %d, ctid: %d, cmdline: %s}\n",
        p.name ? p.name : "NULL",
        pid,
        m->proc_ev.event_data.fork.parent_pid,
        m->proc_ev.event_data.fork.child_tgid,
        m->proc_ev.event_data.fork.child_pid,
        p.cmdline ? p.cmdline : "NULL"
    );
}

static inline void
handle_exec_ev(struct nlcn_recv_msg *m, int target_pid)
{
    if (filter_target_pid(m, target_pid))
        return;
    
    int pid = m->proc_ev.event_data.fork.parent_tgid;
    struct proc p = get_process_info_bypid(pid);

    printf("exec -> {name: %s, pid: %d, tid: %d, cmdline: %s}\n",
        p.name ? p.name : "NULL",
        pid,
        m->proc_ev.event_data.exec.process_pid,
        p.cmdline ? p.cmdline : "NULL"
    );
}

static inline void
handle_uid_ev(struct nlcn_recv_msg *m, int target_pid)
{
    if (filter_target_pid(m, target_pid))
        return;

    printf("uid change -> {pid:%d, tid: %d, uid_from:%d, uid_to: %d}\n",
        m->proc_ev.event_data.id.process_tgid,
        m->proc_ev.event_data.id.process_pid,
        m->proc_ev.event_data.id.r.ruid,
        m->proc_ev.event_data.id.e.euid
    );
}

static inline void
handle_gid_ev(struct nlcn_recv_msg *m, int target_pid)
{
    if (filter_target_pid(m, target_pid))
        return;
    
    printf("gid change -> {pid: %d, tid: %d, gid_from: %d, gid_to: %d}\n",
        m->proc_ev.event_data.id.process_tgid,
        m->proc_ev.event_data.id.process_pid,
        m->proc_ev.event_data.id.r.rgid,
        m->proc_ev.event_data.id.e.egid
    );
}

static inline void
handle_exit_ev(struct nlcn_recv_msg *m, int target_pid)
{
    if (filter_target_pid(m, target_pid))
        return;
    
    int pid = m->proc_ev.event_data.fork.parent_tgid;
    struct proc p = get_process_info_bypid(pid);

    printf("exit -> {name: %s, pid: %d, tid: %d, exit_code: %d, cmdline: %s}\n",
        p.name ? p.name : "NULL",
        pid,
        m->proc_ev.event_data.exit.process_pid,
        m->proc_ev.event_data.exit.exit_code,
        p.cmdline ? p.cmdline : "NULL"
    );
}


/*
 * handle a single process event
 */
int 
handle_proc_ev(int nl_sock, int target_pid, bool *need_exit) 
{
    int n;
    struct nlcn_recv_msg msg;

    while (!(*need_exit)) {
        n = nl_sock_recv(nl_sock, &msg);

        if (n == 0) {   /* shutdown? */
            return 0;
        }

        if (n == -1) {
            if (errno == EINTR) {
                continue;
            }
            perror("netlink recv");
            return -1;
        }

        switch (msg.proc_ev.what) {
        case PROC_EVENT_NONE:
            printf("set mcast listen ok\n");
            break;
        case PROC_EVENT_FORK:
            handle_fork_ev(&msg, target_pid);
            break;
        case PROC_EVENT_EXEC: 
            handle_exec_ev(&msg, target_pid);
            break;
        case PROC_EVENT_UID:
            handle_uid_ev(&msg, target_pid);
            break;
        case PROC_EVENT_GID:
            handle_gid_ev(&msg, target_pid);
            break;
        case PROC_EVENT_EXIT:
            handle_exit_ev(&msg, target_pid);
            break;
        default:
            // printf("unhandled proc event\n");
            break;
        }
    }

    return 0;
}
