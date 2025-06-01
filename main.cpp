#define _USE_MATH_DEFINES
#include "orbit.h"
#include <cmath>
#include <iostream>
#include <SFML/Graphics.hpp>
#include <string.h>
#include <vector>
#include <iomanip>
#include <sstream>
#define G (4 * M_PI * M_PI) //Gravitational constant

const unsigned short screen_type_resolution[4][2] = { {3840, 2160}, {1920, 1080}, {1366, 768} ,{1680, 1050} };
unsigned short system_number = 0;
int planet_number = 0;
bool show_help = 0;
unsigned short last_system = system_number;
sf::Font font("Nasalization.otf");
double stars[1000][3];
sf::RectangleShape ui_elements[6];
bool key_repeat = false;
sf::RenderWindow window;


sf::Clock sim_clock; // counting fps and making worse computers run the simulation smoother
unsigned short frameCount = 0;
float fpsTimer = 0.0f;
unsigned short fps = 1;
float time_accumulator = 0.0f;

//// i know i could delete those static_casts as you said on the lesson but im too lazy to do so, i am not touching this project anymore
//// things could be done better but i did it my way
//// change up the simulation variables to your pc's liking
//// have fun

//Simulation Variables
const unsigned short screen_type = 0;           // changes the resolution of the screen  0 - 3840 2160 , 1 - 1920 1080, 2 - 1366 768, 3 - 1680 1050
const bool fullscreen = true;                   // if true makes the fullscreen
const bool shit_computer = false;                // disables the creating of asteroids and enables making breaks in simulating bodies based on simulation_time_delay
const float simulation_time_delay = 0;          // value of this variable delays the simulation by an amount of frames (max 60) for bad pcs
const int max_fps = 60;                         // sets the max amount of fps

int WIDTH = static_cast<int>(screen_type_resolution[screen_type][0]);
int HEIGHT = static_cast<int>(screen_type_resolution[screen_type][1]);
sf::Vector2f pos = { static_cast<float>(-WIDTH / 2), static_cast<float>((HEIGHT / 10) * 4) };  // a helping vector for drawing the ui

//Drawing text with a large double value
template <typename T>
static void DrawText(sf::Text text, std::string word, T value, std::string word2, bool scientific) {
    if (scientific) {
        std::ostringstream oss;
        oss << std::scientific << std::setprecision(6) << value;

        text.setString(word + oss.str() + word2);
    }
    else {
        text.setString(word + std::to_string(value) + word2);
    }
    window.draw(text);
}

//Drawing text with a int value
static void DrawText(sf::Text text, std::string word, std::string value) {
    text.setString(word + value);
    window.draw(text);
}

//Drawing text with a int value and a text after it
static void DrawText(sf::Text text, std::string word, Vector2d value, std::string word2) {
    std::ostringstream val_x;
    val_x << std::scientific << std::setprecision(6) << value.x; 
    std::ostringstream val_y;
    val_y << std::scientific << std::setprecision(6) << value.y;
    text.setString(word + val_x.str() + ", " + val_y.str() + word2);
    window.draw(text);
}

//Setting text with a position, size and color  
static sf::Text SetText(Vector2d pos, unsigned short size, sf::Color color) {
    sf::Text text(font);
    text.setString("");
    text.setCharacterSize(size);
    text.setFillColor(color);
    text.setPosition(sf::Vector2f(static_cast<float>(pos.x), static_cast<float>(pos.y)));
    return text;
}

//Scaling the value by the display resolution
static double ScaleByDisplay(double scale) {
    if (screen_type == 1) 
        scale /= 2;
    else if (screen_type == 2)
        scale /= 2.826;

    else if (screen_type == 3)
        scale /= 2.286;
    return scale;
}

