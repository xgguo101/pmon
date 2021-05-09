#include "sock.h"
#include "handler.h"
#include <signal.h>
#include <getopt.h>     /* getopt_long, optarg */
#include <string.h>     /* atoi */
#include <stdlib.h>     /* exit */
#include <stdio.h>      

#define VERSION "pmon/0.0.1"
static bool _exit = false;
static bool * const g_exit_signal = &_exit;

static void 
on_sigint(int unused)
{
    _exit = true;
}

static void 
usage()
{
    fprintf(stderr, "Usage: pmon [option]\n"
            "       -p, --pid pid   Process ID\n"
            "       -V, --version   Show version\n"
            "       --help          Show usage\n"
            );
}

static struct option long_options[] = {
    {"pid",         required_argument, NULL, 'p'},
    {"version",     no_argument,       NULL, 'V'},
    {"help",        no_argument,       NULL, 'h'},
};


int 
main(int argc, char *argv[])
{  
    int opt;
    int opt_idx = 0;
    int pid = -1;

    while ((opt = getopt_long(argc, argv, "p:Vh", long_options, &opt_idx)) != EOF) {
        switch (opt) {
        case 'h':
            usage();
            exit(1);
        case 'V':
            printf("%s\n", VERSION);
            exit(1);
        case 'p':
            pid = atoi(optarg);
            break;
        default:
            usage();
            exit(1);
        }
    }

    int sockfd;
    int rv;

    signal(SIGINT, &on_sigint);
    siginterrupt(SIGINT, true);

    sockfd = nl_sock_create();
    nl_sock_bind(sockfd);

    if ((nl_sock_listen(sockfd)) == -1)
        goto err;

    rv = handle_proc_ev(sockfd, pid, g_exit_signal);
    if (rv == -1)
        goto err;
    
    nl_sock_close(sockfd);

    return 0;

err:
    nl_sock_close(sockfd);
    return 1;
}
