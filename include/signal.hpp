#ifndef RLRPG_SIGNAL
#define RLRPG_SIGNAL

#include<list>
#include<functional>
#include<tl/optional.hpp>

template<class... Args>
class Signal {
public:
    using Signature = void(Args...);
    using Slot = std::function<Signature>;

    class Connection {
    public:
        bool connected() const {
            return slotIter.has_value();
        }

        void disconnect() {
            if (connected()) {
                parent.slots.erase(*slotIter);
                slotIter.clear();
            }
        }

    private:
        friend Signal;
        using SlotIter = typename std::list<Slot>::const_iterator;

        Connection(Signal& sig, SlotIter iter)
            : parent(sig)
            , slotIter(iter) {}

        Signal& parent;
        tl::optional<SlotIter> slotIter;
    };

    Connection connect(Slot newSlot) {
        auto const it = slots.insert(slots.end(), std::move(newSlot));
        return Connection(*this, it);
    }

    int fire(Args const&... args) const {
        for (auto& slot: slots) {
            slot(args...);
        }
        return slots.size();
    }

    int operator()(Args const&... args) const {
        return fire(args...);
    }

private:
    std::list<Slot> slots;
};

#endif // RLRPG_SIGNAL

