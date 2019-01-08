#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>

int main()
{
    //sf::RenderWindow window(sf::VideoMode(200, 200), "SFML works!");
    //sf::CircleShape shape (100.f);
    //shape.setFillColor(sf::Color::Green);

    sf::Window window(sf::VideoMode(800, 600), "OpenGL", sf::Style::Default, sf::ContextSettings(64));
    window.setVerticalSyncEnabled(true);

    window.setActive(true);

    bool running = true;
    while (running)
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed){
		    running = false;
            }
            else if (event.type == sf::Event::Resized){
                glViewport(0, 0, event.size.width, event.size.height);
            }
        }


        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //window.clear();
        // window.draw(shape);
        window.display();
    }

    return 0;
}
