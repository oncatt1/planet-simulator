#define _USE_MATH_DEFINES
#include "orbit.h"
#include <cmath>
#include <iostream>
#include <SFML/Graphics.hpp>

#define G (4 * M_PI * M_PI)  //Gravitational constant
#define G_SI 6.67430e-11  //Gravitational constant in si

Vector2d::Vector2d() : x(0), y(0) {}

Vector2d::Vector2d(double x, double y) : x(x), y(y) {}

Vector2d Vector2d::operator+(const Vector2d& other) const { return { x + other.x, y + other.y }; }

Vector2d Vector2d::operator-(const Vector2d& other) const { return { x - other.x, y - other.y }; }

Vector2d Vector2d::operator*(double scalar) const { return { x * scalar, y * scalar }; }

Vector2d Vector2d::operator*(int scalar) const { return { x * static_cast<double>(scalar), y * static_cast<double>(scalar) }; }

Vector2d Vector2d::operator*(const Vector2d& other) const { return { x * other.x, y * other.y }; }

Vector2d Vector2d::operator/(double scalar) const { return { x / scalar, y / scalar }; }

double Vector2d::Dot(const Vector2d& other) const { return x * other.x + y * other.y; }

double Vector2d::Cross(const Vector2d& other) const { return x * other.y - y * other.x; }

double Vector2d::Magnitude() const { return std::sqrt(x * x + y * y); }

double Vector2d::MagnitudeSquared() const { return x * x + y * y; }

Planet::Planet() : position(0, 0), velocity(0, 0), acceleration(0, 0), radius(0), mass(0), distance_scale(0), radius_scale(0), color_r(0), color_g(0), color_b(0), tag(""), eccentricity(0), orbital_period(0), gravity(0) {}


Planet::Planet(Vector2d position, double r, double mass, double eccentricity, unsigned short color_r, unsigned short color_g, unsigned short color_b, double distance_scale, double radius_scale, std::string tag)
      : position(position / 1.496e11), radius(r / 1.496e11), mass(mass / 1.98848e30), eccentricity(eccentricity),
        color_r(color_r), color_g(color_g), color_b(color_b), distance_scale(distance_scale), radius_scale(radius_scale), tag(tag) {
    velocity = Vector2d(0, 0);
	gravity = CalculateGravity();
	orbital_period = 0;
}

void Planet::Draw(sf::RenderWindow& window) const
{
    double scaled_radius = radius * radius_scale;
    Vector2d scaled_distance = { position.x * distance_scale, position.y * distance_scale };
    sf::Vector2f screen_position(static_cast<float>(scaled_distance.x), static_cast<float>(scaled_distance.y));
    sf::CircleShape shape(static_cast<float>(scaled_radius));

    shape.setOrigin(sf::Vector2f(static_cast<float>(scaled_radius), static_cast<float>(scaled_radius)));
    shape.setFillColor(sf::Color(color_r, color_g, color_b));
    shape.setPosition(screen_position);
    window.draw(shape);
}

void Planet::CalculateOrbitalVelocity(Planet &centralBody)
{
    Vector2d direction = position - centralBody.position;
    double distance = direction.Magnitude();

    double semi_major_axis = distance / (1 - eccentricity);

    double orbitalSpeed = std::sqrt(G * centralBody.mass * (2.0 / distance - 1.0 / semi_major_axis));

    double angle = std::atan2(direction.y, direction.x);
    if (tag == "venus" || tag == "uranus")
        angle -= M_PI / 2;
    else
        angle += M_PI / 2;

    Vector2d relativeVelocity(std::cos(angle) * orbitalSpeed, std::sin(angle) * orbitalSpeed);
    velocity = relativeVelocity;
    
}

void Planet::CalculateOrbitalPeriod(Planet& planet)
{
	double distance = position.x * 1.496e11;
    double orbit = (2 * M_PI * sqrt(pow(distance, 3) / (G_SI * planet.mass * 2e30)));
    orbital_period = orbit;
}

double Planet::CalculateGravity()
{
    double _mass = mass * 1.98848e30;
    double _radius = radius * 1.496e11;
    return (G_SI * _mass) / (_radius * _radius);
}

static Vector2d GravitationalForce(Planet main, Planet other)
{
    Vector2d direction = other.position - main.position;
	if (direction.Magnitude() == 0) return { 0, 0 };
    double distance = direction.Magnitude();
    double forceMagnitude = (G * main.mass * other.mass) / (distance * distance);
    Vector2d normalized = { direction.x / distance, direction.y / distance };
    return normalized * forceMagnitude;
}

static void ApplyGravitationalForce(Planet* planets, unsigned short planetCount, double dt) // multi body problem
{
    for (unsigned short i = 0; i < planetCount; ++i) {
        Vector2d netForce = { 0,0 };
        for (unsigned short j = 0; j < planetCount; ++j) {
            if (i == j) continue;
            netForce = netForce + GravitationalForce(planets[i], planets[j]);
        }

        planets[i].acceleration = netForce / planets[i].mass;
        planets[i].velocity = planets[i].velocity + planets[i].acceleration * dt;
        planets[i].position = planets[i].position + planets[i].velocity * dt;
    }
}

CelestialSystem::CelestialSystem(Planet* planets, unsigned short num_planets, bool planet_system)
    : planets(planets), num_planets(num_planets), planet_system(planet_system)
{
    if (planets[0].position.x != 0) {
        planets[0].radius *= 5;

    }
    planets[0].position.x = 0;
    for (int i = 1; i < num_planets; ++i) {
        planets[i].CalculateOrbitalVelocity(planets[0]);
		planets[i].CalculateOrbitalPeriod(planets[0]);
    }
}

CelestialSystem::CelestialSystem(bool planet_system) : planets(nullptr), num_planets(0), planet_system(planet_system){}

CelestialSystem::~CelestialSystem()
{
    delete[] planets;
}

void CelestialSystem::AddVelocities()
{
    Planet* main = &planets[0];
    for (int i = 0; i < num_planets; ++i) {
        if (planets[i].tag == "sun") {
            main = &planets[i];
            break;
        }
    }
    for (int i = 0; i < num_planets; ++i) {
        if (planets[i].tag != "sun") {
            planets[i].CalculateOrbitalVelocity(*main);
			planets[i].CalculateOrbitalPeriod(*main);
        }
    }
}

void CelestialSystem::Simulate(double dt) const
{
    ApplyGravitationalForce(planets, num_planets, dt);  
}

void CelestialSystem::Draw(sf::RenderWindow& window) const
{
    for (unsigned short i = 0; i < num_planets; i++) {
        planets[i].Draw(window);
    }
}

void CelestialSystem::ChangeDt(double& dt)
{
    dt = planet_system ? 315.0 / 31557600.0 : 315576.0 / 31557600.0;
}

void CelestialSystem::AddPlanet(Planet planet)
{
    Planet* newPlanets = new Planet[num_planets + 1];
    std::copy(planets, planets + num_planets, newPlanets);
    delete[] planets;
    planets = newPlanets;
    planets[num_planets] = planet;
    num_planets++;
}
