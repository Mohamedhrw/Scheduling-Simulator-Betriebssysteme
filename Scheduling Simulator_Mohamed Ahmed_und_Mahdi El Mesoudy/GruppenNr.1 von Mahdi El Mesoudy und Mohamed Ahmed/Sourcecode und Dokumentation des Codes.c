

/*
*
* Autor: Mahdi El Mesoudy und Mohamed Ahmed
* Zweck: Scheduling Simulator (Projekt 1)
* Programmiersprache: C
* Simulation: CMD Shell
*
*/


/*
 *
 * Verwendte Bibliotheken
 *
 *
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>
#include <pthread.h>
#define MAX_PROCESS 15


/*
 *
 * Strukturen und deren beschreibung
 *
 *
*/

/*
 * Prozess Struktur
 * Gebraucht für die bearbeitung aller Algorithem
 */
struct process {
    // Prozess Id
    int pid;
    // ArbeitZeit
    int burst_time;
    // Wartezeit
    int waiting_time;
    // Durchlaufzeit
    int turnaround_time;
    // Priorität
    int priority;
    // Zustand
    char status [6];
};

/*
 * cpu Struktur
 * gebraucht für Round Robin und Round Robin mit Priorität
 */
struct cpu {
    // CPU Garitt
    int garitt;
    // CPU Zeit
    int time;
};



/*
 *
 * Globale Variabelen
 *
 *
*/

// Prozess Variabel
typedef struct process Process;

// CPU Variabel
typedef struct cpu cpu;

// Quantumzeit
int quantum = 0;

//reguest_queue Array
int request_queue[100];

// orginal Burst_Zeit Array
int original_burst_time[MAX_PROCESS];

// Durchschnit der Wartezeit
float average_waiting_time;

// Durchschnitt der Durchlaufzeit
float average_turnaround_time;

// Globale Variabel, darauf threads zugreifen
char character;

// mutex
pthread_mutex_t mutex;


/*
 *
 * Funktionen deklaration und beschreibung
 *
 *
*/

/*
 * berechnet die Wartzeit für jede Prozess
*/
void calculate_waiting_time(Process* p, int n);

/*
 * berechnet die Durchlaufzeit für einen Prozess
 */
void calculate_turnaround_time(Process* p, int n);

/*
 * berechnet den Durchschnitt der Wartezeit für alle Prozesse
 */
float calculate_average_waiting_time(Process* p, int n);

/*
 * berechnet den Durchschnitt der Durchlaufzeit für alle Prozesse
 */
float calculate_average_turnaround_time(Process* p, int n);

/*
 * Zeigt Wartezeit, Bearbeitungszeit und Burst-Zeit für jeden Prozess
 *  in einer Tabelle bei FCFS-Algorthmus an
 */
void print_FCFS_table(Process* p, int n);

/*
 * Diese Funktion simuliert alle Prozesse, bis alle bei FCFS-algorthmus erledigt sind
 */
void FCFS_simulation(Process* p, int n);

/*
 *sortiert alle Prozesse mit dem kürzesten SJF-Algorithmus
 */
void sort_SJF_process(Process* p, int n);

/*
 * Zeigt Wartezeit, Bearbeitungszeit und Burst-Zeit für jeden Prozess
 *  in einer Tabelle bei SJF Algorthmus an
 */
void print_SJF_table(Process* p, int n);

/*
 * diese Funktion zeigt eine Schedul_SJF_Tabelle , die erklärt wie
 * Prozessen nach SJF_Algorithmus nacheinander bearbeitet werden
 */
void schedul_SJF_table(Process* p, int n);

/*
 * Diese Funktion simuliert alle Prozesse, bis alle bei SJF-Algorthmus durchgeführt wurden
 */
void SJF_simulation(Process* p, int n);

/*
 * Zeigt Prozss_Id, Status und Priorität für jeden Prozess in Tabellen an
 */
void print_process_table(Process* p, int n);

/*
 * diese Funktion zeigt eine Simple_Schedul_RR_Tabelle , die erklärt wie
 * Prozessen nach simple_RR_Algorithmus nacheinander bearbeitet werden
 */
void simple_round_robin_scheduler(Process* p, int n);

/*
 * sortiert alle Prozssen nach Warte-zeit und burst_time bei RR-Algorthmus
 * Pozessen werden aufsteigend sortiert
 */
void sort_RR_process(Process* p, int n);

/*
 * Zeigt Wartezeit, Bearbeitungszeit und Burst-Zeit für jeden Prozess
 *  in einer Tabelle bei RR-Algorthmus an
 */
void print_RR_table(Process* p, int n);

/*
 * sortiert alle Prozssen nach Priorität bei RRP-Algorthmus
 * Pozessen werden aufsteigend sortiert
 */
void sort_RRP_process(Process* p, int n);

/*
 * Zeigt Wartezeit, Bearbeitungszeit und Burst-Zeit für jeden Prozess
 *  in einer Tabelle bei RRP-Algorthmus an
 */
void print_RRP_table(Process* p, int n);

/*
 * Zeigt Wartezeit, Bearbeitungszeit und Burst-Zeit für jeden Prozess
 *  in einer Tabelle bei RRP-Algorthmus an
 */
void round_robin_with_priority(Process* p, int n);

/*
 * diese Funktion zeigt eine Standard_Schedul_RR_Tabelle , die erklärt wie
 * Prozessen nach standard_RR_Algorithmus nacheinander bearbeitet werden
 */
void Standard_schedul_order_request_queue(Process* p, int n);

/*
 * Diese Funktion greift auf die globale Variable 'character' zu und initialisiert diese mit '+' Zeichen
 */
void* initialize_character_thread_1(void);

