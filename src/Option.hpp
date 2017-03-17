#ifndef PROJECT_OPTION_HPP
#define PROJECT_OPTION_HPP

#include "pnl/pnl_matrix.h"

class Option {
public:
    /**
     * payoff : return the option payoff
     *
     * @param path : a path for the underlying assets
     * @param timeIndex : pay off at which index time ? (time = index * step)
     * @return the payoff with this path
     */
    virtual double payoff(PnlMat *path, int timeIndex) const = 0;
};


#endif //PROJECT_OPTION_HPP
