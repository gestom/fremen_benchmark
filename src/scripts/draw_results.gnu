set terminal fig color
set size 1.00,0.8
set title 'Prediction and estimation error as a function of time'  
set ylabel 'Prediction/estimation error [%]'
set xlabel 'FreMEn order [-]'
plot \
'results/orig_0.txt' with lines lw 1 title 'Original - Estimation error',\
'results/orig_1.txt' with lines lw 1 title 'Original - Prediction error - day 1',\
'results/orig_2.txt' with lines lw 1 title 'Original - Prediction error - day 2',\
'results/test_0.txt' with lines lw 1 title 'Modified - Estimation error',\
'results/test_1.txt' with lines lw 1 title 'Modified - Prediction error - day 1',\
'results/test_2.txt' with lines lw 1 title 'Modified - Prediction error - day 2'
