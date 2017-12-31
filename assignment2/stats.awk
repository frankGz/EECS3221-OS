BEGIN{
    run   = 0;
    prev  = 0;
    sum   = 0;
    sumsq = 0;
    cnt   = 0;
}
{
    if (prev==$1)
	run++;
    else {
	prev    =$1;
	sum    += run;
	sumsq  += run*run;
	cnt++;
	run=1;
    }
}
END {
    sum    += run;
    sumsq  += run*run;
#    printf "sum vs NR %d vs %d\n", sum, NR;
#    printf "Number of runs %d\n", cnt;
    avg = sum/cnt;
    avgsq = sumsq/cnt;
    printf "Average %f\n", avg;
    printf "Standard Dev. %f\n", sqrt(avgsq-avg*avg);
}
