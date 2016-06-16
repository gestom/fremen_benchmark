#evaluate predition/estimation error on three datasets 
for i in $(seq 0 2);
do 
	./scripts/evaluateone.sh data/test_times_$i.txt data/test_data_$i.txt >results/test_$i.txt
done
gnuplot scripts/draw_results.gnu >results/results.fig
fig2dev -Lpng -m 5 results/results.fig results/results.png
rm tmp/*
