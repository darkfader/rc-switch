
# Defines the RPI variable which is needed by rc-switch/RCSwitch.h
CXXFLAGS += -DRPI -I ../..
LDFLAGS += -lwiringPi -lwiringPiDev -lcrypt
export CXXFLAGS

all: send

send: ../../RCSwitch.o send.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $+ -o $@

../../RCSwitch.o: ../../RCSwitch.cpp ../../RCSwitch.h
	$(MAKE) -C ../.. RCSwitch.o

clean:
	$(RM) *.o send

