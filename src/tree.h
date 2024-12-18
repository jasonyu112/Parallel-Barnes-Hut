#pragma once
#include "Particle.h"
#include <vector>
#include <iostream>
#include "helpers.h"
class Tree {
private:
	double x_min, x_max;
	double y_min, y_max;
	double total_mass;
	double center_of_mass_x, center_of_mass_y;
	Particle* particle;
	Tree* children[4];
public:
	Tree(double x_min, double x_max, double y_min, double y_max);

	~Tree();

	bool isLeaf();

	void insert(Particle* p);

	void subdivide();

	int getChildIndex(double x, double y)const;

	void print() const;

	Tree* getChildren(int index);

	double getBoundingXStart();

	double getBoundingXEnd();

	double getBoundingYStart();

	double getBoundingYEnd();

	double getTotalMass();

	double getCenterMassX();

	double getCenterMassY();

	void compute_force(Particle* p, double theta);
};