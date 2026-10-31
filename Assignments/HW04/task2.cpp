#include <cstddef>
#include <iostream>
#include <fstream>  
#include <cmath>
#include <cstdlib>  
#include <ctime>   
#include <chrono>
#include <random>

using std::chrono::high_resolution_clock;
using std::chrono::duration;

// Constants
const double G = 1.0;          // Gravitational constant
const double softening = 0.1;  // Softening length
const double dt = 0.01;        // Time step
const double board_size = 4.0; // Size of the board

// Function to calculate acceleration due to gravity
void getAcc(const double pos[][3], const double mass[], double acc[][3], int N) {

    for (int particle = 0; particle < N; ++particle) {
        acc[particle][0] = 0.0;
        acc[particle][1] = 0.0;
        acc[particle][2] = 0.0;
    }

    const double softeningSquared = softening * softening;
    for (int particle = 0; particle < N; ++particle) {
        const double positionX = pos[particle][0];
        const double positionY = pos[particle][1];
        const double positionZ = pos[particle][2];
        const double particleMass = mass[particle];
        double accelerationX = acc[particle][0];
        double accelerationY = acc[particle][1];
        double accelerationZ = acc[particle][2];

        for (int neighbor = particle + 1; neighbor < N; ++neighbor) {
            const double deltaX = pos[neighbor][0] - positionX;
            const double deltaY = pos[neighbor][1] - positionY;
            const double deltaZ = pos[neighbor][2] - positionZ;
            const double distanceSquared = deltaX * deltaX + deltaY * deltaY
                                         + deltaZ * deltaZ + softeningSquared;
            const double gravityScale = G / (distanceSquared * std::sqrt(distanceSquared));
            const double neighborScale = gravityScale * mass[neighbor];
            const double particleScale = gravityScale * particleMass;

            accelerationX += deltaX * neighborScale;
            accelerationY += deltaY * neighborScale;
            accelerationZ += deltaZ * neighborScale;
            acc[neighbor][0] -= deltaX * particleScale;
            acc[neighbor][1] -= deltaY * particleScale;
            acc[neighbor][2] -= deltaZ * particleScale;
        }

        acc[particle][0] = accelerationX;
        acc[particle][1] = accelerationY;
        acc[particle][2] = accelerationZ;
    }

}

// For debug: save positions to a CSV file
void savePositionsToCSV(const double pos[][3], int N, int step, const std::string &filename) {
    std::ofstream file;
    
    // Open the file in append mode
    file.open(filename, std::ios_base::app);

    if (file.is_open()) {
        file << step << ",[";
        for (int i = 0; i < N; i++) {
            if (i != N - 1)
                file << "[" << pos[i][0] << "," << pos[i][1] << "," << pos[i][2] << "],";
            else
                file << "[" << pos[i][0] << "," << pos[i][1] << "," << pos[i][2] << "]";
        }
        file << "]\n";  // Newline for separation between steps
        file.close();
    } else {
        std::cerr << "Unable to open file!" << std::endl;
    }
}

int main(int argc, char *argv[]) {
    high_resolution_clock::time_point start;
    high_resolution_clock::time_point end;
    duration<double, std::milli> duration_sec;
    start = high_resolution_clock::now();

    // Check if correct number of arguments are provided
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <number_of_particles> <simulation_end_time>" << std::endl;
        return 1;
    }

    // Read N and tEnd from command line
    int N = std::stoi(argv[1]);     // Number of particles
    double tEnd = std::stod(argv[2]); // Time at which simulation ends

    // File to save positions
    std::string filename = "positions.csv";

    // Clear the file before starting simulation (optional)
    std::ofstream file;
    file.open(filename, std::ofstream::out | std::ofstream::trunc);
    file.close();

    // Allocate dynamic arrays based on N
    double* mass = new double[N];
    double(*pos)[3] = new double[N][3];
    double(*vel)[3] = new double[N][3];
    double(*acc)[3] = new double[N][3];

    // Create a random number engine
    std::mt19937 generator(std::random_device{}());

    // Create random distributions
    std::uniform_real_distribution<double> uniform_dist(0.0, 1.0);
    std::normal_distribution<double> normal_dist(0.0, 1.0);

    // Set initial masses and random positions/velocities
    for (int i = 0; i < N; i++) {
        mass[i] = uniform_dist(generator);

        pos[i][0] = normal_dist(generator);
        pos[i][1] = normal_dist(generator);
        pos[i][2] = normal_dist(generator);

        vel[i][0] = normal_dist(generator);
        vel[i][1] = normal_dist(generator);
        vel[i][2] = normal_dist(generator);
    }

    // Convert to Center-of-Mass frame
    double velCM[3] = {0.0, 0.0, 0.0};
    double totalMass = 0.0;
    for (int i = 0; i < N; i++) {
        velCM[0] += vel[i][0] * mass[i];
        velCM[1] += vel[i][1] * mass[i];
        velCM[2] += vel[i][2] * mass[i];
        totalMass += mass[i];
    }

    velCM[0] /= totalMass;
    velCM[1] /= totalMass;
    velCM[2] /= totalMass;

    for (int i = 0; i < N; i++) {
        vel[i][0] -= velCM[0];
        vel[i][1] -= velCM[1];
        vel[i][2] -= velCM[2];
    }

    // Initial accelerations
    getAcc(pos, mass, acc, N);

    // Number of timesteps
    int Nt = int(tEnd / dt);

    // Main simulation loop
    for (int step = 0; step < Nt; step++) {
        
        // (1/2) kick
        for (int particle = 0; particle < N; ++particle) {
            vel[particle][0] += acc[particle][0] * (0.5 * dt);
            vel[particle][1] += acc[particle][1] * (0.5 * dt);
            vel[particle][2] += acc[particle][2] * (0.5 * dt);
        }

        // Drift
        for (int particle = 0; particle < N; ++particle) {
            pos[particle][0] += vel[particle][0] * dt;
            pos[particle][1] += vel[particle][1] * dt;
            pos[particle][2] += vel[particle][2] * dt;
        }
      

        for (int particle = 0; particle < N; ++particle) {
            for (int axis = 0; axis < 3; ++axis) {
                if (pos[particle][axis] > board_size) {
                    pos[particle][axis] = board_size;
                } else if (pos[particle][axis] < -board_size) {
                    pos[particle][axis] = -board_size;
                }
            }
        }

        // Update accelerations
        getAcc(pos, mass, acc, N);

        for (int particle = 0; particle < N; ++particle) {
            vel[particle][0] += acc[particle][0] * (0.5 * dt);
            vel[particle][1] += acc[particle][1] * (0.5 * dt);
            vel[particle][2] += acc[particle][2] * (0.5 * dt);
        }

        // For debug: save positions to CSV at each step
         savePositionsToCSV(pos, N, step, filename);
    }

    // Clean up dynamically allocated memory
    delete[] mass;
    delete[] pos;
    delete[] vel;
    delete[] acc;

    end = high_resolution_clock::now();
    duration_sec = std::chrono::duration_cast<duration<double, std::milli> >(end - start);
    std::cout << "time: " << duration_sec.count() << "ms\n";

    return 0;
}
