/* 
 * passive.c  - CHAP Proxy passive mode
 *
 * Copyright (c) 2014 Yang Li. All rights reserved.
 *
 * This program may be distributed according to the terms of the GNU
 * General Public License, version 3 or (at your option) any later version.
 */

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/select.h>
#include <unistd.h>
#include <syslog.h>

#include "queue.h"

#include "passive.h"
#include "worker.h"
#include "global.h"

int 
passive_main(){
    int nready;
    int sockfd;
    int requestfd;
    int pid;
    int fdmaxp1;
    fd_set readset;

    struct queue_int requests;

    queue_init(&requests);

    FD_ZERO(&readset);
    fdmaxp1 = (listenfd>passivefd? listenfd: passivefd) + 1;

    for(;;){
        FD_SET(listenfd, &readset);
        FD_SET(passivefd, &readset);
        if((nready = select(fdmaxp1, &readset, NULL, NULL, NULL)) < 0){
            switch(errno){
                case EINTR:
                    continue;
                    break;
                default:
                    fprintf(stderr, "select: %m");
                    syslog(LOG_CRIT, "select: %m");
                    exit(EXIT_FAILURE);
            }
        }

        if(FD_ISSET(listenfd, &readset)){
            if((sockfd = accept(listenfd, NULL, NULL)) < 0){
                switch(errno){
                    case EINTR:
                        continue;
                        break;
                    default:
                        fprintf(stderr, "accept: %m");
                        syslog(LOG_CRIT, "accept: %m");
                        exit(EXIT_FAILURE);
                }
            }
            if(queue_int_enqueue(&requests, sockfd) < 0){
                fprintf(stderr, "queue: %m");
                syslog(LOG_CRIT, "queue: %m");
                exit(EXIT_FAILURE);
            }
            if((pid = fork()) < 0){
                fprintf(stderr, "Couldn't fork: %m");
                syslog(LOG_CRIT, "Couldn't fork: %m");
                exit(EXIT_FAILURE);
            }else if(pid == 0){
                close(listenfd);
                close(passivefd);
                closelog();
                openlog("chap-proxy", LOG_PID, LOG_DAEMON);
                worker_main(-1, connect_script);
            }
        }

        if(FD_ISSET(passivefd, &readset)){
            if((sockfd = accept(passivefd, NULL, NULL)) < 0){
                switch(errno){
                    case EINTR:
                        continue;
                        break;
                    default:
                        fprintf(stderr, "accept: %m");
                        syslog(LOG_CRIT, "accept: %m");
                        exit(EXIT_FAILURE);
                }
            }
            if(queue_isempty(&requests)){
                close(sockfd);
                fprintf(stderr, "No requests");
                syslog(LOG_WARNING, "NO requests");
                continue;
            }
            requestfd = queue_int_dequeue(&requests);
            if((pid = fork()) < 0){
                fprintf(stderr, "Couldn't fork: %m");
                syslog(LOG_CRIT, "Couldn't fork: %m");
                exit(EXIT_FAILURE);
            }else if(pid == 0){
                close(listenfd);
                close(passivefd);
                closelog();
                openlog("chap-proxy", LOG_PID, LOG_DAEMON);
                return worker_passive(requestfd, sockfd);
            }
        }
    }
}

int
worker_passive(int requestfd, int passivefd){
    int nread;
    unsigned char buf[64];

    int nready;
    int fdmaxp1;
    fd_set readset;

    FD_ZERO(&readset);
    fdmaxp1 = (requestfd>passivefd? listenfd: passivefd) + 1;

    syslog(LOG_NOTICE, "CHAP proxy passive worker");

    for(;;){
        FD_SET(requestfd, &readset);
        FD_SET(passivefd, &readset);
        if((nready = select(fdmaxp1, &readset, NULL, NULL, NULL)) < 0){
            switch(errno){
                case EINTR:
                    continue;
                    break;
                default:
                    fprintf(stderr, "select: %m");
                    syslog(LOG_CRIT, "select: %m");
                    exit(EXIT_FAILURE);
            }
        }

        if(FD_ISSET(requestfd, &readset)){
            if((nread = read(requestfd, buf, sizeof(buf))) < 0){
                switch(errno){
                    case EINTR:
                        continue;
                        break;
                    default:
                        exit(EXIT_SUCCESS);
                }
            }
            write(passivefd, buf, nread);
        }

        if(FD_ISSET(passivefd, &readset)){
            if((nread = read(passivefd, buf, sizeof(buf))) < 0){
                switch(errno){
                    case EINTR:
                        continue;
                        break;
                    default:
                        exit(EXIT_SUCCESS);
                }
            }
            write(requestfd, buf, nread);
        }
    }
}
