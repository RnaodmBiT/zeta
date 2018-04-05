#pragma once

#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <vector>

class EventContext;

class EventBase {
public:
    virtual void unregister_context(EventContext* context) = 0;
    virtual void swap_contexts(EventContext* old, EventContext* ctx) = 0;
};

template <class ...Args>
class Event : EventBase {
public:
    typedef std::function<void(Args...)> Delegate;

    ~Event() {
        for (auto& d : delegates) {
            d.first->clear_event(this);
        }
    }

    void operator()(const Args&... args) {
        for (auto& d : delegates) {
            if (!remove.count(d.first)) {
                d.second(std::forward<Args>(args)...);
            }
        }

        clear_contexts();
    }

private:
    std::unordered_map<EventContext*, Delegate> delegates;
    std::unordered_set<EventContext*> remove;

    void register_context(EventContext* context, Delegate delegate) {
        delegates[context] = delegate;
    }

    void unregister_context(EventContext* context) override {
        remove.insert(context);
    }

    void swap_contexts(EventContext* old, EventContext* ctx) override {
        delegates[ctx] = delegates[old];
        delegates.erase(old);
    }

    void clear_contexts() {
        for (auto c : remove) {
            delegates.erase(c);
        }
        remove.clear();
    }

    friend class EventContext;
};


/*
    Inherit from this class if you want to be able to register to events from other objects.
*/
class EventContext {
public:

    EventContext() { }

    EventContext(EventContext&& move) {
        *this = std::move(move);
    }

    void operator=(EventContext&& move) {
        std::swap(events, move.events);

        for (auto event : events) {
            event->swap_contexts(&move, this);
        }
    }

    EventContext(const EventContext&) = delete;
    void operator=(const EventContext&) = delete;

    ~EventContext() {
        for (auto event : events) {
            event->unregister_context(this);
        }
    }

    template <class ...Args>
    void register_event(Event<Args...>& event, typename Event<Args...>::Delegate delegate) {
        event.register_context(this, delegate);
        events.insert(&event);
    }

    template <class ...Args>
    void unregister_event(Event<Args...>& event) {
        event.unregister_context(this);
        events.erase(&event);
    }

private:

    void clear_event(EventBase* event) {
        events.erase(event);
    }

    std::unordered_set<EventBase*> events;

    template <class ...Args> friend class Event;
};