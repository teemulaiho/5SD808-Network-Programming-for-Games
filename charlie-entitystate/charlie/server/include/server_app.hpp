// server_app.hpp

#ifndef SERVER_APP_HPP_INCLUDED
#define SERVER_APP_HPP_INCLUDED

#include <charlie_application.hpp>

using namespace charlie;

struct ClientList {
    ClientList()
        : next_(0)
    {
    }

    int32 add_client(const uint64 connection)
    {
        const int32 id = next_++;
        clients_.push_back({ id, connection });
        return id;
    }

    int32 find_client(const uint64 connection)
    {
        for (auto& client : clients_) {
            if (client.connection_ == connection) {
                return client.id_;
            }
        }
        return -1;
    }

    void remove_client(const uint64 connection)
    {
        auto it = clients_.begin();
        while (it != clients_.end()) {
            if ((*it).connection_ == connection) {
                clients_.erase(it);
                break;
            }
            ++it;
        }
    }

    struct Client {
        int32  id_{ -1 };
        uint64 connection_{};
    };

    int32 next_;
    DynamicArray<Client> clients_;
};

struct InputList {
    InputList()
        : next_(0)
        , lasttick_(0)
    {
    }

    int32 add_input(uint8 input_bits, uint32 tick, Vector2 position)
    {
        const int32 id = next_++; 
       
        if (tick > lasttick_)
        {
            lasttick_ = tick;
            inputs_.push_back({ id, input_bits, tick, position });
        }

        return id;
    }

    int32 add_input(uint8* input_bits, uint64 count, uint32 tick)
    {
        //const int32 id = next_++;
        //inputs_.push_back({ id, input_bits, tick });
        //return id;

        // Client: inputs[21]
        // inputs[0] = tick_ 101;
        // inputs[1] = tick_ 100;

        for (int i = 0; i < count; i++)
        {
            inputs_.at(i).input_bits_ = input_bits[i];
            inputs_.at(i).tick_ = tick - i;
        }

        return 0;
    }

    int32 add_input(uint8 input_bits, uint32 tick)
    {
        const int32 id = next_++;
        inputs_.push_back({ id, input_bits, tick});
        return id;
    }

    uint8 find_input(uint32 tick)
    {
        for (auto& input : inputs_) {
            if (input.tick_ == tick) {
                return input.input_bits_;
            }
        }

        //printf("no input found for tick: %u\n", tick);
        return 0;
    }

    Vector2 find_pos(uint32 tick)
    {
        Vector2 no_pos{ -1, -1 };
        for (auto& command : inputs_) {
            if (command.tick_ == tick) {
                return command.predicted_pos_;
            }
        }

        printf("Could not find predicted position on tick: %u\n", tick);
        return no_pos;
    }

    void remove_input(uint32 tick)
    {
        auto it = inputs_.begin();
        while (it != inputs_.end()) {
            if ((*it).tick_ == tick) {
                inputs_.erase(inputs_.begin(), it);
                break;
            }
            ++it;
        }
    }

    struct Input {
        int32 id_{ -1 };
        uint8 input_bits_;
        uint32 tick_;
        Vector2 predicted_pos_;
    };

    int32 next_;

    uint32 lasttick_;
    DynamicArray<Input> inputs_;
};

struct PlayerList {
    PlayerList()
        : next_(0)
    {
    }

    void add_player(const int32 id)
    {
        players_.push_back({ id });
    }

    int32 find_player(const int32 id)
    {
        for (auto& player : players_) {
            if (player.id_ == id) {
                return player.id_;
            }
        }
        return -1;
    }

    struct Player {
        int32 id_{ -1 };
        InputList inputs_;
        Vector2 position_;
    };

    int32 next_;
    DynamicArray<Player> players_;
};

struct ServerApp final : Application, network::IServiceListener, network::IConnectionListener {
   ServerApp();

   // note: Application
   virtual bool on_init();
   virtual void on_exit();
   virtual bool on_tick(const Time &dt);
   virtual void on_draw();

   // note: IServiceListener
   virtual void on_timeout(network::Connection *connection);
   virtual void on_connect(network::Connection *connection);
   virtual void on_disconnect(network::Connection *connection);

   // note: IConnectionListener 
   virtual void on_acknowledge(network::Connection *connection, const uint16 sequence);
   virtual void on_receive(network::Connection *connection, network::NetworkStreamReader &reader);
   virtual void on_send(network::Connection *connection, const uint16 sequence, network::NetworkStreamWriter &writer);

   const Time tickrate_;
   Time accumulator_;
   uint32 tick_;
   ClientList clients_;
   PlayerList players_;

   gameplay::Entity entity_;
   Vector2 send_position_;

   Random random_;
   gameplay::Player player_;
   uint8 player_input_bits_;

   int networkservertickpackagecount_;
   int networkserverpositionpackagecount_;
};

#endif // !SERVER_APP_HPP_INCLUDED
