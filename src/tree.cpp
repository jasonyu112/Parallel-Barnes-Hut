#include "tree.h"
Tree::Tree(double x_min, double x_max, double y_min, double y_max) {
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

Tree::~Tree() {
	for (int i = 0; i < 4; i++) {
		if (this->children[i] != nullptr) {
			delete this->children[i];
		}
	}
}

bool Tree::isLeaf() {
	return children[0] == nullptr && children[1] == nullptr &&
		children[2] == nullptr && children[3] == nullptr;
}

void Tree::subdivide() {
	double x_mid = (this->x_min + this->x_max) / 2.0;
	double y_mid = (this->y_min + this->y_max) / 2.0;

	children[0] = new Tree(this->x_min, x_mid, this->y_min, y_mid);
	children[1] = new Tree(x_mid, this->x_max, this->y_min, y_mid);
	children[2] = new Tree(this->x_min, x_mid, y_mid, this->y_max);
	children[3] = new Tree(x_mid, this->x_max, y_mid, this->y_max);
}

void Tree::insert(Particle* p) {
	if (isLeaf()) {	//if this tree is external
		if (this->particle == nullptr) {
			this->particle = p;
			this->total_mass += p->getMass();
			this->center_of_mass_x = p->getX();
			this->center_of_mass_y = p->getY();
		}
		else {//subdivides region and turns it into internal node
			subdivide();

			this->total_mass = this->particle->getMass() + p->getMass();
			this->center_of_mass_x = (this->particle->getX() * this->particle->getMass() + p->getX() * p->getMass()) / this->total_mass;
			this->center_of_mass_y = (this->particle->getY() * this->particle->getMass() + p->getY() * p->getMass()) / this->total_mass;

			int index = getChildIndex(this->particle->getX(), this->particle->getY());
			this->children[index]->insert(this->particle);

			index = getChildIndex(p->getX(), p->getY());
			this->children[index]->insert(p);

			this->particle = nullptr;

		}
	}
	else { //if tree is internal
		int index = getChildIndex(p->getX(), p->getY());
		if (this->children[index] == nullptr) {
			double x_mid = (this->x_min + this->x_max) / 2.0;
			double y_mid = (this->y_min + this->y_max) / 2.0;

			if (index == 0) this->children[index] = new Tree(this->x_min, x_mid, this->y_min, y_mid);
			else if (index == 1) this->children[index] = new Tree(x_mid, this->x_max, this->y_min, y_mid);
			else if (index == 2) this->children[index] = new Tree(this->x_min, x_mid, y_mid, this->y_max);
			else if (index == 3) this->children[index] = new Tree(x_mid, this->x_max, y_mid, this->y_max);
		}
		this->children[index]->insert(p);

		double new_total_mass = this->total_mass + p->getMass();
		this->center_of_mass_x = (this->center_of_mass_x * this->total_mass + p->getX() * p->getMass()) / new_total_mass;
		this->center_of_mass_y = (this->center_of_mass_y * this->total_mass + p->getY() * p->getMass()) / new_total_mass;
		this->total_mass = new_total_mass;
	}
}

int Tree::getChildIndex(double x, double y) const {
	double x_mid = (this->x_min + this->x_max) / 2.0;
	double y_mid = (this->y_min + this->y_max) / 2.0;

	if (x <= x_mid && y <= y_mid) return 0;
	if (x > x_mid && y <= y_mid) return 1;  
	if (x <= x_mid && y > y_mid) return 2;  
	return 3;                              
}

void Tree::print() const {
	std::cout << "Tree [x_min: " << this->x_min << ", x_max: " << this->x_max
		<< ", y_min: " << this->y_min << ", y_max: " << this->y_max
		<< ")]\n";
	if (particle != nullptr) {
		std::cout << "  Particle at (" << this->particle->getX() << ", " << this->particle->getY() << ")\n";
	}
	for (int i = 0; i < 4; i++) {
		if (this->children[i] != nullptr) {
			this->children[i]->print();
		}
	}
}

Tree* Tree::getChildren(int index) {
	return this->children[index];
}

double Tree::getBoundingXStart() {
	return this->x_min;
}

double Tree::getBoundingXEnd() {
	return this->x_max;
}

double Tree::getBoundingYStart() {
	return this->y_min;
}

double Tree::getBoundingYEnd() {
	return this->y_max;
}

double Tree::getCenterMassX() {
	return this->center_of_mass_x;
}

double Tree::getCenterMassY() {
	return this->center_of_mass_y;
}

double Tree::getTotalMass() {
	return this->total_mass;
}

void Tree::compute_force(Particle* p, double theta) {
	if (this->particle == nullptr && isLeaf()) { //null tree
		return;
	}
	if (this->particle != nullptr && !same(this->particle, p)) {
		//external node case
		//calculate force and add to particle p
		double g = .0001;
		double rlimit = .03;
		double d = calculateParticleDistance(this->particle, p);
		if (d < rlimit) {
			d = rlimit;
		}
		double dx = this->particle->getX() - p->getX();
		double dy = this->particle->getY() - p->getY();
		double fx = (g * p->getMass() * this->particle->getMass() * dx / (d * d))/d;
		double fy = (g * p->getMass() * this->particle->getMass() * dy / (d * d))/d;
		p->addForce(fx,fy);
	}
	else {
		//internal node case
		double s = this->getBoundingXEnd() - this->getBoundingXStart();
		double d = calculateDistanceXY(this->center_of_mass_x,this->center_of_mass_y, p->getX(),p->getY());
		if ((s/d)<theta) {
			//approximate force and add to particle p
			double g = .0001;
			double rlimit = .03;
			double d = calculateDistanceXY(this->center_of_mass_x, this->center_of_mass_y, p->getX(), p->getY());
			if (d < rlimit) {
				d = rlimit;
			}
			double dx = this->center_of_mass_x - p->getX();
			double dy = this->center_of_mass_y - p->getY();
			double fx = (g * p->getMass() * this->total_mass * dx / (d*d))/d;
			double fy = (g * p->getMass() * this->total_mass * dy / (d*d))/d;
			p->addForce(fx, fy);
		}
		else {
			//recursively compute force using children
			for (int i = 0; i < 4; i++) {
				Tree* childTree = this->getChildren(i);
				if (childTree != nullptr) {
					childTree->compute_force(p, theta);
				}
			}
		}
	}
}