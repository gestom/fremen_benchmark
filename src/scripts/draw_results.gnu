set terminal fig color
set size 1.00,0.8
set title 'Prediction and estimation error as a function of time'  
set ylabel 'Prediction/estimation error [%]'
set xlabel 'FreMEn order [-]'
plot [0:10] [0.0:0.3] \
'results/test_0.txt' with lines lw 1 title 'Estimation error',\
'results/test_1.txt' with lines lw 1 title 'Prediction error - day 1',\
'results/test_2.txt' with lines lw 1 title 'Prediction error - day 2'
