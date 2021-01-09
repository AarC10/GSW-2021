#ifndef NM_H
#define NM_H

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <mqueue.h>
#include <string>
#include "lib/vcm/vcm.h"
#include "common/types.h"

namespace nm {

    static const size_t MAX_Q_SIZE = 2048;

    // handles the physical sending and receiving of messages over the network
    // should only ever have one instance per device
    class NetworkManager {
    public:
        NetworkManager(vcm::VCM* vcm);
        ~NetworkManager();
        RetType Open();
        RetType Close(); // returns fail if anything goes wrong
        //return FAILURE if anything goes wrong
        RetType Iterate(); // send a message from the mqueue and then receive any messages available and place them into shmeme
        // TODO consider adding priority to sending some messages (like deployment) and then check other messages last
    private:
        mqd_t mq;
        std::string mqueue_name;
        vcm::VCM* vcm;

        struct sockaddr_in servaddr;
        int sockfd;

        char* buffer;
        bool open;
    };

    // allows a process to queue a message to send
    // can have many instances
    class NetworkInterface {
    public:
        NetworkInterface(vcm::VCM* vcm);
        ~NetworkInterface();
        RetType Open();
        RetType Close();
        RetType QueueUDPMessage(const char* msg, size_t size);
        bool open;
    private:
        std::string mqueue_name;
        mqd_t mq;
    };
}

#endif
