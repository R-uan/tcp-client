#include <iostream>
#include <sstream>
#include <bits/ostream.tcc>

#include "synapse_net.h"
#include "utils/bytes.hpp"
#include "utils/checksum.h"
#include "utils/logger.hpp"
#include "network/protocol.h"

void Protocol::handle_packet(const std::vector<uint8_t> &bytes) {
    std::stringstream ss;
    const auto packet = Packet::parse_packet(bytes);
    if (!packet.has_value() || !check_the_sum(packet.value())) handle_invalid();

    switch (packet.value().header.message_type) {
        case MessageType::GAMESTATE:
            handle_game_state(packet.value());
            break;
        default:
            ss << "Received INVALID packet of " << packet->payload.size() << " bytes" << std::endl;
            std::string message2 = ss.str();
            Logger::info(message2);
            handle_invalid();
            break;
    }

}

void Protocol::handle_invalid() {
    std::string message = "Invalid packet";
    const auto payload = std::vector<uint8_t>(message.begin(), message.end());
    const Packet packet = Packet::create_packet(MessageType::ERROR, payload);
    int _ = tcp_socket_ptr->send_packet(packet);
}

void Protocol::handle_game_state(const Packet &packet) {
    std::stringstream ss;
    ss << "Received GAME STATE packet of " << packet.payload.size() << " bytes" << std::endl;
    std::string message = ss.str();
    Logger::info(message);

    std::unique_lock<std::mutex> lock(tcp_socket_ptr->game_state_mutex);
    tcp_socket_ptr->game_state_queue.push(packet);
}

std::optional<MessageType> Header::try_from(const uint8_t &value) {
    switch (value) {
        case 0x00:
            return MessageType::DISCONNECT;
        case 0x01:
            return MessageType::CONNECT;
        case 0x10:
            return MessageType::GAMESTATE;
        case 0xFB:
            return MessageType::ALREADY_CONNECTED;
        case 0xFC:
            return MessageType::INVALID_PLAYER_DATA;
        case 0xFD:
            return MessageType::INVALID_CHECKSUM;
        case 0xFE:
            return MessageType::INVALID_HEADER;
        case 0xFF:
            return MessageType::ERROR;
        default:
            return std::nullopt;
    }
}

std::vector<uint8_t> Header::serialize_header() const {
    std::vector<uint8_t> header_data;

    header_data.push_back(static_cast<uint8_t>(this->message_type));
    header_data.push_back(static_cast<uint8_t>((this->length >> 8) & 0xFF));
    header_data.push_back(static_cast<uint8_t>(this->length & 0xFF));
    header_data.push_back(static_cast<uint8_t>((this->checksum >> 8) & 0xFF));
    header_data.push_back(static_cast<uint8_t>(this->checksum & 0xFF));
    header_data.push_back(0x0A);

    return header_data;
}

std::optional<Header> Header::parse_header(const std::vector<uint8_t> &bytes) {
    if (bytes.size() < 5)
        return std::nullopt;

    std::optional<MessageType> header_type = Header::try_from(bytes[0]);

    if (!header_type.has_value()) {
        std::cout << "Invalid header" << std::endl;
        throw std::errc::protocol_error;
    }

    const uint16_t checksum = u8::to_u16(bytes[3], bytes[4]);
    const uint16_t message_length = u8::to_u16(bytes[1], bytes[2]);

    return Header{
            header_type.value(),
            message_length,
            checksum,
    };
}

Header Header::create_header(const MessageType type, const std::vector<uint8_t> &payload) {
    const uint16_t checksum = static_cast<uint16_t>(xor_checksum(payload));
    const uint16_t payload_length = static_cast<uint16_t>(payload.size());
    return Header{
            type,
            payload_length,
            checksum,
    };
}

std::optional<Packet> Packet::parse_packet(const std::vector<uint8_t> &bytes) {
    const auto header = Header::parse_header(std::vector<uint8_t>(bytes.begin(), bytes.begin() + 5));
    if (!header.has_value()) return std::nullopt;
    return Packet{header.value(), std::vector<uint8_t>(bytes.begin() + 6, bytes.end())};
}

Packet Packet::create_packet(const MessageType &type, const std::vector<uint8_t> &payload) {
    return Packet{Header::create_header(type, payload), payload};
}
