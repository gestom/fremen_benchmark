set terminal fig color
set multiplot
set size 1.00,0.8
set origin 0.00,0.00
set key horizontal samplen 3 at 14,3.5
set title 'Comparison of observed, predicted, and estimated states' offset 0,1.0 
set ylabel 's''(t)        p(t)        s(t)' offset 1,-0.6
set ytics ('0' 0,'1' 0.75,'0' 1,'1' 1.75,'0' 2,'1' 2.75) 
set xlabel 'Time [days]'
set xtics nomirror ('Tue' 1,'Wed' 2,'Thu' 3,'Fri' 4,'Sat' 5,'Sun' 6, 'Mon' 7, 'Tue' 8,'Wed' 9,'Thu' 10,'Fri' 11,'Sat' 12,'Sun' 13, 'Mon' 14) offset -2.0,0.0 
set label 1 "Training set" at 2.5,3.0
set arrow from 2.5,3.0 rto 2.0,3.0
set label 2 "Testing set 1" at 9.8,2.55
set label 3 "Testing set 2" at 10.2,2.25
plot [0:14] [-0.2:3.2] \
'training.txt' using ($0/24/3600):($1*0.75+2) with lines lc 2 lw 1 title 'Observations - s(t)',\
'probability.txt' using ($0/24/3600):($1*0.75+1) with lines lc 1 lw 1 title 'Probability - p(t)',\
'probability.txt' using ($0/24/3600):(($1>0.5)*0.75+0) with lines lc 3 lw 1 title 'Estimate - s''(t)',\
'day2.txt' using ($0/24/3600+8):($1*0.75+2) with lines lc 2 lw 1 notitle,\
'day1.txt' using ($0/24/3600+13):($1*0.75+2) with lines lc 2 lw 1 notitle
#'day2.txt' using ($0/24/3600+8):($1*0.75+2) with lines lc 4 lw 1 title 'State observation (testing set 1) - s(t)',\
#'day1.txt' using ($0/24/3600+13):($1*0.75+2) with lines lc 5 lw 1 title 'State observation (testing set 2) - s(t)'


#'oneweek.txt' using ($0/24/3600):(($1>0.5)*0.75+1) with lines lc 3 lw 1 title 'State estimate - s''(t)',\
#'oneweek.txt' using ($0/24/3600+7):(($1>0.5)*0.75+1) with lines lc 4 lw 1 title 'State prediction - s''(t)'
