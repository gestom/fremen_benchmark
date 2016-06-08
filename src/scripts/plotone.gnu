set terminal fig color
unset ytics

min(x,y) = (x < y) ? x : y
max(x,y) = (x > y) ? x : y

#set title 'Probability of door being open' offset 0,-0.6
set title 'Probability of student presence' offset 0,-0.6
set size   0.45,0.44
set origin 0.0,0.1
set ytics nomirror 0,1
unset xlabel
set ylabel 'p(t)' offset 2.68,0
set xtics nomirror ('Mon' 1, 'Tue' 2,'Wed' 3,'Thu' 4,'Fri' 5,'Sat' 6,'Sun' 7) offset -2.1,0.2 
plot [0:7] [-0.2:1.2] \
'tmp/prob.txt' using ($0/24/60):(max(min(0.3+0.6*sin($0/24/60*6.28-3)+0.3*sin($0/24/60/7*6.28-1),1),0)) with lines lc 2 lw 2 notitle
#'tmp/prob.txt' using ($0/24/60):(max(min($1+0.2,1),0)) with lines lc 2 lw 2 notitle
