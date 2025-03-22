#include "2s2h/resource/importer/CutsceneFactory.h"
#include "2s2h/resource/type/Cutscene.h"
#include "spdlog/spdlog.h"
// TODO headers
// extern "C" {
//#include "z64cutscene.h"
//}
typedef enum {
    /* 0x00A */ CS_CMD_TEXT = 10,
    /* 0x05A */ CS_CMD_CAMERA_SPLINE = 90,
    /* 0x064 */ CS_CMD_ACTOR_CUE_100 = 100,
    /* 0x065 */ CS_CMD_ACTOR_CUE_101,
    /* 0x066 */ CS_CMD_ACTOR_CUE_102,
    /* 0x067 */ CS_CMD_ACTOR_CUE_103,
    /* 0x068 */ CS_CMD_ACTOR_CUE_104,
    /* 0x069 */ CS_CMD_ACTOR_CUE_105,
    /* 0x06A */ CS_CMD_ACTOR_CUE_106,
    /* 0x06B */ CS_CMD_ACTOR_CUE_107,
    /* 0x06C */ CS_CMD_ACTOR_CUE_108,
    /* 0x06D */ CS_CMD_ACTOR_CUE_109,
    /* 0x06E */ CS_CMD_ACTOR_CUE_110,
    /* 0x06F */ CS_CMD_ACTOR_CUE_111,
    /* 0x070 */ CS_CMD_ACTOR_CUE_112,
    /* 0x071 */ CS_CMD_ACTOR_CUE_113,
    /* 0x072 */ CS_CMD_ACTOR_CUE_114,
    /* 0x073 */ CS_CMD_ACTOR_CUE_115,
    /* 0x074 */ CS_CMD_ACTOR_CUE_116,
    /* 0x075 */ CS_CMD_ACTOR_CUE_117,
    /* 0x076 */ CS_CMD_ACTOR_CUE_118,
    /* 0x077 */ CS_CMD_ACTOR_CUE_119,
    /* 0x078 */ CS_CMD_ACTOR_CUE_120,
    /* 0x079 */ CS_CMD_ACTOR_CUE_121,
    /* 0x07A */ CS_CMD_ACTOR_CUE_122,
    /* 0x07B */ CS_CMD_ACTOR_CUE_123,
    /* 0x07C */ CS_CMD_ACTOR_CUE_124,
    /* 0x07D */ CS_CMD_ACTOR_CUE_125,
    /* 0x07E */ CS_CMD_ACTOR_CUE_126,
    /* 0x07F */ CS_CMD_ACTOR_CUE_127,
    /* 0x080 */ CS_CMD_ACTOR_CUE_128,
    /* 0x081 */ CS_CMD_ACTOR_CUE_129,
    /* 0x082 */ CS_CMD_ACTOR_CUE_130,
    /* 0x083 */ CS_CMD_ACTOR_CUE_131,
    /* 0x084 */ CS_CMD_ACTOR_CUE_132,
    /* 0x085 */ CS_CMD_ACTOR_CUE_133,
    /* 0x086 */ CS_CMD_ACTOR_CUE_134,
    /* 0x087 */ CS_CMD_ACTOR_CUE_135,
    /* 0x088 */ CS_CMD_ACTOR_CUE_136,
    /* 0x089 */ CS_CMD_ACTOR_CUE_137,
    /* 0x08A */ CS_CMD_ACTOR_CUE_138,
    /* 0x08B */ CS_CMD_ACTOR_CUE_139,
    /* 0x08C */ CS_CMD_ACTOR_CUE_140,
    /* 0x08D */ CS_CMD_ACTOR_CUE_141,
    /* 0x08E */ CS_CMD_ACTOR_CUE_142,
    /* 0x08F */ CS_CMD_ACTOR_CUE_143,
    /* 0x090 */ CS_CMD_ACTOR_CUE_144,
    /* 0x091 */ CS_CMD_ACTOR_CUE_145,
    /* 0x092 */ CS_CMD_ACTOR_CUE_146,
    /* 0x093 */ CS_CMD_ACTOR_CUE_147,
    /* 0x094 */ CS_CMD_ACTOR_CUE_148,
    /* 0x095 */ CS_CMD_ACTOR_CUE_149,
    /* 0x096 */ CS_CMD_MISC,
    /* 0x097 */ CS_CMD_LIGHT_SETTING,
    /* 0x098 */ CS_CMD_TRANSITION,
    /* 0x099 */ CS_CMD_MOTION_BLUR,
    /* 0x09A */ CS_CMD_GIVE_TATL,
    /* 0x09B */ CS_CMD_TRANSITION_GENERAL,
    /* 0x09C */ CS_CMD_FADE_OUT_SEQ,
    /* 0x09D */ CS_CMD_TIME,
    /* 0x0C8 */ CS_CMD_PLAYER_CUE = 200,
    /* 0x0C9 */ CS_CMD_ACTOR_CUE_201,
    /* 0x0FA */ CS_CMD_UNK_DATA_FA = 0xFA,
    /* 0x0FE */ CS_CMD_UNK_DATA_FE = 0xFE,
    /* 0x0FF */ CS_CMD_UNK_DATA_FF,
    /* 0x100 */ CS_CMD_UNK_DATA_100,
    /* 0x101 */ CS_CMD_UNK_DATA_101,
    /* 0x102 */ CS_CMD_UNK_DATA_102,
    /* 0x103 */ CS_CMD_UNK_DATA_103,
    /* 0x104 */ CS_CMD_UNK_DATA_104,
    /* 0x105 */ CS_CMD_UNK_DATA_105,
    /* 0x108 */ CS_CMD_UNK_DATA_108 = 0x108,
    /* 0x109 */ CS_CMD_UNK_DATA_109,
    /* 0x12C */ CS_CMD_START_SEQ = 300,
    /* 0x12D */ CS_CMD_STOP_SEQ,
    /* 0x12E */ CS_CMD_START_AMBIENCE,
    /* 0x12F */ CS_CMD_FADE_OUT_AMBIENCE,
    /* 0x130 */ CS_CMD_SFX_REVERB_INDEX_2,
    /* 0x131 */ CS_CMD_SFX_REVERB_INDEX_1,
    /* 0x132 */ CS_CMD_MODIFY_SEQ,
    /* 0x15E */ CS_CMD_DESTINATION = 350,
    /* 0x15F */ CS_CMD_CHOOSE_CREDITS_SCENES,
    /* 0x190 */ CS_CMD_RUMBLE = 400,
    /* 0x1C2 */ CS_CMD_ACTOR_CUE_450 = 450,
    /* 0x1C3 */ CS_CMD_ACTOR_CUE_451,
    /* 0x1C4 */ CS_CMD_ACTOR_CUE_452,
    /* 0x1C5 */ CS_CMD_ACTOR_CUE_453,
    /* 0x1C6 */ CS_CMD_ACTOR_CUE_454,
    /* 0x1C7 */ CS_CMD_ACTOR_CUE_455,
    /* 0x1C8 */ CS_CMD_ACTOR_CUE_456,
    /* 0x1C9 */ CS_CMD_ACTOR_CUE_457,
    /* 0x1CA */ CS_CMD_ACTOR_CUE_458,
    /* 0x1CB */ CS_CMD_ACTOR_CUE_459,
    /* 0x1CC */ CS_CMD_ACTOR_CUE_460,
    /* 0x1CD */ CS_CMD_ACTOR_CUE_461,
    /* 0x1CE */ CS_CMD_ACTOR_CUE_462,
    /* 0x1CF */ CS_CMD_ACTOR_CUE_463,
    /* 0x1D0 */ CS_CMD_ACTOR_CUE_464,
    /* 0x1D1 */ CS_CMD_ACTOR_CUE_465,
    /* 0x1D2 */ CS_CMD_ACTOR_CUE_466,
    /* 0x1D3 */ CS_CMD_ACTOR_CUE_467,
    /* 0x1D4 */ CS_CMD_ACTOR_CUE_468,
    /* 0x1D5 */ CS_CMD_ACTOR_CUE_469,
    /* 0x1D6 */ CS_CMD_ACTOR_CUE_470,
    /* 0x1D7 */ CS_CMD_ACTOR_CUE_471,
    /* 0x1D8 */ CS_CMD_ACTOR_CUE_472,
    /* 0x1D9 */ CS_CMD_ACTOR_CUE_473,
    /* 0x1DA */ CS_CMD_ACTOR_CUE_474,
    /* 0x1DB */ CS_CMD_ACTOR_CUE_475,
    /* 0x1DC */ CS_CMD_ACTOR_CUE_476,
    /* 0x1DD */ CS_CMD_ACTOR_CUE_477,
    /* 0x1DE */ CS_CMD_ACTOR_CUE_478,
    /* 0x1DF */ CS_CMD_ACTOR_CUE_479,
    /* 0x1E0 */ CS_CMD_ACTOR_CUE_480,
    /* 0x1E1 */ CS_CMD_ACTOR_CUE_481,
    /* 0x1E2 */ CS_CMD_ACTOR_CUE_482,
    /* 0x1E3 */ CS_CMD_ACTOR_CUE_483,
    /* 0x1E4 */ CS_CMD_ACTOR_CUE_484,
    /* 0x1E5 */ CS_CMD_ACTOR_CUE_485,
    /* 0x1E6 */ CS_CMD_ACTOR_CUE_486,
    /* 0x1E7 */ CS_CMD_ACTOR_CUE_487,
    /* 0x1E8 */ CS_CMD_ACTOR_CUE_488,
    /* 0x1E9 */ CS_CMD_ACTOR_CUE_489,
    /* 0x1EA */ CS_CMD_ACTOR_CUE_490,
    /* 0x1EB */ CS_CMD_ACTOR_CUE_491,
    /* 0x1EC */ CS_CMD_ACTOR_CUE_492,
    /* 0x1ED */ CS_CMD_ACTOR_CUE_493,
    /* 0x1EE */ CS_CMD_ACTOR_CUE_494,
    /* 0x1EF */ CS_CMD_ACTOR_CUE_495,
    /* 0x1F0 */ CS_CMD_ACTOR_CUE_496,
    /* 0x1F1 */ CS_CMD_ACTOR_CUE_497,
    /* 0x1F2 */ CS_CMD_ACTOR_CUE_498,
    /* 0x1F3 */ CS_CMD_ACTOR_CUE_499,
    /* 0x1F4 */ CS_CMD_ACTOR_CUE_500,
    /* 0x1F5 */ CS_CMD_ACTOR_CUE_501,
    /* 0x1F6 */ CS_CMD_ACTOR_CUE_502,
    /* 0x1F7 */ CS_CMD_ACTOR_CUE_503,
    /* 0x1F8 */ CS_CMD_ACTOR_CUE_504,
    /* 0x1F9 */ CS_CMD_ACTOR_CUE_SOTCS, // Song of Time Cutscenes (Double SoT, Three-Day Reset SoT)
    /* 0x1FA */ CS_CMD_ACTOR_CUE_506,
    /* 0x1FB */ CS_CMD_ACTOR_CUE_507,
    /* 0x1FC */ CS_CMD_ACTOR_CUE_508,
    /* 0x1FD */ CS_CMD_ACTOR_CUE_509,
    /* 0x1FE */ CS_CMD_ACTOR_CUE_510,
    /* 0x1FF */ CS_CMD_ACTOR_CUE_511,
    /* 0x200 */ CS_CMD_ACTOR_CUE_512,
    /* 0x201 */ CS_CMD_ACTOR_CUE_513,
    /* 0x202 */ CS_CMD_ACTOR_CUE_514,
    /* 0x203 */ CS_CMD_ACTOR_CUE_515,
    /* 0x204 */ CS_CMD_ACTOR_CUE_516,
    /* 0x205 */ CS_CMD_ACTOR_CUE_517,
    /* 0x206 */ CS_CMD_ACTOR_CUE_518,
    /* 0x207 */ CS_CMD_ACTOR_CUE_519,
    /* 0x208 */ CS_CMD_ACTOR_CUE_520,
    /* 0x209 */ CS_CMD_ACTOR_CUE_521,
    /* 0x20A */ CS_CMD_ACTOR_CUE_522,
    /* 0x20B */ CS_CMD_ACTOR_CUE_523,
    /* 0x20C */ CS_CMD_ACTOR_CUE_524,
    /* 0x20D */ CS_CMD_ACTOR_CUE_525,
    /* 0x20E */ CS_CMD_ACTOR_CUE_526,
    /* 0x20F */ CS_CMD_ACTOR_CUE_527,
    /* 0x210 */ CS_CMD_ACTOR_CUE_528,
    /* 0x211 */ CS_CMD_ACTOR_CUE_529,
    /* 0x212 */ CS_CMD_ACTOR_CUE_530,
    /* 0x213 */ CS_CMD_ACTOR_CUE_531,
    /* 0x214 */ CS_CMD_ACTOR_CUE_532,
    /* 0x215 */ CS_CMD_ACTOR_CUE_533,
    /* 0x216 */ CS_CMD_ACTOR_CUE_534,
    /* 0x217 */ CS_CMD_ACTOR_CUE_535,
    /* 0x218 */ CS_CMD_ACTOR_CUE_536,
    /* 0x219 */ CS_CMD_ACTOR_CUE_537,
    /* 0x21A */ CS_CMD_ACTOR_CUE_538,
    /* 0x21B */ CS_CMD_ACTOR_CUE_539,
    /* 0x21C */ CS_CMD_ACTOR_CUE_540,
    /* 0x21D */ CS_CMD_ACTOR_CUE_541,
    /* 0x21E */ CS_CMD_ACTOR_CUE_542,
    /* 0x21F */ CS_CMD_ACTOR_CUE_543,
    /* 0x220 */ CS_CMD_ACTOR_CUE_544,
    /* 0x221 */ CS_CMD_ACTOR_CUE_545,
    /* 0x222 */ CS_CMD_ACTOR_CUE_546,
    /* 0x223 */ CS_CMD_ACTOR_CUE_547,
    /* 0x224 */ CS_CMD_ACTOR_CUE_548,
    /* 0x225 */ CS_CMD_ACTOR_CUE_549,
    /* 0x226 */ CS_CMD_ACTOR_CUE_550,
    /* 0x227 */ CS_CMD_ACTOR_CUE_551,
    /* 0x228 */ CS_CMD_ACTOR_CUE_552,
    /* 0x229 */ CS_CMD_ACTOR_CUE_553,
    /* 0x22A */ CS_CMD_ACTOR_CUE_554,
    /* 0x22B */ CS_CMD_ACTOR_CUE_555,
    /* 0x22C */ CS_CMD_ACTOR_CUE_556,
    /* 0x22D */ CS_CMD_ACTOR_CUE_557,
    /* 0x22E */ CS_CMD_ACTOR_CUE_558,
    /* 0x22F */ CS_CMD_ACTOR_CUE_559,
    /* 0x230 */ CS_CMD_ACTOR_CUE_560,
    /* 0x231 */ CS_CMD_ACTOR_CUE_561,
    /* 0x232 */ CS_CMD_ACTOR_CUE_562,
    /* 0x233 */ CS_CMD_ACTOR_CUE_563,
    /* 0x234 */ CS_CMD_ACTOR_CUE_564,
    /* 0x235 */ CS_CMD_ACTOR_CUE_565,
    /* 0x236 */ CS_CMD_ACTOR_CUE_566,
    /* 0x237 */ CS_CMD_ACTOR_CUE_567,
    /* 0x238 */ CS_CMD_ACTOR_CUE_568,
    /* 0x239 */ CS_CMD_ACTOR_CUE_569,
    /* 0x23A */ CS_CMD_ACTOR_CUE_570,
    /* 0x23B */ CS_CMD_ACTOR_CUE_571,
    /* 0x23C */ CS_CMD_ACTOR_CUE_572,
    /* 0x23D */ CS_CMD_ACTOR_CUE_573,
    /* 0x23E */ CS_CMD_ACTOR_CUE_574,
    /* 0x23F */ CS_CMD_ACTOR_CUE_575,
    /* 0x240 */ CS_CMD_ACTOR_CUE_576,
    /* 0x241 */ CS_CMD_ACTOR_CUE_577,
    /* 0x242 */ CS_CMD_ACTOR_CUE_578,
    /* 0x243 */ CS_CMD_ACTOR_CUE_579,
    /* 0x244 */ CS_CMD_ACTOR_CUE_580,
    /* 0x245 */ CS_CMD_ACTOR_CUE_581,
    /* 0x246 */ CS_CMD_ACTOR_CUE_582,
    /* 0x247 */ CS_CMD_ACTOR_CUE_583,
    /* 0x248 */ CS_CMD_ACTOR_CUE_584,
    /* 0x249 */ CS_CMD_ACTOR_CUE_585,
    /* 0x24A */ CS_CMD_ACTOR_CUE_586,
    /* 0x24B */ CS_CMD_ACTOR_CUE_587,
    /* 0x24C */ CS_CMD_ACTOR_CUE_588,
    /* 0x24D */ CS_CMD_ACTOR_CUE_589,
    /* 0x24E */ CS_CMD_ACTOR_CUE_590,
    /* 0x24F */ CS_CMD_ACTOR_CUE_591,
    /* 0x250 */ CS_CMD_ACTOR_CUE_592,
    /* 0x251 */ CS_CMD_ACTOR_CUE_593,
    /* 0x252 */ CS_CMD_ACTOR_CUE_594,
    /* 0x253 */ CS_CMD_ACTOR_CUE_595,
    /* 0x254 */ CS_CMD_ACTOR_CUE_596,
    /* 0x255 */ CS_CMD_ACTOR_CUE_597,
    /* 0x256 */ CS_CMD_ACTOR_CUE_598,
    /* 0x257 */ CS_CMD_ACTOR_CUE_599,
    /*    -2 */ CS_CMD_ACTOR_CUE_POST_PROCESS = 0xFFFFFFFE,
    /*    -1 */ CS_CAM_STOP // OoT Remnant
} CutsceneCmd;

