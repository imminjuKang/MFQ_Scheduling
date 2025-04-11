#include <stdio.h>
#define TIME_QUANTUM0 2 // q0와 q1의 time_quantum은 고정
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

} Process;

// 프로세스는 모든 큐에서 동일하게 관리되어야 하기 때문에
// 포인터로 선언
typedef struct Queue
{
    int front;
    int rear;
    int size;
    Process *process_info[MAX_LENGTH];

} Queue;

// 프로세스를 큐에 삽입
void enqueue(Queue *q, Process *p)
{
    q->process_info[q->rear] = p;
    q->rear++;
    q->size++;
}

// 프로세스를 큐에서 삭제
Process *dequeue(Queue *q)
{
    Process *p = q->process_info[q->front];
    q->front++;
    q->size--;
    return p;
}

// 해당 큐가 비어 있는지 확인
// 큐가 비어 있으면 다음 순위의 큐로 옮겨가기 위해
int isEmpty(Queue *q)
{
    if (q->front == q->rear)
        return 1;
    else
        return 0;
}

// q2를 실행할 때 필요한 코드
// SPN 기법 적용
Process *spn(Queue *q)
{
    if (q->size == 0)
        return NULL; // q2에 아무것도 없으면 NULL 반환

    // remain_time이 가장 짧은 프로세스를 찾아 실행
    // 남은 시간이 같으면 먼저 들어온 순으로 실행
    int min = q->front;
    for (int i = q->front + 1; i < q->rear; i++)
    {
        if (q->process_info[i]->remain_time < q->process_info[min]->remain_time)
        {
            min = i;
        }
    }

    Process *shortest = q->process_info[min];

    //반환하려는 프로세스를 기준으로 뒤에 있는 프로세스 앞으로 옮기기
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

    // 프로세스를 담고 있는 배열 선언
    Process process[num_process];

    for (int i = 0; i < num_process; i++)
    {
        if (fscanf(fp, "%d, %d, %d\n", &process[i].pid, &process[i].arrival_time, &process[i].burst_time) != 3)
        {
            fprintf(stderr, "No data for processes.\n");
            fclose(fp);
            return 1;
        }
        // remain_time은 처음에 burst_time으로 초기화
        process[i].remain_time = process[i].burst_time;
    }

    fclose(fp);

    // 각 큐 q0, q1, q2를 선언
    Queue q0, q1, q2;
    q0.front = q0.rear = q0.size = 0;
    q1.front = q1.rear = q1.size = 0;
    q2.front = q2.rear = q2.size = 0;

    int gantt_chart[MAX_LENGTH] = {0};
    int clock_time = 0;

    //현재 cpu에서 실행하고 있는 프로세스를 가리키는 포인터
    //process 정보 직접 수정하기 위해 포인터로 선언
    Process *running_p = NULL;
    int time_quantum = 0;

    //cpu 실행 시작(while loop)
    while (1)
    {
        //프로세스 생성 및 q0으로 진입
        for (int i = 0; i < num_process; i++)
        {
            if (process[i].arrival_time == clock_time)
            {
                enqueue(&q0, &process[i]);
            }
        }

        // 모든 실행 과정은 간트 차트에 기록
        //어떤 프로세스가 실행 중에 있는 경우
        //q0이나 q1의 time slice 남아 있거나 q2에서 실행
        if (running_p != NULL)
        {
            gantt_chart[clock_time] = running_p->pid;
            running_p->remain_time--;
            time_quantum++;
            //프로세스 실행 완료된 경우
            if (running_p->remain_time == 0)
            {
                running_p->finish_time = clock_time;
                running_p = NULL;
                time_quantum = 0;
            }
            //q0에 있는 프로세스는 2초 제한
            else if (running_p->current_q == 0 && time_quantum == TIME_QUANTUM0)
            {
                //time quantum 끝나면 하위 큐로 진입
                enqueue(&q1, running_p);
                running_p = NULL;
                time_quantum = 0;
            }
            //q1에 있는 프로세스는 4초 제한
            else if (running_p->current_q == 1 && time_quantum == TIME_QUANTUM1)
            {
                enqueue(&q2, running_p);
                running_p = NULL;
                time_quantum = 0;
            }
        }

        //실행중인 프로세스가 없는 경우
        //time slice가 끝났거나, 프로세스가 종료된 경우
        if (running_p == NULL)
        {
            //우선순위에 따라 q0 -> q1 -> q2 순으로 확인
            if (!isEmpty(&q0))
            {
                //어떤 프로세스가 cpu에 할당되면 간트 차트 기록
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
            //q0, q1, q2가 모두 비어 있으면 cpu 실행 종료
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
    // 가시성을 위해 간트 차트에 기록된 PID가 달라지는 순간만 PID 출력
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
    // cpu 스케줄링이 끝난 시간도 출력
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
