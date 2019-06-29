cd Dropbox/M\ and\ Ms/P001\ -\ efficient\ Multilevel\ thresholding/mlt_mh/

g++ -O3 main.cpp -o pso -std=c++1z `pkg-config --libs --cflags opencv` -lstdc++fs -Wall -Wextra -pedantic-errors

time sudo ./pso -a ccqpso -d BSDS500 -k 5 

time sudo ./pso -a ccqpso -d BSDS500 -k 5 -o -v 2


time sudo ./pso -a otsudp -d BSDS500 -k 5 -o -v 2

#performance measures experiments
./perf_meas.sh
#Quality measure experiments
./qlty_meas.sh

time sudo ./pso -a ccqpso -d QS -k 5 -o 2

# optimal thresholds assesment
./opt_thrs_analysis.sh QS
./sol_perf.sh QS
./subjective_analysis.sh QS