//Creating stars in the background
static void CreateStars() {
	for (int i = 0; i < 1000; ++i) {
        stars[i][0] = -screen_type_resolution[screen_type][0] / 2 + rand() % (screen_type_resolution[screen_type][0] / 2 + screen_type_resolution[screen_type][0] / 2 + 1);
        stars[i][1] = -screen_type_resolution[screen_type][1] / 2 + rand() % (screen_type_resolution[screen_type][1] / 2 + screen_type_resolution[screen_type][1] / 2 + 1);
        stars[i][2] = 0.3 + (1 - 0.3) * (rand() / ((double)RAND_MAX));
	}
}

//Drawing stars in the background
static void DrawStars() {
    for (int i = 0; i < 1000; ++i) {
        sf::CircleShape star(static_cast<float>(stars[i][2]));
        star.setFillColor(sf::Color(255, 255, 255, 130));
        sf::Vector2f screen_position(static_cast<float>(stars[i][0]), static_cast<float>(stars[i][1]));
        star.setPosition(screen_position);
        window.draw(star);
    }
}

//Creating the ui
static void CreateUi() {
    int width = WIDTH;
    int height = HEIGHT / 10;
    sf::Vector2f pos = {static_cast<float>(-screen_type_resolution[screen_type][0] / 2), static_cast<float>((screen_type_resolution[screen_type][1] / 10) * 4 )};

    ui_elements[0] = sf::RectangleShape(sf::Vector2f(width, height)),
    ui_elements[1] = sf::RectangleShape(sf::Vector2f(width / 100, height)),
    ui_elements[2] = sf::RectangleShape(sf::Vector2f(width / 100, height)),
    ui_elements[3] = sf::RectangleShape(sf::Vector2f(width / 100, height * 0.33)),
    ui_elements[4] = sf::RectangleShape(sf::Vector2f(width * 0.34, width / 100));
    ui_elements[5] = sf::RectangleShape(sf::Vector2f(width, width / 200)),

	ui_elements[0].setFillColor(sf::Color(27, 34, 42));
	for (int i = 1; i < 6; ++i) {
		ui_elements[i].setFillColor(sf::Color(50, 56, 70));
	}
    ui_elements[0].setPosition(sf::Vector2f(pos.x, pos.y));
    ui_elements[1].setPosition(sf::Vector2f(pos.x * 0.35, pos.y));
    ui_elements[2].setPosition(sf::Vector2f(pos.x * -0.35 - width / 100, pos.y));
    ui_elements[3].setPosition(sf::Vector2f(-width / 200, height * 4.66));
    ui_elements[4].setPosition(sf::Vector2f(pos.x * 0.35 + width / 100, height * 4.66 - width / 100));
    ui_elements[5].setPosition(sf::Vector2f(pos.x, pos.y - width / 200));

    
}

//Drawing ui
static void DrawUi() {
    for (int i = 0; i < 6; ++i) {
        window.draw(ui_elements[i]);
    }
}

//Function to show some help text
static void ShowHelp(sf::Text* texts) {
    DrawText(texts[15], "Press arrows to change body", ""); //arrows
    DrawText(texts[16], "Press numbers to change system", ""); //numbers
    DrawText(texts[17], "Press shift to change key repeat", ""); //shift
    DrawText(texts[18], "Press escape to exit", ""); //escape
}


