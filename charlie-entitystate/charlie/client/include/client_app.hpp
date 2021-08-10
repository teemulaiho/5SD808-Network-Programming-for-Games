// client_app.hpp

#ifndef CLIENT_APP_HPP_INCLUDED
#define CLIENT_APP_HPP_INCLUDED

#include <charlie_application.hpp>

using namespace charlie;

struct EntityList {
    EntityList()
        : next_(0)
    {
    }

    int32 add_entity(Vector2 position)
    {
        const int32 id = next_++;
        entities_.push_back({ id, position });

        return id;
    }

    int32 find_entity(const int32 id)
    {
        for (auto& entity : entities_) {
            if (entity.id_ == id)
            {
                return entity.id_;
            }
        }
        return -1;
    }

    void remove_entity(const int32 id)
    {
        auto it = entities_.begin();
        while (it != entities_.end()) {
            if ((*it).id_ == id) {
                entities_.erase(it);
                break;
            }
            ++it;
        }
    }

    struct Entity {
        int32 id_{ -1 };
        Vector2 position_;
    };

    int32 next_;
    DynamicArray<Entity> entities_;
};

struct InputList{
    InputList()
        : next_(0)
    {
    }

    int32 add_input(uint8 input_bits, uint32 tick, Vector2 position)
    {
        const int32 id = next_++;
        inputs_.push_back({ id, input_bits, tick, position });
        return id;
    }

    uint8 find_input(uint32 tick)
    {
        for (auto& command : inputs_) {
            if (command.tick_ == tick) {
                return command.input_bits_;
            }
        }
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
    DynamicArray<Input> inputs_;
};

struct Interpolator {
    Interpolator();
    Interpolator(gameplay::Entity& entity);

    void set_interpolation_time(const Time& time);
    void push(const Vector2& position);
    void update(const Time& dt);

    gameplay::Entity& entity_;
    Time interpolation_;
    Time accumulator_;
    int32 index_;
    Vector2 positions_[64];
};

struct ClientApp final : Application, network::IConnectionListener {
   ClientApp();

   // note: Application
   virtual bool on_init();
   virtual void on_exit();
   virtual bool on_tick(const Time &dt);
   virtual void on_draw();

   // note: IConnectionListener 
   virtual void on_acknowledge(network::Connection *connection, const uint16 sequence);
   virtual void on_receive(network::Connection *connection, network::NetworkStreamReader &reader);
   virtual void on_send(network::Connection *connection, const uint16 sequence, network::NetworkStreamWriter &writer);

   Mouse &mouse_;
   Keyboard &keyboard_;
   network::Connection connection_;
   const Time tickrate_;
   Time accumulator_;


   // 2020-10-20 New code begin.
   uint8 input_bits_;
   gameplay::Player player_;
   gameplay::Entity entity_;

   uint32 tick_;
   InputList inputs_;

   uint32 servertick_;

   uint32 tickoffset_;
   // 2020-10-20 New code end.

   // 2020-11-04 New code begin.
   EntityList entities_;

   Interpolator interpolator_0;
   Interpolator interpolator_1;
   Interpolator interpolator_2;
   Interpolator interpolator_3;
   Interpolator interpolator_4;
   Interpolator interpolator_5;
   Interpolator interpolator_6;

   gameplay::Entity player0;
   gameplay::Entity player1;
   gameplay::Entity player2;
   gameplay::Entity player3;
   gameplay::Entity player4;
   gameplay::Entity player5;
   gameplay::Entity player6;

   // 2020-11-04 New code end.

   //gameplay::Entity entity_;
   Interpolator interpolator_;
   Time previous_;
   Time interp_;

   int packagecount_;

   struct EntityState {
      Time previous_time;
      Time dt_;
      uint32 tick_;
      Vector2 position_;
      uint32 accum_;
   };
   DynamicArray<EntityState> buffer_;
   
   EntityState state_;
};

#endif // !CLIENT_APP_HPP_INCLUDED
