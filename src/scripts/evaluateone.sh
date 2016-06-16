#evaluates prediction for different orders on a single file 
for i in $(seq 0 10);
do
../bin/fremen data/training_data.txt $1 $i
e=$(paste tmp/predictions.txt $2 |awk '{a=($1>=0.5)-$2;b+=a*a;i=i+1;}END{print b/i}')
echo $i $e
done