namespace SOH {

static inline uint32_t read_CMD_BBBB(std::shared_ptr<Ship::BinaryReader> reader) {
    uint32_t v;
    reader->Read((char*)&v, sizeof(uint32_t));

    return v;
}

static inline uint32_t read_CMD_BBH(std::shared_ptr<Ship::BinaryReader> reader) {
    uint32_t v;
    reader->Read((char*)&v, sizeof(uint32_t));

    // swap the half word to match endianness
    if (reader->GetEndianness() != Ship::Endianness::Native) {
        uint8_t* b = (uint8_t*)&v;
        uint8_t tmp = b[2];
        b[2] = b[3];
        b[3] = tmp;
    }

    return v;
}

static inline uint32_t read_CMD_HBB(std::shared_ptr<Ship::BinaryReader> reader) {
    uint32_t v;
    reader->Read((char*)&v, sizeof(uint32_t));

    // swap the half word to match endianness
    if (reader->GetEndianness() != Ship::Endianness::Native) {
        uint8_t* b = (uint8_t*)&v;
        uint8_t tmp = b[0];
        b[0] = b[1];
        b[1] = tmp;
    }

    return v;
}

static inline uint32_t read_CMD_HH(std::shared_ptr<Ship::BinaryReader> reader) {
    uint32_t v;
    reader->Read((char*)&v, sizeof(uint32_t));

    // swap the half words to match endianness
    if (reader->GetEndianness() != Ship::Endianness::Native) {
        uint8_t* b = (uint8_t*)&v;
        uint8_t tmp = b[0];
        b[0] = b[1];
        b[1] = tmp;
        tmp = b[2];
        b[2] = b[3];
        b[3] = tmp;
    }

    return v;
}

std::shared_ptr<Ship::IResource>
ResourceFactoryBinaryCutsceneV0::ReadResource(std::shared_ptr<Ship::File> file,
                                              std::shared_ptr<Ship::ResourceInitData> initData) {
    if (!FileHasValidFormatAndReader(file, initData)) {
        return nullptr;
    }

    auto cutscene = std::make_shared<Cutscene>(initData);
    auto reader = std::get<std::shared_ptr<Ship::BinaryReader>>(file->Reader);

    uint32_t numEntries = reader->ReadUInt32();
    cutscene->commands.reserve(numEntries);

    cutscene->numCommands = reader->ReadUInt32();
    cutscene->commands.push_back(cutscene->numCommands);

    cutscene->endFrame = reader->ReadUInt32();
    cutscene->commands.push_back(cutscene->endFrame);

    // BENTODO detect the game
    while (true) {
        uint32_t command = reader->ReadUInt32();
        cutscene->commands.push_back(command);

        if (((command >= CS_CMD_ACTOR_CUE_100) && (command <= CS_CMD_ACTOR_CUE_149)) ||
            (command == CS_CMD_ACTOR_CUE_201) ||
            ((command >= CS_CMD_ACTOR_CUE_450) && (command <= CS_CMD_ACTOR_CUE_599))) {
            goto actorCue;
        }

        switch (command) {
            case CS_CMD_TEXT: {
                uint32_t size = reader->ReadUInt32();
                cutscene->commands.push_back(size);

                for (uint32_t i = 0; i < size; i++) {
                    cutscene->commands.push_back(read_CMD_HH(reader));
                    cutscene->commands.push_back(read_CMD_HH(reader));
                    cutscene->commands.push_back(read_CMD_HH(reader));
                }
                break;
            }
            case CS_CMD_CAMERA_SPLINE: {
                uint32_t size = reader->ReadUInt32();
                cutscene->commands.push_back(size);

                while (1) {
                    // We need to store the first half of the header to get the number of entries.
                    uint32_t header1 = read_CMD_HH(reader);

                    uint32_t numEntries = header1 & 0xFFFF;

                    cutscene->commands.push_back(header1);
                    if (numEntries == 0xFFFF) {
                        break; // Last command is HH(0xFFFF, 0004) which was already read in and pushed into the vector
                    }
                    cutscene->commands.push_back(read_CMD_HH(reader));

                    for (size_t j = 0; j < numEntries * 2; j++) {
                        cutscene->commands.push_back(read_CMD_BBH(reader));
                        cutscene->commands.push_back(read_CMD_HH(reader));
                        cutscene->commands.push_back(read_CMD_HH(reader));
                    }

                    for (size_t j = 0; j < numEntries; j++) {
                        cutscene->commands.push_back(read_CMD_HH(reader));
                        cutscene->commands.push_back(read_CMD_HH(reader));
                    }
                }

                // for (uint32_t i = 0; i < (size / 4); i++) {

                // cutscene->commands.push_back(read_CMD_HH(reader));
                //}
                break;
            }
            case CS_CMD_MISC:
            case CS_CMD_LIGHT_SETTING:
            case CS_CMD_TRANSITION:
            case CS_CMD_MOTION_BLUR:
            case CS_CMD_GIVE_TATL:
            case CS_CMD_START_SEQ:
            case CS_CMD_STOP_SEQ:
            case CS_CMD_SFX_REVERB_INDEX_2:
            case CS_CMD_SFX_REVERB_INDEX_1:
            case CS_CMD_MODIFY_SEQ:
            case CS_CMD_START_AMBIENCE:
            case CS_CMD_FADE_OUT_AMBIENCE:
            case CS_CMD_DESTINATION:
            case CS_CMD_CHOOSE_CREDITS_SCENES:
            default: {
                uint32_t size = reader->ReadUInt32();
                cutscene->commands.push_back(size);

                for (uint32_t i = 0; i < size; i++) {
                    cutscene->commands.push_back(read_CMD_HH(reader));
                    cutscene->commands.push_back(read_CMD_HH(reader));
                }
                break;
            }
            case CS_CMD_TRANSITION_GENERAL: {
                uint32_t size = reader->ReadUInt32();
                cutscene->commands.push_back(size);

                for (uint32_t i = 0; i < size; i++) {
                    cutscene->commands.push_back(read_CMD_HH(reader));
                    cutscene->commands.push_back(read_CMD_HBB(reader));
                    cutscene->commands.push_back(read_CMD_BBBB(reader));
                }
                break;
            }
            case CS_CMD_FADE_OUT_SEQ: {
                uint32_t size = reader->ReadUInt32();
                cutscene->commands.push_back(size);

                for (uint32_t i = 0; i < size; i++) {
                    cutscene->commands.push_back(read_CMD_HH(reader));
                    cutscene->commands.push_back(read_CMD_HH(reader));
                    cutscene->commands.push_back(reader->ReadUInt32());
                }
                break;
            }
            case CS_CMD_TIME: {
                uint32_t size = reader->ReadUInt32();
                cutscene->commands.push_back(size);

                for (uint32_t i = 0; i < size; i++) {
                    cutscene->commands.push_back(read_CMD_HH(reader));
                    cutscene->commands.push_back(read_CMD_HBB(reader));
                    cutscene->commands.push_back(reader->ReadUInt32());
                }
                break;
            }
            case CS_CMD_PLAYER_CUE: {
            actorCue:
                uint32_t size = reader->ReadUInt32();
                cutscene->commands.push_back(size);
                for (uint32_t i = 0; i < size; i++) {
                    cutscene->commands.push_back(read_CMD_HH(reader));
                    cutscene->commands.push_back(read_CMD_HH(reader));
                    cutscene->commands.push_back(read_CMD_HH(reader));
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                }
                break;
            }
            case CS_CMD_RUMBLE: {
                uint32_t size = reader->ReadUInt32();
                cutscene->commands.push_back(size);

                for (uint32_t i = 0; i < size; i++) {
                    cutscene->commands.push_back(read_CMD_HH(reader));
                    cutscene->commands.push_back(read_CMD_HBB(reader));
                    cutscene->commands.push_back(read_CMD_BBBB(reader));
                }
                break;
            }
            case 0xFFFFFFFF: {
                cutscene->commands.push_back(reader->ReadUInt32());
                return cutscene;
            }
        }
    }

    return cutscene;
}

#if 0
void Ship::CutsceneFactoryV0::ParseFileBinaryOoT(std::shared_ptr<BinaryReader> reader,
                                                std::shared_ptr<Cutscene> cutscene) {
    while (true) {
        uint32_t commandId = reader->ReadUInt32();
        cutscene->commands.push_back(commandId);

        switch (commandId) {
            case (uint32_t)CutsceneCommands::SetCameraPos: {
                cutscene->commands.push_back(read_CMD_HH(reader));
                cutscene->commands.push_back(read_CMD_HH(reader));

                while (true) {
                    uint32_t val = read_CMD_BBH(reader);
                    int8_t continueFlag = ((int8_t*)&val)[0];

                    cutscene->commands.push_back(val);
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(read_CMD_HH(reader));
                    cutscene->commands.push_back(read_CMD_HH(reader));

                    if (continueFlag == -1) {
                        break;
                    }
                }
            } break;
            case (uint32_t)CutsceneCommands::SetCameraFocus: {
                cutscene->commands.push_back(read_CMD_HH(reader));
                cutscene->commands.push_back(read_CMD_HH(reader));

                while (true) {
                    uint32_t val = read_CMD_BBH(reader);
                    int8_t continueFlag = ((int8_t*)&val)[0];

                    cutscene->commands.push_back(val);
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(read_CMD_HH(reader));
                    cutscene->commands.push_back(read_CMD_HH(reader));

                    if (continueFlag == -1) {
                        break;
                    }
                }
                break;
            }
            case (uint32_t)CutsceneCommands::SpecialAction: {
                uint32_t size = reader->ReadUInt32();
                cutscene->commands.push_back(size);

                for (uint32_t i = 0; i < size; i++) {
                    cutscene->commands.push_back(read_CMD_HH(reader));
                    cutscene->commands.push_back(read_CMD_HH(reader));
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                }
                break;
            }
            case (uint32_t)CutsceneCommands::SetLighting: {
                uint32_t size = reader->ReadUInt32();
                cutscene->commands.push_back(size);

                for (uint32_t i = 0; i < size; i++) {
                    cutscene->commands.push_back(read_CMD_HH(reader));
                    cutscene->commands.push_back(read_CMD_HH(reader));
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                }
                break;
            }
            case (uint32_t)CutsceneCommands::SetCameraPosLink: {
                cutscene->commands.push_back(read_CMD_HH(reader));
                cutscene->commands.push_back(read_CMD_HH(reader));

                while (true) {
                    uint32_t val = read_CMD_BBH(reader);
                    int8_t continueFlag = ((int8_t*)&val)[0];

                    cutscene->commands.push_back(val);
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(read_CMD_HH(reader));
                    cutscene->commands.push_back(read_CMD_HH(reader));

                    if (continueFlag == -1) {
                        break;
                    }
                }
                break;
            }
            case (uint32_t)CutsceneCommands::SetCameraFocusLink: {
                cutscene->commands.push_back(read_CMD_HH(reader));
                cutscene->commands.push_back(read_CMD_HH(reader));

                while (true) {
                    uint32_t val = read_CMD_BBH(reader);
                    int8_t continueFlag = ((int8_t*)&val)[0];

                    cutscene->commands.push_back(val);
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(read_CMD_HH(reader));
                    cutscene->commands.push_back(read_CMD_HH(reader));

                    if (continueFlag == -1) {
                        break;
                    }
                }
                break;
            }
            case (uint32_t)CutsceneCommands::Cmd09: {
                uint32_t size = reader->ReadUInt32();
                cutscene->commands.push_back(size);

                for (uint32_t i = 0; i < size; i++) {
                    cutscene->commands.push_back(read_CMD_HH(reader));
                    cutscene->commands.push_back(read_CMD_HBB(reader));
                    cutscene->commands.push_back(read_CMD_BBH(reader));
                }
                break;
            }
            case 0x15:
            case (uint32_t)CutsceneCommands::Unknown: {
                uint32_t size = reader->ReadUInt32();
                cutscene->commands.push_back(size);

                for (uint32_t i = 0; i < size; i++) {
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                }
            } break;
            case (uint32_t)CutsceneCommands::Textbox: {
                uint32_t size = reader->ReadUInt32();
                cutscene->commands.push_back(size);

                for (uint32_t i = 0; i < size; i++) {
                    cutscene->commands.push_back(read_CMD_HH(reader));
                    cutscene->commands.push_back(read_CMD_HH(reader));
                    cutscene->commands.push_back(read_CMD_HH(reader));
                }
                break;
            }
            case (uint32_t)CutsceneCommands::SetActorAction0:
            case (uint32_t)CutsceneCommands::SetActorAction1:
            case 17:
            case 18:
            case 23:
            case 34:
            case 39:
            case 46:
            case 76:
            case 85:
            case 93:
            case 105:
            case 107:
            case 110:
            case 119:
            case 123:
            case 138:
            case 139:
            case 144:
            case (uint32_t)CutsceneCommands::SetActorAction2:
            case 16:
            case 24:
            case 35:
            case 40:
            case 48:
            case 64:
            case 68:
            case 70:
            case 78:
            case 80:
            case 94:
            case 116:
            case 118:
            case 120:
            case 125:
            case 131:
            case 141:
            case (uint32_t)CutsceneCommands::SetActorAction3:
            case 36:
            case 41:
            case 50:
            case 67:
            case 69:
            case 72:
            case 74:
            case 81:
            case 106:
            case 117:
            case 121:
            case 126:
            case 132:
            case (uint32_t)CutsceneCommands::SetActorAction4:
            case 37:
            case 42:
            case 51:
            case 53:
            case 63:
            case 65:
            case 66:
            case 75:
            case 82:
            case 108:
            case 127:
            case 133:
            case (uint32_t)CutsceneCommands::SetActorAction5:
            case 38:
            case 43:
            case 47:
            case 54:
            case 79:
            case 83:
            case 128:
            case 135:
            case (uint32_t)CutsceneCommands::SetActorAction6:
            case 55:
            case 77:
            case 84:
            case 90:
            case 129:
            case 136:
            case (uint32_t)CutsceneCommands::SetActorAction7:
            case 52:
            case 57:
            case 58:
            case 88:
            case 115:
            case 130:
            case 137:
            case (uint32_t)CutsceneCommands::SetActorAction8:
            case 60:
            case 89:
            case 111:
            case 114:
            case 134:
            case 142:
            case (uint32_t)CutsceneCommands::SetActorAction9:
            case (uint32_t)CutsceneCommands::SetActorAction10: {
                uint32_t size = reader->ReadUInt32();
                cutscene->commands.push_back(size);

                for (uint32_t i = 0; i < size; i++) {
                    cutscene->commands.push_back(read_CMD_HH(reader));
                    cutscene->commands.push_back(read_CMD_HH(reader));
                    cutscene->commands.push_back(read_CMD_HH(reader));
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                }

                break;
            }
            case (uint32_t)CutsceneCommands::SetSceneTransFX: {
                cutscene->commands.push_back(reader->ReadUInt32());
                cutscene->commands.push_back(read_CMD_HH(reader));
                cutscene->commands.push_back(read_CMD_HH(reader));
                break;
            }
            case (uint32_t)CutsceneCommands::PlayBGM: {
                uint32_t size = reader->ReadUInt32();
                cutscene->commands.push_back(size);

                for (uint32_t i = 0; i < size; i++) {
                    cutscene->commands.push_back(read_CMD_HH(reader));
                    cutscene->commands.push_back(read_CMD_HH(reader));
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                }
                break;
            }
            case (uint32_t)CutsceneCommands::StopBGM: {
                uint32_t size = reader->ReadUInt32();
                cutscene->commands.push_back(size);

                for (uint32_t i = 0; i < size; i++) {
                    cutscene->commands.push_back(read_CMD_HH(reader));
                    cutscene->commands.push_back(read_CMD_HH(reader));
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                }
                break;
            }
            case (uint32_t)CutsceneCommands::FadeBGM: {
                uint32_t size = reader->ReadUInt32();
                cutscene->commands.push_back(size);

                for (uint32_t i = 0; i < size; i++) {
                    cutscene->commands.push_back(read_CMD_HH(reader));
                    cutscene->commands.push_back(read_CMD_HH(reader));
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                }
                break;
            }
            case (uint32_t)CutsceneCommands::SetTime: {
                uint32_t size = reader->ReadUInt32();
                cutscene->commands.push_back(size);

                for (uint32_t i = 0; i < size; i++) {
                    cutscene->commands.push_back(read_CMD_HH(reader));
                    cutscene->commands.push_back(read_CMD_HBB(reader));
                    cutscene->commands.push_back(reader->ReadUInt32());
                }
                break;
            }
            case (uint32_t)CutsceneCommands::Terminator: {
                cutscene->commands.push_back(reader->ReadUInt32());
                cutscene->commands.push_back(read_CMD_HH(reader));
                cutscene->commands.push_back(read_CMD_HH(reader));
                break;
            }
            case 0xFFFFFFFF: // CS_END
            {
                cutscene->commands.push_back(reader->ReadUInt32());
                return;
            }
            default:
                SPDLOG_TRACE("CutsceneV0: Unknown command {}\n", commandId);
                // error?
                break;
        }
    }
}
#endif
} // namespace SOH
