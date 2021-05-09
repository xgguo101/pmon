#include "sock.h"
#include <unistd.h>     /* getpid() */
#include <string.h>     /* memset */


/*
 * create netlink socket,
 * returns netlink socket, or -1 on error 
 */
int
nl_sock_create()
{
    int nl_sock;
    
    /* 与 udp socket 通信类似 */
    nl_sock = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_CONNECTOR);
    if (nl_sock == -1) {
        perror("socket init");
        return -1;
    }

    return nl_sock;
}


/*
 * connected netlink socket
 * returns netlink socket, or -1 on error 
 */
int 
nl_sock_bind(int nl_sock) 
{
    int rv;
    struct sockaddr_nl addr;

    addr.nl_family = AF_NETLINK;
    addr.nl_groups = CN_IDX_PROC;
    addr.nl_pid = getpid();

    rv = bind(nl_sock, (struct sockaddr *)&addr, sizeof(addr));
    if (rv == -1) {
        perror("netlink bind");
        nl_sock_close(nl_sock);
        return -1;
    }

    return nl_sock;
}


/*
 * subscribe on proc events
 * returns 0, or -1 on error 
 */
int 
nl_sock_listen(int nl_sock)
{
    int rv;
    struct nlcn_send_msg msg;

    memset(&msg, 0, sizeof(msg));
    msg.nl_hdr.nlmsg_len = sizeof(msg);
    msg.nl_hdr.nlmsg_pid = getpid();
    msg.nl_hdr.nlmsg_type = NLMSG_DONE;

    msg.cn_msg.id.idx = CN_IDX_PROC;
    msg.cn_msg.id.val = CN_VAL_PROC;
    msg.cn_msg.len = sizeof(enum proc_cn_mcast_op);

    msg.cn_mcast = PROC_CN_MCAST_LISTEN;

    rv = send(nl_sock, &msg, sizeof(msg), 0);
    if (rv == -1) {
        perror("netlink send");
        return -1;
    }

    return 0;
}


/*
 * read N bytes into BUF from netlink socket .
 * returns the number read or -1 for errors
 */
int 
nl_sock_recv(int nl_sock, struct nlcn_recv_msg *msg)
{
    int n;
    n = recv(nl_sock, msg, sizeof(struct nlcn_recv_msg), 0);
    return n;
}


/* closed netlink socket */
void
nl_sock_close(int nl_sock)
{
    close(nl_sock);
}
