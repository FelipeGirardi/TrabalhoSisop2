#ifndef COMMUNICATIONMANAGER_H
#define COMMUNICATIONMANAGER_H

#define DATA        1
#define CMD         2
#define BUFFER_SIZE 256

namespace communicationManager {
    class Packet {
    public:
        uint16_t type;          // Tipo do pacote
        uint16_t seqn;          // Número de sequência
        uint16_t length;        // Comprimento
        uint16_t timestamp;       // Timestamp
        char _payload[BUFFER_SIZE];         // Dados do pacote
    };

    class CommunicationManager {
    private:
        Packet packet;
    public:
        int read_text(int socket, char *buffer);
        int read_packet(int socket, Packet *package, char *buffer);
        int send_packet(int socket, Packet *package);
    };
}


#endif //COMMUNICATIONMANAGER_H
