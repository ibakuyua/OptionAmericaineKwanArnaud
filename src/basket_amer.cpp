#include <iostream>
#include "BlackScholes.hpp"
#include "LongstaffSchwartz.hpp"
#include "parser.hpp"
#include "BasketPut.hpp"
#include "pnl/pnl_finance.h"

using namespace std;

int main(int argc, char **argv){
    int size = 3;
    double strike = 100;
    PnlVect *spots = pnl_vect_create_from_scalar(size,100.);
    double frr = 0.05;
    double maturity = 3.;
    PnlVect *volatilities = pnl_vect_create_from_scalar(size,0.2);
    double correlation = 0.;
    PnlVect *dividends = pnl_vect_create_from_scalar(size,0.);
    PnlVect *weights = pnl_vect_create_from_scalar(size,0.3333);
    int nbStep = 10;
    int nbSamples = 30000;
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

    //Freeing memory
    delete basketPut;
    delete bsmodel;
    delete mc;
    pnl_rng_free(&rng);
    pnl_vect_free(&weights);
    pnl_vect_free(&dividends);
    pnl_vect_free(&spots);
    pnl_vect_free(&volatilities);


    return EXIT_SUCCESS;
}