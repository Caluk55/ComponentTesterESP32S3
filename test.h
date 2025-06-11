#ifndef TEST_H
#define TEST_H

namespace test {
    void detectResistorTP1TP2();
    void detectDiodeTP1TP2();
    void detectDiodeBetween(tp::TPLabel a, tp::TPLabel b);
    void detectDoubleDiodeOrLED(tp::TPLabel a, tp::TPLabel b);
    void detectBJT();
    void detectCapacitor();
    void detectInductor();
    void detectMOSFET();

}

#endif // TEST_H
