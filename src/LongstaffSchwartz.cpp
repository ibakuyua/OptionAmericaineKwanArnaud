#include "LongstaffSchwartz.hpp"

LongstaffSchwartz::LongstaffSchwartz(Option *option, BlackScholes *model,int nbStep, int nbSample)
        : option(option), model(model), nbSample(nbSample), nbStep(nbStep)
{
    discount_factor = exp(- model->frr * model->maturity);
    path = pnl_mat_new();
    path_P = pnl_mat_new();
    past = pnl_mat_new();
}

LongstaffSchwartz::~LongstaffSchwartz() {
    pnl_mat_free(&path);
    pnl_mat_free(&path_P);
    pnl_mat_free(&past);
}

void LongstaffSchwartz::European_price(double &price, double &stddev) const {
    double flow, espFlow = 0., varFlow = 0.;
    // Resize
    pnl_mat_resize(path, nbStep + 1, model->nbAsset);
    // Monte Carlo method
    for (int i = 0; i < nbSample; ++i) {
        model->simulate(path,nbStep);
        flow = option->payoff(path,nbStep);
        espFlow += flow;
        varFlow += flow * flow;
    }
    espFlow /= (double)nbSample;
    varFlow /= (double)nbSample;
    varFlow = fabs(varFlow - espFlow * espFlow);

    price = discount_factor * espFlow;
    stddev = discount_factor * sqrt(varFlow / (double)nbSample);
}

void LongstaffSchwartz::American_price(double &price, double &stddev) const {
    double flow, espFlow = 0., varFlow = 0.;
    // Resize
    pnl_mat_resize(path, nbStep + 1, model->nbAsset);
    pnl_mat_resize(path_P, nbStep + 1, model->nbAsset);
    // Monte Carlo method
    for (int i = 0; i < nbSample; ++i) {
        model->simulate(path,nbStep);
        flow = ComputeAmericanFlow();
        espFlow += flow;
        varFlow += flow * flow;
    }
    //TODO finir avec la moyenne
    espFlow /= nbSample;
    varFlow /= nbSample;
    varFlow = fabs(varFlow - espFlow * espFlow);

    price = espFlow;
    stddev = sqrt(varFlow / (double) nbSample);
}

double LongstaffSchwartz::ComputeAmericanFlow() const{

    double tau_0 = ComputeTau0();
    int indexTau_0 = (int)(tau_0 * (double)nbStep / model->maturity);

    return exp(- model->frr * tau_0) * option->payoff(path, indexTau_0);
}

double LongstaffSchwartz::ComputeTau0() const {
    // past initialization
    pnl_mat_clone(past,path);
    // other initializations
    double tau_i = model->maturity;
    double ti, step = model->maturity / (double)nbStep;
    double P_ti;
    for (int i = nbStep - 1; i >= 0; --i) {
        // Remove the last value of past
        pnl_mat_resize(past, past->m - 1, past->n);
        // Compute tau_i in function of tau_i+1
        ti = i * step;
        P_ti = ComputeP(ti);
        tau_i = (P_ti - option->payoff(path,i) <= 0.)
                ? ti
                : tau_i;
    }
    return tau_i;
}

double LongstaffSchwartz::ComputeP(int index) const {
    // Initialization
    int nbSampleP = nbSample / 100;
    double ti = index * model->maturity/(double)nbStep;
    double espflowCour = 0., espflowMax = -1;
    // For each possible tau
    for (int k = index; k < nbStep; ++k) {
        espflowCour = 0.;
        double tau_k = k * model->maturity/(double)nbStep;
        for (int m = 0; m < nbSampleP; ++m) {
            model->simulate(ti,past,path_P,nbStep);
            espflowCour += option->payoff(path_P, k);
        }
        espflowCour /= nbSampleP;
        espflowCour *= exp(- model->frr * (tau_k - ti));
        // Looking for the max
        if (espflowMax < espflowCour)
            espflowMax = espflowCour;
    }

    return espflowMax;
}
