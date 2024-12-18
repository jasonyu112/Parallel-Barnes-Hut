#include "mpi_particle.h"

void updatePosAndVel(MPI_Particle*p, double dt) {
	double ax = p->x_force / p->mass;
	double ay = p->y_force / p->mass;
	double new_x = p->x_pos + p->x_vel * dt + .5 * ax * dt * dt;
	double new_y = p->y_pos + p->y_vel * dt + .5 * ay * dt * dt;
	double new_x_vel = p->x_vel + ax * dt;
	double new_y_vel = p->y_vel + ay * dt;
	p->x_pos = new_x;
	p->y_pos = new_y;
	p->x_vel = new_x_vel;
	p->y_vel = new_y_vel;
	p->x_force = 0;
	p->y_force = 0;
}

void print(MPI_Particle* p) {
	std::cout << std::setprecision(17) << std::fixed << p->index << " " << p->x_pos << " " << p->y_pos << " " << p->mass << " " << p->x_vel << " " << p->y_vel << std::endl;
}

void addForce(MPI_Particle* p, double x, double y) {
	p->x_force += x;
	p->y_force += y;
}