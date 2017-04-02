#include <pnl/pnl_mathtools.h>
#include <iostream>
#include "LongstaffSchwartz.hpp"






LongstaffSchwartz::LongstaffSchwartz(Option *option, BlackScholes *model,int nbStep, int degree, int nbSample)
        : option(option), model(model), nbSample(nbSample), degree(degree), nbStep(nbStep)
{
    discount_factor = exp(- model->frr * model->maturity);
    path = pnl_mat_new();
    past = pnl_mat_new();

    All_path = new std::vector<PnlMat *>(nbStep+1);
    for(int i = 0; i < nbStep+1; ++i){
        (*All_path)[i] = pnl_mat_create(nbSample,model->nbAsset);
    }

}

LongstaffSchwartz::~LongstaffSchwartz() {
    pnl_mat_free(&path);
    pnl_mat_free(&past);
    for(int i = 0; i < nbStep + 1; ++i){
        pnl_mat_free(&(*All_path)[i]);
    }
    delete All_path;
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

    //Functions g for the polynomial base
    PnlBasis *G = pnl_basis_create_from_degree(PNL_BASIS_CANONICAL, degree, model->nbAsset);
    PnlVect *g_St = pnl_vect_new();

    //As seen in course, setting the interval [a,b] in which S_ti is in with high probability
    double volatility = GET(model->volatilities,0);
    double squareVolatility = volatility * volatility;
    double drift = (model->frr - (squareVolatility/2)) * model->maturity;
    double S0 = GET(model->spots,0);

    double a = S0 * exp(MAX(drift,0) - 3 * volatility * sqrt(model->maturity));
    double b = S0 * exp(MIN(drift,0) - 3 * volatility * sqrt(model->maturity));

    PnlVect*aV = pnl_vect_create_from_scalar(model->nbAsset,a);
    PnlVect*bV = pnl_vect_create_from_scalar(model->nbAsset,b);
    //Setting domain
    pnl_basis_set_domain (G, bV, aV);

    //Adding payoff function to the polynomial base G
    pnl_basis_add_function(G,option->rnFuncR);

    //Creation of tau matrix
    PnlMat *tau = pnl_mat_create_from_scalar(nbSample, nbStep + 1,model->maturity);
    //Vector of coefficient of polynom
    PnlVect* alphaV = pnl_vect_new();
    //Payoff Vector at tau
    PnlVect* payoffVect = pnl_vect_new();
    //TimeStep
    double timeStep = model->maturity/(double)nbStep;

    //Simulation of all trajectories
    for (int i = 0; i < nbSample; i++)
        model->simulatePath(All_path, nbStep, i);

    //For all possible execution dates
    for (int n = nbStep - 1; n > 0; --n) {
        //Getting payoff at tau_t+1
        getPayoffVect(payoffVect,All_path,tau,n+1);
        //Finding the alpha that minimizes the squarred problems
        //Regression
        pnl_basis_fit_ls(G,alphaV,(*All_path)[n],payoffVect);

        //Filling tau at time n
        for (int i = 0; i < nbSample; ++i) {
            double tn = n * timeStep;

            double c1 = exp(- model->frr * tn) * option->payoff((*All_path)[n],i);
            //Getting scalar product alphaV * g
            pnl_mat_get_row(g_St,(*All_path)[n],i);
            double c2 = pnl_basis_eval_vect(G,alphaV,g_St);

            //Managing the indicatrix condition
            MLET(tau,i,n) = (c1>c2)
                            ? tn
                            : MGET(tau,i,n+1);
        }
    }

    //Finally doing MonteCarlo on price
    for (int j = 0; j < nbSample; ++j) {
        double tau_1 = MGET(tau,j,1);
        double timeIndexForTau_1 = tau_1 / timeStep;
        flow = exp(-model->frr * tau_1) * option->payoff((*All_path)[timeIndexForTau_1],j);
        espFlow += flow;
        varFlow += flow * flow;
    }

    espFlow /= nbSample;
    varFlow /= nbSample;
    varFlow = fabs(varFlow - espFlow * espFlow);

    price = espFlow;
    stddev = sqrt(varFlow / (double) nbSample);

    //At 0 we just compute payoff as a simple esperanza
    double payoffAt0 = option->payoff((*All_path)[0],0);

    //Final price
    price = (price > payoffAt0)
            ? price
            : payoffAt0;

    //Freeing memory
    pnl_basis_free(&G);
    pnl_vect_free(&g_St);
    pnl_mat_free(&tau);
    pnl_vect_free(&alphaV);
    pnl_vect_free(&payoffVect);
}

void LongstaffSchwartz::getPayoffVect(PnlVect *pVect, std::vector<PnlMat *> *pVector, PnlMat *tau,
                                 int t) const {
    for(int i = 0; i < pVect->size; ++i) {
        double tau_i = MGET(tau,i,t);
        double timeIndexForTau_i = tau_i * ((double)nbStep/model->maturity);
        LET(pVect, i) = exp(-model->frr * tau_i)
                      * option->payoff((*pVector)[timeIndexForTau_i],i);
    }
}