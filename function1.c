#include <stdio.h>
#define TIME_QUANTUM0 2
#define TIME_QUANTUM1 4
#define MAX_LENGTH 1000

typedef struct Process
{
    int pid;
    int arrival_time;
    int burst_time;
    int remain_time;
    int finish_time;
    int current_q;
    int used_time;

} Process;

void queues(Process **q0, int *q0_size, Process **q1, int *q1_size, Process **q2, int *q2_size,
            int *clock_time, int *gantt_chart)
{
    if (q0_size > 0)
    {
        if (q0[0]->used_time < TIME_QUANTUM0 && q0[0]->remain_time > 0)
        {
            gantt_chart[*clock_time] = q0[0]->pid;
            q0[0]->remain_time--;
            (q0[0]->used_time)++;
            return;
        }

        else
        {
            if (q0[0]->remain_time == 0)
            {
                q0[0]->finish_time = *clock_time;
            }
            else
            {
                q0[0]->current_q = 1;
                q0[0]->used_time = 0;
                q1[*q1_size] = q0[0];
                (*q1_size)++;
            }

            for (int i = 1; i < *q0_size; i++)
            {
                q0[i - 1] = q0[i];
            }

            (*q0_size)--;
            return;
        }
    }
    else if (q1_size > 0)
    {
        if (q1[0]->used_time < TIME_QUANTUM1 && q1[0]->remain_time > 0)
        {
            gantt_chart[*clock_time] = q1[0]->pid;
            q1[0]->remain_time--;
            (q1[0]->used_time)++;
            return;
        }

        else
        {
            if (q1[0]->remain_time == 0)
            {
                q1[0]->finish_time = *clock_time;
            }
            else
            {
                q1[0]->current_q = 1;
                q1[0]->used_time = 0;
                q2[*q2_size] = q1[0];
                (*q2_size)++;
            }

            for (int i = 1; i < *q0_size; i++)
            {
                q1[i - 1] = q1[i];
            }

            (*q1_size)--;
            return;
        }
    }
    else if (q2_size > 0)
    {
        Process *temp;

        for (int i = 0; i < *q2_size; i++)
        {
            for (int j = 0; j < (*q2_size - 1) - i; j++)
            {
                if (q2[j]->remain_time > q2[j + 1]->remain_time)
                {
                    temp = q2[j];
                    q2[j] = q2[j + 1];
                    q2[j + 1] = temp;
                }
            }
        }

        for (int i = 0; i < *q2_size; i++)
        {
            while (q2[i]->remain_time > 0)
            {
                gantt_chart[*clock_time] = q2[i]->pid;
                q2[0]->remain_time--;
                (*clock_time)++;
            }
            q2[i]->finish_time = *clock_time;
        }
        return;
    }
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
    // 프로세스 배열에 있는 프로세스 정보를 직접 관리하기 위해 포인터로 선언
    Process *q0[num_process];
    Process *q1[num_process];
    Process *q2[num_process];

    for (int i = 0; i < num_process; i++)
    {
        if (fscanf(fp, "%d, %d, %d\n", &process[i].pid, &process[i].arrival_time, &process[i].burst_time) != 3)
        {
            fprintf(stderr, "No data for processes.\n");
            fclose(fp);
            return 1;
        }
    }

    fclose(fp);

    // 남은 시간을 burst_time으로 초기화하기
    for (int i = 0; i < num_process; i++)
    {
        process[i].remain_time = process[i].burst_time;
        process[i].finish_time = 0;
    }

    // main 함수에서는 cpu를 구현
    int clock_time = 0;
    int gantt_chart[MAX_LENGTH] = {0};

    // queue size 관리
    int q0_size = 0;
    int q1_size = 0;
    int q2_size = 0;

    // cpu time은 여기서 흘러가도록 구현
    while (1)
    {
        for (int i = 0; i < num_process; i++)
        {
            if (process[i].arrival_time == clock_time)
            {
                process[i].current_q = 0;
                process[i].used_time = 0;
                q0[q0_size] = &process[i];
                q0_size++;
            }
        }
        if (q0_size == 0 && q1_size == 0)
        {
            queues(q0, &q0_size, q1, &q1_size, q2, &q2_size, &clock_time, gantt_chart);
            clock_time;
        }
        else
        {
            queues(q0, &q0_size, q1, &q1_size, q2, &q2_size, &clock_time, gantt_chart);
            break;
        }
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
    printf("\n PID   TT     WT\n");
    printf("==================\n");
    for (int i = 0; i < num_process; i++)
    {
        turnaround_time = process[i].finish_time - process[i].arrival_time;
        waiting_time = turnaround_time - process[i].burst_time;
        avg_tt += turnaround_time;
        avg_wt += waiting_time;
        printf("%3d %5d %6d\n", process[i].pid, turnaround_time, waiting_time);
    }

    // 평균 TT, WT
    printf("\nAvergae TT: %.1f\n", avg_tt / num_process);
    printf("Average WT: %.1f", avg_wt / num_process);
}
