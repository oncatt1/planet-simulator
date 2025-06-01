#include <SFML/Graphics.hpp>


// a simple 2D vector class which i exported to a separate file but havent change it for this
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

    // Returns the dot product of this vector and another vector.
    // Useful for calculating angles, projections, or checking orthogonality.
    double Dot(const Vector2d& other) const;

    // Returns the 2D cross product (scalar) of this vector and another vector.
    // Useful for determining the relative orientation (e.g., left/right turn).
    double Cross(const Vector2d& other) const;

    // Returns the magnitude (length) of the vector.
    // Equivalent to the square root of the sum of squares of components.
    double Magnitude() const;

    // Returns the squared magnitude (length squared) of the vector.
    // More efficient than Magnitude() when only relative lengths are needed.
    double MagnitudeSquared() const;
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
    std::string tag; //tags are just planet names, used for displaying and some calculating exceptions

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
	CelestialSystem(bool planet_system); //this one is only for the solar system, where the planets are added later
    ~CelestialSystem();
    void AddVelocities();
    void Simulate(double dt) const;
    void Draw(sf::RenderWindow& window) const;
	void ChangeDt(double& dt); //change the time step for the simulation
    void AddPlanet(Planet planet);
};
