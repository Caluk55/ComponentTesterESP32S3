#ifndef TP_H
#define TP_H

#include <Arduino.h>

namespace tp {

    enum TPLabel : uint8_t {
        TP1 = 0,
        TP2 = 1,
        TP3 = 2
    };

    void begin();
    void configureAsInput(TPLabel tp);
    void configureAsOutput(TPLabel tp);
    void setLevel(TPLabel tp, bool level);
    int readLevel(TPLabel tp);
    void runTestCycle();  // Stub per logica futura

}

#endif // TP_H
