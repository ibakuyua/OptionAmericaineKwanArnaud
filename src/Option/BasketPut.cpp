#include <pnl/pnl_mathtools.h>
#include "BasketPut.hpp"


//Function pointer on payoff function to add in the polynomiale base L2
typedef struct {
    double strikeP;
    PnlVect* weightsP;
} Parameters;


double pointer_payoff(const PnlVect *S,void *Params){
    Parameters* params = (Parameters*)Params;
    double res = (params->strikeP - pnl_vect_scalar_prod(S, params->weightsP) > 0)
                 ? params->strikeP - pnl_vect_scalar_prod(S, params->weightsP)
                 : 0;
    return res;
}




double BasketPut::payoff(PnlMat *path, int timeIndex) const {
    double payoff = 0;
    for (int i = 0; i < path->n; ++i)
        payoff += GET(weights,i) * MGET(path,timeIndex,i);
    payoff = strike - payoff;

    return (payoff > 0)
           ? payoff
           : 0.;
}

BasketPut::BasketPut(PnlVect *weights, double strike)
        : weights(weights), strike(strike)
{

    //double (*ptr_payoff) (const PnlVect*, void*);
    //ptr_payoff = pointer_payoff;

    Parameters* params = new Parameters();
    params->strikeP = strike;
    params->weightsP = weights;

    rnFuncR = new PnlRnFuncR();
    rnFuncR->F = pointer_payoff;
    rnFuncR->params = (void*) params;
}

BasketPut::~BasketPut() {
    delete rnFuncR;
}
