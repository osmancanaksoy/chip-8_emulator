#include <SFML/Graphics.hpp>
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include "stack.h"
#include <fstream>

using namespace std;

const uint8_t fontset[80] =
{
  //0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
  //0x20, 0x60, 0x20, 0x20, 0x70, // 1
  //0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
  //0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
  //0x90, 0x90, 0xF0, 0x10, 0x10, // 4
  //0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
  //0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
  //0xF0, 0x10, 0x20, 0x40, 0x40, // 7
  //0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
  //0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
  //0xF0, 0x90, 0xF0, 0x90, 0x90, // A
  //0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
  //0xF0, 0x80, 0x80, 0x80, 0xF0, // C
  //0xE0, 0x90, 0x90, 0x90, 0xE0, // D
  //0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
  //0xF0, 0x80, 0xF0, 0x80, 0x80  // F

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

#define SCALE 10

#define HEIGHT_CHIP_8 32
#define WIDTH_CHIP_8  64

#define HEIGHT (HEIGHT_CHIP_8 * SCALE)
#define WIDTH (WIDTH_CHIP_8 * SCALE)

Stack<uint16_t> s;
uint16_t PC;
uint16_t I;
uint16_t opcode;
uint8_t registers[16];
uint8_t memory[4096];
uint8_t delay_timer;
uint8_t sound_timer;
uint8_t pixel_flag[HEIGHT_CHIP_8][WIDTH_CHIP_8];
uint8_t key[16];
bool draw_flag;

bool print_flag = true;

void init();
void read_rom(string file_name);
void emulate_cycle();


int main()
{
    srand(time(0));
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "CHIP-8 Emulator");
    sf::RectangleShape rectangle(sf::Vector2f(SCALE, SCALE));

    init();
    
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            
            if (event.type == sf::Event::Closed)
                window.close();
            else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
                window.close();
			//check for key presses
            
			else if (event.type == sf::Event::KeyPressed)
			{
				//program/debugger keys
				if (event.key.code == sf::Keyboard::Escape)
					exit(0);
				//gamepad keys
				else if (event.key.code == sf::Keyboard::Num1)
					key[0x1] = 1;
				else if (event.key.code == sf::Keyboard::Num2)
					key[0x2] = 1;
				else if (event.key.code == sf::Keyboard::Num3)
					key[0x3] = 1;
				else if (event.key.code == sf::Keyboard::Num4)
					key[0xC] = 1;
				else if (event.key.code == sf::Keyboard::Q)
					key[0x4] = 1;
				else if (event.key.code == sf::Keyboard::W)
					key[0x5] = 1;
				else if (event.key.code == sf::Keyboard::E)
					key[0x6] = 1;
				else if (event.key.code == sf::Keyboard::R)
					key[0xD] = 1;
				else if (event.key.code == sf::Keyboard::A)
					key[0x7] = 1;
				else if (event.key.code == sf::Keyboard::S)
					key[0x8] = 1;
				else if (event.key.code == sf::Keyboard::D)
					key[0x9] = 1;
				else if (event.key.code == sf::Keyboard::F)
					key[0xE] = 1;
				else if (event.key.code == sf::Keyboard::Z)
					key[0xA] = 1;
				else if (event.key.code == sf::Keyboard::X)
					key[0x0] = 1;
				else if (event.key.code == sf::Keyboard::C)
					key[0xB] = 1;
				else if (event.key.code == sf::Keyboard::V)
					key[0xF] = 1;
			}

			//key up events to clear gamepad keys
			else if (event.type == sf::Event::KeyReleased)
			{
				if (event.key.code == sf::Keyboard::Num1)
					key[0x1] = 0;
				else if (event.key.code == sf::Keyboard::Num2)
					key[0x2] = 0;
				else if (event.key.code == sf::Keyboard::Num3)
					key[0x3] = 0;
				else if (event.key.code == sf::Keyboard::Num4)
					key[0xC] = 0;
				else if (event.key.code == sf::Keyboard::Q)
					key[0x4] = 0;
				else if (event.key.code == sf::Keyboard::W)
					key[0x5] = 0;
				else if (event.key.code == sf::Keyboard::E)
					key[0x6] = 0;
				else if (event.key.code == sf::Keyboard::R)
					key[0xD] = 0;
				else if (event.key.code == sf::Keyboard::A)
					key[0x7] = 0;
				else if (event.key.code == sf::Keyboard::S)
					key[0x8] = 0;
				else if (event.key.code == sf::Keyboard::D)
					key[0x9] = 0;
				else if (event.key.code == sf::Keyboard::F)
					key[0xE] = 0;
				else if (event.key.code == sf::Keyboard::Z)
					key[0xA] = 0;
				else if (event.key.code == sf::Keyboard::X)
					key[0x0] = 0;
				else if (event.key.code == sf::Keyboard::C)
					key[0xB] = 0;
				else if (event.key.code == sf::Keyboard::V)
					key[0xF] = 0;
			}
		}       

        //sf::sleep(sf::milliseconds(20));

        
        emulate_cycle();
        sf::Clock clock;
        while (draw_flag) {
            sf::Time elapsed = clock.getElapsedTime();
            if (elapsed.asMilliseconds() > 1) {

                //clock.restart();

                    window.clear(sf::Color::White);

                    for (int y = 0; y < HEIGHT_CHIP_8; y++) {

                        for (int x = 0; x < WIDTH_CHIP_8; x++) {

                            uint8_t flag = pixel_flag[y][x];

                            for (int xline = 0; xline < 8; xline++)
                            {
                                if ((flag & (0x80 >> xline)) != 0)
                                {
                                    rectangle.setPosition((float)(SCALE * x), (float)(SCALE * y));
                                    rectangle.setFillColor(sf::Color::Green);
                                    window.draw(rectangle);
                                }
                            }
                        }
                    }
                    window.display();
                    draw_flag = false;
            }

        }

    }

    return 0;
}