/*
 * Diese Funktion greift auf die globale Variable 'character' zu und initialisiert diese mit '-' Zeichen
 */
void* initialize_character_thread_2(void);

/*
 * Diese Funktion greift auf die globale Variable 'character' zu und initialisiert diese mit '*' Zeichen
 */
void* initialize_character_thread_3(void);

/*
 * Threads simulieren ohne Mutex zu verwenden
 */
void threads_Simulation(pthread_t* p);

/*
 * Threads simulieren mit Mutex
 */
void threads_Simulation_With_Mutex(pthread_t* p);

/*
 * Threads tabelle anzeigen, die Prozess_Id, Status und Priorität enthält
 */
void threads_Table(pthread_t* p);


/*
 *
 * Main Funktion
 *
 *
*/

int main()
{
    //Maximale Anzahl von Prozessen, die der Benutzer eingeben kann
    Process p[MAX_PROCESS];
    // Anzahl der Prozesse, die der Benutzer eingegeben hat
    int n;
    // Variable für for-Schleifen
    int i;
    // Option aus dem Hauptmenü
    int option;
    // Zufallszahl
    int random;
    // scheduler ID, Es gibt zwei Scheduler, ID:0 und ID:1
    int schedulerID;
    // Option aus dem RR_Menü
    int RRoption;
    // Option aus dem Menü
    char ch;

    printf("\n\n\t\t CPU SCHEDULING BY MAHDI EL MESOUDY AND AHMAD MOHAMED");
    printf("\n\t\t ====================================================");

    printf("\n\nScheduling Algorithm Options: \n");
    printf("\n\t1) First Come First Served (SJF)\n");
    printf("\t2) Shortest Job First (SJF)\n");
    printf("\t3) Roun Robin\n");
    printf("\t4) Roun Robin with priority\n");
    printf("\t5) Threads\n");

    printf("\nChoose Option : ");
    scanf("%d", &option);

    switch(option){

        case 1:{

            average_waiting_time=0;
            average_turnaround_time=0;

            printf("\n\t\tFirst Come First Served (FCFS) Scheduling Algorithm\n");
            printf("\t\t===================================================\n\n");
            printf("Enter number of processes (max. %d): ", MAX_PROCESS);
            scanf("%d", &n);

            if(n == 0 || n < 0) {
                printf("invalid entry, try again\n");
                main();
            }
            printf("\nEnter Process Burst Time:\n");
            for(i=0; i<n; i++){
                printf("P[%d] : ", i+1);
                scanf("%d", &p[i].burst_time);
            }

            // first process get executed immediately
            p[0].waiting_time = 0;

            //calculating waiting time and turnaround time
            calculate_waiting_time(p, n);
            calculate_turnaround_time(p, n);

            //FCFS_Tabelle ausgeben
            print_FCFS_table(p, n);

            // Durchschnitt der Wartezeit berechnen und ausgeben
            printf("\nAverage Waiting Time: %.3f", calculate_average_waiting_time(p, n));

            // Durchschnittliche Bearbeitungszeit berechnen und ausdrucken
            printf("\nAverage Turnaround Time: %.3f\n", calculate_average_turnaround_time(p, n));

            // Prozesstabelle ausgeben
            printf("\nPress t to show ProcessTable: ");
            scanf(" %c", &ch);
            if(ch == 't' || ch == 'T'){
                print_process_table(p, n);
            }

            // Burst-Zeit simulieren
            printf("\nPress s for Simulation => ");
            scanf(" %c", &ch);
            if(ch == 's' || ch == 'S'){
                FCFS_simulation(p, n);
            }

            // gehe zu den Optionen zurück
            printf("\nPress m for the Menu => ");
            scanf(" %c", &ch);
            if(ch == 'm' || ch == 'M'){
                main();
            }
            break;
        }

        case 2: {

            average_waiting_time = 0;
            average_turnaround_time = 0;

            printf("\n\t\tShortest Job First (SJF) Scheduling Algorithm\n");
            printf("\t\t=============================================\n\n");
            printf("Enter number of processes (max. %d): ", MAX_PROCESS);
            scanf("%d", &n);

            // Benutzer nach der Eingabe der Burst-Zeit fragen
            if(n == 0 || n < 0) {
                printf("invalid entry, try again\n");
                main();
            }
            printf("\nEnter Process Burst Time:\n");
            for(i=0; i<n; i++){
                printf("P[%d] : ", i+1);
                scanf("%d", &p[i].burst_time);
            }

            // Der erste Prozess wird sofort ausgeführt
            p[0].waiting_time = 0;

            /*
             * Kopieren Sie vor dem Sortieren des Arrays das burst_time-Array in das Globale
             *  original_burst_time-Array, um es in der Funktion zu vergleichen
             */
            for(i=0; i<n; i++){
                original_burst_time[i] = p[i].burst_time;
            }

            //der Prozesse in SJF-Reihenfolge vor der Berechnung sortieren
            sort_SJF_process(p, n);

           // Wartezeit und Durchlaufzeit berechnen
            calculate_waiting_time(p, n);
            calculate_turnaround_time(p, n);

            printf("\nchoose scheduler: ");
            printf("\nrandom scheduler press 'r'\nchoosing scheduler press 'c'");
            printf("\n\n==========> ");
            scanf(" %c", &ch);
            if(ch == 'r' || ch == 'R'){
                //die aktuelle Zeit als Startwert für den Zufallsgenerator benutzen
                srand(time(0));

                // Zufallszahl zwischen 0 und 2 erzeugen
                random = (rand() % 2);

                if(random == 0){
                    // SJF_Tabelle ausgeben
                    print_SJF_table(p, n);
                }else if (random == 1){
                    schedul_SJF_table(p, n);
                }
            }else if(ch == 'c' || ch == 'C'){
                printf("Enter 0 for Scheduler: 0 \nEnter 1 for Scheduler: 1");
                printf("\n\n==========> ");
                scanf("%d", &schedulerID);
                if(schedulerID == 0){
                    print_SJF_table(p, n);
                }else if(schedulerID == 1){
                    schedul_SJF_table(p, n);
                }else {
                    printf("\ninvalid entry, try again\n\n");
                    main();
                }
            }else {
                printf("\ninvalid entry, try again\n\n");
                main();
            }


            // Durchschnittliche Wartezeit berechnen und ausgeben
            printf("\nAverage Waiting Time: %.3f", calculate_average_waiting_time(p, n));

            // Durchschnittliche Bearbeitungszeit berechnen und ausgeben
            printf("\nAverage Turnaround Time: %.3f\n", calculate_average_turnaround_time(p, n));

            // Benutzer Auswahl, Burst-Zeit simulieren oder Prozesstabelle ausgeben oder zur Menu
            printf("\nPress t to show ProcessTable: ");
            printf("\nPress s for Simulation");
            printf("\nPress m for the Menu");
            printf("\n=> ");
            scanf(" %c", &ch);
            if(ch == 's' || ch == 'S'){
                SJF_simulation(p, n);
            }else if(ch == 'm' || ch == 'M'){
                main();
            }else if(ch == 't' || ch == 'T'){
                print_process_table(p, n);
            }

            // zu den Optionen zurückgehen
            printf("\nPress m for the Menu => ");
            scanf(" %c", &ch);
            if(ch == 'm' || ch == 'M'){
                main();
            }

            break;
        }

        case 3: {
            average_waiting_time=0;
            average_turnaround_time=0;
            
            printf("\n\t\tRound Robin (RR) Scheduling Algorithm\n");
            printf("\t\t=====================================\n");

            // Benutzer Nachfragen um eine Option auszuwälen
            printf("\n1) Simple Round Robin Scheduler");
            printf("\n2) Standard Round Robin Scheduler");
            printf("\n\n==========> ");
            scanf("%d", &RRoption);

        switch(RRoption){

            case 1: {
                printf("\nEnter number of processes (max. %d): ", MAX_PROCESS);
                scanf("%d", &n);
                if(n == 0 || n < 0) {
                    printf("invalid entry, try again\n");
                    main();
                }
                
                // Fragen nach Eingabe
                printf("\nEnter Arrival Time and Burst Time for each process\n");
                for(i=0; i<n; i++){
                    p[i].pid = i+1;
                    printf("P[%d]: \t", p[i].pid);
                    printf("Arrival Time: ");
                    scanf("%d", &p[i].waiting_time);
                    printf("\tBurst Time: ");
                    scanf("%d", &p[i].burst_time);
                }

                p[0].waiting_time = 0;

                printf("\nEnter the Quantum ==>  ");
                scanf("%d", &quantum);

                // Prozessen sortieren
                sort_RR_process(p, n);

                // Durchlaufzeit berechnen
                calculate_turnaround_time(p, n);

                // RR_Tabelle ausgeben
                print_RR_table(p, n);

                // Durchschnittliche Wartezeit berechnen und ausgeben
                printf("\nAverage Waiting Time: %.3f", calculate_average_waiting_time(p, n));

                // Durchschnittliche Bearbeitungszeit berechnen und ausgeben
                printf("\nAverage Turnaround Time: %.3f\n", calculate_average_turnaround_time(p, n));

                // Laufzeittabelle ausgeben
                simple_round_robin_scheduler(p, n);

                // zu den Options zurückgehen
                printf("\nPress m for the Menu => ");
                scanf(" %c", &ch);
                if(ch == 'm' || ch == 'M'){
                    main();
                }
                break;
            }

            case 2: {
                printf("\nEnter number of processes (max. %d): ", MAX_PROCESS);
                scanf("%d", &n);
                if(n == 0 || n < 0) {
                    printf("invalid entry, try again\n");
                    main();
                }

                printf("\nEnter Arrival Time and Burst Time for each process\n");
                for(i=0; i<n; i++){
                    p[i].pid = i+1;
                    printf("P[%d]: \t", p[i].pid);
                    printf("Arrival Time: ");
                    scanf("%d", &p[i].waiting_time);
                    printf("\tBurst Time: ");
                    scanf("%d", &p[i].burst_time);
                }

                p[0].waiting_time = 0;

                printf("\nEnter the Quantum ==> ");
                scanf("%d", &quantum);

                // RR sortieren
                sort_RR_process(p, n);

                // Durchlaufzeit berechnen
                calculate_turnaround_time(p, n);

                // RR_Tabelle ausgeben
                print_RR_table(p, n);

                // Durchschnittliche Wartezeit berechnen und ausgeben
                printf("\nAverage Waiting Time: %.3f", calculate_average_waiting_time(p, n));

                // Durchschnittliche Bearbeitungszeit berechnen und ausgeben
                printf("\nAverage Turnaround Time: %.3f\n", calculate_average_turnaround_time(p, n));

                Standard_schedul_order_request_queue(p, n);

                // zu den Options zurückgehen
                printf("\nPress m for the Menu => ");
                scanf(" %c", &ch);
                if(ch == 'm' || ch == 'M'){
                    main();
                }
                break;
            }

            default: {
                printf("\ninvalid entry, try again\n\n");
                main();
                }

            }
            break;
        }

        case 4: {
            average_waiting_time=0;
            average_turnaround_time=0;

            printf("\n\t\tRound Robin (RRP) Scheduling Algorithm\n");
            printf("\t\t=====================================\n\n");
            printf("Enter number of processes (max. %d): ", MAX_PROCESS);
            scanf("%d", &n);
            if(n == 0 || n < 0) {
                printf("invalid entry, try again\n");
                main();
            }

            printf("\nEnter Arrival Time and Burst Time for each process\n");
            for(i=0; i<n; i++){
                p[i].pid = i+1;
                printf("P[%d]: \t", p[i].pid);
                printf("Arrival Time: ");
                scanf("%d", &p[i].waiting_time);
                printf("\tBurst Time: ");
                scanf("%d", &p[i].burst_time);
                printf("\tPriority: ");
                scanf("%d", &p[i].priority);
            }

            printf("\nNo Quantum needed \n");

            // Prozessen sortieren
            sort_RRP_process(p, n);

            // Durchlaufzeit berechnen
            calculate_turnaround_time(p, n);

            // RRP_Tabelle ausgeben
            print_RRP_table(p, n);

            // Durchschnittliche Wartezeit berechnen und ausgeben
            printf("\nAverage Waiting Time: %.3f", calculate_average_waiting_time(p, n));

            // Durchschnittliche Bearbeitungszeit berechnen und ausgeben
            printf("\nAverage Turnaround Time: %.3f\n", calculate_average_turnaround_time(p, n));

            // RRP algorithm anwenden
            round_robin_with_priority(p, n);

            // zu den Options zurückgehen
            printf("\nPress m for the Menu => ");
            scanf(" %c", &ch);
            if(ch == 'm' || ch == 'M'){
                main();
            }
            break;
        }
       case 5:{
        char c;
        while (1) {
        printf("\n\nThread Option : \n");
        printf("\n\t1) Press M to show thread Simulator with Mutex:\n");
        printf("\t2) Press S to show thread Simulator ohne Mutex:\n");
        printf("\t3) Press T to show thread Table:\n");
        printf("\t4) Press E to Exit from menu:\n");
        printf("\nChoose Option : ");
        scanf(" %c", &c);
        printf("\n");
        pthread_t p[3];
        if(c=='s'||c=='S'){
            // Threads simulieren
            threads_Simulation(p);

        }else if (c=='m'||c=='M') {
            // Threads mit Mutex simulieren
            threads_Simulation_With_Mutex(p);

        }else if(c=='t'||c=='T'){
            // Threads Tabelle ausgeben
            threads_Table(p);

        }else if (c=='e'||c=='E') {
            // zu Hauptmenü zurückgehen
            main();
            break;

        }else {
             printf("\ninvalid entry, try again");
        }
        }
        break;
        }

        default:{
            printf("\ninvalid entry, try again\n\n");
            // zu Hauptmenü zurückgehen
            main();
        }

    }

    return 0;
}


