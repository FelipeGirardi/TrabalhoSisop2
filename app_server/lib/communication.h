#pragma once

typedef struct __packet {
    uint16_t type;          //Tipo do pacote (p.ex. DATA | CMD)
    uint16_t seqn;          //Número de sequência
    uint16_t length;        //Comprimento do payload 
    time_t timestamp;       //Timestamp do dado
    char* _payload;   //Dados da mensagem
} packet;
