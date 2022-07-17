
#include <memory>
#include <mutex>
#include <optional>

template<typename T>
class MultithreadQueue
{
public:
    struct Node
    {
        std::optional<T> data;
        std::unique_ptr<Node> next;
    };

    MultithreadQueue()
        : head(std::make_unique<Node>(std::nullopt, nullptr))
        , tail(head.get())
    {
    }

    void push_back(T data)
    {
        auto newTail = std::make_unique<Node>(std::nullopt, nullptr);
        std::scoped_lock lock(tailMux);
        tail->data = std::move(data);
        tail->next = std::move(newTail);
        tail = tail->next.get();
    }


    std::optional<T> try_pop()
    {
        std::scoped_lock lock(headMux);
        auto currentTail = getSafeTail();
        if (currentTail == head.get())
        {
            return std::nullopt;
        }
        auto ret = head->data;
        head = std::move(head->next);
        return ret;
    }

private:
    Node* getSafeTail()
    {
        std::scoped_lock lock(tailMux);
        return tail;
    }

    std::unique_ptr<Node> head;
    Node* tail;
    std::mutex headMux;
    std::mutex tailMux;
};


int main()
{
    MultithreadQueue<int> que;
    return 0;
}