/*
 *
 * berechnung der Warte Zeit Funktion
 *
 *
*/
void calculate_waiting_time(Process* p, int n){
    for(int i=0; i<n; i++){
        p[i+1].waiting_time =  p[i].waiting_time + p[i].burst_time;
    }
}


/*
 *
 * berechnung der Durchschnittliche Warte Zeit Funktion
 *
 *
*/
float calculate_average_waiting_time(Process* p, int n){
    for(int i=0; i<n; i++){
        average_waiting_time += p[i].waiting_time;
    }
    return average_waiting_time /= n;
}


/*
 *
 * berechnung der turnaround Time Funktion
 *
 *
*/
void calculate_turnaround_time(Process* p, int n){
    for(int i=0; i<n; i++){
        p[i].turnaround_time = p[i].burst_time + p[i].waiting_time;
    }
}


/*
 *
 * berechnung der Durchschnittliche turnaround Time Funktion
 *
 *
*/
float calculate_average_turnaround_time(Process* p, int n){
    for(int i=0; i<n; i++){
        average_turnaround_time += p[i].turnaround_time;
    }
    return average_turnaround_time /= n;
}


/*
 *
 * First Come First Served Funktionen
 *
 *
*/
void print_FCFS_table(Process* p, int n){
    int i;
    
    // Tabelle Ausgeben
    printf("\n\t+---------+------------+--------------+-----------------+");
    printf("\n\t| Process | Burst Time | Waiting Time | Turnaround Time |");
    printf("\n\t+---------+------------+--------------+-----------------+");
    for(i=0; i<n; i++){
        p[i].pid = i+1;
        printf("\n\t|  P[%d]         %d             %d              %d        \n",
            p[i].pid,p[i].burst_time,p[i].waiting_time,p[i].turnaround_time);
        printf("\t+---------+------------+--------------+-----------------+");
    }
}

