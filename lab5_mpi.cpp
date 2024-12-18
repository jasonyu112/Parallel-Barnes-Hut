#include <iostream>
#include <mpi.h>
#include "mpi_particle.h"
#include "argparse.h"
#include "helpers.h"
#include <vector>
#include "Tree_mpi.h"
#include <fstream>

int main(int argc, char* argv[])
{
    int rank, size;
    double start, end; //used for timing the program later

    struct options_t opts;
    get_opts(argc, argv, &opts);

    //init MPI 
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Datatype mpi_particle_type;
    start = MPI_Wtime();
    // Create MPI_Datatype for sending particles.
    MPI_Aint displacements_c[8] = {
        offsetof(MPI_Particle, index),
        offsetof(MPI_Particle, x_pos),
        offsetof(MPI_Particle, y_pos),
        offsetof(MPI_Particle, mass),
        offsetof(MPI_Particle, x_vel),
        offsetof(MPI_Particle, y_vel),
        offsetof(MPI_Particle, x_force),
        offsetof(MPI_Particle, y_force)
    };
    int block_lengths_c[8] = { 1, 1, 1, 1, 1, 1, 1, 1 };
    MPI_Datatype dtypes_c[8] = { MPI_INT, MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE};
    MPI_Type_create_struct(8, block_lengths_c, displacements_c, dtypes_c, &mpi_particle_type);
    MPI_Type_commit(&mpi_particle_type);

    // OpenGL setup only for rank 0
    GLFWwindow* window = nullptr;
    if (opts.v && rank == 0) {
        const int width = 3840, height = 2160;
        if (rank == 0) {
            if (!glfwInit()) {
                fprintf(stderr, "Failed to initialize GLFW\n");
                MPI_Finalize();
                return -1;
            }
            window = glfwCreateWindow(width, height, "Simulation", NULL, NULL);
            if (window == NULL) {
                fprintf(stderr, "Failed to open GLFW window.\n");
                glfwTerminate();
                MPI_Finalize();
                return -1;
            }
            glfwMakeContextCurrent(window);
            if (glewInit() != GLEW_OK) {
                fprintf(stderr, "Failed to initialize GLEW\n");
                MPI_Finalize();
                return -1;
            }
            glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
        }
    }
    //
    
    //read input
    std::vector<MPI_Particle*> particles;
    if (rank == 0) {
        readinput_mpi(opts.in_file, &particles);
    }
    MPI_Particle* particle_array = (MPI_Particle*)malloc(particles.size() * sizeof(MPI_Particle));
    if (rank == 0) {
        for (unsigned int i = 0; i < particles.size(); i++) {
            particle_array[i].index = particles[i]->index;
            particle_array[i].x_pos = particles[i]->x_pos;
            particle_array[i].y_pos = particles[i]->y_pos;
            particle_array[i].mass = particles[i]->mass;
            particle_array[i].x_vel = particles[i]->x_vel;
            particle_array[i].y_vel = particles[i]->y_vel;
            particle_array[i].x_force = particles[i]->x_force;
            particle_array[i].y_force = particles[i]->y_force;
        }
        MPI_Bcast(particle_array, particles.size(), mpi_particle_type, 0, MPI_COMM_WORLD);
    }
    else {
        MPI_Bcast(particle_array, particles.size(), mpi_particle_type, 0, MPI_COMM_WORLD);
    }
    MPI_Barrier(MPI_COMM_WORLD);
    
    for (int i = 0; i < opts.steps; i++) {
        //create tree
        Tree_MPI quadTree = Tree_MPI(0, 4, 0, 4);
        for (unsigned int j = 0; j < particles.size(); j++) {
            //insert particles into tree
            if (inBoundaryMPI(particle_array[j])) {
                quadTree.insert(&particle_array[j]);
            }
        }
        
        int chunk = 0;
        int last_chunk = 0;
        if ((particles.size() / size) == 0) {
            chunk = particles.size() / size;
            last_chunk = chunk;
        }
        else {
            chunk = particles.size() / size;
            last_chunk = particles.size() - ((size - 1) * chunk);
        }

        int startpos = rank * chunk;
        int endpos = rank == (size - 1) ? particles.size() : rank * chunk + chunk;

        //compute force for every particle
        for (int index = startpos; index < endpos; index++) {
            MPI_Particle* p = &(particle_array[index]);
            if (inBoundaryMPI(particle_array[index])){
                quadTree.compute_force(p, opts.theta);
            }
        }

        //update position and velocity of every particle
        for (int index = startpos; index < endpos; index++) {
            MPI_Particle* p = &(particle_array[index]);
            if (inBoundaryMPI(particle_array[index])) {
                updatePosAndVel(p, opts.dt);
            }
        }

        for (int index = 0; index < size;index++) {
            int tmp_chunk = (index == (size - 1)) ? last_chunk : chunk;
            MPI_Particle* updated_particles = (MPI_Particle*)malloc(tmp_chunk * sizeof(MPI_Particle));
            if (index == rank) {
                for (int f = 0; f < tmp_chunk; f++) {
                    int idx = rank * chunk + f;
                    updated_particles[f].index = particle_array[idx].index;
                    updated_particles[f].x_pos = particle_array[idx].x_pos;
                    updated_particles[f].y_pos = particle_array[idx].y_pos;
                    updated_particles[f].mass = particle_array[idx].mass;
                    updated_particles[f].x_vel = particle_array[idx].x_vel;
                    updated_particles[f].y_vel = particle_array[idx].y_vel;
                    updated_particles[f].x_force = particle_array[idx].x_force;
                    updated_particles[f].y_force = particle_array[idx].y_force;
                }
            }
            MPI_Bcast(updated_particles, tmp_chunk, mpi_particle_type, index, MPI_COMM_WORLD);

            if (index != rank) {
                // Update particles.
                for (int k = 0; k < tmp_chunk; k++) {
                    // Get the index of the particle that needs updating.
                    int idx = updated_particles[k].index;
                    if (particle_array[idx].index == idx) {
                        particle_array[idx].x_pos = updated_particles[k].x_pos;
                        particle_array[idx].y_pos = updated_particles[k].y_pos;
                        particle_array[idx].mass = updated_particles[k].mass;
                        particle_array[idx].x_vel = updated_particles[k].x_vel;
                        particle_array[idx].y_vel = updated_particles[k].y_vel;
                        particle_array[idx].x_force = updated_particles[k].x_force;
                        particle_array[idx].y_force = updated_particles[k].y_force;
                    }
                }
            }
            free(updated_particles);
        }

        MPI_Barrier(MPI_COMM_WORLD);
        if (opts.v && rank ==0) {
            //opengl stuff
            glClear(GL_COLOR_BUFFER_BIT);
            //drawOctreeBounds2D(&quadTree);
            for (unsigned int drawIndex = 0; drawIndex < particles.size();drawIndex++) {
                MPI_Particle* temp_p = &particle_array[drawIndex];
                double x_window = 2 * (temp_p->x_pos / 4) - 1;
                double y_window = 2 * (temp_p->y_pos / 4) - 1;
                double radius = .0005 * temp_p->mass;
                drawParticle2D(x_window, y_window, radius);
            }
            // Swap buffers
            glfwSwapBuffers(window);
            glfwPollEvents();
            //opengl stuff
        }
    }
    //writing to file when finished
    if (rank == 0) {
        std::ofstream myFile(opts.out_file);
        if (myFile.is_open()) {
            myFile << particles.size() << std::endl;
            for (unsigned int i = 0; i < particles.size(); i++) {
                myFile<<std::scientific << particle_array[i].index << " " << particle_array[i].x_pos << " " << particle_array[i].y_pos << " " << particle_array[i].mass << " " << particle_array[i].x_vel << " " << particle_array[i].y_vel << std::endl;
            }
        }
    }
    free(particle_array);
    //free particles
    for (MPI_Particle* p : particles) {
        delete p;
    }
    end = MPI_Wtime()- start;
    if (rank == 0) printf("Processor Count: %d | Run Time: %f |\n", size, end);
    MPI_Finalize();
    return 0;
}

