/* Tsunami for KallistiOS ##version##

   genmenu.cpp
   Copyright (C)2003 Dan Potter

   The included font (typewriter.txf) was pulled from the dcplib example
   in ../../cpp/dcplib.

*/

/*

This example shows off the generic menu class. It only exercises a very
small subset of the possible functionality of genmenu, but it shows the
basics.

*/

#include <kos.h>
#include <math.h>
#include <tsu/genmenu.h>
#include <tsu/font.h>

#include <tsu/drawables/label.h>
#include <tsu/anims/logxymover.h>
#include <tsu/anims/expxymover.h>
#include <tsu/anims/alphafader.h>
#include <tsu/triggers/death.h>

#include "vibration/VibrationTestKOS.h"
#include "vibration/VibrationTestRaw.h"

extern uint8 romdisk[];
KOS_INIT_FLAGS(INIT_DEFAULT | INIT_MALLOCSTATS);
KOS_INIT_ROMDISK(romdisk);



int main(int argc, char **argv) {
    int done = 0, done2 = 0;

    maple_init();

    // Guard against an untoward exit during testing.
    cont_btn_callback(0, CONT_START | CONT_A | CONT_B | CONT_X | CONT_Y,
                      (void (*)(unsigned char, long  unsigned int))arch_exit);

    // Get 3D going
    pvr_init_defaults();

    // Load a font
    //RefPtr<Font> fnt = new Font("/rd/typewriter.txf");
    RefPtr<Font> fnt = new Font("/rd/axaxax.txf");

    // Create a menu
    RefPtr<VibrationMenuKOS> vibrationMenuKOS = new VibrationMenuKOS(fnt);
    RefPtr<VibrationMenuRaw> vibrationMenuRaw = new VibrationMenuRaw(fnt);

    // Do the menu
    // vibrationMenuRaw->doMenu();
    vibrationMenuKOS->doMenu();

    return 0;
}