void FCFS_simulation(Process* p, int n){

    int i, j;

    for(i=0; i<n; i++){
        p[i].pid = i+1;
        printf("P[%d]\t\t",p[i].pid);

        sleep(1);
        for(j=0; j<p[i].waiting_time; j++){
            printf(" ");
        }
        
        // jede sekunde eines burst_time
        // wurde mit Unterzeichen gezeichnet
        for(j=0; j<p[i].burst_time; j++){
            printf("_");
        }
        printf("\n");
    }
}
/*
 *
 * ende First Come First Served Funktionen
 *
 *
*/


/*
 *
 * Shortest Job First Funktionen
 *
 *
*/
void sort_SJF_process(Process* p, int n){
    int i, j, temp_bt, temp_pid;
    // SJF sortieren nach burst_time
    for(i=0; i<n; i++){
        for(j=0; j<n; j++){
            if(p[i].burst_time < p[j].burst_time){
                temp_bt = p[i].burst_time;
                p[i].burst_time = p[j].burst_time;
                p[j].burst_time = temp_bt;

                temp_pid = p[i].pid;
                p[i].pid = p[j].pid;
                p[j].pid = temp_pid;
            }
        }
    }
}

void print_SJF_table(Process* p, int n){

    printf("\nScheduler: 0\n");

    // orginal burst_time Array
    int i, j;

    printf("\n\t+---------+------------+--------------+-----------------+");
    printf("\n\t| Process | Burst Time | Waiting Time | Turnaround Time |");
    printf("\n\t+---------+------------+--------------+-----------------+");
    for(i=0; i<n; i++){
        for(j=n-1; j>=0; j--){
            if(p[i].burst_time == original_burst_time[j]){

                original_burst_time[j] = 0;
                p[i].pid = j+1;

                printf("\n\t|  P[%d]         %d             %d              %d        \n",
                    p[i].pid,p[i].burst_time,p[i].waiting_time,p[i].turnaround_time);
                printf("\t+---------+------------+--------------+-----------------+");
                break;
            }
        }
    }
}

