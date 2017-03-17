#ifndef PROJECT_BASKETPUT_HPP
#define PROJECT_BASKETPUT_HPP

#include "Option.hpp"
#include "pnl/pnl_vector.h"

class BasketPut : public Option {
public:
    /// Members
    PnlVect *weights; /// The weight of each asset in the pay off
    double strike; /// The strike
    /// Constructor
    BasketPut(PnlVect *weights, double strike);
    /// Virtual method
    virtual double payoff(PnlMat *path) const;
};


#endif //PROJECT_BASKETPUT_HPP
