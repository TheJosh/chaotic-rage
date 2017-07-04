/**
* Thread safe queue, uses STL queue + SDL thread primitaves
*
* Original source:
*    https://github.com/zeebok/SDL_Threadpool
*    Ryan Kornheisl
*
* License delcared in this email:
*    Glad you like the code! I don't have it under any license,
*    but you're free to steal it and do with it as you please :)
*    I guess that would technically make it the MIT license?
**/

#ifndef _THREADSAFEQUEUE_H_
#define _THREADSAFEQUEUE_H_

#include <queue>
#include <SDL2/SDL.h>

template <class T>
class ThreadSafeQueue
{
    private:
        std::queue<T> workQueue;
        SDL_mutex* lock;
        SDL_cond* available;

    public:
        ThreadSafeQueue(void);
        ~ThreadSafeQueue(void);

        ThreadSafeQueue(ThreadSafeQueue* q);

        void push(T job);
        T pop(void);
        bool trypop(T& top);
};

template <class T>
ThreadSafeQueue<T>::ThreadSafeQueue(void)
{
    lock = SDL_CreateMutex();
    available = SDL_CreateCond();
}

template <class T>
ThreadSafeQueue<T>::~ThreadSafeQueue(void)
{
    SDL_LockMutex(lock);
    while(!workQueue.empty())
        workQueue.pop();
    SDL_UnlockMutex(lock);

    SDL_DestroyMutex(lock);
    SDL_DestroyCond(available);
}

//ThreadSafeQueue::ThreadSafeQueue(ThreadSafeQueue* q);

template <class T>
void ThreadSafeQueue<T>::push(T job)
{
    SDL_LockMutex(lock);
    workQueue.push(job);
    SDL_CondBroadcast(available);
    SDL_UnlockMutex(lock);
}

template <class T>
T ThreadSafeQueue<T>::pop(void)
{
    SDL_LockMutex(lock);
    while(workQueue.empty())
        SDL_CondWait(available,lock);
    T top = workQueue.front();
    workQueue.pop();
    SDL_UnlockMutex(lock);

    return top;
}

template <class T>
bool ThreadSafeQueue<T>::trypop(T& top)
{
    SDL_LockMutex(lock);
    if (workQueue.empty()) {
        SDL_UnlockMutex(lock);
        return false;
    } else {
        top = workQueue.front();
        workQueue.pop();
        SDL_UnlockMutex(lock);
        return true;
    }
}

#endif
