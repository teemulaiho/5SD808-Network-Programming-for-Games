// client_app.cc

#include "client_app.hpp"
#include <charlie_messages.hpp>
#include <cstdio>

template <typename T, std::size_t N>
constexpr auto array_size(T(&)[N])
{
    return N;
}

Interpolator::Interpolator()
    : entity_(entity_)
    , interpolation_(1.0/5.0)
    , index_(0)
{

}

Interpolator::Interpolator(gameplay::Entity& entity)
    : entity_(entity)
    , interpolation_(1.0 / 5.0)
    , index_(0)
{    
}

void Interpolator::set_interpolation_time(const Time& time)
{
    interpolation_ = time;
}

void Interpolator::push(const Vector2& position)
{
    accumulator_ = {};
    positions_[index_] = position;  
    index_ = (index_ + 1) % array_size(positions_);
}

void Interpolator::update(const Time& dt) 
{
    accumulator_ += dt;
    const Time delta = (interpolation_ - accumulator_).as_seconds();
    const float t = 1.0f -  (delta.as_seconds() / interpolation_.as_seconds());
    const int32 iA = (index_ - 2) % array_size(positions_);
    const int32 iB = (index_ - 1) % array_size(positions_);
    const Vector2 vA = positions_[iA];
    const Vector2 vB = positions_[iB];
    entity_.position_ = Vector2::lerp(vA, vB, t);
}

ClientApp::ClientApp()
    : mouse_(window_.mouse_)
    , keyboard_(window_.keyboard_)
    , tickrate_(1.0 / 60.0)
    , tick_(0)
    , interp_(1.0 / 2.0)
    , interpolator_(entity_)
    , packagecount_(0)
    , tickoffset_(0)
    , interpolator_0(player0)
    , interpolator_1(player1)
    , interpolator_2(player2)
    , interpolator_3(player3)
    , interpolator_4(player4)
    , interpolator_5(player5)
    , interpolator_6(player6)
{
    Vector2 d(0.0f,0.0f);
    for (int i = 0; i < 16; i++)
    {
        inputs_.add_input(0, 0, d);
    }
}

bool ClientApp::on_init()
{
   if (!network_.initialize({})) {
      return false;
   }

    //2020-11-18 Server Discovery Begin
    //Code credit to Amr M. Saleh
   {
        network::IPAddress address;
        network::ServerDiscovery server_discoverer;

        //DynamicArray<network::IPAddress> address_list;

        address = server_discoverer.search_server();

        connection_.set_listener(this);

        if (!address.host_ == 0)
        {
            connection_.connect(address);
        }

   }
    //2020-11-18 Server Discovery End   

   
   // Original code
   {
       //connection_.set_listener(this);
       //connection_.connect(network::IPAddress(192, 168, 1, 101, 54345));
   }

   return true;
}

void ClientApp::on_exit()
{
}

bool ClientApp::on_tick(const Time &dt)
{
   if (keyboard_.pressed(Keyboard::Key::Escape)) {
      return false;
   }

   accumulator_ += dt;

   while (accumulator_ >= tickrate_) {
       accumulator_ -= tickrate_;
       tick_++;

       input_bits_ = 0;
       if (keyboard_.down(Keyboard::Key::W)) {
           input_bits_ |= (1 << int32(gameplay::Action::Up));
       }
       if (keyboard_.down(Keyboard::Key::S)) {
           input_bits_ |= (1 << int32(gameplay::Action::Down));
       }
       if (keyboard_.down(Keyboard::Key::A)) {
           input_bits_ |= (1 << int32(gameplay::Action::Left));
       }
       if (keyboard_.down(Keyboard::Key::D)) {
           input_bits_ |= (1 << int32(gameplay::Action::Right));
       }

       // ------------------
       // "INPUT PREDICTION" Update player position based on input_bits_. 
       // ------------------
       {
           // note: update player (identical to Sever Game Logic.)
           const bool player_move_up = input_bits_ & (1 << int32(gameplay::Action::Up));
           const bool player_move_down = input_bits_ & (1 << int32(gameplay::Action::Down));
           const bool player_move_left = input_bits_ & (1 << int32(gameplay::Action::Left));
           const bool player_move_right = input_bits_ & (1 << int32(gameplay::Action::Right));

           Vector2 direction;
           if (player_move_up) {
               direction.y_ -= 1.0f;
           }
           if (player_move_down) {
               direction.y_ += 1.0f;
           }
           if (player_move_left) {
               direction.x_ -= 1.0f;
           }
           if (player_move_right) {
               direction.x_ += 1.0f;
           }

           const float speed = 100.0;
           if (direction.length() > 0.0f) {
               direction.normalize();
               player_.position_ += direction * speed * tickrate_.as_seconds();
           }
       }

       // note: entity interpolation goes here
       interpolator_.update(tickrate_);

       interpolator_0.update(tickrate_);
       interpolator_1.update(tickrate_);
       interpolator_2.update(tickrate_);
       interpolator_3.update(tickrate_);
       interpolator_4.update(tickrate_);
       interpolator_5.update(tickrate_);
       interpolator_6.update(tickrate_);

       //for (int i = 0; i < entities_.entities_.size(); i++)
       //{
       //    interpolator_array_[i].update(tickrate_);
       //    //entities_.entities_.at(i).interpolator_.update(tickrate_);

       //    //interpolator_.

       //    //entities_.interpolators_.at(i).update(tickrate_);
       //}

       // add inputs to input_ list

       //inputs_.add_input(input_bits_, tick_ + tickoffset_, player_.position_);

       int size = static_cast<int>(inputs_.inputs_.size());

       // Hard-coded max 16 inputs. See ClientApp::ClientApp(){} for input list initalization.
       if (size < 16)
       {
           inputs_.add_input(input_bits_, tick_ + tickoffset_, player_.position_);
       }
       else 
       {
           inputs_.inputs_.at(tick_ % size).input_bits_ = input_bits_;
           inputs_.inputs_.at(tick_ % size).predicted_pos_ = player_.position_;
           inputs_.inputs_.at(tick_ % size).tick_ = tick_ + tickoffset_;
       }


  }

   return true;
}

