#pragma once
#include <cmath>
#include <iostream>
#include <iomanip>
class  Particle {
private:
	int index;
	double x_pos;
	double y_pos;
	double mass;
	double x_vel;
	double y_vel;
	double x_force;
	double y_force;
public:
	Particle(int index, double x_pos, double y_pos, double mass, double x_vel, double y_vel);
	Particle();

	void setX(double newX);

	void setY(double newY);

	void setXVel(double newX);

	void setYVel(double newY);

	~Particle();

	double getX();
	double getY();
	double getMass();
	double getXVel();
	double getYVel();
	int getIndex();
	void setForce(double x, double y);
	void addForce(double x, double y);

	void updatePosAndVel(double dt);

	void print();        
};