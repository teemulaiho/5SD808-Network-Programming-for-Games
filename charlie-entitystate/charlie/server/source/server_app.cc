// server_app.cc

#include "server_app.hpp"
#include <charlie_messages.hpp>
#include <cstdio>
#include <cmath>

ServerApp::ServerApp()
   :  tickrate_(1.0 / 60.0)
    , tick_(0)
    , networkservertickpackagecount_(0)
    , networkserverpositionpackagecount_(0)
{
}

bool ServerApp::on_init()
{
   network_.set_send_rate(Time(1.0 / 2.0));
   network_.set_allow_connections(true);
   if (!network_.initialize(network::IPAddress(network::IPAddress::ANY_HOST, 54345))) {
      return false;
   }

   network_.add_service_listener(this);

   entity_.position_ = { 300.0f, 200.0f };

   return true;
}

void ServerApp::on_exit()
{
}

bool ServerApp::on_tick(const Time &dt)
{
   accumulator_ += dt;
   //if (accumulator_ >= Time(1.0 / 60.0)) {
   //   accumulator_ -= Time(1.0 / 60.0);

      while (accumulator_ >= tickrate_) {
          accumulator_ -= tickrate_;
          tick_++;

          // 2020-11-03 NEW CODE BEGIN
          // Update each player from players_ list of players.
          for (int i = 0; i < players_.players_.size(); i++)
          {
              player_input_bits_ = players_.players_.at(i).inputs_.find_input(tick_);
              
              // note: update player (Identical to Client Game Logic.)
              const bool player_move_up = player_input_bits_ & (1 << int32(gameplay::Action::Up));
              const bool player_move_down = player_input_bits_ & (1 << int32(gameplay::Action::Down));
              const bool player_move_left = player_input_bits_ & (1 << int32(gameplay::Action::Left));
              const bool player_move_right = player_input_bits_ & (1 << int32(gameplay::Action::Right));

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
                  players_.players_.at(i).position_ += direction * speed * tickrate_.as_seconds();
              }

              //printf("tick: %u - bits_: %u\n", tick_, player_input_bits_);
              //printf("tick: %u - bits_: %u - pos_x: %.1f - pos_y: %.1f \n", tick_, player_input_bits_, player_.position_.x_, player_.position_.y_);
              players_.players_.at(i).inputs_.remove_input(tick_);
          }
          // 2020-11-03 NEW CODE END

          //player_input_bits_ = players_.players_.at(0).inputs_.find_input(tick_);

          //// note: update player (Identical to Client Game Logic.)
          //const bool player_move_up = player_input_bits_ & (1 << int32(gameplay::Action::Up));
          //const bool player_move_down = player_input_bits_ & (1 << int32(gameplay::Action::Down));
          //const bool player_move_left = player_input_bits_ & (1 << int32(gameplay::Action::Left));
          //const bool player_move_right = player_input_bits_ & (1 << int32(gameplay::Action::Right));

          //Vector2 direction;
          //if (player_move_up) {
          //    direction.y_ -= 1.0f;
          //}
          //if (player_move_down) {
          //    direction.y_ += 1.0f;
          //}
          //if (player_move_left) {
          //    direction.x_ -= 1.0f;
          //}
          //if (player_move_right) {
          //    direction.x_ += 1.0f;
          //}

          //const float speed = 100.0;
          //if (direction.length() > 0.0f) {
          //    direction.normalize();
          //  player_.position_ += direction * speed * tickrate_.as_seconds();
          //}
          //         
          ////printf("tick: %u - bits_: %u\n", tick_, player_input_bits_);
          ////printf("tick: %u - bits_: %u - pos_x: %.1f - pos_y: %.1f \n", tick_, player_input_bits_, player_.position_.x_, player_.position_.y_);
          //players_.players_.at(0).inputs_.remove_input(tick_);

          entity_.position_.x_ = 300.0f + std::cosf(Time::now().as_seconds()) * 150.0f;
      }
   //}

   return true;
}

