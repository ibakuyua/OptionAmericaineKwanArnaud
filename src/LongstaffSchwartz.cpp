#include "LongstaffSchwartz.hpp"

LongstaffSchwartz::LongstaffSchwartz(Option *option, BlackScholes *model,int nbStep, int degree, int nbSample)
        : option(option), model(model), nbSample(nbSample), degree(degree), nbStep(nbStep)
{
    discount_factor = exp(- model->frr * model->maturity);
    path = pnl_mat_new();
    past = pnl_mat_new();

    All_trajectories = new std::vector<PnlMat *>(nbStep+1);
    for(int i = 0; i < nbStep+1; ++i){
        (*All_trajectories)[i] = pnl_mat_create(nbSample,model->nbAsset);
    }

}

LongstaffSchwartz::~LongstaffSchwartz() {
    pnl_mat_free(&path);
    pnl_mat_free(&past);
    for(int i = 0; i < nbStep + 1; ++i){
        pnl_mat_free(&(*All_trajectories)[i]);
    }
    delete All_trajectories;
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


    //Creation of tau matrix
    PnlMat *tau = pnl_mat_create_from_scalar(nbSample, nbStep + 1,model->maturity);
    //Vector of coefficient of polynom
    PnlVect* alphaV = pnl_vect_new();
    //Payoff Vector at tau
    PnlVect* payoffVect = pnl_vect_new();
    //TimeStep
    double timeStep = model->maturity/(double)nbStep;

    //Simulation of all trajectories
    for (int i = 0; i < nbSample; i++) {
        model->simulate((*All_trajectories)[i],nbStep,);
    }

    //For all possible execution dates
    for (int n = nbStep - 1; n > 0; --n) {
        //Getting payoff at tau_t+1
        getPayoffVect(payoffVect,All_trajectories,tau,n+1);
        //Finding the alpha that minimizes the squarred problems
        //TODO : add the payoff function to the base L^2
        pnl_basis_fit_ls(G,alphaV,(*All_trajectories)[n],payoffVect);
        //Filling tau at time n
        for (int i = 0; i < nbSample; ++i) {
            double tn = n * timeStep;

            double a = exp(- model->frr * tn) * option->payoff((*All_trajectories)[n],i);
            pnl_mat_get_row(g_St,(*All_trajectories)[n],i);
            //Getting scalar product alphaV * g
            double b = pnl_basis_eval_vect(G,alphaV,g_St);

            //Managing the indicatrix condition
            MLET(tau,i,n) = (a>b)
                            ? tn
                            : MGET(tau,i,n+1);
        }
    }

    //Finally doing MonteCarlo on price
    for (int j = 0; j < nbSample; ++j) {
        double tau_1 = MGET(tau,1,j);
        double timeIndexForTau_1 = tau_1 / timeStep;
        flow = exp(-model->frr * tau_1) * option->payoff((*All_trajectories)[timeIndexForTau_1],j);
        espFlow += flow;
        varFlow += flow * flow;
    }

    espFlow /= nbSample;
    varFlow /= nbSample;
    varFlow = fabs(varFlow - espFlow * espFlow);

    price = espFlow;
    stddev = sqrt(varFlow / (double) nbSample);

    //At 0 we just compute payoff as a simple esperanza
    double payoffAt0 = option->payoff((*All_trajectories)[0],0);

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

