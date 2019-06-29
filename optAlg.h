
#pragma once
#include "Otsu.h"

class optAlg {
public:
    float gfit;  // best fitness
protected:
    Otsu *pb;
    int populationCount;
    int iterationCount;
    int parameter;
    int verbosity;
    int elapsedIterations;
    int globalStagnationCount;

    void feasibleValidSolution(uint8_t * cv){
        if(verbosity >= 6) cout << "Generate initial CV: ";

        for(int i = 0; i < this->parameter; i++){
            int rnd_range;
            uint8_t min_lmt;
            if(i == 0)
                min_lmt = (pb->lb)[i]; 
            else
                min_lmt = cv[i-1]+1; 
            
            rnd_range = 1 + (pb->ub)[i] - min_lmt;
            assert(rnd_range > 0);
         //   if(verbosity >= 1) cout << "rnd_range: " << rnd_range << ". min_lmt: " << (int)min_lmt << endl;
            cv[i] = rand() % rnd_range + min_lmt;
            if(verbosity >= 6) cout << (int)cv[i] << " ";
        }
        if(verbosity >= 6) cout << endl;        
    }
    
    void copyTo(uint8_t *a , uint8_t *b, int n){
        for(int i = 0; i < n; i++){
            b[i] = a[i];
        }
    }
public:
    uint8_t *cv; // best solution

public:
    optAlg(Otsu *pb,
        int populationCount,
        int iterationCount,
        int verbosity
        )
    {
        this->pb = pb;
        this->populationCount = populationCount;
        this->iterationCount = iterationCount;
        this->verbosity = verbosity;
        this->parameter = pb->parameter;
		
        gfit = 0.0;
        cv = new uint8_t[this->parameter];
        globalStagnationCount = 0;
    }
    
    virtual void evolve() = 0;
    virtual ~optAlg(){
       delete [] cv;        
    }
    int getElapsedIterations(){
        return elapsedIterations;
    }
    void dumpOptThrslds(string &fn ){
//    uint8_t* labelimage = new uint8_t[img8.rows * img8.cols];
    // generate label image
        fstream resultFile (fn,std::fstream::out);
        for(int i = 0; i < parameter; i++){
            resultFile << (int)cv[i] << endl;
        }
        resultFile.close();
    }
    bool assessThrs(string &fn, double &variance){
        fstream resultFile (fn);
        int optthrs;
        bool findoptimal = true;
        variance = 0.0;
        for(int i = 0; i < parameter; i++){
            resultFile >>optthrs;
            variance += pow((cv[i]-optthrs),2);
            if(optthrs != cv[i]){
                findoptimal = false;
            }
        }
        resultFile.close();    
        //if(parameter != 1)
        variance = sqrt((1/(double)parameter) *variance);
        return findoptimal;
    }
};
