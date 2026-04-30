SRC=../../src
g++ -DDEBUG -Wall -g -std=c++11 -I$SRC -I../../../lib8tion \
    $SRC/FakeLED.cpp test.cpp $SRC/ColorScheduler.cpp $SRC/LEDCurve.cpp \
    $SRC/Projection.cpp $SRC/Shape.cpp $SRC/SpiralEffect.cpp \
    $SRC/FlameEffect.cpp $SRC/MonoColorEffect.cpp $SRC/PulseEffect.cpp \
    $SRC/SignalTransmissionEffect.cpp $SRC/WaveEffect.cpp \
    $SRC/BlinkEffect.cpp $SRC/BlinkEffect2.cpp \
    -o a.exe
