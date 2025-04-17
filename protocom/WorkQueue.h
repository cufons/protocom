//
// Created by cufon on 17.04.24.
//

#ifndef PROTOCOM_WORKQUEUE_H
#define PROTOCOM_WORKQUEUE_H
#include "IFrameIO.h"
#include <queue>
#include <mutex>
#include <condition_variable>
namespace protocom {
    using WorkItem = std::pair<int,PFrame>;
    class WorkQueue {
        std::queue<WorkItem> queue;
        std::mutex queueMutex;
        std::condition_variable cv;
        unsigned long itemLimit;
        bool fetchCancelled;
    public:
        class CancelledException : std::exception {
        public:
        private:
            const char *what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW override;

        };
        WorkQueue();
        void setItemLimit(unsigned long n);
        void pushItem(const WorkItem& itm);
        void pushItem(WorkItem&& itm);
        void fetchItemBlocking(WorkItem& itm);
        bool fetchItemNonBlocking(WorkItem& itm);
        int getQueuedCount();
        void cancelFetch();
    };

} // protocom

#endif //PROTOCOM_WORKQUEUE_H
