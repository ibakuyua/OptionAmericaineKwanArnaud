#include "parser.hpp"
#include "LongstaffSchwartz.hpp"
#include "BasketPut.hpp"


//TODO : Renommer ce fichier test en basket_amer pour le rendu

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

    parser->extract("model size",size);
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

    cout << "Taille : " << size<<endl;
    cout << "Strike : " << strike<<endl;
    cout << "Vector of spots : " << endl;
    pnl_vect_print(spots);
    cout << "Maturity : " << maturity<<endl;
    cout <<"Vector of volatility : "<<endl;
    pnl_vect_print(volatilities);
    cout << "Free Risk Rate : " <<frr<<endl;
    cout << "Correlation : " <<correlation<<endl;
    cout << "Vector of dividends : " << endl;
    pnl_vect_print(dividends);
    cout << "Vector of wieghts : " << endl;
    pnl_vect_print(weights);
    cout << "Step number : " << nbStep<<endl;
    cout << "Sample Number : " << nbSamples<<endl;
    cout << "Degree of polynome : " <<degree<< endl;

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
    //delete basketPut; //Problem il delete un truc abstrait il dit...
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