void init() {

    PC = 0x200;
    I = 0;
    opcode = 0;
    delay_timer = 0;
    sound_timer = 0;
    draw_flag = false;

    for (int i = 0; i < HEIGHT_CHIP_8; i++) {
        for (int j = 0; j < WIDTH_CHIP_8; j++) {
            pixel_flag[i][j] = 0;
        }
    }

    for (int i = 0; i < 16; i++) {
        key[i] = 0;
    }

    for (int i = 0; i < 16; i++) {
        registers[i] = 0;
    }

    for (int i = 0; i < 4096; i++) {
        memory[i] = 0;
    }

    for (int i = 0; i < 80; ++i) {

        memory[i] = fontset[i];
    }

    read_rom("test_opcode.ch8");
    //read_rom("ibm.ch8");
    //read_rom("tetris.ch8");
    //read_rom("pong.ch8");
    //read_rom("snake.ch8");
    //read_rom("picture.ch8");
    //read_rom("random.ch8");
    //read_rom("keypad_test.ch8");
    //read_rom("1dcell.ch8");
}

void read_rom(string file_name) {

    ifstream file;
    size_t length;

    file.open(file_name);
    bool found = file.good();
    file.close();

    uint8_t* buffer;

    if (!found) {

        cerr << "File not found: " << file_name << endl;

        system("PAUSE");
        exit(EXIT_FAILURE);
    }

    else {

        file.open(file_name, ios::binary);

        if (!file) {

            file.close();

            cerr << "Could not open file: " << file_name << endl;

            system("PAUSE");
            exit(EXIT_FAILURE);
        }

        cout << "File opened successfully: " << file_name << endl;

        // get length of file
        file.seekg(0, std::ios::end);
        length = file.tellg();
        file.seekg(0, std::ios::beg);

        buffer = new uint8_t[length];

        //read file
        file.read((char*)buffer, length);

        file.close();
    }

    /*unsigned short opcode;

    for (int i = 0; i <= (length - 2); i += 2) {

        cout << i << " ";

        opcode = 0;

        opcode = (((opcode | buffer[i]) << 8) | buffer[i + 1]);

        cout << "0x" << hex << uppercase << opcode << endl;
    }
    */

	// Copy buffer to Chip8 memory
	for (int i = 0; i < length; ++i)
			memory[i + 512] = buffer[i];

    delete[] buffer;
}

