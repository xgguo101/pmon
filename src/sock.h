#ifndef _PMON_SOCK_H_
#define _PMON_SOCK_H_

#include <linux/netlink.h>
#include <linux/cn_proc.h>  
#include <linux/connector.h>
#include <sys/socket.h>


struct nlcn_send_msg {
    struct nlmsghdr nl_hdr;
    struct {
        struct cn_msg cn_msg;
        enum proc_cn_mcast_op cn_mcast;
    } __attribute__((__packed__));

} __attribute__((aligned(NLMSG_ALIGNTO)));


struct nlcn_recv_msg {
    struct nlmsghdr nl_hdr;
    struct {
        struct cn_msg cn_msg;
        struct proc_event proc_ev;
    } __attribute__((__packed__));

} __attribute__((aligned(NLMSG_ALIGNTO)));


/* create netlink socket, returns netlink socket, or -1 on error */
int nl_sock_create();

/* connected netlink socket, returns netlink socket, or -1 on error */
int nl_sock_bind(int nl_sock); 

/* subscribe on proc events, returns 0 on success, or -1 on error */
int nl_sock_listen(int nl_sock);

/*
 * read N bytes into BUF from netlink socket
 * returns the number read or -1 for errors
 */
int nl_sock_recv(int nl_sock, struct nlcn_recv_msg *msg);

/* closed netlink socket */
void nl_sock_close(int nl_sock);

#endif /* _PMON_SOCK_H_ */
