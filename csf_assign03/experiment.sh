# A file to run experiments of various cache sizes and store the results
# in results.txt (run with bash experiment.sh)

#!/bin/bash
make csim
replacement_policies=("lru" "fifo")
writing_policies=("write-allocate write-back" "write-allocate write-through" "no-write-allocate write-through")
num_sets=(1 2 4 8 16 32 64 128 256 )
num_blocks=(1 2 4 8 16 32 64 128)
num_bytes=(4 8 16 32 64 128 256 512 1024 2048 4096)
trace="swim.trace"
output_file="results.txt"
echo "" > $output_file


for r_policy in ${replacement_policies[@]}
do
    for w_policy in "${writing_policies[@]}"
    do
        for set in ${num_sets[@]}
        do
            for block in ${num_blocks[@]}
            do
                for byte in ${num_bytes[@]}
                do
										a=$(( set*block*byte ))
										# echo $a &>> $output_file
										if [ $a == "524288" ]; then
                    # FIXME: only printing out the first word in the writing policies
										# echo $set $block $byte $w_policy $r_policy &>> $output_file
										# echo $r_policy &>> $output_file
                    ./csim $set $block $byte $w_policy $r_policy < $trace &>> $output_file
										# echo "
										# " &>> $output_file
										fi
                done
            done
        done
    done
done