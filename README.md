# GIF-7104-Parallel Programming TPs Repository
#### TP1: Parallelize sieve of Eratosthene
========================================
In this this assignment we were asked to begin with a sequential version
of the algorithm and to parallelize it using the "pthread" library in C.

We have produced a rapport that details all the steps that we made to accomplish that
task and produce the results we wanted.
The latex source file fr the report is present in the subfolder "Report" of the
folder "tp1".

For more details about the code, clone the repository in the SourceFile subfolder,
and run the command:

```bash
$cmake .
```
then the command
```bash
$make
```
You will have an executable, run it with this command:
```bash
$./Primes [limit] [nthreads]
```
