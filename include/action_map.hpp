#ifndef RLRPG_ACTION_MAP
#define RLRPG_ACTION_MAP

#include<signal.hpp>

#include<functional>
#include<unordered_map>
#include<variant>

template<class Key>
class ActionMap {
public:
    using HandlerNoArgs = std::function<void()>;
    using HandlerWithKeyArg = std::function<void(Key const&)>;

    class Connection {
    public:
        bool connected() const noexcept {
            return not std::holds_alternative<NoConnection>(varParentConnection);
        }

        void disconnect() {
            if (connected()) {
                std::visit([] (auto& conn) {
                    conn.disconnect();
                }, varParentConnection);
                varParentConnection = NoConnection{};
            }
        }

    private:
        using NoArgsConnection = Signal<>::Connection;
        using KeyArgConnection = typename Signal<Key const&>::Connection;
        struct NoConnection {};

        friend ActionMap;

        template<class Conn>
        Connection(ActionMap& actions, Conn conn)
            : parent(actions)
            , varParentConnection(conn) {}

        ActionMap& parent;
        std::variant<NoConnection, NoArgsConnection, KeyArgConnection> varParentConnection;
    };

    Connection connect(Key const& key, HandlerNoArgs handler) {
        return Connection(*this,
            handlersWithoutArgs[key].connect(std::move(handler)));
    }

    Connection connect(Key const& key, HandlerWithKeyArg handler) {
        return Connection(*this,
            handlersWithKeyArg[key].connect(std::move(handler)));
    }

    int run(Key const& key) const {
        int totalHandlers{};

        if (const auto it = handlersWithoutArgs.find(key); it != handlersWithoutArgs.end())
            totalHandlers += it->second.fire();

        if (const auto it = handlersWithKeyArg.find(key); it != handlersWithKeyArg.end())
            totalHandlers += it->second.fire(key);

        return totalHandlers;
    }

private:
    std::unordered_map<Key, Signal<>> handlersWithoutArgs;
    std::unordered_map<Key, Signal<Key const&>> handlersWithKeyArg;
};

#endif // RLRPG_ACTION_MAP

