#ifndef PROJECT_OPTION_HPP
#define PROJECT_OPTION_HPP

#include "pnl/pnl_matrix.h"

class Option {
public:
    /**
     * payoff : return the option payoff
     *
     * @param path : a path for the underlying assets
     * @return the payoff with this path
     */
    virtual double payoff(PnlMat *path, int timeIndex = path->m - 1) const = 0;
};


#endif //PROJECT_OPTION_HPP
