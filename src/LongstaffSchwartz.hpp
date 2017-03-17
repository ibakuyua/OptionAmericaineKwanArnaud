#ifndef PROJECT_LONGSTAFFSCHWARTZ_HPP
#define PROJECT_LONGSTAFFSCHWARTZ_HPP


#include "Option.hpp"
#include "BlackScholes.hpp"

#define NBSAMPLE_DEFAULT 10000

class LongstaffSchwartz {
private:
    PnlMat *path; /// Path for each simulation
    double discount_factor;

    /**
     * ComputeAmericanFlow : permit to compute the american flow for a path and for the option
     *
     * @param path : the simulating path
     * @return flow for american option
     */
    double ComputeAmericanFlow(PnlMat *path) const;

    /**
     * ComputeTau0 : permit to compute Tau_0 in an american option for a path
     *
     * @param path : the simulating path
     *
     * @return tau_0 for the american option
     */
    double ComputeTau0(PnlMat *path) const;
public:
    /// Members
    Option *option; /// The option to price
    BlackScholes *model; /// The BS model to compute path
    int nbSample; /// Sample number for monte carlo method
    int nbStep; /// Number of step for the simulation
    /// Constructor
    /**
     * LongstaffSchwartz : constructor
     *
     * @param option : the option to price
     * @param model : the BS model to compute path
     * @param nbStep : number of step for the simulation
     * @param nbSample : sample number for monte carlo method
     */
    LongstaffSchwartz(Option *option, BlackScholes *model, int nbStep, int nbSample = NBSAMPLE_DEFAULT);
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
