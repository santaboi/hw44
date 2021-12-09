# Parallel Programing HW4 (Pthread)
## compile
> g++ -g -Wall -o hw4 hw4.cpp -lpthread -lrt
## OUTPUT
>no difference betweeen non_pthread_output and pthread_output

![](https://i.imgur.com/wORflL8.png)
![](https://i.imgur.com/RiN65yr.png)
## What have you done
```
1.seperate rows of bmp_img to each threads :
    assign remainder to the last thread(due to hardware limitaions,# of threads won't be too large)
```
```
2. pthread_barrier_wait(&barrier);
    synchronize each threads operation
```
```
3. use strtol(argv[1], NULL, 10); 
    to get # of threads at runtime ,  convert string to int (in base 10)
```
```
4. sequentially create and join threads by for loop
```
```
5. get_time() to calculate runtime
    reference: (stack overflow) https://stackoverflow.com/questions/15976790/how-can-i-calculate-the-running-time-of-a-pthread-matrix-multiplication-program
```

## Analysis on your result
note : I've tested three times each cases, and take the smallest run time as example
> case1 : 1000 smoothing time
### discuss on the results
![](https://i.imgur.com/5UBu6tM.png)
1. There is hardware limitation ; take (16_threads 2000 loops) as an extreme example.The runtime is even slightly longer than the 8 threads one.
2. runtime of thread6 is longer than threads4 in both cases ->the reason is that bmpinfo.biheight can't be devided by 6 
* thread 8
    >![](https://i.imgur.com/m6HobRP.png)
* thread 6
    >![](https://i.imgur.com/KOx4xac.png)
* thread 4
    >![](https://i.imgur.com/eHmkGM5.png)
* thread 2
    >![](https://i.imgur.com/6F0m8Ub.png)
* thread 1
    >![](https://i.imgur.com/nySowyx.png)

> case2 : 2000 smoothing time
* thread 8
    >![](https://i.imgur.com/1BD7476.png)
* thread 6
    >![](https://i.imgur.com/QNJ0MBj.png)
* thread 4
    >![](https://i.imgur.com/xor1OOd.png)
* thread 2
    >![](https://i.imgur.com/v5QYtkO.png)
* thread 1
    >![](https://i.imgur.com/0ZgIPLq.png)



## Any difficulties?
1.having a hard time simply use time.h ctime() function
solved by referencing this stack overflow example:
https://stackoverflow.com/questions/15976790/how-can-i-calculate-the-running-time-of-a-pthread-matrix-multiplication-program
## Feedback to TAs
totally appreciate to you guys , those meme posted on moodle are dope asf!!
