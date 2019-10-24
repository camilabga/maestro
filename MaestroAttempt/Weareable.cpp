#include "Weareable.h"

struct sockaddr_in myaddr, remaddr;

Weareable::Weareable(){
    slen = sizeof(remaddr);
    if ((fd=socket(AF_INET, SOCK_DGRAM, 0))==-1)
        cout<<"socket created\n"<<endl;
}

void Weareable::start(){
    memset((char *)&myaddr, 0, sizeof(myaddr));
    myaddr.sin_family = AF_INET;
    myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    myaddr.sin_port = htons(0);

    if (bind(fd, (struct sockaddr *)&myaddr, sizeof(myaddr)) < 0) {
        cout<<"bind failed"<<endl;
        //exit(1);
    }

    memset((char *) &remaddr, 0, sizeof(remaddr));
    remaddr.sin_family = AF_INET;
    remaddr.sin_port = htons(SERVICE_PORT);
    if (inet_aton(ip_esp, &remaddr.sin_addr)==0) {
        cout<<"inet_aton() failed\n";
        //exit(1);
    }
}

Weareable::~Weareable(){}

void Weareable::send(char *to_send){
    cout<<"Sending packet to "<<ip_esp<<" port "<< SERVICE_PORT<<endl;
    strcpy(buf, to_send);
    if (sendto(fd, buf, strlen(buf), 0, (struct sockaddr *)&remaddr, slen)==-1)
        cout<<"sendto"<<endl;
}
