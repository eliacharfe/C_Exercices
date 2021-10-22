/* ./ex2a.c
 * =============================================
 * ==== Written by: Eliachar Feig. ID: 302278338 ===
 * =============================================
 * This program make born a child which will cast lots of integers and
 * find the 10 first different prime numbers and if find them before 3 secods
 * alarm that the father give to him to do the job, he blocks the signal
 * SIGTERM from father and prints those primes, after that happens the father
 * prints "good boy" (by a signal handler function when it gets the signal
 * SIGCHILD). If the boy didnt finish within 3 seconds the father kill the
 * son with SIGTERM signal and prints "bad boy" then terminate program
 *
 * No input
 * Output : according to signals
 *
     gcc -Wall ex2a.c -o ex2a
=============================================================
/* ./ex2b.c
 * =============================================
 * ==== Written by: Eliachar Feig. ID: 302278338 ===
 * =============================================
 * This program make born a child then both father and son will run in an
 * infinit loop in which in every iteration cst a lots between 1 and 10 if
 * the number is smaller then 3 the process will print that he ends (with
 * telling his pid). if not it will send a signal to the other process, if
 * the other process didnt gets the signal within 5 seconds it will prints
 * that he was left alone and terminate program. if the process gets the sig-
 * nal within 5 sec, it will print its pid and say that he have a partner.

 * No input
 * Output: according to the signal each process gets

     gcc -Wall ex2b.c -o ex2b
=============================================================
