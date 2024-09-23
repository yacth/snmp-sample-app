#pragma once
#include <libsnmp.h>
#include "snmp_pp/snmp_pp.h"

#ifdef SNMP_PP_NAMESPACE
using namespace Snmp_pp;
#endif

#define SENSURON_RECEIVE_DATA_OID "1.3.6.1.2.1.1.5.0"
#define SENSURON_SEND_DATA_OID "1.3.6.1.2.1.1.1.0"

#define SENSURON_IP_ADDR "localhost"
#define SENSURON_PORT "1601"

#define PAYLOAD_ARRAY_SIZE 2048

#define SINGLE_UINT_PAYLOAD 4
#define SINGLE_FLOAT_PAYLOAD 4
#define ARRAY_FLOATS_PAYLOAD 4 * 2048

#define PAYLOAD_SIZE SINGLE_UINT_PAYLOAD + SINGLE_FLOAT_PAYLOAD + ARRAY_FLOATS_PAYLOAD

namespace sensuron
{
#pragma pack(push, 1) // Set alignment to 1 byte
  struct SensuronPayload
  {
    uint32_t payload1;
    float payload2;
    float payload3[PAYLOAD_ARRAY_SIZE];
  } typedef sensuronPayload_t;
#pragma pack(pop) // Restore the previous alignment

  class SensuronAgent
  {
  public:
    SensuronAgent();
    ~SensuronAgent();

    int init();

    void handleSetRequest();

    void handleGetRequest();

    void sendPayload();

    void receiveCommand();

    void setPayload(const char *payloadBuffer, size_t payloadBufferSize);

    void setSensuronIpAddress(const char *sensuronIpAddress);
    void setSensuronPort(const char *sensuronPort);
    void setReceiveDataOid(const char *receiveDataOid);
    void setSendDataOid(const char *sendDataOid);

    void stop();

    sensuronPayload_t getPayload();


  private:
    Snmp *_snmp;

    Pdu *_receiveDataPdu;
    Vb *_receiveDataVb;
    Pdu *_sendDataPdu;
    Vb *_sendDataVb;

    CTarget *_ctarget;

    char _sensuronIpAddress[128];
    char _sensuronPort[128];
    char _receiveDataOid[128];
    char _sendDataOid[128];

    char _payloadBuffer[PAYLOAD_SIZE];
    char _commandBuffer[PAYLOAD_SIZE];
    sensuronPayload_t _sensuronPayload;
  };
}