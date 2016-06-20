#evaluate predition/estimation error on three datasets 
if [ -e results/summary.txt ]
then
	rm results/summary.txt
fi
for i in $(seq 0 2);
do 
	./scripts/evaluateone.sh data/test_times_$i.txt data/test_data_$i.txt >results/test_$i.txt
	echo New method performed $(paste results/orig_$i.txt results/test_$i.txt|tr \\t ' '|cut -f 2,4 -d ' '|./scripts/t-test) on testing data $i.  >>results/summary.txt
done
gnuplot scripts/draw_results.gnu >results/results.fig
fig2dev -Lpng -m 5 results/results.fig results/results.png
rm tmp/*
