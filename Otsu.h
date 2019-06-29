#pragma once

#include <cstdint>

#include <opencv2/opencv.hpp>
using namespace cv;

#include <string>
using namespace std;

class Otsu {
private:
    void calculateHistogram()
    {
        
        //zero histogram
        for (int i = 0; i < GREY_LEVELS; i++)
            hist[i] = 0;

        for (int row = 0; row < img8.rows; row++)
        {
            for (int col = 0; col < img8.cols; col++)
            {
                auto val = img8(row, col);
                hist[val]++;
            }
        }
    }


public:
    int parameter ;
    int *value ;
    uint8_t *lb;
    uint8_t *ub;
    int verbosity;
private:
    Mat_<uchar> img8;
    long long hist[256];
    //a =Sum i fi  from 0-->index-1
    //b =Sum fi   from 0-->index-1
    //first element is zero
    long long a[257], b[257];
public:    
    void calculateCumulativeHistograms()
    {
        a[0] = 0;
        b[0] = 0;
        for (int i = 0; i < 256; i++)
        {
            a[i + 1] = a[i] + i*hist[i];
            b[i + 1] = b[i] + hist[i];
        }
    }

    float Q(int p, int q) const
    {
//        assert(p < q);
  //      assert(p < q || !(std::cerr<<"Q-function : Invalid Inputs; p = " << p << " q = " << q << endl));
        assert(q <= 256);

        long long num = a[q] - a[p];
        num = num*num;
        long long den = b[q] - b[p];

        if (den == 0)
            return 0;

        return (float)num / (float)den;
    }

public:
    Otsu(){
        cout << "ERROR: Otsu() is called" << endl;
        assert(0);
    }
    
    ~Otsu(){
        img8.release();
    }
    
    Otsu(   int p , 
            const int *v ,
            string &ImageName,
            int verbosity){
        parameter = p ;
        value = new int[p] ;
        this->verbosity = verbosity;

        for( int i = 0 ; i < p ; i++ )
            value[i] = v[i] ;
        // open image

        Mat image;
        image = cv::imread(ImageName);

        cv::cvtColor(image, image, CV_RGB2GRAY);

        cv::namedWindow("Original Image");
        cv::imshow("Original Image", image);
        img8 = image;

        calculateHistogram();
        calculateCumulativeHistograms();

        image.release();

        lb = new uint8_t[parameter];
        ub = new uint8_t[parameter];
        if(verbosity >= 6)  cout << "Lower bounds: ";
        for(int i = 1; i <= parameter; i++){
            lb[i-1]= i;
            if(verbosity >= 6) cout << "lb[" << i-1 << "] = " << (int)lb[i-1] << " , ";
        }
        if(verbosity >= 6)  cout << endl;
        
        if(verbosity >= 6)  cout << "Upper bounds: ";

        for(int i = 0; i <parameter; i++){
            ub[i] = GREY_LEVELS - 1 + i - (parameter - 1);
            if(verbosity >= 6) cout << "ub[" << i << "] = " << (int)ub[i] << " , ";
        }
        
        if(verbosity >= 6)  cout << endl;

    }

    // Mandatory, computes ... well ... the fitness
    float fitness( uint8_t ths[]) 
    {
        //assert(sizeof(ths) == parameter);
        float val = Q(0, ths[0]);
        bool invalidinput = false;

        for (int i = 0; i < parameter - 1; i++)
        {
            if(ths[i]>=ths[i+1]) {
                val = 0;
                cout << "threshold constaints don't satisfied: threshold " << i << " = " << (int)ths[i] << ". Threshold " << i+1 << " = " << (int)ths[i+1] << endl;
                assert(0);
                invalidinput = true;
                break;
            }
            val += Q(ths[i], ths[i + 1]);
        }

        if(!invalidinput)
            val += Q(ths[parameter - 1], 256);

        return val;
    }
    void generateLabeledImage(uint8_t * cv, string &fn ){
    //    uint8_t* labelimage = new uint8_t[img8.rows * img8.cols];
        // generate label image
        ofstream resultFile (fn);
        for (int row = 0; row < img8.rows; row++)
        {
            for (int col = 0; col < img8.cols; col++)
            {
                int val = img8(row, col);
                int index;
                for (index = 0; val > (int)cv[index] && index < parameter; index++){
                //    cout << "val: " << (int)val << " . cv[" << index << "] = " << (int)cv[index] << endl;
                }
                resultFile << index+1;
                // index + 1
             //   cout << "index: " << index << " . parameter: " << parameter << ". val: " << (int)val << endl; 
               // assert(index <= parameter); 
                //int tmpidx = row * img8.rows + col;
               // labelimage[tmpidx] = index;
                if(col != img8.cols - 1) resultFile << " , " ;
            }
            resultFile << endl;
        }
        
        // dump label image to excel file
    }
};