void schedul_SJF_table(Process* p, int n){

    printf("\nScheduler: 1\n");

    int i, j;
    
    // Tabelle anzeigen
    printf("\n\t+---------+------------+--------------+-----------------+");
    printf("\n\t| Process | Burst Time | Waiting Time | Turnaround Time |");
    printf("\n\t+---------+------------+--------------+-----------------+");
    for(i=0; i<n; i++){
        for(j=0; j<n; j++){
            if(p[i].burst_time == original_burst_time[j]){

                original_burst_time[j] = 0;
                p[i].pid = j+1;

                printf("\n\t|  P[%d]         %d             %d              %d        \n",
                    p[i].pid,p[i].burst_time,p[i].waiting_time,p[i].turnaround_time);
                printf("\t+---------+------------+--------------+-----------------+");

                break;
            }
        }
    }
}

void SJF_simulation(Process* p, int n){

    // original burst_time Array
    int original_burst_time[n], i, j;

    printf("\nProcess\t\tBT Simulation\n");

    // bt Array in orgbt Array kopieren
    for(i=0; i<n; i++){
        original_burst_time[i] = p[i].burst_time;
    }

    for(i=0; i<n; i++){
        for(j=0; j<n; j++){
            if(p[i].burst_time == original_burst_time[j]){
                printf("P[%d]\t\t",p[i].pid);
                break;
            }
        }
        sleep(1);
        for(j=0; j<p[i].waiting_time; j++){
            printf(" ");
        }
        for(j=0; j<p[i].burst_time; j++){
            printf("_");
        }
        printf("\n");
    }
}
/*
 *
 * ende Shortest Job First Funktionen
 *
 *
*/


/*
 *
 * Round Robin Funktionen
 *
 *
*/
void Standard_schedul_order_request_queue(Process* p, int n){
    int i, j=0, cpu_size=0, time, gar, count=0;
    bool hold = true, hold1 = true, pass = true;

    // CPU size berechnen
    for(i=0; i<n; i++){
        original_burst_time[i] = p[i].burst_time;
        cpu_size += p[i].burst_time;
        }

    while(hold == true){
        for(i=0; i<n; i++){
            if(original_burst_time[i] != 0){
                original_burst_time[i] -= quantum;
                if(original_burst_time[i] <= 0){
                    original_burst_time[i] = 0;
                }
                count++;
            }
            j++;
            if(j == n*3){
                hold = false;
                break;
            }
        }
    }
    cpu_size = count;

    cpu cpu[cpu_size];
    int ready_queue[cpu_size];

    for(i=0; i<cpu_size; i++){
        cpu[i].garitt = 0;
        cpu[i].time = 0;
        ready_queue[i] = 0;
    }

    i = 0;
    while(i < cpu_size){
        for(j=0; j<n; j++){
            if(p[j].waiting_time == 0){
                if(p[j].burst_time != 0){
                    if(i==0){
                        cpu[i-1].time = 0;
                    }
                    ready_queue[i] = p[j].pid;
                    cpu[i].garitt = ready_queue[i];

                    if(p[j].burst_time > quantum){
                        p[j].burst_time -= quantum;
                        cpu[i].time = cpu[i-1].time + quantum;
                    }else{
                        cpu[i].time = cpu[i-1].time + p[j].burst_time;
                        p[j].burst_time = 0;
                    }
                    ++i;
                    continue;
                }
            }else if(p[j].waiting_time <= quantum){
                 if(p[j].burst_time != 0){
                    ready_queue[i] = p[j].pid;
                    cpu[i].garitt = ready_queue[i];
                    gar = cpu[i].garitt;
                    if(p[j].burst_time > quantum){
                        p[j].burst_time -= quantum;
                        cpu[i].time = cpu[i-1].time + quantum;
                        time = cpu[i].time;
                    }else{
                        cpu[i].time = cpu[i-1].time + p[j].burst_time;
                        time = cpu[i].time;
                        p[j].burst_time = 0;
                    }
                    ++i;
                    continue;
                }
            }else if(pass == true){
                pass = false;
                continue;
            }if(p[j].waiting_time > quantum){
                if(p[j].burst_time != 0){
                    ready_queue[i] = p[j].pid;
                    cpu[i].garitt = ready_queue[i];
                    gar = cpu[i].garitt;
                    if(p[j].burst_time > quantum){
                        p[j].burst_time -= quantum;
                        cpu[i].time = cpu[i-1].time + quantum;
                        time = cpu[i].time;
                    }else{
                        cpu[i].time = cpu[i-1].time + p[j].burst_time;
                        time = cpu[i].time;
                        p[j].burst_time = 0;
                    }
                    ++i;
                    continue;
                }
            }
        }
    }

    int space_size = cpu_size/2;
    printf("\n\n\t\t\t**********************");
    printf("\n\t\t\t** CPU Garitt Chart **");
    printf("\n\t\t\t**********************\n");

    // Zeichner
    printf("\n|");
    for(i=0; i<cpu_size; i++){
        for(j=0; j<cpu_size; j++){
            printf("-");
        }
        printf("|");
    }

    printf("\n");
    for(i=0; i<cpu_size; i++){
        for(j=0; j<space_size; j++){
            printf(" ");
        }
        printf("%d", cpu[i].garitt);
        space_size = cpu_size;
    }

    // Zeichner
    printf("\n|");
    for(i=0; i<cpu_size; i++){
        for(j=0; j<cpu_size; j++){
            printf("-");
        }
        printf("|");
    }

    // Timing
    hold = true;
    int timing_space_size = cpu_size, minus=1;
    printf("\n0");
    for(i=0; i<cpu_size; i++){
        if(cpu[i].time > 10 && hold == true){
            timing_space_size -= minus;
            hold = false;
        }
        for(j=0; j<timing_space_size; j++){
            printf(" ");
        }
        printf("%d", cpu[i].time);
    }
}


