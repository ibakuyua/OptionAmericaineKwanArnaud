#include "BlackScholes.hpp"

void BlackScholes::simulate(PnlMat *path, int nbStep) {
    // Initialization
    double step = maturity/(double)nbStep;
    double sqrtStep = sqrt(step);
    // Compute path

}

void BlackScholes::simulate(double t, PnlMat *past, PnlMat *path, int nbStep) {
    if (t == 0. || past == NULL)
        simulate(path,nbStep);

}
