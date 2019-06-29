#pragma once

#include "optAlg.h"


class OtsuDp : public optAlg {
private:
	
public:
	OtsuDp(	Otsu *pb,
          	int verbosity)
	: optAlg(pb, 1,1,verbosity)
	{
		//ths = 0;	
	}
	
	void evolve(){
		if(verbosity >= 2) cout << "evolve begin" << endl;
		
		float(*optimalObjs)[256] = new float[pb->parameter][256];
		short(*prevOptimalLevel)[256] = new short[pb->parameter - 1][256];


		//the case i=0
		for (int k = 0; k < pb->parameter - 1; k++)
		{
			optimalObjs[k][0] = 0;
			prevOptimalLevel[k][0] = -1;
		}
		optimalObjs[pb->parameter - 1][0] = 0;

		//the case i=1 --> 255
		for (int i = 1; i < 256; i++)
		{
			//zero inter-threshold case
			auto qVal = pb->Q(0, i);
			optimalObjs[0][i] = qVal;

			//from k=1 to pb->parameter-1 thresholds
			for (int k = 1; k < pb->parameter; k++)
			{
				float optimalQ = 0;
				int bestPrevLevel = -1;
				for (int j = 0; j < i; j++)
				{
					//if (k == 1)
						//prevOptimalLevel[0][i] = -1;
					if (k >= 2 && prevOptimalLevel[k - 2][j] == -1)
						continue;

					qVal = pb->Q(j, i);
					if (optimalObjs[k - 1][j] + qVal > optimalQ)
					{
						optimalQ = optimalObjs[k - 1][j] + qVal;

						bestPrevLevel = j;
					}
				}
				optimalObjs[k][i] = optimalQ;
				prevOptimalLevel[k - 1][i] = bestPrevLevel;
			}
		}

		//Find the best thresholding
		float optimalObj = -1;
		int bestTh_1Level = 0;
		for (int i = 0; i < 255; i++)
		{
			float totalObj = optimalObjs[pb->parameter - 1][i] + pb->Q(i, 256);
			if (prevOptimalLevel[pb->parameter - 2][i] != -1 &&
				totalObj > optimalObj)
			{
				optimalObj = totalObj;
				bestTh_1Level = i;
			}
		}
		gfit = optimalObj;

	//	delete[] ths;

	//	ths = new int[pb->parameter + 1];
	//	ths[pb->parameter] = 256;

		int curTh = bestTh_1Level;

		for (int i = pb->parameter - 1; i > 0; i--)
		{
			cv[i] = curTh;
			curTh = prevOptimalLevel[i - 1][curTh];
		}
		cv[0] = curTh;

		delete[] optimalObjs;
		delete[] prevOptimalLevel;

		//apply thresolding to the image

		//calculate average gray level in each band
/*
		int* meanGray = new int[pb->parameter + 1];

		meanGray[0] = averageGray(0, ths[0]);
		for (int i = 1; i < pb->parameter + 1; i++)
		{
			meanGray[i] = averageGray(ths[i - 1], ths[i]);
		}


		//construct the threshold mapping
		uint8_t grayMap[256];

		int level = 0;
		int gray = meanGray[0];
		int thIndex = 0;
		while (level < 256)
		{
			grayMap[level] = gray;
			level++;
			if (level == ths[thIndex])
			{
				thIndex++;
				gray = meanGray[thIndex];
			}
		}


		delete[] meanGray;

		Frame<>& out = (Frame<>&)GetOutputImage();

		for (int col = out.left(); col < out.right(); col++)
		{
			for (int row = out.top(); row < out.bottom(); row++)
			{
				auto& val = out(row, col);
				val = grayMap[val];
			}
		}
		*/
/*
        if(verbosity >= 1) {
             cout << "Best solution: " ;
            for(int k = 0; k < parameter; k++){
                cout << (int)cv[k] << " "; 
            }
            cout << endl;
            if (verbosity >= 2) cout << "best fitness: " << gfit << endl;
        }        
		*/
    }

};

