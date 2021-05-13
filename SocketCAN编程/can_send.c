#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#define CAN0 "can0"
#define CAN0_CONFIG "ip link set can0 up type can \
tq 50 prop-seg 8 phase-seg1 6 phase-seg2 5 sjw 2 \
dtq 10 dprop-seg 8 dphase-seg1 6 dphase-seg2 5 dsjw 2 fd on"
#define CAN0_DOWN "ip link set can0 down"
#define CANFD_TEST_LEN 64
int main(int argv, char **argc)
{
    struct sockaddr_can addr;
    struct ifreq ifr;
    struct canfd_frame frame;
    int enable_canfd = 1;
    int i = 0, nbytes;
    int s;
    system(CAN0_DOWN);
    system(CAN0_CONFIG);
    /* init socket */
    if((s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0){
    perror("socket");
    return -1;
}
strcpy(ifr.ifr_name, CAN0);
ifr.ifr_ifindex = if_nametoindex(ifr.ifr_name);
if(!ifr.ifr_ifindex){
    perror("if_nametoindex");
    return -1;
}
memset(&addr, 0, sizeof(addr));
addr.can_family = AF_CAN;
addr.can_ifindex = ifr.ifr_ifindex;
if(ioctl(s, SIOCGIFINDEX, &ifr) < 0){
    perror("SIOCGIFINDEX");
    return -1;
}
if(setsockopt(s, SOL_CAN_RAW, CAN_RAW_FD_FRAMES, &enable_canfd, sizeof(enable_canfd))){
    printf("error when enableing CANFD support\n");
    return -1;
}
if(bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0){
    perror("bind");
    return -1;
}

/* send data */
frame.can_id = 0x666;
for(i = 0; i < CANFD_TEST_LEN; i++){
    frame.data[i] = i;
}
frame.len = CANFD_TEST_LEN;
nbytes = sizeof(frame);
if (write(s, &frame, nbytes) != nbytes) {
    perror("write");
}
close(s);
return 0;
}