#ifndef PROJECT_LONGSTAFFSCHWARTZ_HPP
#define PROJECT_LONGSTAFFSCHWARTZ_HPP


#include "Option.hpp"
#include "BlackScholes.hpp"

#define NBSAMPLE_DEFAULT 10000

class LongstaffSchwartz {
private:
    PnlMat *path; /// Path for each simulation
    PnlMat *path_P; /// Path for P during computing tau_0
    PnlMat *past; /// Past for each step to compute tau_0
    double discount_factor;

    /**
     * ComputeAmericanFlow : permit to compute the american flow for a path and for the option
     *
     * @param path : the simulating path
     * @return flow for american option
     */
    double ComputeAmericanFlow() const;

    /**
     * ComputeTau0 : permit to compute Tau_0 in an american option for a path
     *
     * @param path : the simulating path
     *
     * @return tau_0 for the american option
     */
    double ComputeTau0() const;

    /**
     * ComputeP : permit to compute the Bermudian price at ti
     * @param index : the index time to price (time = index * step)
     * @param path : simulation for the past values
     *
     * @return the bermudian price of the option
     */
    double ComputeP(int index) const;
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
    LongstaffSchwartz(Option *option, BlackScholes *model, int nbStep, int nbSample = NBSAMPLE_DEFAULT);
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