void simple_round_robin_scheduler(Process* p, int n){

    int i, j=0, cpu_size=0, time, gar, count=0;
    bool hold = true, hold1 = true, pass = true;

    // CPU size berechnen
    for(i=0; i<n; i++){
        original_burst_time[i] = p[i].burst_time;
        cpu_size += p[i].burst_time;
        }

    while(hold == true){
        for(i=0; i<n; i++){
            if(original_burst_time[i] != 0){
                original_burst_time[i] -= quantum;
                if(original_burst_time[i] <= 0){
                    original_burst_time[i] = 0;
                }
                count++;
            }
            j++;
            if(j == n*3){
                hold = false;
                break;
            }
        }
    }
    cpu_size = count;

    cpu cpu[cpu_size];
    int ready_queue[cpu_size];

    for(i=0; i<cpu_size; i++){
        cpu[i].garitt = 0;
        cpu[i].time = 0;
        ready_queue[i] = 0;
    }

    i = 0;
    while(i < cpu_size){
        for(j=0; j<n; j++){
            if(p[j].waiting_time == 0){
                if(p[j].burst_time != 0){
                    if(i==0){
                        cpu[i-1].time = 0;
                    }
                    ready_queue[i] = p[j].pid;
                    cpu[i].garitt = ready_queue[i];

                    if(p[j].burst_time > quantum){
                        p[j].burst_time -= quantum;
                        cpu[i].time = cpu[i-1].time + quantum;
                    }else{
                        cpu[i].time = cpu[i-1].time + p[j].burst_time;
                        p[j].burst_time = 0;
                    }
                    ++i;
                    continue;
                }
            }else if(p[j].waiting_time <= quantum){
                 if(p[j].burst_time != 0){
                    ready_queue[i] = p[j].pid;
                    cpu[i].garitt = ready_queue[i];
                    gar = cpu[i].garitt;
                    if(p[j].burst_time > quantum){
                        p[j].burst_time -= quantum;
                        cpu[i].time = cpu[i-1].time + quantum;
                        time = cpu[i].time;
                    }else{
                        cpu[i].time = cpu[i-1].time + p[j].burst_time;
                        time = cpu[i].time;
                        p[j].burst_time = 0;
                    }
                    ++i;
                    continue;
                }
            }else if(p[j].waiting_time > quantum){
                if(p[j].burst_time != 0){
                    ready_queue[i] = p[j].pid;
                    cpu[i].garitt = ready_queue[i];
                    gar = cpu[i].garitt;
                    if(p[j].burst_time > quantum){
                        p[j].burst_time -= quantum;
                        cpu[i].time = cpu[i-1].time + quantum;
                        time = cpu[i].time;
                    }else{
                        cpu[i].time = cpu[i-1].time + p[j].burst_time;
                        time = cpu[i].time;
                        p[j].burst_time = 0;
                    }
                    ++i;
                    continue;
                }
            }
        }
    }

    int space_size = cpu_size/2;
    printf("\n\n\t\t\t**********************");
    printf("\n\t\t\t** CPU Garitt Chart **");
    printf("\n\t\t\t**********************\n");

    // Zeichner
    printf("\n|");
    for(i=0; i<cpu_size; i++){
        for(j=0; j<cpu_size; j++){
            printf("-");
        }
        printf("|");
    }

    printf("\n");
    for(i=0; i<cpu_size; i++){
        for(j=0; j<space_size; j++){
            printf(" ");
        }
        printf("%d", cpu[i].garitt);
        space_size = cpu_size;
    }

    // Zeichner
    printf("\n|");
    for(i=0; i<cpu_size; i++){
        for(j=0; j<cpu_size; j++){
            printf("-");
        }
        printf("|");
    }

    // Timing
    hold = true;
    int timing_space_size = cpu_size, minus=1;
    printf("\n0");
    for(i=0; i<cpu_size; i++){
        if(cpu[i].time > 10 && hold == true){
            timing_space_size -= minus;
            hold = false;
        }
        for(j=0; j<timing_space_size; j++){
            printf(" ");
        }
        printf("%d", cpu[i].time);
    }
}


void print_RR_table(Process* p, int n){

    int original_waiting_time[n], i, j;


    // CPU size berechnen
    for(i=0; i<n; i++){
        original_waiting_time[i] = p[i].waiting_time;
        }

    printf("\n\t+---------+------------+--------------+-----------------+");
    printf("\n\t| Process | Burst Time | Waiting Time | Turnaround Time |");
    printf("\n\t+---------+------------+--------------+-----------------+");
    for(i=0; i<n; i++){
        for(j=n-1; j>=0; j--){
            if(p[i].waiting_time == original_waiting_time[j]){

                original_waiting_time[j] = 0;

                printf("\n\t|  P[%d]         %d             %d              %d        \n",
                    p[i].pid,p[i].burst_time,p[i].waiting_time,p[i].turnaround_time);
                printf("\t+---------+------------+--------------+-----------------+");
                break;
            }
        }
    }
}

