#include "Tree_mpi.h"
Tree_MPI::Tree_MPI(double x_min, double x_max, double y_min, double y_max) {
	this->particle = nullptr;
	this->x_min = x_min;
	this->x_max = x_max;
	this->y_min = y_min;
	this->y_max = y_max;
	this->total_mass = 0;
	this->center_of_mass_x = 0;
	this->center_of_mass_y = 0;
	for (int i = 0; i < 4; i++) {
		this->children[i] = nullptr;
	}
}

Tree_MPI::~Tree_MPI() {
	for (int i = 0; i < 4; i++) {
		if (this->children[i] != nullptr) {
			delete this->children[i];
		}
	}
}

bool Tree_MPI::isLeaf() {
	return children[0] == nullptr && children[1] == nullptr &&
		children[2] == nullptr && children[3] == nullptr;
}

void Tree_MPI::subdivide() {
	double x_mid = (this->x_min + this->x_max) / 2.0;
	double y_mid = (this->y_min + this->y_max) / 2.0;

	children[0] = new Tree_MPI(this->x_min, x_mid, this->y_min, y_mid);
	children[1] = new Tree_MPI(x_mid, this->x_max, this->y_min, y_mid);
	children[2] = new Tree_MPI(this->x_min, x_mid, y_mid, this->y_max);
	children[3] = new Tree_MPI(x_mid, this->x_max, y_mid, this->y_max);
}

int Tree_MPI::getChildIndex(double x, double y) const {
	double x_mid = (this->x_min + this->x_max) / 2.0;
	double y_mid = (this->y_min + this->y_max) / 2.0;

	if (x <= x_mid && y <= y_mid) return 0;
	if (x > x_mid && y <= y_mid) return 1;
	if (x <= x_mid && y > y_mid) return 2;
	return 3;
}

void Tree_MPI::print() const {
	std::cout << "Tree [x_min: " << this->x_min << ", x_max: " << this->x_max
		<< ", y_min: " << this->y_min << ", y_max: " << this->y_max
		<< ")]\n";
	if (particle != nullptr) {
		std::cout << "  Particle at (" << this->particle->x_pos << ", " << this->particle->y_pos << ")\n";
	}
	for (int i = 0; i < 4; i++) {
		if (this->children[i] != nullptr) {
			this->children[i]->print();
		}
	}
}

Tree_MPI* Tree_MPI::getChildren(int index) {
	return this->children[index];
}

double Tree_MPI::getBoundingXStart() {
	return this->x_min;
}

double Tree_MPI::getBoundingXEnd() {
	return this->x_max;
}

double Tree_MPI::getBoundingYStart() {
	return this->y_min;
}

double Tree_MPI::getBoundingYEnd() {
	return this->y_max;
}

double Tree_MPI::getCenterMassX() {
	return this->center_of_mass_x;
}

double Tree_MPI::getCenterMassY() {
	return this->center_of_mass_y;
}

double Tree_MPI::getTotalMass() {
	return this->total_mass;
}

void Tree_MPI::insert(MPI_Particle* p) {
	if (isLeaf()) {	//if this tree is external
		if (this->particle == nullptr) {
			this->particle = p;
			this->total_mass += p->mass;
			this->center_of_mass_x = p->x_pos;
			this->center_of_mass_y = p->y_pos;
		}
		else {//subdivides region and turns it into internal node
			subdivide();

			this->total_mass = this->particle->mass + p->mass;
			this->center_of_mass_x = (this->particle->x_pos * this->particle->mass + p->x_pos * p->mass) / this->total_mass;
			this->center_of_mass_y = (this->particle->y_pos * this->particle->mass + p->y_pos * p->mass) / this->total_mass;

			int index = getChildIndex(this->particle->x_pos, this->particle->y_pos);
			this->children[index]->insert(this->particle);

			index = getChildIndex(p->x_pos, p->y_pos);
			this->children[index]->insert(p);

			this->particle = nullptr;

		}
	}
	else { //if tree is internal
		int index = getChildIndex(p->x_pos, p->y_pos);
		if (this->children[index] == nullptr) {
			double x_mid = (this->x_min + this->x_max) / 2.0;
			double y_mid = (this->y_min + this->y_max) / 2.0;

			if (index == 0) this->children[index] = new Tree_MPI(this->x_min, x_mid, this->y_min, y_mid);
			else if (index == 1) this->children[index] = new Tree_MPI(x_mid, this->x_max, this->y_min, y_mid);
			else if (index == 2) this->children[index] = new Tree_MPI(this->x_min, x_mid, y_mid, this->y_max);
			else if (index == 3) this->children[index] = new Tree_MPI(x_mid, this->x_max, y_mid, this->y_max);
		}
		this->children[index]->insert(p);

		double new_total_mass = this->total_mass + p->mass;
		this->center_of_mass_x = (this->center_of_mass_x * this->total_mass + p->x_pos * p->mass) / new_total_mass;
		this->center_of_mass_y = (this->center_of_mass_y * this->total_mass + p->y_pos * p->mass) / new_total_mass;
		this->total_mass = new_total_mass;
	}
}

void Tree_MPI::compute_force(MPI_Particle* p, double theta) {
	if (this->particle == nullptr && isLeaf()) { //null tree
		return;
	}
	if (this->particle != nullptr && !sameMPI(this->particle, p)) {
		//external node case
		//calculate force and add to particle p
		double g = .0001;
		double rlimit = .03;
		double d = calculateParticleDistanceMPI(this->particle, p);
		if (d < rlimit) {
			d = rlimit;
		}
		double dx = this->particle->x_pos - p->x_pos;
		double dy = this->particle->y_pos - p->y_pos;
		double fx = (g * p->mass * this->particle->mass * dx / (d * d)) / d;
		double fy = (g * p->mass * this->particle->mass * dy / (d * d)) / d;
		addForce(p, fx, fy);
	}
	else {
		//internal node case
		double s = this->getBoundingXEnd() - this->getBoundingXStart();
		double d = calculateDistanceXY(this->center_of_mass_x, this->center_of_mass_y, p->x_pos, p->y_pos);
		if ((s / d) < theta) {
			//approximate force and add to particle p
			double g = .0001;
			double rlimit = .03;
			double d = calculateDistanceXY(this->center_of_mass_x, this->center_of_mass_y, p->x_pos, p->y_pos);
			if (d < rlimit) {
				d = rlimit;
			}
			double dx = this->center_of_mass_x - p->x_pos;
			double dy = this->center_of_mass_y - p->y_pos;
			double fx = (g * p->mass * this->total_mass * dx / (d * d)) / d;
			double fy = (g * p->mass * this->total_mass * dy / (d * d)) / d;
			addForce(p,fx, fy);
		}
		else {
			//recursively compute force using children
			for (int i = 0; i < 4; i++) {
				Tree_MPI* childTree = this->getChildren(i);
				if (childTree != nullptr) {
					childTree->compute_force(p, theta);
				}
			}
		}
	}
}