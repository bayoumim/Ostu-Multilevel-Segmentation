/*******************************************************************************************************************************/
// Created by Mahmoud Atef on Saturday Oct 14, 2017.
// Implemenetation of Multilevel threshoding with:
// -----------------------------------------------------------------------------------------------------------------------------
// 1 - HCOCLPSO (hcoclpso option) : Implemenetation of the following paper
// Maitra, Madhubanti, and Amitava Chatterjee.
// "A hybrid cooperative–comprehensive learning based PSO algorithm for image segmentation using multilevel thresholding." 
// Expert Systems with Applications 34.2 (2008): 1341-1350. APA	
// -----------------------------------------------------------------------------------------------------------------------------
// 2 - CCQPSO (ccqpso option): implementation of the following paper
// Li, Y., Jiao, L., Shang, R., & Stolkin, R. (2015). 
// Dynamic-context cooperative quantum-behaved particle swarm optimization based on multilevel thresholding applied to medical image segmentation. 
// Information Sciences, 294, 408-422.
/*******************************************************************************************************************************/


#pragma once

#include "optAlg.h"

enum class PsoVariant {HCOCLPSO, CCQPSO}; 

class Particle {
public:
    uint8_t X;
    uint8_t pbest;
    uint8_t velocity;
    float pfit;
    
    bool operator<( const Particle& b)
    {
    // Define your logic here and return true if a is considered lesser than b.
        return pfit < b.pfit;
    }
};

class Pso : public optAlg {
private:
    PsoVariant psoAlgo;
    // parameter related to hcoclpso
    float IntialInteriaWeight;
    float intertiaSlope;
    Particle ** population;
    uint8_t maxVelocity;
    int ci;
    float selectionProbability;
    int stagnationLimit;
    int *stagnation;
    int Sreplace;
    // Parameters used in CCQPSO only
    double *mbest; 
    uint8_t * gbest;
public:
    Pso(Otsu *pb,
        int populationCount,
        int iterationCount,
        int verbosity,
        PsoVariant &psoAlgo,
        float IntialInteriaWeight = 0.9,
        float intertiaSlope = 2.4e-4,
        uint8_t maxVelocity = 6,
        int ci = 2,
        float selectionProbability = .2,
        int stagnationLimit = 3,
        int Sreplace = 10
        )
        : optAlg(pb, populationCount,iterationCount,verbosity)
    {
        this->psoAlgo = psoAlgo;
        this->IntialInteriaWeight = IntialInteriaWeight;
        this->intertiaSlope = intertiaSlope;
        this->maxVelocity = maxVelocity;
        this->ci = ci;
        this->selectionProbability = selectionProbability;
        this->stagnationLimit = stagnationLimit;
        this->Sreplace = Sreplace;
        
        population = new Particle*[parameter];
        
        for(int i =0; i < this->parameter; i++){
            population[i] = new Particle[this->populationCount];
        }
        // generate random population
        if(verbosity >= 3)  cout << "Generate Initial Population" << endl;
        mbest = new double[this->parameter];
        gbest = new uint8_t[this->parameter];
        for(int i =0; i< this->parameter; i++){
            int rnd_range = (pb->ub)[i] - (pb->lb)[i];
            mbest[i] = 0.0;
            for(int j = 0; j< this->populationCount; j++){
                population[i][j].X = rand() % rnd_range + (pb->lb)[i];
                population[i][j].pbest = population[i][j].X;
                population[i][j].velocity = rand() % (this->maxVelocity+1);
                population[i][j].pfit =  0.0 ;
                mbest[i] += population[i][j].pbest;
                gbest[i] = 0;
            }
            mbest[i] /= this->populationCount;
            if(verbosity >= 6)  cout << "mbest[" << i << "] = " << mbest[i] << endl;
        }
        
        // initialize context vector
        feasibleValidSolution(cv);
        
        stagnation = new int[this->parameter];
        for(int i =0; i < this->parameter; i++)
            stagnation[i] = 0;
    }
    
