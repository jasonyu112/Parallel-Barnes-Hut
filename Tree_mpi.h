#pragma once
#include "mpi_particle.h"
#include <iostream>
#include "helpers.h"
class Tree_MPI {
private:
	double x_min, x_max;
	double y_min, y_max;
	double total_mass;
	double center_of_mass_x, center_of_mass_y;
	MPI_Particle* particle;
	Tree_MPI* children[4];
public:
	Tree_MPI(double x_min, double x_max, double y_min, double y_max);

	~Tree_MPI();

	bool isLeaf();

	void insert(MPI_Particle* p);

	void subdivide();

	int getChildIndex(double x, double y)const;

	void print() const;

	Tree_MPI* getChildren(int index);

	double getBoundingXStart();

	double getBoundingXEnd();

	double getBoundingYStart();

	double getBoundingYEnd();

	double getTotalMass();

	double getCenterMassX();

	double getCenterMassY();

	void compute_force(MPI_Particle* p, double theta);
};