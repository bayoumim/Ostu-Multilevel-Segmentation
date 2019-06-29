# all algorithm 
algs=(otsudp hcoclpso ccqpso csm cscm)

echo -n > $1_solution_performance_result_table.txt;
for k in {2..15} {16..64..4}; do
#	echo $k
	echo $k >> $1_solution_performance_result_table.txt 
	# assess algorithms
	for j in ${algs[*]}; do
		sudo ./pso -a $j -d $1 -k $k -o 5 2>> log_$1\_$j\_solution_performance_results.txt;
	done
done

# combine log file results
for j in ${algs[*]}; do
	sudo paste $1_solution_performance_result_table.txt log_$1\_$j\_solution_performance_results.txt > log_tmp.txt;
	cat log_tmp.txt >$1_solution_performance_result_table.txt;
	rm -f log_tmp.txt;
	#read i;
done

# remove temporary log files
for j in ${algs[*]}; do
	sudo rm -rf log_$1_$j\_solution_performance_results.txt;
done