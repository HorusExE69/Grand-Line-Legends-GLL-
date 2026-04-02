#include <SFML/Graphics.hpp>
#include <iostream>

int main()
{
    sf::RenderWindow window(sf::VideoMode(400, 300), "Test SFML");

    sf::Font font;
    if (!font.loadFromFile("assets/arial.ttf")) {
        std::cerr << "Erreur chargement font !" << std::endl;
        return 1;
    }

    sf::Text text("SFML fonctionne !", font, 20);
    text.setFillColor(sf::Color::White);
    text.setPosition(50, 130);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear(sf::Color::Black);
        window.draw(text);
        window.display();
    }

    return 0;
}

