dataset='door'
grep -v '#' ../src/models/test_models.txt >models.tmp
rm -rf ../results/$dataset/
mkdir ../results/$dataset/
for m in $(cut -f 1 -d ' ' models.tmp)
do	
for i in $(cat models.tmp |grep $m|sed  -e 's/\s\+/\ /g'|cut -f 2-100 -d ' ');
do
rm predictions.txt
for d in 0 1 2
do 
echo ../bin/fremen ../data/$dataset/training_data.txt ../data/$dataset/testing_times_$d.txt $m $i
../bin/fremen ../data/$dataset/training_data.txt ../data/$dataset/test_times_$d.txt $m $i
e=$(paste predictions.txt ../data/$dataset/test_data_$d.txt |awk '{a=($1>=0.5)-$2;b+=a*a;i=i+1;}END{print b/i}')
echo $e >>../results/$dataset/$m\_$i.txt
echo Model $m, parameter $i
done
done
done
