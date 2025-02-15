# thread_work_Stealing_example

Work Stealing Thread Pool – Function Overview
1. WorkStealingThreadPool(int num_threads)
Description:
Initializes a thread pool with num_threads worker threads.
Each worker has its own priority queue for task execution.
Workers execute their own tasks first, then steal tasks from others if idle.
Parameters:
num_threads (int) – Number of worker threads to create.
2. void submit(int priority, std::function<void()> task, int thread_id)
Description:
Submits a new task to the thread pool with a given priority.
The task is assigned to a specific thread’s priority queue.
Higher priority tasks (higher integer value) execute first.
Parameters:
priority (int) – Priority level (higher values get executed first).
task (std::function<void()>) – The function to execute as a task.
thread_id (int) – The thread ID to which the task is assigned.
3. void worker(int thread_id)
Description:
The main function that runs inside each worker thread.
Fetches and executes tasks from its own queue first.
If its queue is empty, it attempts to steal work from other threads.
If no tasks are found, it sleeps and waits for new work.
Parameters:
thread_id (int) – The ID of the worker thread.
4. ~WorkStealingThreadPool()
Description:
Safely shuts down the thread pool.
Signals all threads to stop execution.
Ensures all worker threads finish and join back before exiting.
Helper Components
🔹 std::vector<std::priority_queue<Task>> task_queues;
Stores the tasks for each thread as a priority queue.
Tasks are executed in order of highest priority first.
🔹 std::vector<std::mutex> queue_mutexes;
Each task queue is protected by a separate mutex for thread safety.
🔹 std::condition_variable cv;
Used to wake up idle threads when new tasks arrive.
Execution Flow
Initialize Pool: Create worker threads and start execution.
Submit Tasks: Tasks are pushed into worker queues based on priority.
Work Execution: Threads process their own tasks first.
Work Stealing: If idle, threads attempt to steal tasks from others.
Graceful Shutdown: When the pool is destroyed, all threads are stopped safely
