#include "BlackScholes.hpp"

void BlackScholes::simulate(PnlMat *path, int nbStep) const{
    // Initialization
    double step = maturity/(double)nbStep;
    double sqrtStep = sqrt(step);
    // Compute path

}

void BlackScholes::simulate(double t, PnlMat *past, PnlMat *path, int nbStep) const{
    if (t == 0. || past == NULL)
        simulate(path,nbStep);

}

BlackScholes::BlackScholes(int nbAsset, double maturity, double frr, PnlVect *dividends, PnlVect *volatilities, PnlVect *spots,
                           PnlRng *rng, double correl)
        : maturity(maturity), frr(frr), dividends(dividends), volatilities(volatilities),
          rng(rng), spots(spots), nbAsset(nbAsset)
{
    cholCorrelMatrix = pnl_mat_create_from_scalar(nbAsset,nbAsset, correl);
    for (int i = 0; i < cholCorrelMatrix->m; ++i)
        MLET(cholCorrelMatrix,i,i) = 1.;
    pnl_mat_chol(cholCorrelMatrix);
}

BlackScholes::~BlackScholes() {
    pnl_mat_free(&cholCorrelMatrix);
}
