/*
 Usage: ./send <pattern> [pulseLength]
 */

#include "RCSwitch.h"
#include <stdlib.h>
#include <stdio.h>

#define dprintf(...) ({ /**/ })
// #define dprintf(...) printf(__VA_ARGS__)

#define ASSERT(c) ({                                   \
    if (!(c)) {                                        \
        fprintf(stderr, "assertion failed: %s\n", #c); \
        return -1;                                     \
    }                                                  \
})

#define NEXT_STRING() ({        \
    ASSERT(argi < argc);        \
    const char *v = argv[argi]; \
    argi++;                     \
    v;                          \
})

#define NEXT_CHAR(min, max) ({     \
    const char *s = NEXT_STRING(); \
    char        c = s[0];          \
    ASSERT(c >= min && c <= max);  \
    ASSERT(s[1] == '\0');          \
    c;                             \
})

#define NEXT_NUMBER(min, max) ({                \
    ASSERT(argi < argc);                        \
    unsigned v = strtoul(argv[argi], NULL, 10); \
    argi++;                                     \
    if (v < (min) || v > (max)) {               \
        return -1;                              \
    }                                           \
    v;                                          \
})

#define NEXT_BOOL() ({                          \
    ASSERT(argi < argc);                        \
    char *   end;                               \
    unsigned v = strtoul(argv[argi], &end, 10); \
    if (argv[argi] == end) {                    \
        v = argv[argi][0] == 'O' &&             \
            argv[argi][1] == 'N';               \
    }                                           \
    argi++;                                     \
    v;                                          \
})

#define OPTIONAL_PULSE_LENGTH() ({ \
    if (argi < argc) { \
        mySwitch.setPulseLength( strtoul(argv[argi++], NULL, 10)); \
        dprintf("overriding default pulse length\n"); \
    } \
})

int main(int argc, const char *argv[]) {
    /*
     output PIN is hardcoded for testing purposes
     see https://projects.drogon.net/raspberry-pi/wiringpi/pins/
     for pin mapping of the raspberry pi GPIO connector
     */
    int PIN = 0;

    if (argc < 3) {
        printf("Sending 433 MHz remote plug control codes, hardcoded on wiringpi pin %d.\n", PIN);
        printf("Usage: %s <protocol> <type> <parameters...> [pulseLength]\n", argv[0]);
        printf("\n");
        printf("# protocol pulseLength\n");
        printf("  1 ?, 350us\n");
        printf("  2 ?, 650us\n");
        printf("  3 ?, 100us\n");
        printf("  4 ?, 380us\n");
        printf("  5 ?, 500us\n");
        printf("  6 HT6P20B, 450us, inverted signal\n");
        printf("  7 HS2303-PT i. e. used in AUKEY Remote, 150us\n");
        printf("  8 FHT-7901 uses type E encoding hence different from protocol 1, 150us\n");
        printf("\n");
        printf("type + parameters: (s=string, n=number, b=boolean)\n");
        printf("  A sGroup sDevice bStatus\n");
        printf("    10 pole DIP switches\n");
        printf("  B nGroupNumber, int nSwitchNumber, bool bStatus\n");
        printf("    two rotary/sliding switches\n");
        printf("  C char sFamily, int nGroup, int nDevice, bool bStatus\n");
        printf("    Intertechno\n");
        printf("  D char group, int nDevice, bool bStatus\n");
        printf("    REV\n");
        printf("  E const char *sGroup, const char *sDevice, bool bStatus\n");
        printf("    10 pole DIP switches\n");
        printf("\n");
        printf("pulseLength - optional pulse length\n");
        return -1;
    }

    if (wiringPiSetup() == -1) {
        return -1;
    }

    int  argi      = 1;
    int  nProtocol = NEXT_NUMBER(1, 8);
    char cType     = NEXT_CHAR('A', 'E');

    RCSwitch mySwitch = RCSwitch();
    mySwitch.setProtocol(nProtocol);
    mySwitch.enableTransmit(PIN);

    switch (cType) {
        case 'A': {
            const char *sGroup  = NEXT_STRING();
            const char *sDevice = NEXT_STRING();
            int         bStatus = NEXT_BOOL();
            dprintf("%c %s %s %u\n", cType, sGroup, sDevice, bStatus);
            OPTIONAL_PULSE_LENGTH();
            if (bStatus) {
                mySwitch.switchOn(sGroup, sDevice);
            } else {
                mySwitch.switchOff(sGroup, sDevice);
            }
            break;
        }
        case '@':
        case 'B': {
            int nAddressCode = NEXT_NUMBER(1, 4);
            int nChannelCode = NEXT_NUMBER(1, 4);
            int bStatus      = NEXT_BOOL();
            dprintf("%c %u %u %u\n", cType, nAddressCode, nChannelCode, bStatus);
            OPTIONAL_PULSE_LENGTH();
            if (bStatus) {
                mySwitch.switchOn(nAddressCode, nChannelCode);
            } else {
                mySwitch.switchOff(nAddressCode, nChannelCode);
            }
            break;
        }
        case 'C': {
            char sFamily = NEXT_CHAR('a', 'f');
            int  nGroup  = NEXT_NUMBER(1, 4);
            int  nDevice = NEXT_NUMBER(1, 4);
            int  bStatus = NEXT_BOOL();
            dprintf("%c %c %u %u %u\n", cType, sFamily, nGroup, nDevice, bStatus);
            OPTIONAL_PULSE_LENGTH();
            if (bStatus) {
                mySwitch.switchOn(sFamily, nGroup, nDevice);
            } else {
                mySwitch.switchOff(sFamily, nGroup, nDevice);
            }
            break;
        }
        case 'D': {
            char sGroup  = NEXT_CHAR('A', 'D');
            int  nDevice = NEXT_NUMBER(1, 3);
            int  bStatus = NEXT_BOOL();
            dprintf("%c %c %u %u\n", cType, sGroup, nDevice, bStatus);
            OPTIONAL_PULSE_LENGTH();
            if (bStatus) {
                mySwitch.switchOn(sGroup, nDevice);
            } else {
                mySwitch.switchOff(sGroup, nDevice);
            }
            break;
        }
        case 'E': {
            const char *sGroup  = NEXT_STRING();
            const char *sDevice = NEXT_STRING();
            int         bStatus = NEXT_BOOL();
            dprintf("%c %s %s %u\n", cType, sGroup, sDevice, bStatus);
            OPTIONAL_PULSE_LENGTH();
            if (bStatus) {
                mySwitch.switchOn(sGroup, sDevice);
            } else {
                mySwitch.switchOff(sGroup, sDevice);
            }
            break;
        }
        default: {
            return -1;
        }
    }

    dprintf("OK\n");

    return 0;
}