void ServerApp::on_draw()
{
   renderer_.render_text({ 2, 2 }, Color::Aqua, 1, "SERVER");

   renderer_.render_text({ 2, 12 }, Color::Aqua, 1, "NMS:");
   renderer_.render_text_va({ 50, 12 }, Color::Green, 1, "%d", networkserverpositionpackagecount_);


   renderer_.render_text({ 2, 24 }, Color::Aqua, 1, "NME:");
   renderer_.render_text_va({ 50, 24 }, Color::Green, 1, "%d", networkserverpositionpackagecount_);

   renderer_.render_rectangle_fill(
      { 
         static_cast<int32>(entity_.position_.x_), 
         static_cast<int32>(entity_.position_.y_), 
         20, 
         20 
      }, 
      Color::Red);

   renderer_.render_rectangle_fill(
       { 
           static_cast<int32>(player_.position_.x_), 
           static_cast<int32>(player_.position_.y_),  
           20, 
           20 
       }, 
       Color::Magenta);

   // 2020-11-03 NEW CODE BEGIN
   // Reder players_ list.

   for (int i = 0; i < players_.players_.size(); i++)
   {
       renderer_.render_rectangle_fill(
           {
               static_cast<int32>(players_.players_.at(i).position_.x_),
               static_cast<int32>(players_.players_.at(i).position_.y_),
               20,
               20
           },
           Color::Magenta);
   }
   // 2020-11-03 NEW CODE END
}

// "Observer" -pattern
void ServerApp::on_timeout(network::Connection *connection)
{
   connection->set_listener(nullptr);
}

void ServerApp::on_connect(network::Connection *connection)
{
   connection->set_listener(this);
}

void ServerApp::on_disconnect(network::Connection *connection)
{
   connection->set_listener(nullptr);
}

void ServerApp::on_acknowledge(network::Connection *connection, 
                               const uint16 sequence)
{
}

void ServerApp::on_receive(network::Connection* connection,
    network::NetworkStreamReader& reader)
{
    if (clients_.find_client((uint64)connection) == -1)
    {
        auto id = clients_.add_client((uint64)connection);
        players_.add_player(id);
    }

    auto id = clients_.find_client((uint64)connection);

    while (reader.position() < reader.length()) {

        if (reader.peek() != network::NETWORK_MESSAGE_INPUT_COMMAND) {
            break;
        }
        
        network::NetworkMessageInputCommand command;
        if (!command.read(reader)) {
            assert(!"could not read command!");
        }

        players_.players_.at(id).inputs_.add_input(command.bits_, command.tick_);
    }
}

void ServerApp::on_send(network::Connection *connection, 
                        const uint16 sequence, 
                        network::NetworkStreamWriter &writer)
{
   {
      network::NetworkMessageServerTick message(Time::now().as_ticks(),
                                                tick_);
      if (!message.write(writer)) {
         assert(!"failed to write message!");
      }

      networkservertickpackagecount_++;
   }

   auto id = clients_.find_client((uint64)connection);
   
   if (id != -1)
   {
       {
           network::NetworkMessagePlayerState message(players_.players_.at(id).position_);
           if (!message.write(writer)) {
               assert(!"failed to write message!");
           }
       }
   }
   //players_[id]. ..;

   //{
   //    network::NetworkMessagePlayerState message(player_.position_);
   //    if (!message.write(writer)) {
   //        assert(!"failed to write message!");
   //    }
   //}

   for (auto& entity : players_.players_)
   {
       if (entity.id_ != id)
       {
           network::NetworkMessageEntityState message(entity.position_, entity.id_);
           if (!message.write(writer)) {
               assert(!"failed to write message!");
           }
       }
   }   
     
   {
      network::NetworkMessageEntityState message(entity_.position_);
      if (!message.write(writer)) {
         assert(!"failed to write message!");
      }

      networkserverpositionpackagecount_++;
   }
}
