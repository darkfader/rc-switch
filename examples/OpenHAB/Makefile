
# Defines the RPI variable which is needed by rc-switch/RCSwitch.h
CXXFLAGS += -DRPI -I ../..
LDFLAGS += -lwiringPi -lwiringPiDev -lcrypt


all: send

send: ../../RCSwitch.o send.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $+ -o $@

../../RCSwitch.o:
	$(MAKE) -C ../.. RCSwitch.o

clean:
	$(RM) *.o send

