#ifndef PROJECT_BLACKSCHOLES_HPP
#define PROJECT_BLACKSCHOLES_HPP

#include "pnl/pnl_matrix.h"
#include "pnl/pnl_random.h"

class BlackScholes {
private:
    PnlMat *cholCorrelMatrix; /// Matrix of cholesky correlation matrix
public:
    /// Members
    int nbAsset; /// Number of asset
    double maturity; /// Maturity
    double frr; /// Free risk rate
    PnlVect *dividends; /// Dividend for each asset
    PnlVect *volatilities; /// Volatility for each asset
    PnlVect *spots; /// Spot t=0 for each asset
    PnlRng *rng; /// Random generator

    /// Constructor / Destructor
    /**
     * BlackScholes : constructor
     *
     * @param nbAsset : number of underlying asset
     * @param maturity : maturity T
     * @param frr : the free interest rate
     * @param dividends : dividend for each asset
     * @param volatilities : volatility for each asset
     * @param spots : spot t=0 for each asset
     * @param rng : the random generator
     * @param correl : global correlation for underlying assets
     */
    BlackScholes(int nbAsset, double maturity, double frr,
                 PnlVect *dividends, PnlVect *volatilities, PnlVect *spots,
                 PnlRng *rng, double correl);
    /**
     * ~BlackScholes : destructor
     */
    ~BlackScholes();

    /// Methods
    /**
     * simulate : permit to simulate a blackscholes path at t=0
     *
     * @param path : the simulate path
     * @param nbStep : the number of step
     */
    void simulate(PnlMat *path, int nbStep) const;

    /**
     * simulate : permit to simulate a blackscholes path at t
     *
     * @param t : time to simulate
     * @param past : the past matrix with St
     * @param path : the simulate path
     * @param nbStep : the number of step
     */
    void simulate(double t, PnlMat *past, PnlMat *path, int nbStep) const;

};


#endif //PROJECT_BLACKSCHOLES_HPP
