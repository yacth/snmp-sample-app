#include <libsnmp.h>
#include "snmp_pp/snmp_pp.h"
#include "sensuron_agent.h"
#include <iomanip>

#ifdef SNMP_PP_NAMESPACE
using namespace Snmp_pp;
#endif

#define MAX_ARG_STRING_SIZE

int main(int argc, char *argv[])
{
  char sensuronPortIpAddress[MAX_ARG_STRING_SIZE] = SENSURON_IP_ADDR;
  char sensuronPort[MAX_ARG_STRING_SIZE] = SENSURON_PORT;
  char inputOid[MAX_ARG_STRING_SIZE] = SENSURON_RECEIVE_DATA_OID;
  char outputOid[MAX_ARG_STRING_SIZE] = SENSURON_SEND_DATA_OID;

  if (argc > 1 && argc < 5)
  {
    std::cerr << "Usage: " << argv[0] << " [IP_ADDRESS] [IP_PORT] [INPUT_OID] [OUTPUT_OID] [COMMAND]" << std::endl;
    std::strncpy(sensuronPortIpAddress, argv[1], sizeof(sensuronPortIpAddress) - 1);
    sensuronPortIpAddress[sizeof(sensuronPortIpAddress) - 1] = '\0';
    std::strncpy(sensuronPort, argv[2], sizeof(sensuronPort) - 1);
    sensuronPort[sizeof(sensuronPort) - 1] = '\0';
    std::strncpy(inputOid, argv[3], sizeof(inputOid) - 1);
    inputOid[sizeof(inputOid) - 1] = '\0';
    std::strncpy(outputOid, argv[4], sizeof(outputOid) - 1);
    outputOid[sizeof(outputOid) - 1] = '\0';
    return 1;
  }

  DefaultLog::log()->set_filter(ERROR_LOG, 0);
  DefaultLog::log()->set_filter(DEBUG_LOG, 0); // Set logging level of DEBUG to 0 to suppress debug logging

  sensuron::SensuronAgent *sensuronAgent = new sensuron::SensuronAgent();

  sensuronAgent->setSensuronIpAddress(sensuronPortIpAddress);
  sensuronAgent->setSensuronPort(sensuronPort);
  sensuronAgent->setSendDataOid(inputOid);
  sensuronAgent->setReceiveDataOid(outputOid);

  char payloadBuffer[PAYLOAD_SIZE] = {0};
  sensuron::sensuronPayload_t sensuronPayload{0};

  sensuronPayload.payload1 = 123456;
  sensuronPayload.payload2 = 123.456;
  for (int i = 0; i < sizeof(sensuronPayload.payload3) / sizeof(sensuronPayload.payload3[0]); i++)
  {
    sensuronPayload.payload3[i] = i + 0.123;
  }

  memcpy(payloadBuffer, &sensuronPayload, sizeof(sensuronPayload));

  sensuronAgent->init();

  sensuronAgent->setPayload(payloadBuffer);

  while (true)
  {
    sensuronAgent->sendPayload();
  }

  return 0;
}