void ClientApp::on_draw()
{
   renderer_.render_text({ 2, 2 }, Color::White, 1, "CLIENT");

   renderer_.render_text({ 2, 12 }, Color::White, 1, "RTT:");
   renderer_.render_text_va({ 50, 12 }, Color::Green, 1, "%3.3f", connection_.round_trip_time().as_milliseconds());

   renderer_.render_text({ 2, 24 }, Color::White, 1, "PKC:");
   renderer_.render_text_va({ 50, 24 }, Color::Green, 1, "%d", packagecount_);

   renderer_.render_text({ 2, 36 }, Color::White, 1, "BR :");
   renderer_.render_text_va({ 50, 36 }, Color::Green, 1, "%d", packagecount_ * 4 * 2 + packagecount_ * 1 + packagecount_ * 8 + packagecount_ * 4);
   
   renderer_.render_text({ 2, 48 }, Color::White, 1, "CNT:");
   renderer_.render_text_va({ 50, 48 }, Color::Green, 1, "%d", connection_.state_);


   renderer_.render_rectangle_fill({ int32(entity_.position_.x_), int32(entity_.position_.y_), 20, 20 }, Color::Green);
   renderer_.render_rectangle_fill({ int32(player_.position_.x_), int32(player_.position_.y_), 20, 20 }, Color::Magenta);


   // 2020-11-04 NEW CODE BEGIN 
   // Render other players

   renderer_.render_rectangle_fill({ int32(player0.position_.x_), int32(player0.position_.y_), 20, 20 }, Color::Magenta);
   renderer_.render_rectangle_fill({ int32(player1.position_.x_), int32(player1.position_.y_), 20, 20 }, Color::Magenta);
   renderer_.render_rectangle_fill({ int32(player2.position_.x_), int32(player2.position_.y_), 20, 20 }, Color::Magenta);
   renderer_.render_rectangle_fill({ int32(player3.position_.x_), int32(player3.position_.y_), 20, 20 }, Color::Magenta);
   renderer_.render_rectangle_fill({ int32(player4.position_.x_), int32(player4.position_.y_), 20, 20 }, Color::Magenta);
   renderer_.render_rectangle_fill({ int32(player5.position_.x_), int32(player5.position_.y_), 20, 20 }, Color::Magenta);
   renderer_.render_rectangle_fill({ int32(player6.position_.x_), int32(player6.position_.y_), 20, 20 }, Color::Magenta);

   //for (int i = 0; i < entities_.entities_.size(); i++)
   //{
   //    renderer_.render_rectangle_fill(
   //        {
   //            static_cast<int32>(entities_.entities_.at(i).position_.x_),
   //            static_cast<int32>(entities_.entities_.at(i).position_.y_),
   //            20,
   //            20
   //        },
   //        Color::Magenta);
   //}
   // 2020-11-04 NEW CODE END

}

void ClientApp::on_acknowledge(network::Connection *connection, 
                               const uint16 sequence)
{   
}

bool is_sequence_newer(const uint32 a, const uint32 b) {
    return ((a > b) && ((a - b) <= 0x80000000)) ||
        ((a < b) && ((b - a) > 0x80000000));
}

