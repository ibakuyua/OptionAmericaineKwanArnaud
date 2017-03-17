#include "LongstaffSchwartz.hpp"

LongstaffSchwartz::LongstaffSchwartz(Option *option, BlackScholes *model,int nbStep, int nbSample)
        : option(option), model(model), nbSample(nbSample), nbStep(nbStep)
{
    discount_factor = exp(- model->frr * model->maturity);
}

void LongstaffSchwartz::European_price(double &price, double &stddev) const {
    double flow, espFlow = 0., varFlow = 0.;
    // Resize
    pnl_mat_resize(path, nbStep + 1, model->nbAsset);
    // Monte Carlo method
    for (int i = 0; i < nbSample; ++i) {
        model->simulate(path,nbStep);
        flow = option->payoff(path);
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
    // Monte Carlo method
    for (int i = 0; i < nbSample; ++i) {
        model->simulate(path,nbStep);
        flow = ComputeAmericanFlow(path);
        espFlow += flow;
        varFlow += flow * flow;
    }
    //TODO finir avec la moyenne
}

double LongstaffSchwartz::ComputeAmericanFlow(PnlMat *path) const{

    double tau_0 = ComputeTau0(path);
    int indexTau_0 = (int)(tau_0 * (double)nbStep / model->maturity);

    return exp(- model->frr * tau_0) * option->payoff(path, indexTau_0);
}

double LongstaffSchwartz::ComputeTau0(PnlMat *path) const {
    return 0;
}
