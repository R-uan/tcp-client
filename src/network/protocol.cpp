#include "network/protocol.h"

#include <iostream>

#include "synapse_net.h"

#include <bits/ostream.tcc>

#include "utils/bytes.hpp"
#include "utils/checksum.h"

void Protocol::handle_packet(std::vector<uint8_t> bytes) {
    auto packet = Packet::parse_packet(bytes);
    if (!packet.has_value()) {
        std::shared_lock lock(socket_mutex);

        Packet packet = Packet::create_packet(MessageType::ERROR,)
    }


    std::cout << "Pretend I handled the packet." << std::endl;
}

void Protocol::handle_invalid() {
    std::shared_lock lock(socket_mutex);

    socket_ptr->send_packet();
}

void Protocol::handle_game_state(std::vector<uint8_t> &payload) {}

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
    if (!header.has_value())
        return std::nullopt;
    return Packet{header.value(), std::vector<uint8_t>(bytes.begin() + 6, bytes.end())};
}

Packet Packet::create_packet(const MessageType &type, const std::vector<uint8_t> &payload) {
    return Packet{Header::create_header(type, payload), payload};
}