void emulate_cycle() {
    
	opcode = memory[PC] << 8 | memory[PC + 1];

	// Process opcode
	switch (opcode & 0xF000)
	{
	case 0x0000:
		switch (opcode & 0x000F)
		{
		case 0x0000: // 0x00E0: Clears the screen
			for (int i = 0; i < HEIGHT_CHIP_8; i++) {
				for (int j = 0; j < WIDTH_CHIP_8; j++) {
					pixel_flag[i][j] = 0;
				}
			}
			draw_flag = true;
			PC += 2;
			if (print_flag == true)
				cout << hex << uppercase << opcode << ": CLS" << endl;
			break;

		case 0x000E: // 0x00EE: Returns from subroutine
			PC = s.pop();		// 16 levels of stack, decrease stack pointer to prevent overwrite
			// Put the stored return address from the stack back into the program counter					
			PC += 2;		// Don't forget to increase the program counter!
			if (print_flag == true)
				cout << hex << uppercase << opcode << ": RET" << endl;
			break;

		default:
			printf("Unknown opcode [0x0000]: 0x%X\n", opcode);
		}
		break;

	case 0x1000: // 0x1NNN: Jumps to address NNN
		PC = opcode & 0x0FFF;
		if (print_flag == true)
			cout << hex << uppercase << opcode << ": JMP 0x" << hex << uppercase << (opcode & 0x0FFF) << endl;
		break;

	case 0x2000: // 0x2NNN: Calls subroutine at NNN.
		s.push(PC);			// Store current address in stack
				// Increment stack pointer
		PC = opcode & 0x0FFF;	// Set the program counter to the address at NNN
		if (print_flag == true)
			cout << hex << uppercase << opcode << ": CALL " << hex << uppercase << (opcode & 0x0FFF) << "." << endl;
		break;

	case 0x3000: // 0x3XNN: Skips the next instruction if VX equals NN
		if (registers[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF))
			PC += 4;
		else
			PC += 2;
		if (print_flag == true)
			cout << hex << uppercase << opcode << ": SE " << "V" << hex << uppercase << ((opcode & 0x0F00) >> 8) << " " << hex << uppercase << (opcode & 0x00FF) << endl;
		break;

	case 0x4000: // 0x4XNN: Skips the next instruction if VX doesn't equal NN
		if (registers[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF))
			PC += 4;
		else
			PC += 2;
		if (print_flag == true)
			cout << hex << uppercase << opcode << ": SNE " << "V" << hex << uppercase << (opcode & 0x0F00) << " " << hex << uppercase << (opcode & 0x00FF) << endl;
		break;

	case 0x5000: // 0x5XY0: Skips the next instruction if VX equals VY.
		if (registers[(opcode & 0x0F00) >> 8] == registers[(opcode & 0x00F0) >> 4])
			PC += 4;
		else
			PC += 2;
		if (print_flag == true)
			cout << hex << uppercase << opcode << ": SE " << "V" << hex << uppercase << (opcode & 0x0F00) << " " << "V" << hex << uppercase << (opcode & 0x00F0) << endl;
		break;

	case 0x6000: // 0x6XNN: Sets VX to NN.
		registers[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
		PC += 2;
		if (print_flag == true)
			cout << hex << uppercase << opcode << ": LD " << "V" << hex << uppercase << ((opcode & 0x0F00) >> 8) << " " << hex << uppercase << (opcode & 0x00FF) << "." << endl;
		break;

	case 0x7000: // 0x7XNN: Adds NN to VX.
		registers[(opcode & 0x0F00) >> 8] += opcode & 0x00FF;
		PC += 2;
		if (print_flag == true)
			cout << hex << uppercase << opcode << ": ADD " << "V" << hex << uppercase << (opcode & 0x0F00) << " " << hex << uppercase << (opcode & 0x00FF) << endl;
		break;

	case 0x8000:
		switch (opcode & 0x000F)
		{
		case 0x0000: // 0x8XY0: Sets VX to the value of VY
			registers[(opcode & 0x0F00) >> 8] = registers[(opcode & 0x00F0) >> 4];
			PC += 2;
			if (print_flag == true)
				cout << hex << uppercase << opcode << ": LD " << "V" << hex << uppercase << (opcode & 0x0F00) << " " "V" << hex << uppercase << (opcode & 0x00F0) << endl;
			break;

		case 0x0001: // 0x8XY1: Sets VX to "VX OR VY"
			registers[(opcode & 0x0F00) >> 8] |= registers[(opcode & 0x00F0) >> 4];
			PC += 2;
			if (print_flag == true)
				cout << hex << uppercase << opcode << ": OR " << "V" << hex << uppercase << (opcode & 0x0F00) << " " << "V" << hex << uppercase << (opcode & 0x00F0) << endl;
			break;

		case 0x0002: // 0x8XY2: Sets VX to "VX AND VY"
			registers[(opcode & 0x0F00) >> 8] &= registers[(opcode & 0x00F0) >> 4];
			PC += 2;
			if (print_flag == true)
				cout << hex << uppercase << opcode << ": AND " << "V" << hex << uppercase << (opcode & 0x0F00) << " " << "V" << hex << uppercase << (opcode & 0x00F0) << endl;
			break;

		case 0x0003: // 0x8XY3: Sets VX to "VX XOR VY"
			registers[(opcode & 0x0F00) >> 8] ^= registers[(opcode & 0x00F0) >> 4];
			PC += 2;
			if (print_flag == true)
				cout << hex << uppercase << opcode << ": XOR " << "V" << hex << uppercase << (opcode & 0x0F00) << " " << "V" << hex << uppercase << (opcode & 0x00F0) << endl;
			break;

		case 0x0004: // 0x8XY4: Adds VY to VX. VF is set to 1 when there's a carry, and to 0 when there isn't					
			if (registers[(opcode & 0x00F0) >> 4] > (0xFF - registers[(opcode & 0x0F00) >> 8]))
				registers[0xF] = 1; //carry
			else
				registers[0xF] = 0;
			registers[(opcode & 0x0F00) >> 8] += registers[(opcode & 0x00F0) >> 4];
			PC += 2;
			if (print_flag == true)
				cout << hex << uppercase << opcode << ": ADD " << "V" << hex << uppercase << (opcode & 0x0F00) << " " << "V" << hex << uppercase << (opcode & 0x00F0) << endl;
			break;

		case 0x0005: // 0x8XY5: VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 when there isn't
			if (registers[(opcode & 0x00F0) >> 4] > registers[(opcode & 0x0F00) >> 8])
				registers[0xF] = 0; // there is a borrow
			else
				registers[0xF] = 1;
			registers[(opcode & 0x0F00) >> 8] -= registers[(opcode & 0x00F0) >> 4];
			PC += 2;
			if (print_flag == true)
				cout << hex << uppercase << opcode << ": SUB " << "V" << hex << uppercase << (opcode & 0x0F00) << " " << "V" << hex << uppercase << (opcode & 0x00F0) << endl;
			break;

		case 0x0006: // 0x8XY6: Shifts VX right by one. VF is set to the value of the least significant bit of VX before the shift
			registers[0xF] = registers[(opcode & 0x0F00) >> 8] & 0x1;
			registers[(opcode & 0x0F00) >> 8] >>= 1;
			PC += 2;
			if (print_flag == true)
				cout << hex << uppercase << opcode << ": SHR " << "V" << hex << uppercase << (opcode & 0x0F00) << " " << "1" << endl;
			break;

		case 0x0007: // 0x8XY7: Sets VX to VY minus VX. VF is set to 0 when there's a borrow, and 1 when there isn't
			if (registers[(opcode & 0x0F00) >> 8] > registers[(opcode & 0x00F0) >> 4])	// VY-VX
				registers[0xF] = 0; // there is a borrow
			else
				registers[0xF] = 1;
			registers[(opcode & 0x0F00) >> 8] = registers[(opcode & 0x00F0) >> 4] - registers[(opcode & 0x0F00) >> 8];
			PC += 2;
			if (print_flag == true)
				cout << hex << uppercase << opcode << ": SUBN " << "V" << hex << uppercase << (opcode & 0x0F00) << " " << "V" << hex << uppercase << (opcode & 0x00F0) << " " << "V" << hex << uppercase << (opcode & 0x0F00) << endl;
			break;

		case 0x000E: // 0x8XYE: Shifts VX left by one. VF is set to the value of the most significant bit of VX before the shift
			registers[0xF] = registers[(opcode & 0x0F00) >> 8] >> 7;
			registers[(opcode & 0x0F00) >> 8] <<= 1;
			PC += 2;
			if (print_flag == true)
				cout << hex << uppercase << opcode << ": SHL " << "V" << hex << uppercase << (opcode & 0x0F00) << " " << "1" << endl;
			break;

		default:
			printf("Unknown opcode [0x8000]: 0x%X\n", opcode);
		}
		break;

	case 0x9000: // 0x9XY0: Skips the next instruction if VX doesn't equal VY
		if (registers[(opcode & 0x0F00) >> 8] != registers[(opcode & 0x00F0) >> 4])
			PC += 4;
		else
			PC += 2;
		if (print_flag == true)
			cout << hex << uppercase << opcode << ": SNE " << "(V" << hex << uppercase << ((opcode & 0x0F00) >> 8) << " " << "V" << hex << uppercase << ((opcode & 0x00F0) >> 4) << ")" << endl;
		break;

	case 0xA000: // ANNN: Sets I to the address NNN
		I = opcode & 0x0FFF;
		PC += 2;
		if (print_flag == true)
			cout << hex << uppercase << opcode << ": LD " << "I " << hex << uppercase << (opcode & 0x0FFF) << endl;
		break;

	case 0xB000: // BNNN: Jumps to the address NNN plus V0
		PC = (opcode & 0x0FFF) + registers[0];
		if (print_flag == true)
			cout << hex << uppercase << opcode << ": JMP " << "PC V" << hex << uppercase << registers[0] << " " << hex << uppercase << (opcode & 0x0FFF) << endl;
		break;

	case 0xC000: // CXNN: Sets VX to a random number and NN
		registers[(opcode & 0x0F00) >> 8] = (rand() % 0xFF) & (opcode & 0x00FF);
		PC += 2;
		if (print_flag == true)
			cout << hex << uppercase << opcode << ": RND " << "V" << hex << uppercase << (opcode & 0x0F00) << " " << "rand() & " << hex << uppercase << (opcode & 0x00FF) << endl;
		break;

	case 0xD000: // DXYN: Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N pixels. 
		// Each row of 8 pixels is read as bit-coded starting from memory location I; 
		// I value doesn't change after the execution of this instruction. 
		// VF is set to 1 if any screen pixels are flipped from set to unset when the sprite is drawn, 
		// and to 0 if that doesn't happen
	{

		uint16_t x = registers[(opcode & 0x0F00) >> 8] % (WIDTH_CHIP_8);
		uint16_t y = registers[(opcode & 0x00F0) >> 4] % (HEIGHT_CHIP_8);
		uint16_t height = opcode & 0x000F;
		uint16_t pixel;

		registers[0xF] = 0;

		for (int yline = 0; yline < height; yline++)
		{
			pixel = memory[I + yline];

			for (int xline = 0; xline < 8; xline++)
			{
				if ((pixel & (0x80 >> xline)) != 0)
				{
					if (pixel_flag[y + yline][x + xline] == 1)
						registers[0xF] = 1;

					pixel_flag[y + yline][x + xline] ^= 1;
				}
			}
		}

		draw_flag = true;
		PC += 2;
		if (print_flag == true)
			cout << hex << uppercase << opcode << ": DRW " << "V" << ((opcode & 0x0F00) >> 8) << " V" << hex << uppercase << ((opcode & 0x00F0) >> 4) << " N " << hex << uppercase << (opcode & 0x00FF) << endl;
	}
	break;

	case 0xE000:
		switch (opcode & 0x00FF)
		{
		case 0x009E: // EX9E: Skips the next instruction if the key stored in VX is pressed
			if (key[registers[(opcode & 0x0F00) >> 8]] != 0)
				PC += 4;
			else
				PC += 2;
			if (print_flag == true)
				cout << hex << uppercase << opcode << ": SKP " << "key() == V" << hex << uppercase << ((opcode & 0x0F00) >> 8) << endl;
			break;

		case 0x00A1: // EXA1: Skips the next instruction if the key stored in VX isn't pressed
			if (key[registers[(opcode & 0x0F00) >> 8]] == 0)
				PC += 4;
			else
				PC += 2;
			if (print_flag == true)
				cout << hex << uppercase << opcode << ": SKNP " << "key() != V" << hex << uppercase << ((opcode & 0x0F00) >> 8) << endl;;
			break;

		default:
			printf("Unknown opcode [0xE000]: 0x%X\n", opcode);
		}
		break;

	case 0xF000:
		switch (opcode & 0x00FF)
		{
		case 0x0007: // FX07: Sets VX to the value of the delay timer
			registers[(opcode & 0x0F00) >> 8] = delay_timer;
			PC += 2;
			if (print_flag == true)
				cout << hex << uppercase << opcode << ": LD " << "V" << hex << uppercase << (opcode & 0x0F00) << " = get_delay()" << endl;
			break;

		case 0x000A: // FX0A: A key press is awaited, and then stored in VX		
		{
			bool keyPress = false;

			for (int i = 0; i < 16; ++i)
			{
				if (key[i] != 0)
				{
					registers[(opcode & 0x0F00) >> 8] = i;
					keyPress = true;
				}
			}

			// If we didn't received a keypress, skip this cycle and try again.
			if (!keyPress)
				return;

			PC += 2;
			if (print_flag == true)
				cout << hex << uppercase << opcode << ": LD " << "V" << hex << uppercase << (opcode & 0x0F00) << " = get_key()" << endl;
		}
		break;

		case 0x0015: // FX15: Sets the delay timer to VX
			delay_timer = registers[(opcode & 0x0F00) >> 8];
			PC += 2;
			if (print_flag == true)
				cout << hex << uppercase << opcode << ": LD " << "delay_timer(V" << hex << uppercase << (opcode & 0x0F00) << ")" << endl;
			break;

		case 0x0018: // FX18: Sets the sound timer to VX
			sound_timer = registers[(opcode & 0x0F00) >> 8];
			PC += 2;
			if (print_flag == true)
				cout << hex << uppercase << opcode << ": LD " << "sound_timer(V" << hex << uppercase << (opcode & 0x0F00) << ")" << endl;
			break;

		case 0x001E: // FX1E: Adds VX to I
			if (I + registers[(opcode & 0x0F00) >> 8] > 0xFFF)	// VF is set to 1 when range overflow (I+VX>0xFFF), and 0 when there isn't.
				registers[0xF] = 1;
			else
				registers[0xF] = 0;
			I += registers[(opcode & 0x0F00) >> 8];
			PC += 2;
			if (print_flag == true)
				cout << hex << uppercase << opcode << ": ADD " << "I += V" << hex << uppercase << (opcode & 0x0F00) << endl;
			break;

		case 0x0029: // FX29: Sets I to the location of the sprite for the character in VX. Characters 0-F (in hexadecimal) are represented by a 4x5 font
			I = registers[(opcode & 0x0F00) >> 8] * 0x5;
			PC += 2;
			if (print_flag == true)
				cout << hex << uppercase << opcode << ": LD " << "I = sprite_addr[V" << hex << uppercase << (opcode & 0x0F00) << "]" << endl;
			break;

		case 0x0033: // FX33: Stores the Binary-coded decimal representation of VX at the addresses I, I plus 1, and I plus 2
			memory[I] = registers[(opcode & 0x0F00) >> 8] / 100;
			memory[I + 1] = (registers[(opcode & 0x0F00) >> 8] / 10) % 10;
			memory[I + 2] = (registers[(opcode & 0x0F00) >> 8] % 100) % 10;
			PC += 2;
			if (print_flag == true)
				cout << hex << uppercase << opcode << ": LD " << "set_BCD(Vx), *(I + 0) = BCD(3), *(I + 1) = BCD(2), *(I + 2) = BCD(1)" << endl;
			break;

		case 0x0055: // FX55: Stores V0 to VX in memory starting at address I					
			for (int i = 0; i <= ((opcode & 0x0F00) >> 8); ++i)
				memory[I + i] = registers[i];

			// On the original interpreter, when the operation is done, I = I + X + 1.
			I += ((opcode & 0x0F00) >> 8) + 1;
			PC += 2;
			if (print_flag == true)
				cout << hex << uppercase << opcode << ": LD " << "reg_dump(Vx, &I)" << endl;
			break;

		case 0x0065: // FX65: Fills V0 to VX with values from memory starting at address I					
			for (int i = 0; i <= ((opcode & 0x0F00) >> 8); ++i)
				registers[i] = memory[I + i];

			// On the original interpreter, when the operation is done, I = I + X + 1.
			I += ((opcode & 0x0F00) >> 8) + 1;
			PC += 2;
			if (print_flag == true)
				cout << hex << uppercase << opcode << ": LD " << "reg_load(Vx, &I)" << endl;
			break;

		default:
			printf("Unknown opcode [0xF000]: 0x%X\n", opcode);
		}
		break;

	default:
		printf("Unknown opcode: 0x%X\n", opcode);
	}

	// Update timers
	if (delay_timer > 0)
		--delay_timer;

	if (sound_timer > 0)
	{
		if (sound_timer == 1)
			printf("BEEP!\n");
		--sound_timer;
	}
}