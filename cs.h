/*******************************************************************************************************************************/
// Created by Mahmoud Atef on Saturday Oct 16, 2017.
// Implemenetation of Multilevel threshoding with:
// -----------------------------------------------------------------------------------------------------------------------------
// 1 - Mantegna Method (csm option): 
// -----------------------------------------------------------------------------------------------------------------------------
// 2 - McCullen Method (csmc option): implementation of the following paper
// Suresh, Shilpa, and Shyam Lal. 
// "An efficient cuckoo search algorithm based multilevel thresholding for segmentation of satellite images using different objective functions." 
// Expert Systems with Applications 58 (2016): 184-209.
/*******************************************************************************************************************************/
#pragma once

#include "optAlg.h"
#include <random>

class egg{
    
public:
    void init(int par){
        X = new uint8_t[par];
    }
    ~egg(){        
        delete [] X;     
    }
    uint8_t * X;
    float pfit;
};

class Cs : public optAlg {
private:
    float alienEggDiscoveryRate;
    bool enableMcCullochMethod;
    egg * nest;

    
    void get_cuckoos(){
        if(verbosity >= 2) cout << "Get cuckoos..." << endl;

        double pi = 3.1415926535897;
        float beta = 1.5;
        float sigma = pow(tgammaf(beta+1)* sin(pi*beta/2) / ( tgammaf((beta+1)/2) * beta * pow(2,(beta-1)/2)),1/beta);
        std::default_random_engine generator;
        std::normal_distribution<double> distribution(0.0,1.0);
        
        float stepsize ;
        uint8_t *tmpX = new uint8_t[pb->parameter];
        for(int i = 0 ; i < populationCount; i++){
            if(verbosity >= 6) cout << "egg no. " << i << endl;
            for(int j = 0; j < pb->parameter; j++){
                if(verbosity >= 6) cout << "parameter no. " << j << endl;
                int y;
                if(enableMcCullochMethod){
                    float alpha = 1.5;
                    float phi = ((double) rand() / (RAND_MAX) - 0.5) * pi;
                    float omega = - log10(distribution(generator)) ;
                    float delta = ( nest[i].X[j] - cv[j] );
                    float c = 1;
                    stepsize = c * pow(cos((1-alpha)* phi) / omega , 1/alpha -1) * pow(sin(alpha * phi) / cos(phi),1/alpha) + delta;
                    y = (int)nest[i].X[j] + stepsize;
                }
                else {
                    float u = distribution(generator) * sigma;
                    float v = distribution(generator);
                    float step = u/pow(abs(v),1/beta);
                    stepsize = 0.01 * step * ( nest[i].X[j] - cv[j] );
                    y = (int)nest[i].X[j] + stepsize*distribution(generator);
                }
                uint8_t minX; 
                if(j == 0 )
                    minX = pb->lb[0];
                else
                    minX = max(tmpX[j-1] + 1, (int)pb->lb[j] );
                tmpX[j] = (uint8_t) max(  y % ((int)pb->ub[j] + 1) , (int) minX ); 
            }
            float fit = pb->fitness(tmpX);
            if(fit> nest[i].pfit){
                // update with the new vector and fitness
                if(verbosity >= 6)  cout << "found new pfit is found = " << fit <<endl;
                copyTo(tmpX,nest[i].X,pb->parameter);
                nest[i].pfit = fit;
            }
            if(fit> gfit){
                // update with the new vector and fitness
                if(verbosity >= 6)  cout << "found new gfit is found = " << fit <<endl;
              //  if(verbosity >= 6) cout << "setting cv[" << j << "] to " << "population[" << j << "][" << k << "].X =" << (int)population[j][k].X << endl;
                copyTo(tmpX,cv,pb->parameter);
                gfit = fit;
                globalStagnationCount = 0;
            }
        }
        delete [] tmpX;
    }

