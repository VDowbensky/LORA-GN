  // read the raw frequency error register values
  uint8_t efeRaw[3] = {0};
  int16_t state = readRegister(SX126X_REG_FREQ_ERROR_RX_CRC, &efeRaw[0], 1);
  ASSERT(state);
  state = readRegister(SX126X_REG_FREQ_ERROR_RX_CRC + 1, &efeRaw[1], 1);
  ASSERT(state);
  state = readRegister(SX126X_REG_FREQ_ERROR_RX_CRC + 2, &efeRaw[2], 1);
  ASSERT(state);
  uint32_t efe = ((uint32_t) efeRaw[0] << 16) | ((uint32_t) efeRaw[1] << 8) | efeRaw[2];
  efe &= 0x0FFFFF;

  float error = 0;

  // check the first bit
  if (efe & 0x80000) {
    // frequency error is negative
    efe |= (uint32_t) 0xFFF00000;
    efe = ~efe + 1;
    error = 1.55f * (float) efe / (1600.0f / (float) this->bandwidthKhz) * -1.0f;
  } else {
    error = 1.55f * (float) efe / (1600.0f / (float) this->bandwidthKhz);
  }

  return(error);
  
  // SX126X register map
#define SX126X_REG_RX_GAIN_RETENTION_0                 0x029F // SX1268 datasheet v1.1, section 9.6
#define SX126X_REG_RX_GAIN_RETENTION_1                 0x02A0 // SX1268 datasheet v1.1, section 9.6
#define SX126X_REG_RX_GAIN_RETENTION_2                 0x02A1 // SX1268 datasheet v1.1, section 9.6
#define SX126X_REG_VERSION_STRING                      0x0320
#define SX126X_REG_HOPPING_ENABLE                      0x0385
#define SX126X_REG_LR_FHSS_PACKET_LENGTH               0x0386
#define SX126X_REG_LR_FHSS_NUM_HOPPING_BLOCKS          0x0387
#define SX126X_REG_LR_FHSS_NUM_SYMBOLS_FREQX_MSB(X)    (0x0388 + (X)*6)
#define SX126X_REG_LR_FHSS_NUM_SYMBOLS_FREQX_LSB(X)    (0x0389 + (X)*6)
#define SX126X_REG_LR_FHSS_FREQX_0(X)                  (0x038A + (X)*6)
#define SX126X_REG_LR_FHSS_FREQX_1(X)                  (0x038B + (X)*6)
#define SX126X_REG_LR_FHSS_FREQX_2(X)                  (0x038C + (X)*6)
#define SX126X_REG_LR_FHSS_FREQX_3(X)                  (0x038D + (X)*6)
#define SX126X_REG_SPECTRAL_SCAN_RESULT                0x0401
#define SX126X_REG_DIOX_OUT_ENABLE                     0x0580
#define SX126X_REG_DIOX_DRIVE_STRENGTH                 0x0582
#define SX126X_REG_DIOX_IN_ENABLE                      0x0583
#define SX126X_REG_DIOX_PULL_UP_CTRL                   0x0584
#define SX126X_REG_DIOX_PULL_DOWN_CTRL                 0x0585

#define SX126X_REG_TX_BITBANG_ENABLE_0                 0x0587
#define SX126X_REG_PATCH_UPDATE_ENABLE                 0x0610
#define SX126X_REG_TX_BITBANG_ENABLE_1                 0x0680
#define SX126X_REG_WHITENING_INITIAL_MSB               0x06B8
#define SX126X_REG_WHITENING_INITIAL_LSB               0x06B9
#define SX126X_REG_RX_TX_PLD_LEN                       0x06BB
#define SX126X_REG_CRC_INITIAL_MSB                     0x06BC
#define SX126X_REG_CRC_INITIAL_LSB                     0x06BD
#define SX126X_REG_CRC_POLYNOMIAL_MSB                  0x06BE
#define SX126X_REG_CRC_POLYNOMIAL_LSB                  0x06BF

#define SX126X_REG_SYNC_WORD_0                         0x06C0
#define SX126X_REG_SYNC_WORD_1                         0x06C1
#define SX126X_REG_SYNC_WORD_2                         0x06C2
#define SX126X_REG_SYNC_WORD_3                         0x06C3
#define SX126X_REG_SYNC_WORD_4                         0x06C4
#define SX126X_REG_SYNC_WORD_5                         0x06C5
#define SX126X_REG_SYNC_WORD_6                         0x06C6
#define SX126X_REG_SYNC_WORD_7                         0x06C7
#define SX126X_REG_NODE_ADDRESS                        0x06CD
#define SX126X_REG_BROADCAST_ADDRESS                   0x06CE

#define SX126X_REG_PAYLOAD_LENGTH                      0x0702
#define SX126X_REG_PACKET_PARAMS                       0x0704
#define SX126X_REG_LORA_SYNC_TIMEOUT                   0x0706

#define SX126X_REG_IQ_CONFIG                           0x0736
#define SX126X_REG_LORA_SYNC_WORD_MSB                  0x0740
#define SX126X_REG_LORA_SYNC_WORD_LSB                  0x0741

#define SX126X_REG_LORA_RX_CODING_RATE                 0x0749
#define SX126X_REG_FREQ_ERROR_RX_CRC                   0x076B
#define SX126X_REG_SPECTRAL_SCAN_STATUS                0x07CD
#define SX126X_REG_RX_ADDR_PTR                         0x0803
#define SX126X_REG_RANDOM_NUMBER_0                     0x0819
#define SX126X_REG_RANDOM_NUMBER_1                     0x081A
#define SX126X_REG_RANDOM_NUMBER_2                     0x081B
#define SX126X_REG_RANDOM_NUMBER_3                     0x081C

#define SX126X_REG_SENSITIVITY_CONFIG                  0x0889 // SX1268 datasheet v1.1, section 15.1
#define SX126X_REG_RF_FREQUENCY_0                      0x088B
#define SX126X_REG_RF_FREQUENCY_1                      0x088C
#define SX126X_REG_RF_FREQUENCY_2                      0x088D
#define SX126X_REG_RF_FREQUENCY_3                      0x088E
#define SX126X_REG_RSSI_AVG_WINDOW                     0x089B
#define SX126X_REG_RX_GAIN                             0x08AC
#define SX126X_REG_TX_CLAMP_CONFIG                     0x08D8

#define SX126X_REG_ANA_LNA                             0x08E2
#define SX126X_REG_LNA_CAP_TUNE_N                      0x08E3
#define SX126X_REG_LNA_CAP_TUNE_P                      0x08E4
#define SX126X_REG_ANA_MIXER                           0x08E5
#define SX126X_REG_OCP_CONFIGURATION                   0x08E7
#define SX126X_REG_RTC_CTRL                            0x0902
#define SX126X_REG_XTA_TRIM                            0x0911
#define SX126X_REG_XTB_TRIM                            0x0912
#define SX126X_REG_DIO3_OUT_VOLTAGE_CTRL               0x0920
#define SX126X_REG_EVENT_MASK                          0x0944
#define SX126X_REG_PATCH_MEMORY_BASE                   0x8000