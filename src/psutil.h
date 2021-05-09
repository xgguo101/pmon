#ifndef _PROCESS_INFO_H_
#define _PROCESS_INFO_H_

struct proc {
    int pid;
    int ppid;
    int uid;
    char *name;
    char *exe;
    char *state;
    char *cmdline;
};

struct proc get_process_info_bypid(int pid);

#endif  /* _PROCESS_INFO_H_ */
