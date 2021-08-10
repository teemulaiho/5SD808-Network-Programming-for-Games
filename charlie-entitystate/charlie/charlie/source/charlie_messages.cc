// charlie_messages.cc

#include "charlie_messages.hpp"
#include "charlie_network.hpp"

namespace charlie {
   namespace network {
      NetworkMessageServerTick::NetworkMessageServerTick()
         : type_(NETWORK_MESSAGE_SERVER_TICK)
         , server_time_(0)
         , server_tick_(0)
      {
      }

      NetworkMessageServerTick::NetworkMessageServerTick(const int64  server_time,
                                                         const uint32 server_tick)
         : type_(NETWORK_MESSAGE_SERVER_TICK)
         , server_time_(server_time)
         , server_tick_(server_tick)
      {
      }

      bool NetworkMessageServerTick::read(NetworkStreamReader &reader)
      {
         return serialize(reader);
      }

      bool NetworkMessageServerTick::write(NetworkStreamWriter &writer)
      {
         return serialize(writer);
      }

      NetworkMessageEntityState::NetworkMessageEntityState()
          : type_(NETWORK_MESSAGE_ENTITY_STATE)
          , id_{ -1 }
      {
      }

      NetworkMessageEntityState::NetworkMessageEntityState(const Vector2& position)
          : type_(NETWORK_MESSAGE_ENTITY_STATE)
          , position_(position)
          , id_{ -1 }
      {
      }

      NetworkMessageEntityState::NetworkMessageEntityState(const Vector2& position, int32 id)
          : type_(NETWORK_MESSAGE_ENTITY_STATE)
          , position_(position)
          , id_(id)
      {
      }

      bool NetworkMessageEntityState::read(NetworkStreamReader &reader)
      {
         return serialize(reader);
      }

      bool NetworkMessageEntityState::write(NetworkStreamWriter &writer)
      {
         return serialize(writer);
      }

      NetworkMessageInputCommand::NetworkMessageInputCommand()
          : type_(NETWORK_MESSAGE_INPUT_COMMAND)
          , bits_(0)
          , tick_(0)
          , count_(0)
          //, inputs_(nullptr)
      {
      }

      NetworkMessageInputCommand::NetworkMessageInputCommand(uint8 bits)
          : type_(NETWORK_MESSAGE_INPUT_COMMAND)
          , bits_(bits)
          , tick_(0)
          , count_(0)
          //, inputs_(nullptr)
      {
      }

      NetworkMessageInputCommand::NetworkMessageInputCommand(uint8 bits, uint32 tick)
          : type_(NETWORK_MESSAGE_INPUT_COMMAND)
          , bits_(bits)
          , tick_(tick)
          , count_(0)
          //, inputs_(nullptr)
      {
      }

      NetworkMessageInputCommand::NetworkMessageInputCommand(uint8 bits, uint32 tick, uint64 count)
          : type_(NETWORK_MESSAGE_INPUT_COMMAND)
          , bits_(bits)
          , tick_(tick)
          , count_(count)
          //, inputs_(nullptr)
      {

      }

      NetworkMessageInputCommand::NetworkMessageInputCommand(uint8* bits, uint32 tick, uint64 count)
          : type_(NETWORK_MESSAGE_INPUT_COMMAND)
          , bits_(0)
          , tick_(tick)
          , count_(count)
          //, inputs_(nullptr)
      {
          for (int i = 0; i < count; i++)
          {
              inputs_[i] = bits[i];
          }
      }

      NetworkMessageInputCommand::NetworkMessageInputCommand(const uint32 tick, const uint64 count, uint8* inputs)
          : type_(NETWORK_MESSAGE_INPUT_COMMAND)
          , bits_(0)
          , tick_(tick)
          , count_(count)
          //, inputs_(nullptr)
      {
          for (int i = 0; i < count; i++)
          {
              inputs_[i] = inputs[i];
          }
      }

      bool NetworkMessageInputCommand::read(NetworkStreamReader& reader)
      {
          return serialize(reader);
      }

      bool NetworkMessageInputCommand::write(NetworkStreamWriter& writer)
      {
          return serialize(writer);
      }

      NetworkMessagePlayerState::NetworkMessagePlayerState()
          : type_(NETWORK_MESSAGE_PLAYER_STATE)
      {
      }

      NetworkMessagePlayerState::NetworkMessagePlayerState(const Vector2& position)
          : type_(NETWORK_MESSAGE_PLAYER_STATE)
          , position_(position)
      {
      }

      bool NetworkMessagePlayerState::read(NetworkStreamReader& reader)
      {
          return serialize(reader);
      }

      bool NetworkMessagePlayerState::write(NetworkStreamWriter& writer)
      {
          return serialize(writer);
      }
   } // !network
} // !messages
