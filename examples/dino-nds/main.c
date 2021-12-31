#include <nds.h>

#include "../../w2c2_base.h"

#define INPUT e_mem->data[0]
#define FP(o) (e_mem->data[0x5000 + (o)] / 8)

/* Imports */

F32 Math_random(void) {
    return (F32)rand()/(F32)(RAND_MAX);
}

F32 (*f_Math_random)(void) = &Math_random;

void
trap(
    Trap trap
) {
    abort();
}


/* Exports */

extern wasmMemory (*e_mem);

extern void (*e_run)();

extern void init();

void vblank() {
    scanKeys();

    int current = keysUp();
    if (current & KEY_A) {
        INPUT &= ~1;
    }
    if (current & KEY_DOWN) {
        INPUT &= ~2;
    }

    int down = keysDown();
    if (down & KEY_A) {
        INPUT |= 1;
    }
    if (down & KEY_DOWN) {
        INPUT |= 2;
    }

    (*e_run)();

    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            if (y >= 75) {
                VRAM_A[SCREEN_WIDTH * y + x] = RGB5(31, 31, 31);
            } else {
                int off = (300 * 4) * y + x * 4;
                VRAM_A[SCREEN_WIDTH * y + x] = RGB5(
                    FP(off),
                    FP(off + 1),
                    FP(off + 2)
                );
            }
        }
    }
}

int main(int argc, char* argv[]) {

    powerOn(POWER_ALL_2D);
    vramSetBankA(VRAM_A_LCD);
    videoSetMode(MODE_FB0);
    irqSet(IRQ_VBLANK, vblank);
    lcdSwap();

    /* Initialize module */
    init();

    while (true) {
        swiWaitForVBlank();
    }
}