    void empty_nests(){
        if(verbosity >= 2) cout << "empty nest..." << endl;
        std::default_random_engine generator;
        std::normal_distribution<double> distribution(0.0,1.0);
        
        float stepsize ;
        uint8_t *tmpX = new uint8_t[pb->parameter];
        for(int i = 0 ; i < populationCount; i++){
            if(verbosity >= 6) cout << "egg no. " << i << endl;
            int i1 = rand() % populationCount;
            int i2 = rand() % populationCount;
            for(int j = 0; j < pb->parameter; j++){
                if(verbosity >= 6) cout << "parameter no. " << j << endl;
                float r1 = (double) rand() / (RAND_MAX);
                if(verbosity >= 6) cout << "r1 = " << r1 << endl;
                int K = ((double) rand() / (RAND_MAX)) > alienEggDiscoveryRate;
                if(verbosity >= 6) cout << "K =  " << K << endl;
                stepsize = r1  * ( nest[i1].X[j] - nest[i2].X[j] );
                if(verbosity >= 6) cout << "stepsize =  " << stepsize << endl;
                int y = (int)nest[i].X[j] + stepsize*K;
                if(verbosity >= 6) cout << "y =  " << y << endl;
                uint8_t minX; 
                if(j ==0 )
                    minX = pb->lb[0];
                else
                    minX = max(tmpX[j-1] + 1, (int)pb->lb[j] );
                if(verbosity >= 6) cout << "minX =  " << (int)minX << endl;
                tmpX[j] = (uint8_t) max(  y % ((int)pb->ub[j] + 1) , (int) minX ); 
                if(verbosity >= 6) cout << "tmpX[" << j << "] = " << (int)tmpX[j] << endl;
            }
            float fit = pb->fitness(tmpX);
            if(fit> nest[i].pfit){
                // update with the new vector and fitness
                if(verbosity >= 6)  cout << "found new pfit is found = " << fit <<endl;
                copyTo(tmpX,nest[i].X,pb->parameter);
                nest[i].pfit = fit;
            }
            if(fit> gfit){
                // update with the new vector and fitness
                if(verbosity >= 6)  cout << "found new gfit is found = " << fit <<endl;
              //  if(verbosity >= 6) cout << "setting cv[" << j << "] to " << "population[" << j << "][" << k << "].X =" << (int)population[j][k].X << endl;
                copyTo(tmpX,cv,pb->parameter);
                gfit = fit;
                globalStagnationCount = 0;
            }
        }
        delete [] tmpX;
    }

    
    public:
    Cs(    Otsu *pb,
             int populationCount,
             int iterationCount,
             int verbosity,
             float alienEggDiscoveryRate,
             bool enableMcCullochMethod = false
         
        )
        : optAlg(pb, populationCount,iterationCount,verbosity)
    {
        this->alienEggDiscoveryRate = alienEggDiscoveryRate;
        this->enableMcCullochMethod = enableMcCullochMethod;
        nest = new egg[populationCount];
        // generate n eggs and initiate it with valid solutions, compute its fitness
		
        for(int i = 0; i < populationCount; i++){
            nest[i].init(pb->parameter);
            feasibleValidSolution(nest[i].X);
			float fit = pb->fitness(nest[i].X);
            nest[i].pfit = fit;
            if(fit > gfit){
                gfit = fit;
                copyTo(nest[i].X,cv,pb->parameter);
                
            }
        }        
    }
    
    void evolve(){
        for(int i = 0; i < iterationCount; i++){ // for every iteration
            globalStagnationCount++;
            if(verbosity >= 2) 
                cout << "Iteration no. : " << i << endl;
            // Generate new solutions (but keep the current best)
            get_cuckoos();
            // Discovery and randomization
            empty_nests();
            
            if(globalStagnationCount == 10){
                elapsedIterations = i;
                if(verbosity >= 1) cout << "Cs stopped after " << i << " iterations." << endl; 
                break;
            }
        }
        
        if(verbosity >= 1) {
             cout << "Best solution: " ;
            for(int k = 0; k < parameter; k++){
                cout << (int)cv[k] << " "; 
            }
            cout << endl;
            if (verbosity >= 2) cout << "best fitness: " << gfit << endl;
        }        
    }
    
};