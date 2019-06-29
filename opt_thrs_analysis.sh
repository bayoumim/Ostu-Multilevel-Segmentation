# all algorithm EXCEPT iterative DP as it will be used for benchmarking
algs=(hcoclpso ccqpso csm cscm)

echo -n > $1_opt_thrs_result_table.txt;
for k in {2..15} {16..64..4}; do
#	echo $k
	# generate benchmarks
	sudo ./pso -a otsudp -d $1 -k $k -o 3;
	echo $k >> $1_opt_thrs_result_table.txt 
	# assess other algorithms
	for j in ${algs[*]}; do
		sudo ./pso -a $j -d $1 -k $k -o 4 2>> log_$1\_$j\_opt\_thrs_results.txt;
	done
	
	# remove benchmarks
	sudo rm -rf $1_otsudp_result_optimal_thresholds_$k
done

# combine log file results
for j in ${algs[*]}; do
	sudo paste $1_opt_thrs_result_table.txt log_$1\_$j\_opt\_thrs_results.txt > log_tmp.txt;
	cat log_tmp.txt >$1_opt_thrs_result_table.txt;
	rm -f log_tmp.txt;
	#read i;
done

# remove temporary log files
for j in ${algs[*]}; do
	sudo rm -rf log_$1_$j\_opt\_thrs_results.txt;
done
