#include <stdio.h>
#define TIME_QUANTUM0 2
#define TIME_QUANTUM1 4
#define MAX_LENGTH 100

typedef struct Process
{
    int pid;
    int arrival_time;
    int burst_time;
    int remain_time;
    int finish_time;
    int current_q;

} Process;

typedef struct Queue
{
    int front;
    int rear;
    int size;
    Process *process_info[MAX_LENGTH];

} Queue;

void enqueue(Queue *q, Process *p)
{
    q->process_info[q->rear] = p;
    q->rear++;
    q->size++;
}

Process *dequeue(Queue *q)
{
    Process *p = q->process_info[q->front];
    q->front++;
    q->size--;
    return p;
}

int isEmpty(Queue *q)
{
    if (q->front == q->rear)
        return 1;
    else
        return 0;
}

Process *spn(Queue *q)
{
    if (q->size == 0)
        return NULL;

    int min = q->front;
    for (int i = q->front + 1; i < q->rear; i++)
    {
        if (q->process_info[i]->remain_time < q->process_info[min]->remain_time)
        {
            min = i;
        }
    }

    Process *shortest = q->process_info[min];

    for (int i = min; i < q->rear - 1; i++)
    {
        q->process_info[i] = q->process_info[i + 1];
    }

    q->rear--;
    q->size--;

    return shortest;
}

int main()
{
    // 입력
    FILE *fp = fopen("input.txt", "r");
    if (fp == NULL)
    {
        printf("Error: Cannot open file");
        return 1;
    }

    int num_process = 0;

    if (fscanf(fp, "Number of Processes: %d\n", &num_process) != 1)
    {
        fprintf(stderr, "No input for process count\n");
        fclose(fp);
        return 1;
    }

    // 프로세스를 담고 있는 배열
    Process process[num_process];

    for (int i = 0; i < num_process; i++)
    {
        if (fscanf(fp, "%d, %d, %d\n", &process[i].pid, &process[i].arrival_time, &process[i].burst_time) != 3)
        {
            fprintf(stderr, "No data for processes.\n");
            fclose(fp);
            return 1;
        }
        process[i].remain_time = process[i].burst_time;
    }

    fclose(fp);

    Queue q0, q1, q2;
    q0.front = q0.rear = q0.size = 0;
    q1.front = q1.rear = q1.size = 0;
    q2.front = q2.rear = q2.size = 0;

    int gantt_chart[MAX_LENGTH] = {0};
    int clock_time = 0;

    Process *running_p = NULL;
    int time_quantum = 0;

    while (1)
    {
        for (int i = 0; i < num_process; i++)
        {
            if (process[i].arrival_time == clock_time)
            {
                enqueue(&q0, &process[i]);
            }
        }

        if (running_p != NULL)
        {
            gantt_chart[clock_time] = running_p->pid;
            running_p->remain_time--;
            time_quantum++;
            if (running_p->remain_time == 0)
            {
                running_p->finish_time = clock_time;
                running_p = NULL;
                time_quantum = 0;
            }
            else if (running_p->current_q == 0 && time_quantum == TIME_QUANTUM0)
            {
                enqueue(&q1, running_p);
                running_p = NULL;
                time_quantum = 0;
            }
            else if (running_p->current_q == 1 && time_quantum == TIME_QUANTUM1)
            {
                enqueue(&q2, running_p);
                running_p = NULL;
                time_quantum = 0;
            }
        }

        if (running_p == NULL)
        {
            if (!isEmpty(&q0))
            {
                running_p = dequeue(&q0);
                running_p->current_q = 0;
                time_quantum = 0;
                gantt_chart[clock_time] = running_p->pid;
            }
            else if (!isEmpty(&q1))
            {
                running_p = dequeue(&q1);
                running_p->current_q = 1;
                time_quantum = 0;
                gantt_chart[clock_time] = running_p->pid;
            }
            else if (!isEmpty(&q2))
            {
                running_p = spn(&q2);
                running_p->current_q = 2;
                time_quantum = 0;
                gantt_chart[clock_time] = running_p->pid;
            }
            else
            {
                break;
            }
        }

        clock_time++;
    }

    // 출력
    // Gantt Chart
    printf("Gantt Chart\n");
    // PID 출력
    int prev = -1;
    for (int i = 0; i < clock_time; i++)
    {
        if (gantt_chart[i] != prev)
        {
            printf("|P%-2d", gantt_chart[i]);
            prev = gantt_chart[i];
        }
    }
    printf("|\n");

    // 실행시간 출력
    prev = -1;
    for (int i = 0; i < clock_time; i++)
    {
        if (gantt_chart[i] != prev)
        {
            printf("%-4d", i);
            prev = gantt_chart[i];
        }
    }
    printf("%d\n", clock_time);

    int turnaround_time = 0;
    int waiting_time = 0;
    double avg_tt = 0;
    double avg_wt = 0;

    // 각 프로세스별 TT, WT
    printf("\nPID   TT     WT\n");
    printf("==================\n");
    for (int i = 0; i < num_process; i++)
    {
        turnaround_time = process[i].finish_time - process[i].arrival_time;
        waiting_time = turnaround_time - process[i].burst_time;
        avg_tt += turnaround_time;
        avg_wt += waiting_time;
        printf("P%d %5d %6d\n", process[i].pid, turnaround_time, waiting_time);
    }

    // 평균 TT, WT
    printf("\nAvergae TT: %.1f\n", avg_tt / num_process);
    printf("Average WT: %.1f", avg_wt / num_process);
}
