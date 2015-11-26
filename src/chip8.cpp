#include "chip8.h"
#include <fstream>
#include <time.h>

// Font set @ memory location 0x50 == 80
unsigned char chip8_fontset[80] =
{
	0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
	0x20, 0x60, 0x20, 0x20, 0x70, // 1
	0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
	0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
	0x90, 0x90, 0xF0, 0x10, 0x10, // 4
	0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
	0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
	0xF0, 0x10, 0x20, 0x40, 0x40, // 7
	0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
	0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
	0xF0, 0x90, 0xF0, 0x90, 0x90, // A
	0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
	0xF0, 0x80, 0x80, 0x80, 0xF0, // C
	0xE0, 0x90, 0x90, 0x90, 0xE0, // D
	0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
	0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

chip8::chip8()
{
}


chip8::~chip8()
{
}

void chip8::initialize()
{
	// Initialize registers and memory once

	pc = 0x200;	// Program counter starts at 0x200
	opcode = 0; // Reset current opcode
	I = 0;		// Reset index register
	sp = 0;		// Reset stack pointer

	// Clear stack
	for (int i = 0; i < stackSize; i++)
		stack[i] = 0;

	// Clear display
	for (int i = 0; i < gfxSize; i++)
		gfx[i] = 0;

	// Clear register V0-VF
	for (int i = 0; i < registerSize; i++)
		V[i] = 0;

	// Clear keys
	for (int i = 0; i < 16; i++)
		key[i] = 0;

	// Clear memory
	for (int i = 0; i < memorySize; i++)
		memory[i] = 0;

	// Load fontset
	for (int i = 0; i < 80; i++)
		memory[i] = chip8_fontset[i];

	// Reset timers
	delay_timer = 0;
	sound_timer = 0;

	// Clear screen
	drawFlag = true;

	// Generate randomness
	srand(time(NULL));
}

void chip8::loadGame(char * game)
{
	// Load game into memory
	std::ifstream inFile;

	if (!inFile.good()) 
	{
		printf("Error opening file %s\n", game);
		return;
	}

	unsigned long size = 0;
	char* buffer = 0;

	inFile.open(game, std::ios::in | std::ios::binary | std::ios::ate);

	if (inFile.is_open())
	{
		size = inFile.tellg();
		inFile.seekg(0, std::ios::beg);

		buffer = new char[size + 1];
		inFile.read(buffer, size);
		buffer[size] = '\0';

		for (unsigned long i = 0; i < size + 1; i++)
		{
			memory[i + 512] = buffer[i];
		}
		inFile.close();

		delete[] buffer;

		printf("Successfully opened file %s\n", game);
	}

}

void chip8::emulateCycle()
{
	// Fetch Opcode
	// Use bitwise OR to merge both bytes to get the opcode
	opcode = memory[pc] << 8 | memory[pc + 1];
	// Go to next byte
	pc += 2;
	
	// Decode Opcode
	decodeOpcode();

	// Update timers
	if (delay_timer > 0)
		--delay_timer;

	if (sound_timer > 0)
	{
		if (sound_timer == 1)
		{
			printf("BEEP!\n");
			--sound_timer;
		}
	}
}

void chip8::decodeOpcode()
{
	//printf("Executing opcode 0x%04x\n", opcode);
	switch (opcode & 0xF000)
	{
	case 0x0000:
		switch (opcode & 0x00FF)
		{
		case 0x00E0:
			for (int i = 0; i < gfxSize; i++)
				gfx[i] = 0x0;
			//drawFlag = true;
			break;

		case 0x00EE:
			--sp;
			pc = stack[sp];
			break;
		default:
			//system("pause");
			//printf("Unkown opcode: 0x%X\n", opcode);
			break;
		}
	case 0x1000:
		pc = opcode & 0x0FFF;
		break;

	case 0x2000:
		stack[sp] = pc;
		++sp;
		pc = opcode & 0x0FFF;
		break;

	case 0x3000:
		if (V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF))
			pc += 2;
		break;

	case 0x4000:
		if (V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF))
			pc += 2;
		break;

	case 0x5000:
		if (V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 4])
			pc += 2;
		break;

	case 0x6000:
		V[(opcode & 0x0F00) >> 8] = (opcode & 0x00FF);
		break;

	case 0x7000:
		V[(opcode & 0x0F00) >> 8] += (opcode & 0x00FF);
		break;

	case 0x8000:
		switch (opcode & 0x000F)
		{
		case 0x0000:
			V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4];
			break;

		case 0x0001:
			V[(opcode & 0x0F00) >> 8] |= V[(opcode & 0x00F0) >> 4];
			break;

		case 0x0002:
			V[(opcode & 0x0F00) >> 8] &= V[(opcode & 0x00F0) >> 4];
			break;

		case 0x0003:
			V[(opcode & 0x0F00) >> 8] ^= V[(opcode & 0x00F0) >> 4];
			break;

		case 0x0004:
			// If sum is bigger than 255 set carry flag
			if (V[(opcode & 0x00F0) >> 4] > (0xFF - V[(opcode & 0x0F00) >> 8]))
				V[0xF] = 1;
			else
				V[0xF] = 0;
			V[(opcode & 0x0F00) >> 8] += V[(opcode & 0x00F0) >> 4];
			break;

		case 0x0005:
			if (V[(opcode & 0x00F0) >> 4] > V[(opcode & 0x0F00) >> 8])
				V[0xF] = 0;
			else
				V[0xF] = 1;
			V[(opcode & 0x0F00) >> 8] -= V[(opcode & 0x00F0) >> 4];
			break;

		case 0x0006:
			// Use logical AND on VX with 0x1 (0x00000001) to get LSB
			V[0xF] = V[(opcode & 0x0F00) >> 8] & 0x1;
			V[(opcode & 0x0F00) >> 8] >>= 1;
			break;

		case 0x0007:
			if (V[(opcode & 0x0F00) >> 8] > V[(opcode & 0x00F0) >> 4])
				V[0xF] = 0;
			else
				V[0xF] = 1;
			V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4] - V[(opcode & 0x0F00) >> 8];
			break;

		case 0x000E:
			// Shift VX by 7 bits to get the MSB
			V[0xF] = V[(opcode & 0x0F00) >> 8] >> 7;
			V[(opcode & 0x0F00) >> 8] <<= 1;
			break;

		default:
			//system("pause");
			//printf("Unkown opcode: 0x%X\n", opcode);
			break;
		}
		break;
	case 0x9000:
		if (V[(opcode & 0x0F00) >> 8] != V[(opcode & 0x00F0) >> 4])
			pc += 2;
		break;

	case 0xA000:
		I = opcode & 0x0FFF;	// Sets I to the address NNN
		break;

	case 0xB000:
		pc = (opcode & 0x0FFF) + V[0]; // Jump to address NNN + V0
		break;

	case 0xC000:
		V[(opcode & 0x0F00) >> 8] = (rand() % 0xFF) & (opcode & 0x00FF);
		break;

	case 0xD000:
	{
		// Draw sprite in memory at position I to position VX and VY and N
		unsigned short x = V[(opcode & 0x0F00) >> 8];
		unsigned short y = V[(opcode & 0x00F0) >> 4];
		unsigned short height = opcode & 0x000F;
		unsigned short sprite;

		V[0xF] = 0;

		for (int j = 0; j < height; j++)
		{
			// Get row I+j of sprite which is 8 bits wide
			sprite = memory[I + j];
			for (int i = 0; i < 8; i++)
			{
				// Check every bit/pixel in width of the sprite to see if it needs to be drawn
				// 0x80 gives the left most bit of a byte (8bits)
				// We shift 0x80 by i to move from left most bit to the right most bit
				if ((sprite & (0x80 >> i)) != 0)
				{
					// If pixel on screen is 1 and the sprite bit is also 1 then set carry flag
					if (gfx[i + x + ((j + y) * 64)] == 1)
						V[0xF] = 1;
					// Use XOR to flip pixel
					gfx[i + x + ((j + y) * 64)] ^= 1;
				}
			}
		}

		drawFlag = true;
		break;
	}
	case 0xE000:
		switch (opcode & 0x00FF)
		{
		case 0x009E:
			// Skip next instruction if key in VX is pressed
			if (key[V[(opcode & 0x0F00) >> 8]] == 1)
				pc += 2;
			break;

		case 0x00A1:
			// Skip next instruction if key in VX is pressed
			if (key[V[(opcode & 0x0F00) >> 8]] == 0)
				pc += 2;
			break;

		default:
			//system("pause");
			//printf("Unkown opcode: 0x%X\n", opcode);
			break;
		}
		break;

	case 0xF000:
		switch (opcode & 0x00FF)
		{
		case 0x0007:
			V[(opcode & 0x0F00) >> 8] = delay_timer;
			break;

		case 0x000A:
		{
			// A key press is awaited, then stored in VX
			bool keyPress = false;

			for (int i = 0; i < 16; i++)
			{
				if (key[i] != 0)
				{
					V[(opcode & 0x0F00) >> 8] = i;
					keyPress = true;
				}
			}

			// If we didn't received a keypress, skip this cycle and try again.
			if (!keyPress)
				return;
			break;
		}
		case 0x0015:
			delay_timer = V[(opcode & 0x0F00) >> 8];
			break;

		case 0x0018:
			sound_timer = V[(opcode & 0x0F00) >> 8];
			break;

		case 0x001E:
			if (I + V[(opcode & 0x0F00) >> 8] > 0xFFF)	// VF is set to 1 when range overflow (I+VX>0xFFF), and 0 when there isn't.
				V[0xF] = 1;
			else
				V[0xF] = 0;
			I += V[(opcode & 0x0F00) >> 8];
			break;

		case 0x0029:
			I = V[(opcode & 0x0F00) >> 8] *5;
			break;

		case 0x0033:
		{
			int hundreds = V[(opcode & 0x0F00) >> 8] / 100;
			int tens = (V[(opcode & 0x0F00) >> 8] / 10) % 10;
			int units = (V[(opcode & 0x0F00) >> 8]) % 10;
			memory[I] = hundreds;
			memory[I + 1] = tens;
			memory[I + 2] = units;
			break;
		}
		case 0x0055:
			for (int vx = 0; vx <= ((opcode & 0x0F00) >> 8); vx++)
				memory[I + vx] = V[vx];

			// On the original interpreter, when the operation is done, I = I + X + 1.
			I += ((opcode & 0x0F00) >> 8) + 1;
			break;

		case 0x0065:
			for (int vx = 0; vx <= ((opcode & 0x0F00) >> 8); vx++)
				V[vx] = memory[I + vx];
			
			// On the original interpreter, when the operation is done, I = I + X + 1.
			I += ((opcode & 0x0F00) >> 8) + 1;
			break;

		default:
			//system("pause");
			//printf("Unkown opcode: 0x%X\n", opcode);
			break;
		}
		break;

	default:
		//system("pause");
		//printf("Unkown opcode: 0x%X\n", opcode);
		break;
	}
}

void chip8::debugRender()
{
	// Draw
	for (int y = 0; y < 32; ++y)
	{
		for (int x = 0; x < 64; ++x)
		{
			if (gfx[(y * 64) + x] == 0)
				printf("O");
			else
				printf(" ");
		}
		printf("\n");
	}
	printf("\n");
}

void chip8::setKeys()
{
}
