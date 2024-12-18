#pragma once
#include "Particle.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include "mpi_particle.h"
//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtc/type_ptr.hpp>
#include <GL/glu.h>
//#include <GL/glut.h>
#include "tree.h"

void read_input(char* input_name, std::vector<Particle*>* particles);
void readinput_mpi(char* input_name, std::vector<MPI_Particle*>* particles);
void drawParticle2D(double x_window, double y_window, double radius);
//void drawOctreeBounds2D(Tree* node);
double calculateParticleDistance(Particle* p1, Particle* p2);
double calculateDistanceXY(double x1, double y1, double x2, double y2);

bool same(Particle* p1, Particle* p2);
bool inBoundary(Particle* p);
bool inBoundaryMPI(MPI_Particle p);

bool sameMPI(MPI_Particle* p1, MPI_Particle* p2);
double calculateParticleDistanceMPI(MPI_Particle* p1, MPI_Particle* p2);