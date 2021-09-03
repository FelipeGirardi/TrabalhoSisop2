#include <stdio.h>
#include <stdlib.h>

#ifndef _NOTIFICATIONS_H
#define _NOTIFICATIONS_H

//  STRUCTS
typedef struct __notification {
    uint32_t id;            // Identificador da notificação (sugere-se um identificador único)
    char author[20];        // Nome do autor
    uint32_t timestamp;     // Timestamp da notificação
    char* _string;          // Mensagem
    uint16_t length;        // Tamanho da mensagem
    uint16_t pending;       // Quantidade de leitores pendentes
} notification;