#include <sound.h>
#include <syscalls.h>

void playSoundLimitReached() {
	playSound(66, 1);
}