    void evolve(){
     //   float oldgfit = gfit;
        float alpha_init = 1.0;
        float alpha_final = 0.5;
        
        float delta = (alpha_final - alpha_init) / (iterationCount - 1);
        uint8_t *xi = new uint8_t[parameter];
        
        for(int i = 0; i < iterationCount; i++){ // for every iteration
            globalStagnationCount++;
            if(verbosity >= 2) 
                cout << "Iteration no. : " << i << endl;
            float wi = IntialInteriaWeight - intertiaSlope * i; // update inertia
            if(verbosity >= 6) cout << "wi = " << wi << endl;
            for(int j = 0; j < this->parameter; j++){ // for every swarm
                if(verbosity >= 6) cout << "swarm no. " << j << endl;
                // copy cv to xi
                if(verbosity >= 6) cout << "copying cv ..." << endl;
                
                for(int k = 0; k < parameter; k++){
                    xi[k] = cv[k];
                    if(verbosity >= 6) cout << "xi[" <<k << "] = " << (int)xi[k] << " "; 
                }
                if(verbosity >= 6) cout << endl;
                
                // cloning
          //      cout << "cloning for stagnation..." << endl;
                if(psoAlgo == PsoVariant::HCOCLPSO){ // clone stagnated swarms ONLY in HCOCLPSO
                    if(stagnation[j] >= stagnationLimit){
                        if(verbosity >= 6) cout << "stagnation happened for swarm no. " << j << endl;
                        stagnation[j] = 0;
                        sort(population[j],population[j]+populationCount);
                        if(verbosity >= 6){
                            cout << "sorted fitness: ";
                            for(int k = 0; k < populationCount; k++){
                                cout << setw(20) << population[j][k].pfit << " ";
                            }
                            cout << endl;
                        }
                        for(int k = 0; k < Sreplace; k++){
                            if(verbosity >= 6) cout << "replacing population[" << j << "][" << k << "] with fitness "<< population[j][k].pfit <<" by  population[" << j << "][" << populationCount - 1 -k << "] with fitness "<< population[j][populationCount - 1 -k].pfit  << endl;
                            population[j][k] = population[j][populationCount - 1 -k];
                        }
                    }
                }
            

                bool stagnationflag = true;
                for(int k = 0; k < populationCount; k++){ // for each particle in the swarm
                    // convert unfeasible solution to feasible ones.
                    if(verbosity >= 6) cout << "Particle no. " << k << endl;

                    if (j != this->parameter-1 && population[j][k].X > min(cv[j+1]-1, (int)pb->ub[j])  ){
                        if(verbosity >= 6) cout << "Value of particle is greater than the upper threshold. population[" << j << "][" << k << "].X =" << (int)population[j][k].X << ". cv[" << j+1 << "]=" << (int)cv[j+1] << endl;
                        if(verbosity >= 6) cout << "setting population[" << j << "][" << k << "].X to " << cv[j+1]  - 1 << endl;
                        population[j][k].X =  min(cv[j+1]-1, (int)pb->ub[j]);
                    }
                    else if(j == this->parameter-1 && population[j][k].X > (int)pb->ub[j] ){
                        population[j][k].X =  (int)pb->ub[j];
                    }
                    if (j != 0 && population[j][k].X <  max(cv[j-1]+1, (int)pb->lb[j])  ){
                        if(verbosity >= 6) cout << "Value of particle is greater than the upper threshold. population[" << j << "][" << k << "].X =" << (int)population[j][k].X << ". cv[" << j-1 << "]=" << (int)cv[j-1] << endl;
                        if(verbosity >= 6) cout << "setting population[" << j << "][" << k << "].X to " << cv[j-1]  + 1 << endl;
                        population[j][k].X = max(cv[j-1]+1, (int)pb->lb[j]);
                    }
                    else if (j == 0 && population[j][k].X <  (int)pb->lb[j] ){
                        population[j][k].X = (int)pb->lb[j];
                    }
                    
                    if(verbosity >= 6) cout << "setting xi[" << j << "] to " << "population[" << j << "][" << k << "].X =" << (int)population[j][k].X << endl;
                    
                    xi[j] = population[j][k].X; // copy particle to cv
          //          cout << "evaluate fitness..." << endl;
                    float fit = pb->fitness(xi);
                    if(verbosity >= 6) cout << "fitness function for xi = " << fit << endl;
                    
                    if(fit > population[j][k].pfit){
                        if(verbosity >= 6)  cout << "found new pfit is found = " << fit <<endl;
                        mbest[j] = (mbest[j] * populationCount + population[j][k].X - population[j][k].pbest ) / populationCount;

                        population[j][k].pbest = population[j][k].X;
                        population[j][k].pfit = fit;
                    }

                    if(fit > gfit){
                        if(verbosity >= 6)  cout << "found new gfit is found = " << fit <<endl;
                        if(verbosity >= 6) cout << "setting cv[" << j << "] to " << "population[" << j << "][" << k << "].X =" << (int)population[j][k].X << endl;
                        cv[j] =  population[j][k].X;
                        gfit = fit;
                        stagnationflag = false;
                        globalStagnationCount = 0;
                        gbest[j] = population[j][k].X;
                    }
                    
                    if(verbosity >= 6)  cout << "update velocity..." << endl;
                    // calculate velocity
         //           cout << "generate random numbers" <<endl;
                    if(psoAlgo == PsoVariant::HCOCLPSO){ 
                        float r1 = (double) rand() / (RAND_MAX);
                        if(verbosity >= 6) cout << "r1 = " << r1 << endl;
                   //     float r2 = (double) rand() / (RAND_MAX);
                        //
                        float r3 = (double) rand() / (RAND_MAX);
                        if(verbosity >= 6)  cout << "r3 = " << r3 << endl;
                        // comprehensive learning
                        if(verbosity >= 6) cout << "apply selection criteria" << endl;
                        uint8_t pbest;
                        if(r3 > selectionProbability){
                            if(verbosity >= 6) cout << "individual best solution is selected." << endl;
                            pbest = population[j][k].pbest;
                        }
                        else {
                            int k1 = rand() % populationCount;
                            int k2 = rand() % populationCount;
                            if(verbosity >= 6) cout << "individual at index k1 = " << k1 << ". and k2 = " << k2 << " are generated" << endl;
                            if(verbosity >= 6) cout << "individual k1 fitness is " << population[j][k1].pfit << endl;
                            if(verbosity >= 6) cout << "individual k2 fitness is " << population[j][k2].pfit << endl;
                            if (population[j][k1].pfit > population[j][k2].pfit){
                                if(verbosity >= 6) cout << "k1 is selected." << endl;
                                if(verbosity >= 6) cout << "individual k1 pbest is " << (int)population[j][k1].pbest << endl;
                                pbest = population[j][k1].pbest;
                            }
                            else{
                                if(verbosity >= 6) cout << "k2 is selected." << endl;
                                if(verbosity >= 6) cout << "individual k2 pbest is " << (int)population[j][k2].pbest << endl;
                                pbest = population[j][k2].pbest;
                            }
                        }
               //         cout << "calculate new velocity.  i = " << i << ". j = " << j << ". k = " << k << endl;  
                        int vi = round( wi * population[j][k].velocity + ci * r1 * (pbest - population[j][k].X)); 
                        if(verbosity >= 6) cout << "vi = " << vi <<endl;

                        if(vi >= 0)
                            vi = min( vi,(int) maxVelocity);
                        else
                            vi = -min( abs(vi),(int) maxVelocity);

                        if(verbosity >= 6) cout << "final vi =  " << vi << endl;
                        population[j][k].X = population[j][k].X + vi;
                    }
                    else if (psoAlgo == PsoVariant::CCQPSO) {
                        float u = (double) rand() / (RAND_MAX);
                        if(verbosity >= 6) cout << "u = " << u << endl;

                        
                        float phi = (double) rand() / (RAND_MAX);
                        if(verbosity >= 6) cout << "phi = " << phi << endl;
                        
                        float P = phi * population[j][k].pbest + (1-phi) * gbest[j];
                        if(verbosity >= 6) cout << "P = " << P <<endl;
                        
                        float r = (double) rand() / (RAND_MAX);
                        float alpha = alpha_init - j * delta;
                        if(r < .5){
                            alpha = -alpha;
                        }
                        int y = P + alpha * (mbest[j] - population[j][k].X ) * log(1/u);
                        population[j][k].X = (uint8_t)( y % (int)pb->ub[j]);
                    }
                }
                
                if(stagnationflag)
                    stagnation[j]++;
            }
            
            // stoping criteria
 /*           if(gfit !=  oldgfit){
                globalStagnationCount = 0;
                oldgfit = gfit;
            }
            else
                globalStagnationCount++;*/
            if(globalStagnationCount == 10){
                elapsedIterations = i;
                if(verbosity >= 1) cout << "PSO stopped after " << i << " iterations." << endl; 
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
        if(verbosity >= 6) cout << "deleting xi ..."  << endl;
        delete [] xi;
        if(verbosity >= 6) cout << "deleting xi is sucessful."  << endl;

    }
   ~Pso(){
        for(int i =0; i < this->parameter; i++){
            delete population[i] ;
        }
       delete [] population;
       delete [] stagnation;
   };
};
