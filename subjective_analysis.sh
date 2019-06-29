#for i in {5..9} 16; do
algs=(otsudp hcoclpso ccqpso csm cscm)
for k in 16; do
	for j in ${algs[*]}; do
		sudo ./pso -a $j -d $1 -k $k -o 2;
		#exit 1
		#echo $k $j
	done
done