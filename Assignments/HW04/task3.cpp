#include <iostream>
#include <fstream>
#include <cmath>
#include <chrono>
#include <cstdlib>
#include <random>
#include <string>
#include <omp.h>

using std::chrono::high_resolution_clock;
using std::chrono::duration;

const double G = 1.0;
const double softening = 0.1;
const double dt = 0.01;
const double board_size = 4.0;

void getAcc(const double pos[][3], const double mass[], double acc[][3], int N) {
    const double softeningSquared = softening * softening;

    #pragma omp parallel for schedule(runtime)
    for (int particle = 0; particle < N; ++particle) {
        const double positionX = pos[particle][0];
        const double positionY = pos[particle][1];
        const double positionZ = pos[particle][2];
        double accelerationX = 0.0;
        double accelerationY = 0.0;
        double accelerationZ = 0.0;

        for (int neighbor = 0; neighbor < N; ++neighbor) {
            if (neighbor == particle) {
                continue;
            }
            const double deltaX = pos[neighbor][0] - positionX;
            const double deltaY = pos[neighbor][1] - positionY;
            const double deltaZ = pos[neighbor][2] - positionZ;
            const double distanceSquared = deltaX * deltaX + deltaY * deltaY
                                         + deltaZ * deltaZ + softeningSquared;
            const double gravityScale = G * mass[neighbor]
                                      / (distanceSquared * std::sqrt(distanceSquared));
            accelerationX += deltaX * gravityScale;
            accelerationY += deltaY * gravityScale;
            accelerationZ += deltaZ * gravityScale;
        }

        acc[particle][0] = accelerationX;
        acc[particle][1] = accelerationY;
        acc[particle][2] = accelerationZ;
    }
}

void savePositionsToCSV(const double pos[][3], int N, int step, const std::string &filename) {
    std::ofstream file(filename, std::ios_base::app);
    if (file.is_open()) {
        file << step << ",[";
        for (int particle = 0; particle < N; ++particle) {
            if (particle != 0) {
                file << ",";
            }
            file << "[" << pos[particle][0] << "," << pos[particle][1]
                 << "," << pos[particle][2] << "]";
        }
        file << "]\n";
    } else {
        std::cerr << "Unable to open file!" << std::endl;
    }
}

int main(int argc, char *argv[]) {
    const auto start = high_resolution_clock::now();

    if (argc != 4) {
        std::cerr << "Usage: " << argv[0]
                  << " <number_of_particles> <simulation_end_time> <num_threads>\n";
        return 1;
    }

    const int N = std::stoi(argv[1]);
    const double tEnd = std::stod(argv[2]);
    const int numThreads = std::stoi(argv[3]);
    if (N <= 0 || !std::isfinite(tEnd) || tEnd < 0.0 || numThreads <= 0) {
        std::cerr << "Particle and thread counts must be positive; "
                  << "simulation end time must be finite and nonnegative.\n";
        return 1;
    }

    omp_set_dynamic(0);
    omp_set_num_threads(numThreads);
    if (std::getenv("OMP_SCHEDULE") == nullptr) {
        omp_set_schedule(omp_sched_static, 0);
    }

    const std::string filename = "positions.csv";
    const char* savePositionsSetting = std::getenv("NBODY_SAVE_POSITIONS");
    const bool savePositions = savePositionsSetting == nullptr
                            || std::string(savePositionsSetting) != "0";
    if (savePositions) {
        std::ofstream(filename, std::ofstream::out | std::ofstream::trunc).close();
    }

    double* mass = new double[N];
    double(*pos)[3] = new double[N][3];
    double(*vel)[3] = new double[N][3];
    double(*acc)[3] = new double[N][3];

    const char* seedSetting = std::getenv("NBODY_SEED");
    std::mt19937 generator(seedSetting == nullptr ? std::random_device{}()
        : static_cast<std::mt19937::result_type>(std::stoul(seedSetting)));
    std::uniform_real_distribution<double> uniform_dist(0.0, 1.0);
    std::normal_distribution<double> normal_dist(0.0, 1.0);

    for (int particle = 0; particle < N; ++particle) {
        mass[particle] = uniform_dist(generator);
        pos[particle][0] = normal_dist(generator);
        pos[particle][1] = normal_dist(generator);
        pos[particle][2] = normal_dist(generator);
        vel[particle][0] = normal_dist(generator);
        vel[particle][1] = normal_dist(generator);
        vel[particle][2] = normal_dist(generator);
    }

    double momentumX = 0.0;
    double momentumY = 0.0;
    double momentumZ = 0.0;
    double totalMass = 0.0;
    #pragma omp parallel for schedule(runtime) reduction(+:momentumX,momentumY,momentumZ,totalMass)
    for (int particle = 0; particle < N; ++particle) {
        momentumX += vel[particle][0] * mass[particle];
        momentumY += vel[particle][1] * mass[particle];
        momentumZ += vel[particle][2] * mass[particle];
        totalMass += mass[particle];
    }

    const double velCM[3] = {
        momentumX / totalMass,
        momentumY / totalMass,
        momentumZ / totalMass
    };

    #pragma omp parallel for schedule(runtime)
    for (int particle = 0; particle < N; ++particle) {
        vel[particle][0] -= velCM[0];
        vel[particle][1] -= velCM[1];
        vel[particle][2] -= velCM[2];
    }

    getAcc(pos, mass, acc, N);
    const int Nt = int(tEnd / dt);

    for (int step = 0; step < Nt; ++step) {
        #pragma omp parallel for schedule(runtime)
        for (int particle = 0; particle < N; ++particle) {
            vel[particle][0] += acc[particle][0] * (0.5 * dt);
            vel[particle][1] += acc[particle][1] * (0.5 * dt);
            vel[particle][2] += acc[particle][2] * (0.5 * dt);
        }

        #pragma omp parallel for schedule(runtime)
        for (int particle = 0; particle < N; ++particle) {
            pos[particle][0] += vel[particle][0] * dt;
            pos[particle][1] += vel[particle][1] * dt;
            pos[particle][2] += vel[particle][2] * dt;
        }

        #pragma omp parallel for schedule(runtime)
        for (int particle = 0; particle < N; ++particle) {
            for (int axis = 0; axis < 3; ++axis) {
                if (pos[particle][axis] > board_size) {
                    pos[particle][axis] = board_size;
                } else if (pos[particle][axis] < -board_size) {
                    pos[particle][axis] = -board_size;
                }
            }
        }

        getAcc(pos, mass, acc, N);

        #pragma omp parallel for schedule(runtime)
        for (int particle = 0; particle < N; ++particle) {
            vel[particle][0] += acc[particle][0] * (0.5 * dt);
            vel[particle][1] += acc[particle][1] * (0.5 * dt);
            vel[particle][2] += acc[particle][2] * (0.5 * dt);
        }

        if (savePositions) {
            savePositionsToCSV(pos, N, step, filename);
        }
    }

    delete[] mass;
    delete[] pos;
    delete[] vel;
    delete[] acc;

    const auto end = high_resolution_clock::now();
    const duration<double, std::milli> duration_sec = end - start;
    std::cout << "time: " << duration_sec.count() << "ms\n";
    return 0;
}