void sort_RR_process(Process* p, int n){
    int i, j, temp_bt, temp_wt, temp_pid;
    
    // Prozess nach burst_time und waiting_time sortieren
    for(i=0; i<n; i++){
        for(j=0; j<n; j++){
            if(p[i].waiting_time < p[j].waiting_time){
                temp_bt = p[i].burst_time;
                p[i].burst_time = p[j].burst_time;
                p[j].burst_time = temp_bt;

                temp_wt = p[i].waiting_time;
                p[i].waiting_time = p[j].waiting_time;
                p[j].waiting_time = temp_wt;

                temp_pid = p[i].pid;
                p[i].pid = p[j].pid;
                p[j].pid = temp_pid;
            }
        }
    }
}
/*
 *
 * ende Round Robin Funktionen
 *
 *
*/


/*
 *
 * Round Robin mit Priorität Funktionen
 *
 *
*/
void sort_RRP_process(Process* p, int n){
    int i, j, temp_bt, temp_wt, temp_pid, temp_priority;
    for(i=0; i<n; i++){
        for(j=0; j<n; j++){
            if(p[i].priority < p[j].priority){
                temp_bt = p[i].burst_time;
                p[i].burst_time = p[j].burst_time;
                p[j].burst_time = temp_bt;

                temp_wt = p[i].waiting_time;
                p[i].waiting_time = p[j].waiting_time;
                p[j].waiting_time = temp_wt;

                temp_priority = p[i].priority;
                p[i].priority = p[j].priority;
                p[j].priority = temp_priority;

                temp_pid = p[i].pid;
                p[i].pid = p[j].pid;
                p[j].pid = temp_pid;
            }
        }
    }
}

void print_RRP_table(Process* p, int n){

    int original_waiting_time[n], i, j;

    // CPU size berechnen
    for(i=0; i<n; i++){
        original_waiting_time[i] = p[i].waiting_time;
        }

    printf("\n\t+---------+------------+--------------+-----------------+----------+");
    printf("\n\t| Process | Burst Time | Waiting Time | Turnaround Time | Priority |");
    printf("\n\t+---------+------------+--------------+-----------------+----------+");
    for(i=0; i<n; i++){
        for(j=n-1; j>=0; j--){
            if(p[i].waiting_time == original_waiting_time[j]){

                original_waiting_time[j] = 0;

                printf("\n\t|  P[%d]         %d             %d               %d              %d     |\n",
                    p[i].pid,p[i].burst_time,p[i].waiting_time,p[i].turnaround_time, p[i].priority);
                printf("\t+---------+------------+--------------+-----------------+----------+");
                break;
            }
        }
    }
}

void round_robin_with_priority(Process* p, int n){

    int i, j=0, cpu_size=0, time, gar, count=0;
    bool hold = true, hold1 = true, pass = true;

    // CPU size berechnen
    for(i=0; i<n; i++){
        original_burst_time[i] = p[i].burst_time;
        cpu_size += p[i].burst_time;
        }

    while(hold == true){
        for(i=0; i<n; i++){
            if(original_burst_time[i] != 0){
                original_burst_time[i] -= quantum;
                if(original_burst_time[i] <= 0){
                    original_burst_time[i] = 0;
                }
                count++;
            }
            j++;
            if(j == n*3){
                hold = false;
                break;
            }
        }
    }

    cpu_size = MAX_PROCESS;

    cpu cpu[cpu_size];
    int ready_queue[cpu_size];

    for(i=0; i<cpu_size; i++){
        cpu[i].garitt = 0;
        cpu[i].time = 0;
        ready_queue[i] = 0;
    }

    int wtHolder = 0;
    i = 0;
    for(i=0; i<n; i++){
            if(p[i].priority == 1 && p[i].waiting_time != 0){
                wtHolder = p[i].waiting_time;
                cpu[1].garitt = p[i].pid;
                cpu[1].time = p[i].burst_time + p[i].waiting_time;
                p[i].burst_time = 0;
            }else if(p[i].waiting_time == 0){
                cpu[0].garitt = p[i].pid;
                cpu[0].time = wtHolder;
                if(p[i].burst_time <= wtHolder){
                    p[i].burst_time = 0;
                }else{
                    p[i].burst_time = p[i].burst_time - wtHolder;
                }
            }
    }

    for(i=2, j=1; i<cpu_size, j<n; i++,j++){
        cpu[i].garitt = p[j].pid;
        cpu[i].time = p[j].burst_time + cpu[i-1].time;
        p[j].burst_time = 0;
    }

    for(i=0; i<n; i++){
        if(p[i].burst_time != 0){
            cpu[i].garitt = p[j].pid;
            cpu[i].time = p[j].burst_time + cpu[i-1].time;
            p[j].burst_time = 0;
        }
    }

    cpu_size = 0;
    // Garitt Array echte größe berechnen
    for(i=0; i<MAX_PROCESS; i++){
        if(cpu[i].garitt != 0){
            ++cpu_size;
        }
    }

    int space_size = cpu_size/2;
    printf("\n\n\t\t\t**********************");
    printf("\n\t\t\t** CPU Garitt Chart **");
    printf("\n\t\t\t**********************\n");

    // Zeichner
    printf("\n|");
    for(i=0; i<cpu_size; i++){
        for(j=0; j<cpu_size; j++){
            printf("-");
        }
        printf("|");
    }

    printf("\n");
    for(i=0; i<cpu_size; i++){
        for(j=0; j<space_size; j++){
            printf(" ");
        }
        printf("%d", cpu[i].garitt);
        space_size = cpu_size;
    }

    // Zeichner
    printf("\n|");
    for(i=0; i<cpu_size; i++){
        for(j=0; j<cpu_size; j++){
            printf("-");
        }
        printf("|");
    }

    // Timing
    hold = true;
    int timing_space_size = cpu_size, minus=1;
    printf("\n0");
    for(i=0; i<cpu_size; i++){
        if(cpu[i].time > 10 && hold == true){
            timing_space_size -= minus;
            hold = false;
        }
        for(j=0; j<timing_space_size; j++){
            printf(" ");
        }
        printf("%d", cpu[i].time);
    }
}
/*
 *
 * ende Round Robin mit Priorität Funktionen
 *
 *
*/


