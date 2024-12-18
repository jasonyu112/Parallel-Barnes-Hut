#include "helpers.h"
void read_input(char* input_name, std::vector<Particle*>* particles) {
	std::ifstream in;
	in.open(input_name);
	int size;

	in >> size;
	
	for (int i = 0; i < size; i++) {
		int row;
		double x, y, mass, x_vel, y_vel;
		in >> row >> x >> y >> mass >> x_vel >> y_vel;
		Particle* p = new Particle(row, x, y, mass, x_vel, y_vel);
		particles->push_back(p);
	}
}

void readinput_mpi(char* input_name, std::vector<MPI_Particle*>* particles) {
    std::ifstream in;
    in.open(input_name);
    int size;

    in >> size;

    for (int i = 0; i < size; i++) {
        int row;
        double x, y, mass, x_vel, y_vel;
        in >> row >> x >> y >> mass >> x_vel >> y_vel;
        MPI_Particle* p = new MPI_Particle{ row, x, y, mass, x_vel, y_vel };
        particles->push_back(p);
    }
}

void drawParticle2D(double x_window, double y_window,double radius) {
    int k = 0;
    float angle = 0.0f;
    glBegin(GL_TRIANGLE_FAN);
    glColor3f(1, 1, 0);
    glVertex2f(x_window, y_window);
    for (k = 0; k < 20; k++) {
        angle = (float)(k) / 19 * 2 * 3.141592;
        glVertex2f(x_window + radius * cos(angle),
            y_window + radius * sin(angle));
    }
    glEnd();
}

void drawOctreeBounds2D(Tree* node) {
    if (node == nullptr) {
        return;
    }
    glBegin(GL_LINES);
    double x_min = node->getBoundingXStart() / 4.0 * 2.0 - 1.0;
    double x_max = node->getBoundingXEnd() / 4.0 * 2.0 - 1.0;
    double y_min = node->getBoundingYStart() / 4.0 * 2.0 - 1.0;
    double y_max = node->getBoundingYEnd() / 4.0 * 2.0 - 1.0;

    double x_mid = (x_min + x_max) / 2.0;
    double y_mid = (y_min + y_max) / 2.0;
    // set the color of lines to be white
    glColor3f(1.0f, 1.0f, 1.0f);
    // specify the start point's coordinates
    glVertex2f(node->getBoundingXStart(), y_mid);
    // specify the end point's coordinates
    glVertex2f(node->getBoundingXEnd(), y_mid);
    // do the same for verticle line
    glVertex2f(x_mid, node->getBoundingYStart());
    glVertex2f(x_mid, node->getBoundingYEnd());
    glEnd();
    for (int i = 0; i < 4;i++) {
        drawOctreeBounds2D(node->getChildren(i));
    }
    
}

double calculateParticleDistance(Particle* p1, Particle* p2) {
    double x1 = p1->getX();
    double y1 = p1->getY();
    double x2 = p2->getX();
    double y2 = p2->getY();

    return sqrt(pow(x2-x1,2)+pow(y2-y1,2));
}

double calculateDistanceXY(double x1, double y1, double x2, double y2) {
    return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}

bool same(Particle* p1, Particle* p2) {
    return p1->getX() == p2->getX() && p1->getY() == p2->getY();
}

bool inBoundary(Particle* p) {
    return p->getX() < 4 && p->getX() > 0 && p->getY() < 4 && p->getY() > 0;
}

bool inBoundaryMPI(MPI_Particle p) {
    return p.x_pos< 4 && p.x_pos > 0 && p.y_pos < 4 && p.y_pos > 0;
}

bool sameMPI(MPI_Particle* p1, MPI_Particle* p2) {
    return p1->x_pos == p2->x_pos && p1->y_pos == p2->y_pos;
}

double calculateParticleDistanceMPI(MPI_Particle* p1, MPI_Particle* p2) {
    double x1 = p1->x_pos;
    double y1 = p1->y_pos;
    double x2 = p2->x_pos;
    double y2 = p2->y_pos;

    return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}