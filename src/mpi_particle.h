#pragma once
#include <cmath>
#include <iostream>
#include <iomanip>
typedef struct MPI_Particle {
    int index;
    double x_pos, y_pos;
    double mass;
    double x_vel, y_vel;
    double x_force, y_force;
} MPI_Particle;

void updatePosAndVel(MPI_Particle* p, double dt);

void print(MPI_Particle* p);

void addForce(MPI_Particle* p, double x, double y);