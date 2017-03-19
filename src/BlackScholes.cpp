#include "BlackScholes.hpp"

void BlackScholes::simulate(PnlMat *path, int nbStep){
    // Initialization
    double step = maturity/(double)nbStep;
    double sqrtStep = sqrt(step);
    double sigma_d, Sd_tiMinus1, LdGi, Sd_ti;
    // Spot initialization
    for (int d = 0; d < path->n; ++d)
        PNL_MSET(path, 0, d, GET(spots,d));
    // Compute path
    for (int i = 1; i < path->m; ++i) {
        pnl_vect_rng_normal(Gi, path->n, rng);
        LGi = pnl_mat_mult_vect(cholCorrelMatrix, Gi);
        for (int d = 0; d < path->n; ++d) {
            sigma_d = GET(volatilities,d);
            Sd_tiMinus1 = PNL_MGET(path, (i-1), d);
            LdGi = GET(LGi,d);

            Sd_ti = Sd_tiMinus1 * exp((frr - sigma_d * sigma_d / 2) * step + sigma_d * sqrtStep * LdGi);
            MLET(path,i,d) = Sd_ti;
        }
    }

}

void BlackScholes::simulate(double t, PnlMat *past, PnlMat *path, int nbStep){
    if (t == 0. || past == NULL) {
        simulate(path, nbStep);
        return;
    }
    // Initialization
    double sigma_d, Sd_t, LdGi, value;
    // Copy the past matrix in path before S_t
    for (int i = 0; i < past->m - 1; ++i)
        for (int d = 0; d < path->n; ++d)
            PNL_MSET(path,i,d,MGET(past,i,d));
    // Get spot at t values
    pnl_mat_get_row(St, past, past->m - 1);
    double step = maturity/nbStep;
    double sqrtStep = sqrt(step);

    // For the first step
    int indexAftert = past->m - 1;
    double firstStep = MAX(step * indexAftert - t, 0.);
    double sqrtFirstStep = sqrt(firstStep);
    // Case when we are in a a step of our temporal grid
    if (firstStep < 0.0000001) {
        pnl_mat_set_row(path, St, indexAftert);
        pnl_vect_set_all(valuet_iminus1,1.);
    }
    else {
        // G_0 First gaussian vector
        pnl_vect_rng_normal_d(Gi,path->n,rng);
        // All the LdGi
        LGi = pnl_mat_mult_vect(cholCorrelMatrix,Gi);
        // For each asset
        for (int d = 0; d < path->n; ++d) {
            sigma_d = GET(volatilities,d);
            Sd_t = GET(St,d);
            LdGi = GET(LGi, d);

            value = exp((frr - sigma_d * sigma_d / 2) * firstStep + sigma_d * sqrtFirstStep * LdGi);
            // Store in path
            MLET(path, indexAftert, d) = Sd_t * value;
            // Store in S_timinus1
            LET(valuet_iminus1,d) = value;
        }
    }

    // For all other simulation
    for (int i = indexAftert + 1; i < path->m; ++i) {
        // Same process that besides but with different step
        pnl_vect_rng_normal_d(Gi,path->n, rng);
        LGi = pnl_mat_mult_vect(cholCorrelMatrix, Gi);
        for (int d = 0; d < path->n; ++d) {
            sigma_d = GET(volatilities,d);
            Sd_t = GET(St, d);
            LdGi = GET(LGi, d);

            value = GET(valuet_iminus1,d) * exp((frr - sigma_d * sigma_d / 2.) * step + sigma_d * sqrtStep * LdGi);
            // Store computed values
            PNL_MSET(path, i, d, Sd_t * value);
            LET(valuet_iminus1,d) = value;
        }
    }
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

    Gi = pnl_vect_new();
    LGi = pnl_vect_new();
    St = pnl_vect_new();
    valuet_iminus1 = pnl_vect_create(nbAsset);
}

BlackScholes::~BlackScholes() {
    pnl_mat_free(&cholCorrelMatrix);
    pnl_vect_free(&Gi);
    pnl_vect_free(&LGi);
    pnl_vect_free(&St);
    pnl_vect_free(&valuet_iminus1);
}
