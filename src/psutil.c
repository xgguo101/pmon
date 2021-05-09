#include "psutil.h"
#include <unistd.h>     /* readlink */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


static int
parse_status(struct proc *proc, int pid)
{
    char filename[64];
    FILE *fp;
    char line[2048];

    int uid;;
    int ppid;
    static char pstate[64];
    static char pname[128];

    sprintf(filename, "/proc/%d/status", pid);
    fp = fopen(filename, "r");
    if (fp == NULL) {
        perror("open status");
        return -1;
    }

    while (fgets(line, sizeof(line), fp) != NULL) {
        char *pos = NULL;
        char s[32];
        memset(s, 0, sizeof(s));

        if ((pos = memchr(line, ':', 16)) != NULL) {
            int index = pos - line;
            strncpy(s, line, index);

            if (!strcmp(s, "Name")) {
                sscanf(line, "Name: %s", pname);
            } else if (!strcmp(s, "State")) {
                sscanf(line, "State: %*s %s", pstate);
            } else if (!strcmp(s, "Uid")) {
                sscanf(line, "Uid: %d", &uid);
            } else if (!strcmp(s, "Ppid")) {
                sscanf(line, "Ppid: %d", &ppid);
            }
        }
    }

    fclose(fp);
    
    proc->uid = uid;
    proc->pid = pid; 
    proc->ppid = ppid;
    proc->name = pname;
    proc->state = pstate;

    return 0;
}


static int
get_cmdline_bypid(struct proc *proc, int pid)
{
    char filename[64];
    FILE *fp;
    static char line[2048];

    sprintf(filename, "/proc/%d/cmdline", pid);
    fp = fopen(filename, "rb");
    if (fp == NULL) {
        perror("open cmdline");
        return -1;
    }

    if (fgets(line, sizeof(line), fp) != NULL) {
        proc->cmdline = line;
    }
    
    fclose(fp);
    return 0;
}


static int
get_exe_bypid(struct proc *proc, int pid)
{
    char filename[64];
    static char line[2048];

    sprintf(filename, "/proc/%d/exe", pid);
    if (readlink(filename, line, sizeof(line)) != -1) {
        proc->exe = line;
        return 0;
    }
    perror("readlink");
    return -1;
}


struct proc 
get_process_info_bypid(int pid)
{
    struct proc p;
    memset(&p, 0, sizeof(p));

    parse_status(&p, pid);
    get_cmdline_bypid(&p, pid);
    get_exe_bypid(&p, pid);

    return p;
}

// int 
// main() 
// {
//     struct proc p;
//     parse_status(&p, 1777);
//     get_cmdline_bypid(&p, 1777);
//     get_exe_bypid(&p, 1777);
//     printf("name:   %s\n    \
//             state:  %s\n    \
//             pid:    %d\n    \
//             ppid:   %d\n    \
//             uid:    %d\n    \
//             cmdline: %s\n   \
//             ",
//             p.name, p.state, p.pid, p.ppid, p.uid, p.cmdline
//     );

//     return 0;
// }