void ClientApp::on_receive(network::Connection *connection, 
                           network::NetworkStreamReader &reader)
{
    while (reader.position() < reader.length()) {
        switch (reader.peek()) {
            case network::NETWORK_MESSAGE_SERVER_TICK:
            {
                network::NetworkMessageServerTick message;
                if (!message.read(reader)) {
                    assert(!"could not read message!");
                }

                // Get for server tick for input prediction comparison.
                servertick_ = message.server_tick_;

                // Consider how client should get "ahead" of server, in terms of ticks.
                int64 send_rate = network_.send_rate_.as_ticks() / tickrate_.as_ticks();
                int64 rtt_offset = connection_.round_trip_time().as_ticks() / tickrate_.as_ticks(); // /*connection_.latency().as_ticks()*/
                uint32 buffer = 10;

                tickoffset_ = servertick_ - tick_ + buffer + static_cast<uint32>(send_rate) + static_cast<uint32>(rtt_offset);
                // variable dynamic buffer = something;      

                // Add the following to the client tick_.
                // Server tick rate:        1 / 60 = 16.666667ms
                // Send rate:               1 / 20 = 50ms
                // RTT / 2:                 connection_.latency();
                // Dynamic buffer:          See Overwatch -video.

                // tick_ = servertick_ + static_cast<uint32>(send_rate) + /* dif between client & server */ +static_cast<uint32>(rtt_offset) + 3 /* + dynamic buffer*/;

                // TO-DO:
                // Overwatch: Send all the inputs from the last server -acknowledged input.
                   
                const Time current = Time(message.server_time_);
                interp_ = current - previous_;
                previous_ = current;
            } break;

        case network::NETWORK_MESSAGE_ENTITY_STATE:
        {
            network::NetworkMessageEntityState message;
            if (!message.read(reader)) {
                assert(!"could not read message!");
            }

            //auto id = entities_.find_entity(message.id_);
            auto id = message.id_;

            if (id == -1)
            {
                //entities_.add_entity(message.position_);
                interpolator_.set_interpolation_time(interp_);
                interpolator_.push(message.position_);
            }
            else
            {
                if (id == 0)
                {
                    interpolator_0.set_interpolation_time(interp_);
                    interpolator_0.push(message.position_);
                }
                else if (id == 1)
                {
                    interpolator_1.set_interpolation_time(interp_);
                    interpolator_1.push(message.position_);
                }
                else if (id == 2)
                {
                    interpolator_2.set_interpolation_time(interp_);
                    interpolator_2.push(message.position_);
                }
                else if (id == 3)
                {
                    interpolator_3.set_interpolation_time(interp_);
                    interpolator_3.push(message.position_);
                }
                else if (id == 4)
                {
                    interpolator_4.set_interpolation_time(interp_);
                    interpolator_4.push(message.position_);
                }
                else if (id == 5)
                {
                    interpolator_5.set_interpolation_time(interp_);
                    interpolator_5.push(message.position_);
                }
                else if (id == 6)
                {
                    interpolator_6.set_interpolation_time(interp_);
                    interpolator_6.push(message.position_);
                }

                //interpolator_.set_interpolation_time(interp_);
                //interpolator_.push(message.position_);

                //entities_.entities_.at(id).position_ = message.position_;
;           }

            packagecount_++;
            //interpolator_.set_interpolation_time(interp_);
            //interpolator_.push(message.position_);
        } break;

        case network::NETWORK_MESSAGE_PLAYER_STATE:
        {
            network::NetworkMessagePlayerState message;
            if (!message.read(reader)) {
                assert(!"could not read message!");
            }

            // Compare Server Tick and Client Tick.
            auto it = inputs_.inputs_.begin();

            while (it != inputs_.inputs_.end())
            {
                if ((*it).tick_ == servertick_)
                {
                    if (Vector2::distance((*it).predicted_pos_, message.position_) >= 5.0f)
                    {
                        // snap the command predicted position to the server position. 
                        // recalculate positions or you could offset each predicted position with the difference.
                        player_.position_ = message.position_;

                        // 20201030 - Implement prediction correction.
                    }

                    // remove old inputs / predicted positions.
                    inputs_.remove_input(servertick_);

                    break;
                }

                ++it;
            }

        } break;

        default:
        {
            assert(!"unknown message type received from server!");
        } break;
        }
    }
}

void ClientApp::on_send(network::Connection *connection, 
                        const uint16 sequence, 
                        network::NetworkStreamWriter &writer)   
{
    // Send last count_ inputs to server.
    //const uint64 count_ = 1;               // 21 inputs = 3 messages worth of inputs. Remember to set count_ in charlie_messages.hpp  !

    //uint8 bits_[count_] = {};
    //for (int i = 0; i < count_; i++)
    //{
    //    bits_[i] = inputs_.find_input(tick_ - i);
    //}

    //printf("%u: ", inputs_.inputs_.);

    //printf("tick: %u - bits_: %u\n", tick_, bits_[0]);
    //printf("tick: %u - bits_: %u\n", tick_-1, bits_[1]);
    //printf("tick: %u - bits_: %u\n", tick_-2, bits_[2]);
    //printf("tick: %u - bits_: %u\n", tick_-3, bits_[3]);
    //printf("tick: %u - bits_: %u\n", tick_-4, bits_[4]);
    //printf("tick: %u - bits_: %u\n", tick_-5, bits_[5]);

    for(auto& input: inputs_.inputs_)
    {
        network::NetworkMessageInputCommand command(input.input_bits_, input.tick_, 1);

        // Too large input issue
        // Timeout issue

        //network::NetworkMessageInputCommand command(bits_, tick_, count_);
        if (!command.write(writer)) {
            assert(!"could not write network command!");
        }
    }
    //printf("sent input for ticks: %u - %u\n", tick_ - static_cast<uint32>(count_), tick_);
}
