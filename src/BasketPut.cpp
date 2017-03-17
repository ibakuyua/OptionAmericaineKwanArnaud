#include "BasketPut.hpp"

double BasketPut::payoff(PnlMat *path) const {
    double payoff = 0;
    for (int i = 0; i < path->n; ++i)
        payoff += GET(weights,i) * MGET(path,path->m-1,i);
    payoff = strike - payoff;

    return (payoff > 0)
           ? payoff
           : 0.;
}

BasketPut::BasketPut(PnlVect *weights, double strike)
        : weights(weights), strike(strike)
{
}
