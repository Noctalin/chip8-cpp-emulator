#pragma once
class chip8
{
public:
	// Size of screen array
	static const unsigned short gfxSize = 64 * 32;
	
	// Screen of 2048 pixel stored in an array of 0 and 1
	unsigned char gfx[gfxSize];

	// HEX based keypad (0x0-0xF) to store current state of the key
	unsigned char key[16];

	// Size of memory array
	static const unsigned short memorySize = 4096;
	
	// Boolean value telling emulator to draw the image
	bool drawFlag;

	chip8();
	~chip8();

	void initialize();
	void loadGame(char* game);
	void emulateCycle();
	void setKeys();
	void debugRender();

private:

	static const unsigned short stackSize = 16;
	static const unsigned short registerSize = 16;

	// Holds one of the 35 Opcodes
	unsigned short opcode;

	// 4k Memory
	unsigned char memory[memorySize];

	// General purpose registers
	unsigned char V[registerSize];

	// Index register
	unsigned short I;

	// Program counter
	unsigned short pc;

	// Timer registers timed @ 60Hz, they count down to zero
	unsigned char delay_timer;
	unsigned char sound_timer;

	// Stack to remember current location before jumping or calling subroutines
	unsigned short stack[stackSize];

	// Pointer for stack
	unsigned short sp;

	void decodeOpcode();
};

