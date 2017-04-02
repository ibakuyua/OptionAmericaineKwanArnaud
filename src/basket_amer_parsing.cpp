#include "parser.hpp"
#include "LongstaffSchwartz.hpp"
#include "BasketPut.hpp"
#include <pnl/pnl_mathtools.h>


using namespace std;

int main(int argc, char **argv) {


    int size;
    double strike;
    PnlVect *spots;
    double maturity;
    PnlVect *volatilities;
    double frr;
    double correlation;
    PnlVect *dividends;

    PnlVect *weights;

    int nbStep;
    int nbSamples;
    int degree;

    char *infile = argv[1];
    Parser * parser = new Parser(infile);

    parser->extract("size",size);
    parser->extract("strike",strike);
    parser->extract("spot",spots,size);
    parser->extract("maturity",maturity);
    parser->extract("volatility",volatilities,size);
    parser->extract("interest rate",frr);
    parser->extract("correlation",correlation);
    parser->extract("dividend rate",dividends,size);
    parser->extract("payoff coefficients",weights,size);
    parser->extract("dates", nbStep);
    parser->extract("MC iterations", nbSamples);
    parser->extract("degree for polynomial regression",degree);



    PnlRng *rng = pnl_rng_create(PNL_RNG_MERSENNE);
    pnl_rng_sseed(rng, std::time(NULL));

    Option * basketPut = new BasketPut(weights,strike);
    BlackScholes *bsmodel =
            new BlackScholes(size,maturity,frr,dividends,volatilities,spots,rng,correlation);

    LongstaffSchwartz *mc = new LongstaffSchwartz(basketPut, bsmodel,nbStep,degree,nbSamples);

    double prix, stddev;
    cout << "Computing price..." << "\n";
    mc->American_price(prix,stddev);

    cout << "\nPRICE : " << prix << "\nSTDDEV : " << stddev << "\n";

    //Freeing memory
    delete basketPut;
    delete bsmodel;
    delete mc;
    pnl_rng_free(&rng);
    pnl_vect_free(&weights);
    pnl_vect_free(&dividends);
    pnl_vect_free(&spots);
    pnl_vect_free(&volatilities);
    delete parser;


    return EXIT_SUCCESS;
}
