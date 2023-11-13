#include <memoryManager.h>
#ifdef BUDDY
#include <lib.h>

typedef struct BuddySystem {
	uint64_t memSize;
	uint64_t minPage;
	uint64_t virtualSize;
	uint64_t size;
	uint64_t reserved;
	unsigned int levels;
	unsigned char *base;
} BuddySystem;

BuddySystem buddy;

memoryInfo memoryInfo;

static uint64_t computeBuddySize(uint64_t memSize, uint64_t minPageSize);
static unsigned int getLevel(uint64_t size);
static uint64_t computeVirtualSize(uint64_t memSize);
static int getValue(unsigned int level, unsigned int offset);
static int checkUpwards(unsigned int level, unsigned int offset);
static void freeBranch(unsigned int level, unsigned int offset);
static void allocateBranch(unsigned int level, unsigned int offset);
static void *getPtr(unsigned int level, unsigned int offset);

void initializeMemoryManager() {
	uint64_t memBase = HEAP_ADDRESS;
	uint64_t memSize = HEAP_SIZE;
	uint64_t minPageSize = 0x1000;
	memSize = memSize - (uint64_t) memBase;

	uint64_t virtualSize = computeVirtualSize(memSize);
	uint64_t size = computeBuddySize(virtualSize, minPageSize);
	int levels = Log2n(virtualSize / minPageSize) + 1;

	buddy.base = memBase;
	buddy.memSize = memSize;
	buddy.minPage = minPageSize;
	buddy.virtualSize = virtualSize;
	buddy.size = size;
	buddy.levels = levels;
	buddy.reserved = 0;

	for (uint64_t i = 0; i < size; i++)
		buddy.base[i] = 0;

	// reservo zona donde esta el buddy
	int level = getLevel(size);
	allocateBranch(level, 0);

	// reservo zona de memoria que en realidad no tenemos
	uint64_t diffSize = virtualSize - memSize;
	if (diffSize != 0) {
		level = getLevel(diffSize);
		allocateBranch(level, pow(2, level) - 1);
	}
}

void *malloc(uint64_t size) {
	int level = getLevel(size);
	int i;
	for (i = 0; i < pow(2, level); i++) {
		if (getValue(level, i) == 0) {
			if (checkUpwards(level, i)) {
				allocateBranch(level, i);
				buddy.reserved += buddy.virtualSize / pow(2, level);
				return getPtr(level, i);
			}
		}
	}
	return NULL;
}

void free(void *address) {
	int offset = ((char *) address - (char *) buddy.base) / buddy.minPage;
	int active = 0;

	int level = buddy.levels - 1;
	while (!active) {
		int baseLevel = pow(2, level) - 1;
		active = getBit(buddy.base + (baseLevel + offset) / 8, (baseLevel + offset) % 8);
		if (!active) {
			offset = offset >> 1;
			level--;
		}
	}
	freeBranch(level, offset);
	buddy.reserved -= buddy.virtualSize / pow(2, level);
	return;
}

memoryInfo *getMemoryInfo() {
	memoryInfo.totalMemory = buddy.virtualSize;
	memoryInfo.freeMemory = buddy.virtualSize - buddy.reserved;
	memoryInfo.usedMemory = buddy.reserved;
	return &memoryInfo;
}

static uint64_t computeBuddySize(uint64_t memSize, uint64_t minPageSize) {
	int levels = Log2n(memSize / minPageSize) + 1;
	return (pow(2, levels)) / 8;
}

static unsigned int getLevel(uint64_t size) {
	if (size < buddy.minPage)
		size = buddy.minPage;
	return Log2n(buddy.virtualSize / size);
}

static uint64_t computeVirtualSize(uint64_t memSize) {
	uint64_t virtualSize = pow(2, Log2n(memSize));
	if (virtualSize < memSize) {
		virtualSize = pow(2, Log2n(memSize) + 1);
	}
	return virtualSize;
}

static int getValue(unsigned int level, unsigned int offset) {
	int baseLevel = pow(2, level) - 1;
	return getBit(buddy.base + (baseLevel + offset) / 8, (baseLevel + offset) % 8);
}

static int checkUpwards(unsigned int level, unsigned int offset) {
	int buddyIndex = (offset % 2) ? offset - 1 : offset + 1;

	while (level > 0 && getValue(level, buddyIndex) == 0) {
		offset = offset >> 1;
		level--;
		buddyIndex = (offset % 2) ? offset - 1 : offset + 1;
		if (getValue(level, offset) == 1)
			return 0;
	}
	return 1;
}

static void freeBranch(unsigned int level, unsigned int offset) {
	int bit = 0;

	int buddyIndex = (offset % 2) ? offset + 1 : offset - 1;
	while (level > 0 && getValue(level, buddyIndex) == 0) {
		clearBit(buddy.base + bit / 8, bit % 8);
		offset = offset >> 1;
		level--;
		buddyIndex = (offset % 2) ? offset + 1 : offset - 1;
		bit = pow(2, level) - 1 + offset;
	}
}

static void allocateBranch(unsigned int level, unsigned int offset) {
	for (int i = level; i >= 0; i--) {
		int bit = pow(2, i) - 1 + offset;
		setBit(buddy.base + bit / 8, bit % 8);
		offset = offset >> 1;
	}
}

static void *getPtr(unsigned int level, unsigned int offset) {
	unsigned int blockSize = buddy.virtualSize / pow(2, level);
	return buddy.base + blockSize * offset;
}

#endif