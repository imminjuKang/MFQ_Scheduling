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

void queue0(Process** q0, int* q0_size, Process** q1, int* q1_size, int* clock_time, int* gantt_chart)
{
    if (q0[0]->used_time < TIME_QUANTUM0 && q0[0]->remain_time > 0)
    {
        gantt_chart[*clock_time] = q0[0]->pid;
        q0[0]->remain_time--;
        (q0[0]->used_time)++;
        return;
    }

    // time slice 끝 혹은 remain_time <= 0인 경우
    else
    {
        // cpu 더 이상 필요 없이 끝났기 때문에 finish
        if (q0[0]->remain_time == 0)
        {
            q0[0]->finish_time = *clock_time;
        }
        // burst time이 남아 있기 때문에 q1로 이동
        else
        {
            q0[0]->current_q = 1;
            q0[0]->used_time = 0;
            q1[*q1_size++] = q0[0];
        }

        // 원활한 관리를 위해 한 칸씩 앞으로 옮기기
        for (int i = 1; i < *q0_size; i++)
        {
            q0[i - 1] = q0[i];
        }

        (*q0_size)--;
        return;
    }
}

void queue1(Process** q1, int* q1_size, Process** q2, int* q2_size, int* clock_time, int* gantt_chart)
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
            q1[0]->current_q = 2;
            q1[0]->used_time = 0;
            q2[*q2_size++] = q1[0];
        }

        for (int i = 1; i < *q1_size; i++)
        {
            q1[i - 1] = q1[i];
        }
        (*q1_size)--;
        return;
    }
}

void queue2(Process** q2, int* q2_size, int* clock_time, int* gantt_chart)
{
    Process* temp;
    int time = 0;

    //편의를 위해 q2 함수를 시작할 때는 항상 정렬부터 해서
    //remain time이 가장 짧게 남은 process가 가장 앞에 오게 한다
    for (int i = 0; i < *q2_size; i++)
    {
        for (int j = 0; j < (*q2_size -1) - i; j++)
        {
            if (q2[j]->remain_time > q2[j+1]->remain_time)
            {
                temp = q2[j];
                q2[j] = q2[j+1];
                q2[j+1] = temp;
            }
        }
    }

    if (q2[0]->remain_time > 0)
    {
        gantt_chart[*clock_time] = q2[0]->pid;
        q2[0]->remain_time--;
        (q2[0]->used_time)++;
        return;
    }
    //remain time 끝나면 finish time 기록하고 return
    else if (q2[0]->remain_time == 0)
    {
        q2[0]->finish_time = *clock_time;
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
    Process* q0[num_process];
    Process* q1[num_process];
    Process* q2[num_process];

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
    int gantt_chart[MAX_LENGTH];

    // queue size 관리
    int q0_size = 0;
    int q1_size = 0;
    int q2_size = 0;

    // 원활한 관리를 위해 1초 단위로 관리
    while (1)
    {
        // cpu clock time과 arrival time이 같으면 q0으로 들어감
        for (int i = 0; i < num_process; i++)
        {
            if (process[i].arrival_time == clock_time)
            {
                process[i].current_q = 0;
                process[i].used_time = 0;
                q0[q0_size++] = &process[i];
            }
        }

        // q0부터 q1, q2 순서로 실행
        // q0이 비어 있지 않으면, q0부터 cpu 할당
        if (q0_size > 0)
        {
            queue0(q0, &q0_size, q1, &q1_size, &clock_time, gantt_chart);
        }
        // q0이 비어 있으면 q1 확인 후 cpu 할당
        else if (q1_size > 0)
        {
            queue1(q1, &q1_size, q2, &q2_size, &clock_time, gantt_chart);
        }
        // q1도 비어 있으면 q2 확인 후 cpu 할당
        else if (q2_size > 0)
        {
            queue2(q2, &q2_size, &clock_time, gantt_chart);
        }
        // 세 개의 큐가 모두 비어 있으면 cpu clock time은 끝남(while loop 탈출)
        else
        {
            break;
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
    printf("\n  PID   TT   WT\n");
    printf("=======================\n");
    for (int i = 0; i < num_process; i++)
    {
        turnaround_time = process[i].finish_time - process[i].arrival_time;
        waiting_time = turnaround_time - process[i].burst_time;
        avg_tt += turnaround_time;
        avg_wt += waiting_time;
        printf("%3d %4d %4d\n", process[i].pid, turnaround_time, waiting_time);
    }

    // 평균 TT, WT
    printf("Avergae TT: %.1f\n", avg_tt / num_process);
    printf("Average WT: %.1f", avg_wt / num_process);
}
