//
// Created by cufon on 17.04.24.
//

#include "protocom/WorkQueue.h"

namespace protocom {
    void WorkQueue::pushItem(const WorkItem &itm) {
        std::lock_guard<std::mutex> lock(queueMutex);
        if(itemLimit && queue.size() >= itemLimit) return; // do nothing if limit hit
        queue.push(itm);
        cv.notify_one();
    }

    void WorkQueue::fetchItemBlocking(WorkItem &itm) {
        std::unique_lock<std::mutex> lock(queueMutex);
        fetchCancelled = false;
        cv.wait(lock,[this] {return !queue.empty() || fetchCancelled;});
        if(fetchCancelled) {
            throw CancelledException();
        }
        itm = std::move(queue.front());
        queue.pop();
    }

    bool WorkQueue::fetchItemNonBlocking(WorkItem &itm) {
        std::lock_guard<std::mutex> lock(queueMutex);
        if(queue.empty()) return false;

        itm = std::move(queue.front());
        queue.pop();
        return true;
    }

    int WorkQueue::getQueuedCount() {
        std::lock_guard<std::mutex> lock(queueMutex);
        return queue.size();
    }

    WorkQueue::WorkQueue() : itemLimit(0), fetchCancelled(false) {}

    void WorkQueue::setItemLimit(unsigned long n) {
        itemLimit = n;
    }

    void WorkQueue::cancelFetch() {
        std::lock_guard<std::mutex> lock(queueMutex);
        fetchCancelled = true;
        cv.notify_all();
    }

    void WorkQueue::pushItem(WorkItem &&itm) {
        std::lock_guard<std::mutex> lock(queueMutex);
        if(itemLimit && queue.size() >= itemLimit) return; // do nothing if limit hit
        queue.emplace(std::move(itm));
        cv.notify_one();
    }

    const char *WorkQueue::CancelledException::what() const noexcept {
        return "Work queue blocking fetch was cancelled";
    }
} // protocom