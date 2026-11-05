# Semaphores: lab templates

A sequencing run in the bioinformatics lab. Each exercise is one step of the
pipeline, rewritten so that the worker processes synchronise on the shared
resource they use: the terminal, a results file, the aligner licences, the
merge step, the submission queue.

```
ex_semaphores_templates/
├── demo/      <- Part 1: the programs shown during the lecture
├── ese_1/     <- Exercise 1: the read pipeline (four stages in order)
├── ese_2/     <- Exercise 2: the results file (one worker at a time)
├── ese_3/     <- Exercise 3: three aligner licences
├── ese_4/     <- Exercise 4: merge after every chromosome
├── ese_5/     <- Exercise 5: sample submission (two programs)
└── report/    <- the run report (needs the 5 confirmation codes)
```

The exercises are independent: solve them in any order. Every exercise has
two files to complete: src/semaphore.c (the semOp helper) and src/main.c
(src/pipeline.c and src/submit.c in exercise 5).

To compile:
> cd ese_1
> make

To run the executable
> ./semaphore_ese1 ...

To remove object file(s) and executable(s)
> make clean

To list and remove the semaphore sets left behind by a crashed run
> ipcs -s
> ipcrm -s <semid>

To file the run report
> ./report/file_report.sh

It can be filed only with the 5 confirmation codes: exercise N reveals
code N when its result is the expected one.
