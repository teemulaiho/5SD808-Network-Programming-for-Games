// charlie_messages.hpp

#ifndef CHARLIE_MESSAGES_HPP_INCLUDED
#define CHARLIE_MESSAGES_HPP_INCLUDED

#include <charlie.hpp>

namespace charlie {
   namespace network {
      struct NetworkStreamReader;
      struct NetworkStreamWriter;

      enum NetworkMessageType {
         NETWORK_MESSAGE_SERVER_TICK,
         NETWORK_MESSAGE_ENTITY_STATE,
         NETWORK_MESSAGE_INPUT_COMMAND,
         NETWORK_MESSAGE_PLAYER_STATE,
         NETWORK_MESSAGE_COUNT,
      };

      static_assert(NETWORK_MESSAGE_COUNT <= 255, "network message type cannot exceed 255!");

      struct NetworkMessageServerTick {
         NetworkMessageServerTick();
         explicit NetworkMessageServerTick(const int64  server_time,
                                           const uint32 server_tick);

         bool read(NetworkStreamReader &reader);
         bool write(NetworkStreamWriter &writer);

         template <typename Stream>
         bool serialize(Stream &stream)
         {
            bool result = true;
            result &= stream.serialize(type_);
            result &= stream.serialize(server_time_);
            result &= stream.serialize(server_tick_);
            return result;
         }

         uint8 type_;
         int64 server_time_;
         uint32 server_tick_;
      };

      struct NetworkMessageEntityState {
         NetworkMessageEntityState();
         explicit NetworkMessageEntityState(const Vector2 &position);
         explicit NetworkMessageEntityState(const Vector2& position, const int32 id);

         bool read(NetworkStreamReader &reader);
         bool write(NetworkStreamWriter &writer);

         template <typename Stream>
         bool serialize(Stream &stream)
         {
            bool result = true;
            result &= stream.serialize(type_);
            result &= stream.serialize(position_.x_);
            result &= stream.serialize(position_.y_);
            result &= stream.serialize(id_);
            return result;
         }

         uint8 type_;
         Vector2 position_;
         int32 id_;
      };

      struct NetworkMessageInputCommand {
          NetworkMessageInputCommand();
          explicit NetworkMessageInputCommand(uint8 bits);
          explicit NetworkMessageInputCommand(uint8 bits, uint32 tick);
          explicit NetworkMessageInputCommand(uint8 bits, uint32 tick, uint64 count);
          explicit NetworkMessageInputCommand(uint8* bits, uint32 tick, uint64 count);
          explicit NetworkMessageInputCommand(const uint32 tick, const uint64 count, uint8* inputs);
          
          bool read(NetworkStreamReader& reader);
          bool write(NetworkStreamWriter& writer);

          template <typename Stream>
          bool serialize(Stream& stream)
          {
              bool result = true;
              result &= stream.serialize(type_);
              result &= stream.serialize(bits_);
              result &= stream.serialize(tick_);


              //result &= stream.serialize(count_);
              //result &= stream.serialize(count_, inputs_);
              
              return result;
          }

          uint8 type_;
          uint8 bits_;

          // write last 3 commands.
          uint32 tick_;
          uint64 count_;        // uint8 could be enough.
          uint8 inputs_[21] = {};
      };

      struct NetworkMessagePlayerState {
          NetworkMessagePlayerState();
          explicit NetworkMessagePlayerState(const Vector2& position);

          bool read(NetworkStreamReader& reader);
          bool write(NetworkStreamWriter& writer);

          template <typename Stream>
          bool serialize(Stream& stream)
          {
              bool result = true;
              result &= stream.serialize(type_);
              result &= stream.serialize(position_.x_);
              result &= stream.serialize(position_.y_);
              return result;
          }

          uint8 type_;
          Vector2 position_;
      };

   } // !network
} // !charlie

#endif // !CHARLIE_MESSAGES_HPP_INCLUDED
