// CHIP8.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "chip8.h"
#include "main.h"

#include <iostream>
#include <SFML/Graphics.hpp>


using namespace std;
using namespace sf;

chip8 myChip8;
short windowScale = 16;
RenderWindow window;


void setupGraphics()
{
	window.create(VideoMode(64 * windowScale, 32 * windowScale), "SFML works!");

}

void setupInput()
{

}

void drawPixels() 
{
	system("cls");

	int scaleX = 1;
	int scaleY = 1;
	sf::RectangleShape pixel;
	pixel.setFillColor(sf::Color::White);
	pixel.setSize(sf::Vector2f(1,1));
	string s = "";
	for (int i = 0; i < myChip8.gfxSize; i++)
	{
		
		//vertices[i].color = sf::Color::White;
		int x = i % 64;
		int y = i / 64;
		pixel.setPosition(x, y);
		//vertices[i].position = sf::Vector2f(x, y);
		if(myChip8.gfx[i] == 1)
			window.draw(pixel);

		//strcat(s, myChip8.gfx[i]);
		s += myChip8.gfx[i];
		//printf("%d", myChip8.gfx[i]);
		if (i % 64 == 0)
			s += "\n";
			//printf("\n");
	}
	//cout << s << endl;
	myChip8.debugRender();
	//window.draw(vertices, myChip8.gfxSize, sf::Quads);
}

int main(int argc, char* argv[])
{
	char basePath[255] = "";
	cout << _fullpath(basePath, argv[0], sizeof(basePath)) << "\n";

	setupGraphics();
	setupInput();

	myChip8.initialize();
	myChip8.loadGame("c:/games/PONG");

	while (window.isOpen())
	{

		myChip8.emulateCycle();

		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed)
			{
				window.close();
			}
		}

		if (myChip8.drawFlag) {
			window.clear();
			drawPixels();
			window.display();
			//myChip8.drawFlag = false;
		}

		myChip8.setKeys();
	}

    return 0;
}
