#include <SFML/Graphics.hpp>

class Vector2d {
public:
    double x;
    double y;

    Vector2d();
    Vector2d(double x, double y);

    Vector2d operator+(const Vector2d& other) const;
    Vector2d operator-(const Vector2d& other) const;
    Vector2d operator*(double scalar) const;
    Vector2d operator*(int scalar) const;
    Vector2d operator*(const Vector2d& other) const;
    Vector2d operator/(double scalar) const;
    double Dot(const Vector2d& other) const;
    double Cross(const Vector2d& other) const;
    double Magnitude() const;
    double MagnitudeSquared() const;
    Vector2d Normalized() const;
};

class Planet {
public:
    Vector2d position;
    Vector2d velocity;
    Vector2d acceleration;
    double radius;
    double mass;
    double eccentricity;
    double gravity;
    double orbital_period;
    double distance_scale;
    double radius_scale;
    unsigned short color_r;
    unsigned short color_g;
    unsigned short color_b;
    std::string tag;

    Planet();
    Planet(Vector2d position, double r, double mass, double eccentricity, unsigned short color_r, unsigned short color_g, unsigned short color_b, double distance_scale, double radius_scale, std::string tag);
    void Draw(sf::RenderWindow& window) const; 
    void CalculateOrbitalVelocity(Planet& planet);
    void CalculateOrbitalPeriod(Planet& planet);
    double CalculateGravity();
};
class CelestialSystem {
public:
    Planet* planets;
    unsigned short num_planets;
    bool planet_system;
	CelestialSystem(Planet* planets, unsigned short num_planets, bool planet_system);
    CelestialSystem(bool planet_system);
    ~CelestialSystem();
    void AddVelocities();
    void Simulate(double dt) const;
    void Draw(sf::RenderWindow& window) const;
    void ChangeDt(double& dt);
    void AddPlanet(Planet planet);
};
