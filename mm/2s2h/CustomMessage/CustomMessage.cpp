
#include "CustomMessage.h"
#include "2s2h/GameInteractor/GameInteractor.h"

extern "C" {
#include "variables.h"
extern f32 sNESFontWidths[160];
}

CustomMessage::Entry activeCustomMessage;

std::string CustomMessage::RemoveColorCodes(const std::string& input) {
    std::string output = input;
    const std::vector<std::string> codes = { "%r", "%w", "%y", "%g", "%b", "%p" };

    for (const auto& code : codes) {
        size_t pos = 0;
        while ((pos = output.find(code, pos)) != std::string::npos) {
            output.erase(pos, code.length());
        }
    }

    return output;
}

void CustomMessage::StartTextbox(std::string msg, CustomMessage::Entry options) {
    options.msg = msg;
    activeCustomMessage = options;

    Message_StartTextbox(gPlayState, CUSTOM_MESSAGE_ID, &GET_PLAYER(gPlayState)->actor);
}

void CustomMessage::SetActiveCustomMessage(std::string msg, CustomMessage::Entry options) {
    options.msg = msg;
    activeCustomMessage = options;
}

void CustomMessage::Replace(std::string* msg, const std::string& placeholder, const std::string& value) {
    size_t pos = 0;
    while ((pos = msg->find(placeholder, pos)) != std::string::npos) {
        msg->replace(pos, placeholder.length(), value);
        pos += value.length();
    }
}

void CustomMessage::ReplaceColorChars(std::string* msg) {
    std::string white = "\x00";
    white += '\x00';
    CustomMessage::Replace(msg, "%r", "\x01");
    CustomMessage::Replace(msg, "%w", white);
    CustomMessage::Replace(msg, "%y", "\x04");
    CustomMessage::Replace(msg, "%g", "\x02");
    CustomMessage::Replace(msg, "%b", "\x03");
    CustomMessage::Replace(msg, "%p", "\x06");
}

void CustomMessage::AddLineBreaks(std::string* msg) {
    const float MAX_TEXTBOX_WIDTH = 300.0f;
    const int MAX_LINES_PER_PAGE = 4;

    float currentLineWidth = 0.0f;
    int currentLineCount = 0;
    size_t lastSpaceIndex = std::string::npos;

    for (size_t i = 0; i < msg->size(); ++i) {
        char currentChar = (*msg)[i];

        if ((uint8_t)currentChar >= 0x20 && (uint8_t)currentChar < 0x20 + ARRAY_COUNTU(sNESFontWidths)) {
            currentLineWidth += sNESFontWidths[(uint8_t)currentChar - 0x20];
        }

        // Increment for existing new liens
        if (currentChar == 0x11) {
            currentLineWidth = 0.0f;
            lastSpaceIndex = std::string::npos;
            ++currentLineCount;
        }

        if (currentChar == ' ') {
            lastSpaceIndex = i;
        }
        if (currentLineWidth > MAX_TEXTBOX_WIDTH) {
            if (lastSpaceIndex != std::string::npos) {
                (*msg)[lastSpaceIndex] = 0x11;
                i = lastSpaceIndex;
            } else {
                msg->insert(i, 1, 0x11);
            }

            currentLineWidth = 0.0f;
            lastSpaceIndex = std::string::npos;
            ++currentLineCount;

            if (currentLineCount >= MAX_LINES_PER_PAGE) {
                // Replace the added new line for a box break instead
                (*msg)[i] = 0x10;
                currentLineCount = 0;
            }
        }
    }
}

// Ensure that the message ends with the message end character
void CustomMessage::EnsureMessageEnd(std::string* msg) {
    if ((unsigned char)msg->back() != 0xBF) {
        msg->push_back(0xBF);
    }
}

CustomMessage::Entry CustomMessage::LoadVanillaMessageTableEntry(u16 textId) {
    MessageContext* msgCtx = &gPlayState->msgCtx;
    MessageTableEntry* msgEntry = msgCtx->messageTableNES;
    while (msgEntry->textId != 0xFFFF) {
        if (msgEntry->textId == textId) {
            break;
        }
        msgEntry++;
    }

    CustomMessage::Entry entry;

    entry.textboxType = msgEntry->segment[0];
    entry.textboxYPos = msgEntry->segment[1];
    entry.icon = msgEntry->segment[2];
    entry.nextMessageID = (msgEntry->segment[3] << 8) | msgEntry->segment[4];
    entry.firstItemCost = (msgEntry->segment[5] << 8) | msgEntry->segment[6];
    entry.secondItemCost = (msgEntry->segment[7] << 8) | msgEntry->segment[8];
    entry.msg = std::string(msgEntry->segment + MESSAGE_HEADER_SIZE, msgEntry->msgSize - MESSAGE_HEADER_SIZE);

    return entry;
}

void CustomMessage::LoadCustomMessageIntoFont(CustomMessage::Entry entry) {
    MessageContext* msgCtx = &gPlayState->msgCtx;
    Font* font = &msgCtx->font;

    char buff[1280] = { 0 };

    // Copy message header
    buff[0] = entry.textboxType;
    buff[1] = entry.textboxYPos;
    buff[2] = entry.icon;
    buff[3] = (entry.nextMessageID & 0xFF00) >> 8;
    buff[4] = (entry.nextMessageID & 0x00FF);
    buff[5] = (entry.firstItemCost & 0xFF00) >> 8;
    buff[6] = (entry.firstItemCost & 0x00FF);
    buff[7] = (entry.secondItemCost & 0xFF00) >> 8;
    buff[8] = (entry.secondItemCost & 0x00FF);
    buff[9] = 0xFF;
    buff[10] = 0xFF;

    if (entry.autoFormat) {
        CustomMessage::ReplaceColorChars(&entry.msg);
        CustomMessage::Replace(&entry.msg, "\n", "\x11");
        CustomMessage::AddLineBreaks(&entry.msg);
        CustomMessage::EnsureMessageEnd(&entry.msg);
    }

    // If message is too long, truncate it and add the message end character
    if (entry.msg.length() > BUFFER_SIZE - MESSAGE_HEADER_SIZE) {
        entry.msg = entry.msg.substr(0, BUFFER_SIZE - MESSAGE_HEADER_SIZE - 1);
        entry.msg += '\xBF';
    }

    memcpy(buff + MESSAGE_HEADER_SIZE, entry.msg.c_str(), entry.msg.length());

    msgCtx->msgLength = entry.msg.length() + MESSAGE_HEADER_SIZE;
    memcpy(&font->msgBuf, buff, msgCtx->msgLength);
}

void CustomMessage::RegisterHooks() {
    GameInteractor::Instance->RegisterGameHookForID<GameInteractor::OnOpenText>(
        CUSTOM_MESSAGE_ID, [](u16* textId, bool* loadFromMessageTable) {
            *loadFromMessageTable = false;
            CustomMessage::LoadCustomMessageIntoFont(activeCustomMessage);
        });
}