int main()
{
    if(fullscreen) window.create(sf::VideoMode({ static_cast<unsigned int>(screen_type_resolution[screen_type][0]), static_cast<unsigned int>(screen_type_resolution[screen_type][0]) }), "planeciory", sf::State::Fullscreen);
    else window.create(sf::VideoMode({ static_cast<unsigned int>(screen_type_resolution[screen_type][0]), static_cast<unsigned int>(screen_type_resolution[screen_type][0]) }), "planeciory");


    sf::View view;

    view.setSize(sf::Vector2f(WIDTH, HEIGHT));
    view.setCenter(sf::Vector2f(0, 0));
    window.setFramerateLimit(max_fps);
    window.setKeyRepeatEnabled(key_repeat);

    double dt = 0.001; // fraction of a year, but fps are speeding this up

    const int solarSystemCount = 9;
    const int earthSystemCount = 2;
    const int marsSystemCount = 3;
    const int jupiterSystemCount = 11;
    const int saturnSystemCount = 11;
    const int uranusSystemCount = 11;
    const int neptuneSystemCount = 11;

    //the solar planets are a dynamic array due the fact that i was going to add a "in game" adding celestial bodies feature
    Planet* solarPlanets = new Planet[solarSystemCount]{
        Planet(Vector2d(0, 0), 6.963e8, 1.989e30, 0.0, 255, 255, 0, 1, ScaleByDisplay(8e3), "sun"),                                 // sun
        Planet(Vector2d(5.791e10, 0), 2.4397e6, 3.3022e23, 0.2056, 255, 255, 50, ScaleByDisplay(1.3e2), ScaleByDisplay(1e5), "mercury"),   // mercury
        Planet(Vector2d(1.082e11, 0), 6.0518e6, 4.8673e24, 0.0068, 227, 115, 75, ScaleByDisplay(1.3e2), ScaleByDisplay(1e5), "venus"),   // venus
        Planet(Vector2d(1.496e11, 0), 6.371e6, 5.972e24, 0.0167, 0, 102, 204, ScaleByDisplay(1.3e2), ScaleByDisplay(1e5), "earth"),      // earth
        Planet(Vector2d(2.279e11, 0), 3.3895e6, 6.417e23, 0.0934, 188, 39, 50, ScaleByDisplay(1.3e2), ScaleByDisplay(1e5), "mars"),     // mars
        Planet(Vector2d(7.785e11, 0), 6.9911e7, 1.898e27, 0.0484, 219, 145, 83, ScaleByDisplay(6.5e1), ScaleByDisplay(4e4), "jupiter"),     // jupiter
        Planet(Vector2d(1.4335e12, 0), 5.8232e7, 5.6834e26, 0.0539, 210, 170, 109, ScaleByDisplay(5.1e1), ScaleByDisplay(4e4), "saturn"), // saturn
        Planet(Vector2d(2.8725e12, 0), 2.5362e7, 8.681e25, 0.0473, 79, 208, 231, ScaleByDisplay(3.6e1), ScaleByDisplay(4e4), "uranus"),   // uranus
        Planet(Vector2d(4.4951e12, 0), 2.4622e7, 1.024e26, 0.0086, 62, 84, 232, ScaleByDisplay(3e1), ScaleByDisplay(4e4), "neptune")     // neptune
    };
    CelestialSystem solarSystem(false);
    for (int i = 0; i < solarSystemCount; i++) {
        solarSystem.AddPlanet(solarPlanets[i]);
    }

    Planet* earthPlanets = new Planet[earthSystemCount]{
        solarPlanets[3],  // earth
        Planet(Vector2d(3.844e8, 0), 1.737e6, 7.347e22, 0.0549, 190, 190, 190, ScaleByDisplay(3e5), ScaleByDisplay(5e5), "moon")     // moon
    };
    Planet* marsPlanets = new Planet[marsSystemCount]{
        solarPlanets[4],  // mars
        Planet(Vector2d(9.378e6, 0), 1.1267e4, 1.0659e16, 0.0151, 169, 169, 169, ScaleByDisplay(9e5), ScaleByDisplay(4e7), "phobos"),      // phobos
        Planet(Vector2d(2.3459e7, 0), 6.2e3, 1.4762e15, 0.0002, 169, 169, 169, ScaleByDisplay(9e5), ScaleByDisplay(4e7), "deimos")        // deimos
    };
    Planet* jupiterPlanets = new Planet[jupiterSystemCount]{
        solarPlanets[5],  // jupiter
        Planet(Vector2d(1.0704e9, 0), 2.6341e6, 1.4819e23, 0.0013, 220, 220, 220, ScaleByDisplay(9e4), ScaleByDisplay(5e5), "ganymede"),    // ganymede
        Planet(Vector2d(1.8827e9, 0), 2.4103e6, 1.0759e23, 0.0074, 169, 169, 169, ScaleByDisplay(9e4), ScaleByDisplay(5e5), "callisto"),    // callisto
        Planet(Vector2d(4.217e8, 0), 1.8216e6, 8.9319e22, 0.0041, 255, 165, 0, ScaleByDisplay(9e4), ScaleByDisplay(5e5), "io"),             // io
        Planet(Vector2d(6.711e8, 0), 1.5608e6, 4.7998e22, 0.0094, 255, 255, 240, ScaleByDisplay(9e4), ScaleByDisplay(5e5), "europa"),       // europa
        Planet(Vector2d(1.81e8, 0), 8.3e4, 2.08e18, 0.0032, 150, 150, 150, ScaleByDisplay(1.3e5), ScaleByDisplay(2e7), "amalthea"),         // amalthea
        Planet(Vector2d(2.22e8, 0), 4.9e4, 4.30e17, 0.0175, 169, 169, 169, ScaleByDisplay(1.3e5), ScaleByDisplay(2e7), "thebe"),            // thebe
        Planet(Vector2d(7.50e8, 0), 4.0e3, 7.00e15, 0.2420, 160, 82, 45, ScaleByDisplay(9e4), ScaleByDisplay(8e7), "themisto"),             // themisto
        Planet(Vector2d(1.28e8, 0), 2.10e4, 3.60e16, 0.0012, 184, 134, 11, ScaleByDisplay(1.3e5), ScaleByDisplay(8e7), "metis"),            // metis
        Planet(Vector2d(2.12e9, 0), 1.4e4, 3.0e16, 0.2435, 150, 130, 110, ScaleByDisplay(9e4), ScaleByDisplay(8e7), "ananke"),              // ananke
        Planet(Vector2d(2.34e9, 0), 2.3e4, 1.2e17, 0.2533, 160, 160, 140, ScaleByDisplay(9e4), ScaleByDisplay(8e7), "carme")                // carme
    };
    Planet* saturnPlanets = new Planet[saturnSystemCount]{
        solarPlanets[6],  // saturn
        Planet(Vector2d(1.222e9, 0), 2.5755e6, 1.3452e23, 0.0288, 210, 180, 140, ScaleByDisplay(3e5), ScaleByDisplay(5e6), "titan"),        // titan
        Planet(Vector2d(5.271e8, 0), 7.645e5, 2.3065e21, 0.0013, 169, 169, 169, ScaleByDisplay(3e5), ScaleByDisplay(5e6), "rhea"),          // rhea
        Planet(Vector2d(2.95e8, 0), 1.5e4, 8.0e16, 0.0005, 169, 169, 169, ScaleByDisplay(3e5), ScaleByDisplay(5e7), "calypso"),             // calypso
        Planet(Vector2d(3.774e8, 0), 5.614e5, 1.0955e21, 0.0022, 192, 192, 192, ScaleByDisplay(3e5), ScaleByDisplay(5e6), "dione"),         // dione
        Planet(Vector2d(2.94619e8, 0), 5.311e5, 6.1745e20, 0.0001, 255, 255, 255, ScaleByDisplay(3e5), ScaleByDisplay(5e6), "tethys"),      // tethys
        Planet(Vector2d(2.38e8, 0), 2.521e5, 1.0802e20, 0.0045, 200, 200, 255, ScaleByDisplay(3e5), ScaleByDisplay(5e6), "enceladus"),      // enceladus
        Planet(Vector2d(1.8552e8, 0), 1.982e5, 3.7493e19, 0.0196, 210, 210, 210, ScaleByDisplay(3e5), ScaleByDisplay(5e6), "mimas"),        // mimas
        Planet(Vector2d(1.42e8, 0), 4.0e4, 1.37e17, 0.0042, 184, 184, 184, ScaleByDisplay(3e5), ScaleByDisplay(8e6), "pandora"),            // pandora
        Planet(Vector2d(1.5166e8, 0), 5.8e4, 5.27e17, 0.0098, 180, 150, 120, ScaleByDisplay(3e5), ScaleByDisplay(7e6), "epimetheus"),       // epimetheus
        Planet(Vector2d(1.51e8, 0), 9.0e4, 1.9e18, 0.0068, 180, 150, 120, ScaleByDisplay(3e5), ScaleByDisplay(5e6), "janus")                // janus
    };
    Planet* uranusPlanets = new Planet[uranusSystemCount]{
        solarPlanets[7],  // uranus
        Planet(Vector2d(4.3591e8, 0), 7.889e5, 3.5276e21, 0.0011, 205, 133, 63, ScaleByDisplay(3e5), ScaleByDisplay(3e6), "titania"),       // titania
        Planet(Vector2d(5.8352e8, 0), 7.614e5, 3.0144e21, 0.0014, 139, 69, 19, ScaleByDisplay(3e5), ScaleByDisplay(3e6), "oberon"),         // oberon
        Planet(Vector2d(2.66e8, 0), 5.847e5, 1.2e21, 0.0039, 150, 150, 150, ScaleByDisplay(3e5), ScaleByDisplay(3e6), "umbriel"),           // umbriel
        Planet(Vector2d(1.90e8, 0), 5.78e5, 1.35e21, 0.0012, 230, 230, 230, ScaleByDisplay(3e5), ScaleByDisplay(3e6), "ariel"),             // ariel
        Planet(Vector2d(1.299e8, 0), 2.358e5, 6.59e19, 0.0013, 210, 210, 210, ScaleByDisplay(3e5), ScaleByDisplay(5e6), "miranda"),         // miranda
        Planet(Vector2d(8.60e7, 0), 8.12e4, 1.1e19, 0.0001, 200, 200, 200, ScaleByDisplay(3e5), ScaleByDisplay(5e6), "puck"),               // puck
        Planet(Vector2d(1.00e8, 0), 4.0e4, 4.0e17, 0.0001, 160, 160, 160, ScaleByDisplay(3e5), ScaleByDisplay(6e6), "belinda"),             // belinda
        Planet(Vector2d(6.30e7, 0), 4.0e4, 4.5e17, 0.0004, 170, 170, 170, ScaleByDisplay(3e5), ScaleByDisplay(7e6), "cressida"),            // cressida
        Planet(Vector2d(6.20e7, 0), 3.5e4, 4.0e17, 0.0001, 165, 165, 165, ScaleByDisplay(3e5), ScaleByDisplay(7e6), "desdemona"),           // desdemona
        Planet(Vector2d(6.60e7, 0), 4.0e4, 4.0e17, 0.0001, 160, 160, 160, ScaleByDisplay(3e5), ScaleByDisplay(7e6), "portia")               // portia
    };
    Planet* neptunePlanets = new Planet[neptuneSystemCount]{
        solarPlanets[8],  // neptune
        Planet(Vector2d(3.548e8, 0), 1.353e6, 2.14e22, 0.0000, 176, 196, 222, ScaleByDisplay(3.245e5), ScaleByDisplay(1e6), "triton"),      // triton
        Planet(Vector2d(9.1e7, 0), 1.7e4, 4.0e15, 0.0001, 165, 165, 165, ScaleByDisplay(3.245e5), ScaleByDisplay(2e7), "hippocamp"),        // hippocamp
        Planet(Vector2d(1.176e8, 0), 2.10e5, 4.4e19, 0.0005, 140, 140, 140, ScaleByDisplay(3.245e5), ScaleByDisplay(0.6e7), "proteus"),     // proteus
        Planet(Vector2d(7.36e7, 0), 9.7e4, 4.2e18, 0.0014, 150, 150, 150, ScaleByDisplay(3.245e5), ScaleByDisplay(1e7), "larissa"),         // larissa
        Planet(Vector2d(6.1953e7, 0), 7.9e4, 2.1e18, 0.0001, 160, 160, 160, ScaleByDisplay(3.245e5), ScaleByDisplay(1e7), "galatea"),       // galatea
        Planet(Vector2d(7.4e7, 0), 7.4e4, 1.8e18, 0.0001, 170, 170, 170, ScaleByDisplay(3.245e5), ScaleByDisplay(1e7), "despina"),          // despina
        Planet(Vector2d(5.0e7, 0), 4.0e4, 7.5e17, 0.0002, 180, 180, 180, ScaleByDisplay(3.245e5), ScaleByDisplay(1e7), "thalassa"),         // thalassa
        Planet(Vector2d(4.82e7, 0), 6.6e4, 1.2e18, 0.0003, 175, 175, 175, ScaleByDisplay(3.245e5), ScaleByDisplay(1e7), "naiad"),           // naiad
        Planet(Vector2d(1.97e8, 0), 3.1e4, 6.0e17, 0.5711, 160, 160, 160, ScaleByDisplay(3.245e5), ScaleByDisplay(1e7), "halimede"),        // halimede
        Planet(Vector2d(5.2e7, 0), 1.6e4, 2.0e17, 0.2931, 140, 140, 140, ScaleByDisplay(3.245e5), ScaleByDisplay(4e7), "sao")               // sao
    };

    CelestialSystem earthSystem(earthPlanets, earthSystemCount, true);
    CelestialSystem marsSystem(marsPlanets, marsSystemCount, true);
    CelestialSystem jupiterSystem(jupiterPlanets, jupiterSystemCount, true);
    CelestialSystem saturnSystem(saturnPlanets, saturnSystemCount, true);
    CelestialSystem uranusSystem(uranusPlanets, uranusSystemCount, true);
    CelestialSystem neptuneSystem(neptunePlanets, neptuneSystemCount, true);
    CelestialSystem* systems[7] = { &solarSystem, &earthSystem, &marsSystem, &jupiterSystem, &saturnSystem, &uranusSystem, &neptuneSystem };

    //add here new solar system planets, elsewere will break the asteroid spawn if shit_computer is false
    
    ///solarSystem.AddPlanet(Planet(Vector2d(5e11, 0), 6.371e6, 5.972e24, 0.5, 0, 102, 204, ScaleByDisplay(1.3e2), ScaleByDisplay(2e5), "czarnadzioora"));
    ///solarSystem.AddPlanet(Planet(Vector2d(1.496e11, 0), 6.371e6, 5.972e30, 0.5, 0, 102, 204, ScaleByDisplay(1.3e2), ScaleByDisplay(2e5), "czarnadzioora2"));

    //the part where you can customize asteroid creation
    const int asteroidCount = 100;
    if (!shit_computer) { // creating the asteroids 

        for (int i = 0; i < asteroidCount / 2; ++i) {
            double distance = (2.4e11 + (rand() % 300) * 3e8);
            double radius = (1.0e3 - (rand() % 200));
            double mass = (1e12 + (rand() % (int)1e9));
            double angle = (rand() % 360) * M_PI / 180.0;
            Vector2d position = Vector2d(distance * cos(angle), distance * sin(angle));

            solarSystem.AddPlanet(Planet(position, radius, mass, 0, 169, 169, 169, ScaleByDisplay(1.3e2), ScaleByDisplay(2e8), "none"));
        }
        for (int i = 0; i < asteroidCount / 2; ++i) {
            double distance = (6.3e12 + (rand() % 300) * 3e8);
            double radius = (1.0e2 - (rand() % 20));
            double mass = (1e19 + (rand() % (int)1e15));
            double angle = (rand() % 360) * M_PI / 180.0;
            Vector2d position = Vector2d(distance * cos(angle), distance * sin(angle));

            solarSystem.AddPlanet(Planet(position, radius, mass, 0, 169, 169, 169, ScaleByDisplay(3e1), ScaleByDisplay(2e9), "none"));
        }
    }

    solarSystem.AddVelocities();

    CreateStars(); // creating the completely random stars
    CreateUi();

	//an array storing the texts that will be drawn on the screen
    sf::Text texts[19] = {
                            SetText(Vector2d(-WIDTH / 2, -HEIGHT / 2), 23, sf::Color::White),       //dt
                            SetText(Vector2d(-WIDTH / 2, -HEIGHT / 2 + 20), 23, sf::Color::White),  //key repeat
                            SetText(Vector2d(-WIDTH / 2, -HEIGHT / 2 + 40), 23, sf::Color::White),  //fps

                            SetText(Vector2d(ui_elements[3].getPosition().x - ScaleByDisplay(50),ui_elements[3].getPosition().y - HEIGHT / 15), ScaleByDisplay(70), sf::Color::White),  //name
                            SetText(Vector2d(ui_elements[3].getPosition().x - ScaleByDisplay(610), ui_elements[3].getPosition().y + ScaleByDisplay(7)), ScaleByDisplay(40), sf::Color::White),  //prev name
                            SetText(Vector2d(ui_elements[3].getPosition().x + ScaleByDisplay(50),ui_elements[3].getPosition().y + ScaleByDisplay(7)), ScaleByDisplay(40), sf::Color::White),  //next name

                            SetText(Vector2d(pos.x + ScaleByDisplay(20), pos.y), ScaleByDisplay(40), sf::Color::White),  //radius
                            SetText(Vector2d(pos.x + ScaleByDisplay(20), pos.y + HEIGHT / 30), ScaleByDisplay(40), sf::Color::White),  //mass
                            SetText(Vector2d(pos.x + ScaleByDisplay(20), pos.y + 2 * HEIGHT / 30), ScaleByDisplay(40), sf::Color::White),  //gravity

                            SetText(Vector2d(ScaleByDisplay(683),pos.y), ScaleByDisplay(45), sf::Color::White),  //position
                            SetText(Vector2d(ScaleByDisplay(683),pos.y + ScaleByDisplay(40)), ScaleByDisplay(45), sf::Color::White),  //velocity
                            SetText(Vector2d(ScaleByDisplay(683),pos.y + ScaleByDisplay(80)), ScaleByDisplay(45), sf::Color::White),  //acceleration
                            SetText(Vector2d(ScaleByDisplay(683),pos.y + ScaleByDisplay(120)), ScaleByDisplay(45), sf::Color::White),  //eccentricty
                            SetText(Vector2d(ScaleByDisplay(683),pos.y + ScaleByDisplay(160)), ScaleByDisplay(45), sf::Color::White),  //orbital period


                            SetText(Vector2d(-pos.x - ScaleByDisplay(800), -HEIGHT / 2), ScaleByDisplay(45), sf::Color::White),  //help
                            SetText(Vector2d(-pos.x - ScaleByDisplay(800), -HEIGHT / 2 + ScaleByDisplay(40)), ScaleByDisplay(45), sf::Color::White),  //arrows
                            SetText(Vector2d(-pos.x - ScaleByDisplay(800), -HEIGHT / 2 + ScaleByDisplay(80)), ScaleByDisplay(45), sf::Color::White),  //numbers
                            SetText(Vector2d(-pos.x - ScaleByDisplay(800), -HEIGHT / 2 + ScaleByDisplay(120)), ScaleByDisplay(45), sf::Color::White),  //shift
                            SetText(Vector2d(-pos.x - ScaleByDisplay(800), -HEIGHT / 2 + ScaleByDisplay(160)), ScaleByDisplay(45), sf::Color::White),  //escape

                            
    };
    
    while (window.isOpen())
    {
        //controlls
        while (const std::optional<sf::Event> event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
            {
                if (keyPressed->scancode == sf::Keyboard::Scancode::Escape) { window.close(); }
                if (keyPressed->scancode == sf::Keyboard::Scancode::Slash) { show_help = !show_help; }
                if (keyPressed->scancode == sf::Keyboard::Scancode::Equal) { dt += (1e3 / 31557600.0); }
                if (keyPressed->scancode == sf::Keyboard::Scancode::Hyphen) { dt -= (1e3 / 31557600.0); }
                if (keyPressed->scancode == sf::Keyboard::Scancode::Space) { 
                    if (dt == 0) systems[system_number]->ChangeDt(dt);
					else dt = 0;
                }
                if (keyPressed->scancode == sf::Keyboard::Scancode::Num1) { system_number = 0; }
                if (keyPressed->scancode == sf::Keyboard::Scancode::Num2) { system_number = 1; }
                if (keyPressed->scancode == sf::Keyboard::Scancode::Num3) { system_number = 2; }
                if (keyPressed->scancode == sf::Keyboard::Scancode::Num4) { system_number = 3; }
                if (keyPressed->scancode == sf::Keyboard::Scancode::Num5) { system_number = 4; }
                if (keyPressed->scancode == sf::Keyboard::Scancode::Num6) { system_number = 5; }
                if (keyPressed->scancode == sf::Keyboard::Scancode::Num7) { system_number = 6; }

                if (last_system != system_number) {
                    last_system = system_number;
                    planet_number = 0;
                    systems[system_number]->ChangeDt(dt);
                }

                if (keyPressed->scancode == sf::Keyboard::Scancode::Left && planet_number > 0) { planet_number--; }
                if (keyPressed->scancode == sf::Keyboard::Scancode::Right && ((planet_number < systems[system_number]->num_planets - asteroidCount - 1 && system_number == 0 && !shit_computer) 
                    || (planet_number < systems[system_number]->num_planets - 1 && system_number == 0 && shit_computer)
                    || (planet_number < systems[system_number]->num_planets - 1 && system_number != 0)))  { planet_number++; }
                
            }
            if (const auto* keyPressed = event->getIf<sf::Event::KeyReleased>()) {
                if (keyPressed->scancode == sf::Keyboard::Scancode::LShift) { key_repeat = !key_repeat; window.setKeyRepeatEnabled(key_repeat); }
            }
        }

        //fps clock
        float deltaTime = sim_clock.restart().asSeconds();
        fpsTimer += deltaTime;
        time_accumulator += deltaTime;
        frameCount++;

        window.setView(view);
        window.clear();

        if (time_accumulator >= simulation_time_delay / max_fps && shit_computer) { // every x th of a frame simulating the systems for poor pcs
            time_accumulator = 0.0f;
            systems[system_number]->Simulate(dt);
        }
        else systems[system_number]->Simulate(dt);

        //writing and calculating fps
        if (fpsTimer >= 1.0f) {
            fps = frameCount / fpsTimer;
            frameCount = 0;
            fpsTimer = 0.0f;

            texts[2].setString("FPS: " + std::to_string(fps));
        }

        //drawing
        DrawStars();
        systems[system_number]->Draw(window);
        DrawUi();

        window.draw(texts[2]);
        DrawText(texts[0], "dt: ", dt, "yr?",false);
        DrawText(texts[1], "key_repeat_state: ", key_repeat,"",false);
            
        DrawText(texts[3], "", systems[system_number]->planets[planet_number].tag);
        DrawText(texts[4], "Prev planet: ", (planet_number > 0) ? systems[system_number]->planets[planet_number - 1].tag : " none");
        DrawText(texts[5], "Next planet: ", (planet_number < systems[system_number]->num_planets - 1 ) ? systems[system_number]->planets[planet_number + 1].tag : " none");
        
        DrawText(texts[6], "Radius: ", systems[system_number]->planets[planet_number].radius * 1.496e11, " m", true);
        DrawText(texts[7], "Mass:", systems[system_number]->planets[planet_number].mass * 1.98848e30, " kg", true);
        DrawText(texts[8], "Gravity:", systems[system_number]->planets[planet_number].gravity, " m/s^2", false);

        DrawText(texts[9], "Position: ", systems[system_number]->planets[planet_number].position * 1.496e11, " m");
        DrawText(texts[10], "Velocity: ", systems[system_number]->planets[planet_number].velocity * 1.496e11, " m/s");
        DrawText(texts[11], "Acceleration: ", systems[system_number]->planets[planet_number].acceleration * 1.496e11, " m/s^2");
        DrawText(texts[12], "Eccentricity: ", systems[system_number]->planets[planet_number].eccentricity,"", false);
        DrawText(texts[13], "Orbital Period: ", systems[system_number]->planets[planet_number].orbital_period, " days", false);

        DrawText(texts[14], "Press ? to show help", "");
        if (show_help) ShowHelp(texts);

        window.display();
    }
}