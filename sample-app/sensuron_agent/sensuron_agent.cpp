#include "sensuron_agent.h"

namespace sensuron
{
  SensuronAgent::SensuronAgent()
      : _snmp(nullptr),
        _receiveDataPdu(new Pdu()),
        _receiveDataVb(new Vb()),
        _sendDataPdu(new Pdu()),
        _sendDataVb(new Vb()),
        _sensuronPayload({0}),
        _payloadBuffer{0},
        _commandBuffer{0},
        _sensuronIpAddress{SENSURON_IP_ADDR},
        _sensuronPort{SENSURON_PORT},
        _receiveDataOid{SENSURON_RECEIVE_DATA_OID},
        _sendDataOid{SENSURON_SEND_DATA_OID},
        _ctarget(nullptr)
  {
  }

  SensuronAgent::~SensuronAgent()
  {
    delete _snmp;
    delete _receiveDataPdu;
    delete _receiveDataVb;
    delete _sendDataPdu;
    delete _sendDataVb;
    delete _ctarget;
    stop();
  }

  int SensuronAgent::init()
  {
    Snmp::socket_startup();
    int status;
    _snmp = new Snmp(status);

    char updAddress[257] = {};
    strcat(updAddress, _sensuronIpAddress);
    strcat(updAddress, "/");
    strcat(updAddress, _sensuronPort);

    _ctarget = new CTarget(UdpAddress(updAddress));

    if (SNMP_CLASS_SUCCESS != status)
    {
      std::cerr << "Sensuron::Error: Session creation failed: " << _snmp->error_msg(status) << std::endl;
      return -1;
    }

    _receiveDataVb->set_oid(_receiveDataOid);
    *_receiveDataPdu += *_receiveDataVb;

    _sendDataVb->set_oid(_sendDataOid);
    *_sendDataPdu += *_sendDataVb;

    return 0;
  }

  void SensuronAgent::handleSetRequest()
  {
    for (int i = 0; i < _sendDataPdu->get_vb_count(); ++i)
    {
      Vb vb;

      if (_sendDataPdu->get_vb(vb, i))
      {

        vb.set_value(_commandBuffer);
        _sendDataPdu->set_vb(vb, i);
      }
    }
  }

  void SensuronAgent::handleGetRequest()
  {
    for (int i = 0; i < _receiveDataPdu->get_vb_count(); ++i)
    {
      Vb vb;

      if (_receiveDataPdu->get_vb(vb, i))
      {

        const char *output = vb.get_printable_value();
        size_t outputLength = strlen(output);
        if (outputLength < sizeof(_payloadBuffer))
        {
          memcpy(_payloadBuffer, output, outputLength + 1); // +1 to include null terminator
        }
      }
    }
  }

  void SensuronAgent::sendCommand()
  {
    int status = 0;

    handleSetRequest();
    status = _snmp->set(*_sendDataPdu, *_ctarget);

    if (SNMP_CLASS_SUCCESS != status)
    {
      std::cerr << "Debug::Sensuron::Error: SNMP SetRequest failed " << _snmp->error_msg(status) << std::endl;
    }
  }

  void SensuronAgent::receivePayload()
  {
    int status = 0;

    status = _snmp->get(*_receiveDataPdu, *_ctarget);
    if (SNMP_CLASS_SUCCESS != status)
    {
      std::cerr << "Debug::Sensuron::Error: SNMP GetRequest failed " << _snmp->error_msg(status) << std::endl;
    }

    handleGetRequest();
  }

  void SensuronAgent::setCommand(const char *commandBuffer)
  {
    memcpy(_commandBuffer, commandBuffer, sizeof(_commandBuffer));
    std::cout << "SENSURON::DEBUG: Command buffer: " << _commandBuffer << std::endl;
  }

  void SensuronAgent::setSensuronIpAddress(const char *sensuronIpAddress)
  {
    memcpy(_sensuronIpAddress, sensuronIpAddress, sizeof(_sensuronIpAddress));
  }

  void SensuronAgent::setSensuronPort(const char *sensuronPort)
  {
    memcpy(_sensuronPort, sensuronPort, sizeof(_sensuronPort));
  }

  void SensuronAgent::setReceiveDataOid(const char *receiveDataOid)
  {
    memcpy(_receiveDataOid, receiveDataOid, sizeof(_receiveDataOid));
  }

  void SensuronAgent::setSendDataOid(const char *sendDataOid)
  {
    memcpy(_sendDataOid, sendDataOid, sizeof(_sendDataOid));
  }

  void SensuronAgent::stop()
  {
    Snmp::socket_cleanup();
  }

  sensuronPayload_t SensuronAgent::getPayload()
  {
    memcpy(&_sensuronPayload, _payloadBuffer, sizeof(_payloadBuffer));
    std::cout << "SENSURON::DEBUG: Payload buffer: " << _payloadBuffer << std::endl;
    return _sensuronPayload;
  }

}