/*
 *
 * Threads Funktionen
 *
 *
*/
void* initialize_character_thread_1(){
    pthread_mutex_lock(&mutex);
    character = '+';
      sleep (1);
      printf ("Threads+ 1:  %c%c  \n", character,character);
      pthread_mutex_unlock(&mutex);
      return NULL;
}



void *initialize_character_thread_2 ()
{
  pthread_mutex_lock(&mutex);
  character = '-';
  sleep (1);
  printf ("Threads- 2:     %c%c  \n", character,character);
  pthread_mutex_unlock(&mutex);
  return NULL;
}
void *initialize_character_thread_3()
{
  pthread_mutex_lock(&mutex);
  character = '*';
  sleep (1);
  printf ("Threads* 3:       %c%c  \n", character,character);
  pthread_mutex_unlock(&mutex);
  return NULL;
}
void threads_Simulation(pthread_t* p){
    //Mutex initialisieren
    pthread_mutex_init (&mutex, NULL);
    // Mutex zustören
    pthread_mutex_destroy(&mutex);
    // Thread1 erzeugen
    pthread_create (&p[0], NULL,initialize_character_thread_1 , NULL);
    // Thread2 erzeugen
    pthread_create (&p[1], NULL,initialize_character_thread_2, NULL);
    // Thread3 erzeugen
    pthread_create (&p[2], NULL,initialize_character_thread_3 , NULL);
    // auf Beendung des threads warten
    pthread_join (p[0], NULL);
    // auf Beendung des threads warten
    pthread_join (p[1], NULL);
    // auf Beendung des threads warten
    pthread_join (p[2], NULL);
}
void threads_Simulation_With_Mutex(pthread_t* p){
    //Mutex initialisieren
    pthread_mutex_init (&mutex, NULL);
    // Thread1 erzeugen;
    pthread_create (&p[0], NULL,initialize_character_thread_1 , NULL);
    sleep(0);
    // Thread2 erzeugen
    pthread_create (&p[1], NULL,initialize_character_thread_2 , NULL);
    sleep(0);
    // Thread3 erzeugen
    pthread_create (&p[2], NULL,initialize_character_thread_3 , NULL);
    // auf Beendung des threads warten
    pthread_join (p[0], NULL);
    // auf Beendung des threads warten
    pthread_join (p[1], NULL);
    // auf Beendung des threads warten
    pthread_join (p[2], NULL);

}
void threads_Table(pthread_t* p){
    printf("\n\nThreads table :\n\n");
    printf("+--------+-----------------------+-----------+------------+----------+\n");
    printf("|Threads |     Threads_id        | Status    |   Status   |  Status  |\n");
    printf("+--------+-----------------------+-----------+------------+----------+\n");
    printf("|Thread 1|  %lu      | runing    |   finish   |  finish  |\n",p[0]);
    printf("|Thread 2|  %lu      | lock      |   runing   |  finish  |\n",p[1]);
    printf("|Thread 3|  %lu      | lock      |   lock     |  runing  |\n",p[2]);
    printf("+--------+-----------------------+-----------+------------+----------+\n");
}
void print_process_table(Process* p, int n){
    int i, j;
    // prozess_Id in pArray speichern
    int pArray[n];
    for (j=0;j<n;j++) {
        vfork();
        pArray[j]=getppid();
    }

    printf("Prozess Tabelle: \n");

    for (j=0;j<n;j++) {
    printf("\n\t+----------+---------------+----------------+------------------+");
    printf("\n\t|   P.Nr   |     P.Id      |     P.Status   |     P.Priority   |");
    printf("\n\t+----------+---------------+----------------+------------------+\n");
    // Prozess Zustand ermitellen
    for(i=0; i<n; i++){
        if(i==j){
            p[i].status[0]='r';
            p[i].status[1]='u';
            p[i].status[2]='n';
            p[i].status[3]='i';
            p[i].status[4]='n';
            p[i].status[5]='g';
        }else if (i==j+1) {
            p[i].status[0]='r';
            p[i].status[1]='e';
            p[i].status[2]='a';
            p[i].status[3]='d';
            p[i].status[4]='y';
            p[i].status[5]=' ';
        }else if (i>j+1) {
            p[i].status[0]='w';
            p[i].status[1]='a';
            p[i].status[2]='i';
            p[i].status[3]='t';
            p[i].status[4]='n';
            p[i].status[5]='g';

        }else {
            p[i].status[0]='f';
            p[i].status[1]='i';
            p[i].status[2]='n';
            p[i].status[3]='i';
            p[i].status[4]='s';
            p[i].status[5]='h';
        }

             printf("\t|   P[%d]   |     %d     |     %c%c%c%c%c%c     |          %d       | \n",p[i].pid,pArray[i],p[i].status[0],
                     p[i].status[1],p[i].status[2],p[i].status[3],p[i].status[4],p[i].status[5],i+1);
             printf("\t+----------+---------------+----------------+------------------+\n");

        }

    }
}

/*
 *
 * ende Threads Funktionen
 *
 *
*/

