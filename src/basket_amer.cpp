#include <iostream>
#include "BlackScholes.hpp"
#include "LongstaffSchwartz.hpp"
#include "parser.hpp"
#include "BasketPut.hpp"
#include "pnl/pnl_finance.h"

using namespace std;

int main(int argc, char **argv){
    int size = 3;
    double frr = 0.05;
    double strike = 100;
    double maturity = 3.;
    PnlVect *spots = pnl_vect_create_from_scalar(size,100.);
    PnlVect *volatilities = pnl_vect_create_from_scalar(size,0.2);
    double correlation = 0.;
    PnlVect *dividends = pnl_vect_create_from_scalar(size,0.);
    PnlVect *weights = pnl_vect_create_from_scalar(size,0.3333);
    int nbStep = 10;
    int nbSamples = 10000;
    int degree = 3;
    PnlRng *rng = pnl_rng_create(PNL_RNG_MERSENNE);
    pnl_rng_sseed(rng, std::time(NULL));

    Option * basketPut = new BasketPut(weights,strike);
    BlackScholes *bsmodel =
            new BlackScholes(size,maturity,frr,dividends,volatilities,spots,rng,correlation);

    LongstaffSchwartz *mc = new LongstaffSchwartz(basketPut, bsmodel,nbStep,degree,nbSamples);

    double prix, stddev;
    mc->American_price(prix,stddev);
    //mc->European_price(prix,stddev);

    cout << "\nPRIX : " << prix << "\nSTDDEV : " << stddev << "\n";

    return EXIT_SUCCESS;
}