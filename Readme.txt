Command Line Interpreter. C. I wrote from scratch command line interpreter based on Unix Shell. It can work with a process pipeline and a subshell. Some Unix commands such as ls, cat, pwd, cd, cmp, echo, and ln are implemented independently (repository Unix-commands). The input string is divided into words and symbols (list.c), then a tree (tree.c) is built from these words and symbols. When descending the tree, the functions of Unix commands are executed one after the other (exec.c).

// Реализация интерпретатора командной строки

To run:
//Запуск:
	1. make
	2. ./prog
	3. Then print commands //Далее вводить команды
	4. To exit: crtl-D // Для выхода: ctrl-D

Syntax // Синтаксис:
	implemented using an L-graph // с помощью L-графа, файл L_graph.pdf

Implemented // Реализовано:
	1. >, >>, <
	2. |, if cd exist, we ignore it // если внутри cd - игнорируем cd
	3. ;
	4. &, message after ending of background process // по завершении вывод сообщения о завершении фонового процесса
	5. (),  subshell // сабшелл
	6. &&, ||, logic operations // логические операции

Тесты:
	1. cd ..
       	   pwd
	2. echo $HOME $USER
	3. cd
           pwd
	4. cat > f #перенаправление вывода
           a
           b
           c

           cat f #выводит содержимое файла f
           a
           b
           c

           cat > f #перезаписывает файл f
           x
       
           cat f
           x
       
           cat >> f #дописывает в конец файла f
           y
       
           cat f
           x
           y
    	   5. cat < f > f1
    	   6. yes|head (10 "y" и без зомби)
    	   7. yes|yes|yes|yes|yes|head
           8. cat < /dev/null|head|head|head (ничего не печатает)
    	   9. pwd|cat >> f`
    	   10. sleep 5
	      (if ctrl-C is pushed, then only sleep will be ended, Shell by itself ignores crtl-C)
              (при ctrl-C завершается только процесс sleep, но зомби не оставляет. сам shell на ctrl-C не реагирует)
    	   11. yes|yes|tes|sleep 5|pwd
    	   12. cat|sort -r|cat -n
    	   13. sleep 3; pwd
    	   14. sleep 3 &
               (ctrl-C sleep будет игнорировать (т.к. фоновый) как и сам shell, sleep 3 завершится, когда пройдет 3 сек, и выведет "завершился")
    	   15. sleep 3|sleep 4 &
               (conveyor is executed in background, with ps command we can see which processes are still alive)
               (конвейер выполняется в фоновом режиме, по ps можно смотреть, какие процессы есть)
