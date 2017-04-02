#ifndef PROJECT_LONGSTAFFSCHWARTZ_HPP
#define PROJECT_LONGSTAFFSCHWARTZ_HPP


#include <vector>
#include "../Option/Option.hpp"
#include "BlackScholes.hpp"
#include "pnl/pnl_basis.h"
#include "../Parser/parser.hpp"


#define NBSAMPLE_DEFAULT 10000

class LongstaffSchwartz {
private:
    PnlMat *path; /// Path for each simulation
    PnlMat *past; /// Past for each step to compute tau_0
    double discount_factor;
    std::vector<PnlMat *> *All_path; /// Vector of all path
    int degree; /// Degree of the polynomial function


    /**
     * getPayoffVect : permit to get the payoff Vector at time t
     *
     * @param
     */
    void getPayoffVect(PnlVect *pVect, std::vector<PnlMat *> *pVector, PnlMat *tau,
                                          int t) const;


public:
    /// Members
    Option *option; /// The option to price
    BlackScholes *model; /// The BS model to compute path
    int nbSample; /// Sample number for monte carlo method
    int nbStep; /// Number of step for the simulation

    /// Constructor / Destructor
    /**
     * LongstaffSchwartz : constructor
     *
     * @param option : the option to price
     * @param model : the BS model to compute path
     * @param nbStep : number of step for the simulation
     * @param nbSample : sample number for monte carlo method
     */
    LongstaffSchwartz(Option *option, BlackScholes *model, int nbStep, int degree, int nbSample = NBSAMPLE_DEFAULT);
    ~LongstaffSchwartz();

    /// Methods
    /**
     * European_price : Give the european price at 0 for the option
     *
     * @param[out] price : store the price at 0 of the option
     * @param[out] stddev : store the stddev for the price at 0 of the option
     */
    void European_price(double &price, double &stddev) const;
    /**
     * American_price : Give the american price at 0 for the option
     *
     * @param[out] price : store the price at 0 of the option
     * @param[out] stddev : store the stddev for the price at 0 of the option
     */
    void American_price(double &price, double &stddev) const;
};


#endif //PROJECT_LONGSTAFFSCHWARTZ_HPP
