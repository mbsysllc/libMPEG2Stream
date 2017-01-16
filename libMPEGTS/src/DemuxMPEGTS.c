#include "../include/libMPEGTS.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    void TSParsePESPacket(BitInput *BitI, MPEGTransportStream *Stream) { // PES_packet
        int N3 = 0, N2 = 0, N1 = 0; // FIXME: WTF IS N3, N2, and N1?
        Stream->PES->PacketStartCodePrefix                     = ReadBits(BitI, 24);
        Stream->PES->StreamID                                  = ReadBits(BitI, 8);
        Stream->PES->PESPacketSize                             = ReadBits(BitI, 16);
        if (Stream->PES->StreamID != ProgramStreamFolder &&
            Stream->PES->StreamID != AnnexA_DSMCCStream &&
            Stream->PES->StreamID != ProgramStreamMap &&
            Stream->PES->StreamID != PrivateStream2 &&
            Stream->PES->StreamID != PaddingStream &&
            Stream->PES->StreamID != ECMStream &&
            Stream->PES->StreamID != EMMStream &&
            Stream->PES->StreamID != TypeEStream) {
            SkipBits(BitI, 2);
            Stream->PES->PESScramblingControl   = ReadBits(BitI, 2);
            Stream->PES->PESPriority            = ReadBits(BitI, 1);
            Stream->PES->AlignmentIndicator     = ReadBits(BitI, 1);
            Stream->PES->CopyrightIndicator     = ReadBits(BitI, 1);
            Stream->PES->OriginalOrCopy         = ReadBits(BitI, 1);
            Stream->PES->PTSDTSFlags            = ReadBits(BitI, 2);
            Stream->PES->ESCRFlag               = ReadBits(BitI, 1);
            Stream->PES->ESRateFlag             = ReadBits(BitI, 1);
            Stream->PES->DSMTrickModeFlag       = ReadBits(BitI, 1);
            Stream->PES->AdditionalCopyInfoFlag = ReadBits(BitI, 1);
            Stream->PES->PESCRCFlag             = ReadBits(BitI, 1);
            Stream->PES->PESExtensionFlag       = ReadBits(BitI, 1);
            Stream->PES->PESHeaderSize          = ReadBits(BitI, 8);
            if (Stream->PES->PTSDTSFlags == 2) {
                SkipBits(BitI, 4);
                uint8_t  PTS1 = ReadBits(BitI, 3);
                SkipBits(BitI, 1); // marker bit
                uint16_t PTS2 = ReadBits(BitI, 15);
                SkipBits(BitI, 1); // marker bit
                uint16_t PTS3 = ReadBits(BitI, 15);
                SkipBits(BitI, 1); // marker bit
                
                Stream->PES->PTS  = PTS1 << 30;
                Stream->PES->PTS += PTS2 << 15;
                Stream->PES->PTS += PTS3;
            }
            if (Stream->PES->PTSDTSFlags == 3) {
                SkipBits(BitI, 4);
                uint8_t  PTS1 = ReadBits(BitI, 3);
                SkipBits(BitI, 1); // marker bit
                uint16_t PTS2 = ReadBits(BitI, 15);
                SkipBits(BitI, 1); // marker bit
                uint16_t PTS3 = ReadBits(BitI, 15);
                SkipBits(BitI, 1); // marker bit
                
                Stream->PES->PTS  = PTS1 << 30;
                Stream->PES->PTS += PTS2 << 15;
                Stream->PES->PTS += PTS3;
                
                SkipBits(BitI, 4);
                uint8_t  DTS1 = ReadBits(BitI, 3);
                SkipBits(BitI, 1); // marker bit
                uint16_t DTS2 = ReadBits(BitI, 15);
                SkipBits(BitI, 1); // marker bit
                uint16_t DTS3 = ReadBits(BitI, 15);
                SkipBits(BitI, 1); // marker bit
                
                Stream->PES->DTS  = DTS1 << 30;
                Stream->PES->DTS += DTS2 << 15;
                Stream->PES->DTS += DTS3;
            }
            if (Stream->PES->ESCRFlag == true) {
                SkipBits(BitI, 2);
                uint8_t  ESCR1 = ReadBits(BitI, 3);
                SkipBits(BitI, 1); // marker bit
                uint16_t ESCR2 = ReadBits(BitI, 15);
                SkipBits(BitI, 1); // marker bit
                uint16_t ESCR3 = ReadBits(BitI, 15);
                SkipBits(BitI, 1); // marker bit
                
                Stream->PES->ESCR  = ESCR1 << 30;
                Stream->PES->ESCR += ESCR2 << 15;
                Stream->PES->ESCR += ESCR3;
            }
            if (Stream->PES->ESRateFlag == true) {
                SkipBits(BitI, 1);
                Stream->PES->ESRate = ReadBits(BitI, 22);
                SkipBits(BitI, 1);
            }
            if (Stream->PES->DSMTrickModeFlag == true) {
                Stream->PES->TrickModeControl        = ReadBits(BitI, 3);
                if (Stream->PES->TrickModeControl == FastForward) {
                    Stream->PES->FieldID             = ReadBits(BitI, 2);
                    Stream->PES->IntraSliceRefresh   = ReadBits(BitI, 1);
                    Stream->PES->FrequencyTruncation = ReadBits(BitI, 2);
                } else if (Stream->PES->TrickModeControl == SlowMotion) {
                    Stream->PES->RepetitionControl   = ReadBits(BitI, 5);
                } else if (Stream->PES->TrickModeControl == FreezeFrame) {
                    Stream->PES->FieldID             = ReadBits(BitI, 2);
                    SkipBits(BitI, 3);
                } else if (Stream->PES->TrickModeControl == FastRewind) {
                    Stream->PES->FieldID             = ReadBits(BitI, 2);
                    Stream->PES->IntraSliceRefresh   = ReadBits(BitI, 1);
                    Stream->PES->FrequencyTruncation = ReadBits(BitI, 2);
                } else if (Stream->PES->TrickModeControl == SlowRewind) {
                    Stream->PES->RepetitionControl   = ReadBits(BitI, 5);
                } else {
                    SkipBits(BitI, 5);
                }
            }
            if (Stream->PES->AdditionalCopyInfoFlag == true) {
                SkipBits(BitI, 1);
                Stream->PES->AdditionalCopyInfo      = ReadBits(BitI, 7);
            }
            if (Stream->PES->PESCRCFlag == true) {
                Stream->PES->PreviousPESPacketCRC    = ReadBits(BitI, 16);
            }
            if (Stream->PES->PESExtensionFlag == true) {
                Stream->PES->PESPrivateDataFlag          = ReadBits(BitI, 1);
                Stream->PES->PackHeaderFieldFlag         = ReadBits(BitI, 1);
                Stream->PES->ProgramPacketSeqCounterFlag = ReadBits(BitI, 1);
                Stream->PES->PSTDBufferFlag              = ReadBits(BitI, 1);
                Stream->PES->PESExtensionFlag2           = ReadBits(BitI, 1);
                if (Stream->PES->PESPrivateDataFlag == true) {
                    SkipBits(BitI, 128);
                }
                if (Stream->PES->PackHeaderFieldFlag == true) {
                    Stream->PES->PackFieldSize           = ReadBits(BitI, 8);
                    pack_header();
                }
                if (Stream->PES->ProgramPacketSeqCounterFlag == true) {
                    SkipBits(BitI, 1);
                    Stream->PES->ProgramPacketSeqCounter = ReadBits(BitI, 7);
                    SkipBits(BitI, 1);
                    Stream->PES->MPEGVersionIdentifier   = ReadBits(BitI, 1);
                    Stream->PES->OriginalStuffSize       = ReadBits(BitI, 6);
                }
                if (Stream->PES->PSTDBufferFlag == true) {
                    SkipBits(BitI, 2);
                    Stream->PES->PSTDBufferScale         = ReadBits(BitI, 1);
                    Stream->PES->PSTDBufferSize          = ReadBits(BitI, 13);
                }
                if (Stream->PES->PESExtensionFlag2 == true) {
                    SkipBits(BitI, 1);
                    Stream->PES->PESExtensionFieldSize   = ReadBits(BitI, 7);
                    Stream->PES->StreamIDExtensionFlag   = ReadBits(BitI, 1);
                    if (Stream->PES->StreamIDExtensionFlag == false) {
                        Stream->PES->StreamIDExtension   = ReadBits(BitI, 7);
                    } else {
                        SkipBits(BitI, 6);
                        // tref_extension_flag
                        Stream->PES->TREFFieldPresentFlag = ReadBits(BitI, 1);
                        if (Stream->PES->TREFFieldPresentFlag == false) {
                            SkipBits(BitI, 4);
                            uint8_t  TREF1 = ReadBits(BitI, 3);
                            SkipBits(BitI, 1); // marker bit
                            uint16_t TREF2 = ReadBits(BitI, 15);
                            SkipBits(BitI, 1); // marker bit
                            uint16_t TREF3 = ReadBits(BitI, 15);
                            SkipBits(BitI, 1); // marker bit
                            
                            Stream->PES->TREF = TREF1 << 30;
                            Stream->PES->TREF = TREF2 << 15;
                            Stream->PES->TREF = TREF3;
                        }
                    }
                    for (int i = 0; i < N3; i++) {
                        SkipBits(BitI, 8); // Reserved
                    }
                }
                for (int i = 0; i < Stream->PES->PESExtensionFieldSize; i++) {
                    SkipBits(BitI, 8);
                }
            }
            for (int i = 0; i < N1; i++) {
                SkipBits(BitI, 8); // stuffing_byte
            }
            for (int i = 0; i < N2; i++) {
                SkipBits(BitI, 8); // PES_packet_data_byte
            }
        } else if (Stream->PES->StreamID == ProgramStreamFolder ||
                   Stream->PES->StreamID == AnnexA_DSMCCStream ||
                   Stream->PES->StreamID == ProgramStreamMap ||
                   Stream->PES->StreamID == PrivateStream2 ||
                   Stream->PES->StreamID == ECMStream ||
                   Stream->PES->StreamID == EMMStream ||
                   Stream->PES->StreamID == TypeEStream) {
            for (int i = 0; i < Stream->PES->PESPacketSize; i++) {
                SkipBits(BitI, 8); // PES_packet_data_byte
            }
        } else if (Stream->PES->StreamID == PaddingStream) {
            for (int i = 0; i < Stream->PES->PESPacketSize; i++) {
                SkipBits(BitI, 8); // padding_byte
            }
        }
    }
    
    void TSParseAdaptionField(BitInput *BitI, MPEGTransportStream *Stream) { // adaptation_field
        int N = 0; // FIXME: WHAT THE FUCK IS N?
        Stream->Adaptation->AdaptationFieldSize                = ReadBits(BitI, 8);
        Stream->Adaptation->DiscontinuityIndicator             = ReadBits(BitI, 1);
        Stream->Adaptation->RandomAccessIndicator              = ReadBits(BitI, 1);
        Stream->Adaptation->ElementaryStreamPriorityIndicator  = ReadBits(BitI, 1);
        Stream->Adaptation->PCRFlag                            = ReadBits(BitI, 1);
        Stream->Adaptation->OPCRFlag                           = ReadBits(BitI, 1);
        Stream->Adaptation->SlicingPointFlag                   = ReadBits(BitI, 1);
        Stream->Adaptation->TransportPrivateDataFlag           = ReadBits(BitI, 1);
        Stream->Adaptation->AdaptationFieldExtensionFlag       = ReadBits(BitI, 1);
        if (Stream->Adaptation->PCRFlag == true) {
            Stream->Adaptation->ProgramClockReferenceBase      = ReadBits(BitI, 33);
            SkipBits(BitI, 6);
            Stream->Adaptation->ProgramClockReferenceExtension = ReadBits(BitI, 9);
        }
        if (Stream->Adaptation->OPCRFlag == true) {
            Stream->Adaptation->OriginalProgramClockRefBase    = ReadBits(BitI, 33);
            SkipBits(BitI, 6);
            Stream->Adaptation->OriginalProgramClockRefExt     = ReadBits(BitI, 9);
        }
        if (Stream->Adaptation->SlicingPointFlag == true) {
            Stream->Adaptation->SpliceCountdown                = ReadBits(BitI, 8);
        }
        if (Stream->Adaptation->TransportPrivateDataFlag == true) {
            Stream->Adaptation->TransportPrivateDataSize       = ReadBits(BitI, 8);
            for (uint8_t PrivateByte = 0; PrivateByte < Stream->Adaptation->TransportPrivateDataSize; PrivateByte++) {
                Stream->Adaptation->TransportPrivateData[PrivateByte] = ReadBits(BitI, 8);
            }
        }
        if (Stream->Adaptation->AdaptationFieldExtensionFlag == true) {
            Stream->Adaptation->AdaptationFieldExtensionSize    = ReadBits(BitI, 8);
            Stream->Adaptation->LegalTimeWindowFlag             = ReadBits(BitI, 1);
            Stream->Adaptation->PiecewiseRateFlag               = ReadBits(BitI, 1);
            Stream->Adaptation->SeamlessSpliceFlag              = ReadBits(BitI, 1);
            AlignInput(BitI, 1);
            if (Stream->Adaptation->LegalTimeWindowFlag == true) {
                Stream->Adaptation->LegalTimeWindowValidFlag    = ReadBits(BitI, 1);
                if (Stream->Adaptation->LegalTimeWindowValidFlag == true) {
                    Stream->Adaptation->LegalTimeWindowOffset   = ReadBits(BitI, 15);
                }
            }
            if (Stream->Adaptation->PiecewiseRateFlag == true) {
                SkipBits(BitI, 2);
                Stream->Adaptation->PiecewiseRateFlag           = ReadBits(BitI, 22);
            }
            if (Stream->Adaptation->SeamlessSpliceFlag == true) {
                Stream->Adaptation->SpliceType                  = ReadBits(BitI, 8);
                uint8_t  DecodeTimeStamp1                       = ReadBits(BitI, 3);
                SkipBits(BitI, 1); // marker_bit
                uint16_t DecodeTimeStamp2                       = ReadBits(BitI, 15);
                SkipBits(BitI, 1); // marker_bit
                uint16_t DecodeTimeStamp3                       = ReadBits(BitI, 15);
                SkipBits(BitI, 1); // marker_bit
                
                Stream->Adaptation->DecodeTimeStampNextAU  = DecodeTimeStamp1 << 30;
                Stream->Adaptation->DecodeTimeStampNextAU += DecodeTimeStamp2 << 15;
                Stream->Adaptation->DecodeTimeStampNextAU += DecodeTimeStamp3;
            }
            for (int i = 0; i < N; i++) {
                SkipBits(BitI, 8);
            }
        }
        for (int i = 0; i < N; i++) {
            SkipBits(BitI, 8);
        }
    }
    
    void TSParsePacket(BitInput *BitI, MPEGTransportStream *Stream) { // transport_packet
        Stream->Packet->SyncByte                   = ReadBits(BitI, 8);
        Stream->Packet->TransportErrorIndicator    = ReadBits(BitI, 1);
        Stream->Packet->StartOfPayloadIndicator    = ReadBits(BitI, 1);
        Stream->Packet->TransportPriorityIndicator = ReadBits(BitI, 1);
        Stream->Packet->PID                        = ReadBits(BitI, 13);
        Stream->Packet->TransportScramblingControl = ReadBits(BitI, 2);
        Stream->Packet->AdaptationFieldControl     = ReadBits(BitI, 2);
        Stream->Packet->ContinuityCounter          = ReadBits(BitI, 4);
        if (Stream->Packet->AdaptationFieldControl == 2 || Stream->Packet->AdaptationFieldControl == 3) {
            TSParseAdaptionField(BitI, Stream);
        }
        if (Stream->Packet->AdaptationFieldControl == 1 || Stream->Packet->AdaptationFieldControl == 3) {
            int N = 0; // FIXME: I just did this so it would compile what is N?
            for (int i = 0; i < N; i++) {
                SkipBits(BitI, 8); // data_byte
            }
        }
    }
    
    void InitMPEGTransportStream(MPEGTransportStream *Stream) {
        Stream->Packet     = calloc(sizeof(TransportStreamPacket), 1);
        Stream->Adaptation = calloc(sizeof(TSAdaptationField), 1);
        Stream->PES        = calloc(sizeof(PacketizedElementaryStream), 1);
    }
    
#ifdef __cplusplus
}
#endif
