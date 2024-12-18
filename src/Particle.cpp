#include "Particle.h"

Particle::Particle(int index, double x_pos, double y_pos, double mass, double x_vel, double y_vel) {
	this->index = index;
	this->x_pos = x_pos;
	this->y_pos = y_pos;
	this->mass = mass;
	this->x_vel = x_vel;
	this->y_vel = y_vel;
	this->x_force = 0;
	this->y_force = 0;
}

void Particle::setX(double newX) {
	this->x_pos = newX;
}

void Particle::setY(double newY) {
	this->y_pos = newY;
}

void Particle::setXVel(double newX) {
	this->x_vel = newX;
}

void Particle::setYVel(double newY) {
	this->y_vel = newY;
}

double Particle::getMass() {
	return this->mass;
}

double Particle::getX() {
	return this->x_pos;
}

double Particle::getY() {
	return this->y_pos;
}

double Particle::getYVel() {
	return this->y_vel;
}

double Particle::getXVel() {
	return this->x_vel;
}

int Particle::getIndex() {
	return this->index;
}

Particle::~Particle() {

}

void Particle::addForce(double x, double y) {
	this->x_force += x;
	this->y_force += y;
}

void Particle::setForce(double x, double y) {
	this->x_force = x;
	this->y_force = y;
}

void Particle::updatePosAndVel(double dt) {
	double ax = this->x_force / this->mass;
	double ay = this->y_force / this->mass;
	double new_x = this->x_pos + this->x_vel * dt + .5 * ax * dt*dt;
	double new_y = this->y_pos + this->y_vel * dt + .5 * ay * dt*dt;
	double new_x_vel = this->x_vel + ax * dt;
	double new_y_vel = this->y_vel + ay * dt;
	this->x_pos = new_x;
	this->y_pos = new_y;
	this->x_vel = new_x_vel;
	this->y_vel = new_y_vel;
	this->x_force = 0;
	this->y_force = 0;
}

void Particle::print() {
	std::cout<<std::setprecision(17)<<std::fixed<<this->getIndex()<<" " << this->getX() << " " << this->getY() << " " << this->getMass() << " " << this->getXVel() << " " << this->getYVel() << std::endl;
}

Particle::Particle()
	: index(0), x_pos(0.0), y_pos(0.0), mass(0.0), x_vel(0.0), y_vel(0.0), x_force(0.0), y_force(0.0) {
}