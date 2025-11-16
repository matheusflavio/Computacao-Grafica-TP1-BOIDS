#ifndef utils_hpp
#define utils_hpp

#include <stdio.h>
#include <chrono>

// Retorna tempo decorrido em millisegundos (com pausa considerada)
long getMillis();

// Retorna tempo real decorrido em millisegundos (sem pausa considerada)
long getRealMillis();

// Retorna um número aleatório entre 0 e 1
float getRand();

#endif