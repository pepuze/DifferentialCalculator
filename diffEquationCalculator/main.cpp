#include <iostream>
#include <string>
#include <iomanip>
#include <fstream>
#include <SFML/Graphics.hpp>
#include "functions.h"

int main() {
	while (1) {
		std::string func, temp;
		double leftBound, rightBound, step, y0;
		std::cout << "dy/dx = f(x,y)\nInput function f(x,y):\n";
		std::getline(std::cin, func);

		for (char& c : func) c = tolower(c);
		if (!checkFuncSyntax(func)) {
			std::cin.ignore();
			continue;
		}

		Function function(func);
		function.printPostfix();
		std::cout << "\nInput grid limits [a, b] (a > 0):\n";

		std::cout << "a = ";
		std::cin >> temp;
		try {
			leftBound = stod(temp);
			if (leftBound < 0) throw(1);
		}
		catch (...) {
			std::cout << "Invalid input\n\n";
			std::cin.ignore();
		}

		std::cout << "b = ";
		std::cin >> temp;
		try {
			rightBound = stod(temp);
			if (rightBound < leftBound) throw(1);
		}
		catch (...) {
			std::cout << "Invalid input\n\n";
			std::cin.ignore();
		}
		
		std::cout << "Input y0:\n";
		std::cout << "y0 = ";
		std::cin >> temp;
		try {
			y0 = stod(temp);
		}
		catch (...) {
			std::cout << "Invalid input\n\n";
			std::cin.ignore();
		}

		std::cout << "Input grid's step:\n";
		std::cout << "step = ";
		std::cin >> temp;
		try {
			step = stod(temp);
			if (step <= 0) throw(1);
		}
		catch (...) {
			std::cout << "Invalid input\n\n";
			std::cin.ignore();
		}
		std::vector<std::pair<double, double>> solutionEuler = function.EulerSolution(leftBound, rightBound, y0, step);
		std::vector<std::pair<double, double>> solutionRungeKutta = function.RungeKuttaSolution(leftBound, rightBound, y0, step);

		std::ofstream outputEuler("Euler_Method.txt"), outputRungeKutta("Runge_Kutta_Method.txt");
		outputEuler << "dy/dx = " << func << "\n" << "Partial solution (Euler method):\n\n";
		printSolution(solutionEuler, outputEuler);
		outputRungeKutta << "dy/dx = " << func << "\n" << "Partial solution (Runge-Kutta method):\n\n";
		printSolution(solutionRungeKutta, outputRungeKutta);

		outputEuler.close();
		outputRungeKutta.close();

		const unsigned int nPoints = (rightBound == leftBound) ? 1 : std::max((int)ceil((rightBound - leftBound) / step) + 1, 2);

		std::cout << "Partial solutions:\nEuler method:\n";
		std::cout << std::setw(4) << "i" << std::setw(20) << "x" << std::setw(20) << "y\n";
		std::cout << std::setw(4) << 0 << std::setw(20) << std::fixed << std::setprecision(6) << solutionEuler[0].first << std::setw(20) << std::fixed << std::setprecision(6) << solutionEuler[0].second << "\n";
		std::cout << std::setw(4) << nPoints-1 << std::setw(20) << std::fixed << std::setprecision(6) << solutionEuler[nPoints - 1].first << std::setw(20) << std::fixed << std::setprecision(6) << solutionEuler[nPoints - 1].second << "\n";
		std::cout << "Complete partial solution is in Euler_Method.txt\n\n";
		
		std::cout << "Runge-Kutta method:\n";
		std::cout << std::setw(4) << "i" << std::setw(20) << "x" << std::setw(20) << "y\n";
		std::cout << std::setw(4) << 0 << std::setw(20) << std::fixed << std::setprecision(6) << solutionRungeKutta[0].first << std::setw(20) << std::fixed << std::setprecision(6) << solutionRungeKutta[0].second << "\n";
		std::cout << std::setw(4) << nPoints - 1 << std::setw(20) << std::fixed << std::setprecision(6) << solutionRungeKutta[nPoints - 1].first << std::setw(20) << std::fixed << std::setprecision(6) << solutionRungeKutta[nPoints - 1].second << "\n";
		std::cout << "Complete partial solution is in Runge_Kutta_Method.txt\n\n";

		std::cout << "1 - toggle Euler method grid  (red)\n";
		std::cout << "1 + LControl - toggle Euler method grid points\n";
		std::cout << "2 - toggle Runge-Kutta method grid (green)\n";
		std::cout << "2 + LControl - toggle Runge-Kutta method grid points\n";
		std::cout << "I - zoom in\n";
		std::cout << "O - zoom out\n";
		std::cout << "Use left mouse button to move view\n";
		std::cout << "Close graph window to continue...\n\n";

		sf::ContextSettings settings;
		settings.antialiasingLevel = 8;
		sf::RenderWindow window(sf::VideoMode(1024, 768), "Graph", sf::Style::Default, settings);
		sf::View view(sf::Vector2f(0, 0), sf::Vector2f(0, 0));
		view.setSize(1024, 768);
		view.setCenter(sf::Vector2f(leftBound, -1.0f * y0));

		sf::VertexArray oX(sf::Lines, 2); 
		sf::VertexArray oY(sf::Lines, 2);
		sf::VertexArray graphEuler(sf::LinesStrip, nPoints);
		sf::VertexArray graphRungeKutta(sf::LinesStrip, nPoints);

		//Euler solution grid
		for (unsigned int i = 0; i < nPoints; ++i) {
			graphEuler[i].color = sf::Color::Red;
			graphEuler[i].position = sf::Vector2f(solutionEuler[i].first, -1.0f * solutionEuler[i].second);
		}

		for (unsigned int i = 0; i < nPoints; ++i) {
			graphRungeKutta[i].color = sf::Color::Green;
			graphRungeKutta[i].position = sf::Vector2f(solutionRungeKutta[i].first, -1.0f * solutionRungeKutta[i].second);
		}

		oX[0].color = oX[1].color = oY[0].color = oY[1].color = sf::Color::Black;
		oX[0].position = sf::Vector2f(10000000.f, 0.f);
		oX[1].position = sf::Vector2f(0.f, 0.f);
		oY[0].position = sf::Vector2f(0.f, 10000000.f);
		oY[1].position = sf::Vector2f(0.f, -10000000.f);

		bool lmbPressed = 0, toggleEuler = 1, toggleRungeKutta = 1, toggleEulerPoints = 1, toggleRungeKuttaPoints = 1;
		sf::Vector2f oldPos;
		double accZoom = 0.0078125, zoom = 0.0078125;
		view.zoom(zoom);
		window.setView(view);
		while (window.isOpen())
		{
			sf::Event event;
			while (window.pollEvent(event))
			{
				if (event.type == sf::Event::Closed)
					window.close();

				else if (event.type == sf::Event::KeyPressed) {
					switch (event.key.code) {
					case sf::Keyboard::I:
						zoom = .5f;
						accZoom *= zoom;
						view.zoom(zoom);
						window.setView(view);
						break;

					case sf::Keyboard::O:
						zoom = 2.f;
						accZoom *= zoom;
						view.zoom(zoom);
						window.setView(view);
						break;

					case sf::Keyboard::Num1:
						if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)) toggleEulerPoints = !toggleEulerPoints;
						else toggleEuler = !toggleEuler;
						break;
					
					case sf::Keyboard::Num2:
						if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)) toggleRungeKuttaPoints = !toggleRungeKuttaPoints;
						else toggleRungeKutta = !toggleRungeKutta;
						break;


					}
				}

				else if (event.type == sf::Event::MouseButtonPressed) {
					if (event.mouseButton.button == sf::Mouse::Left) {
						lmbPressed = 1;
						oldPos = sf::Vector2f(sf::Mouse::getPosition());
					}
				}

				else if (event.type == sf::Event::MouseButtonReleased) {
					if (event.mouseButton.button == sf::Mouse::Left) {
						lmbPressed = 0;
					}
				}

				else if (event.type == sf::Event::MouseMoved) {
					if (lmbPressed) {
						const sf::Vector2f newPos = sf::Vector2f(sf::Mouse::getPosition());
						sf::Vector2f dPos = oldPos - newPos;
						dPos.x *= accZoom;
						dPos.y *= accZoom;
						view.move(dPos);
						window.setView(view);
						oldPos = newPos;
					}
				}
			}

			window.clear(sf::Color::White);
			window.draw(oX);
			window.draw(oY);
			if (toggleEuler) {
				window.draw(graphEuler);
				if (toggleEulerPoints) {
					for (unsigned int i = 0; i < nPoints; ++i) {
						sf::CircleShape point(2.f * accZoom); point.setOrigin(point.getRadius(), point.getRadius()); point.setFillColor(sf::Color::Red); point.setPosition(graphEuler[i].position);
						window.draw(point);
					}
				}
			}
			if (toggleRungeKutta) {
				window.draw(graphRungeKutta);
				if (toggleRungeKuttaPoints) {
					for (unsigned int i = 0; i < nPoints; ++i) {
						sf::CircleShape point(2.f * accZoom); point.setOrigin(point.getRadius(), point.getRadius()); point.setFillColor(sf::Color::Green); point.setPosition(graphRungeKutta[i].position);
						window.draw(point);
					}
				}
			}
			window.display();
		}

		std::cin.ignore();
	}
	return 0;
}