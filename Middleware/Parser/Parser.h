#ifndef __PARSER_H_
#define __PARSER_H_

#ifdef  __cpluscplus
extern "C"
{
#endif

extern uint8_t FrameUnpack(uint8_t token, uint8_t* pBuffer);
extern uint8_t FramePack(uint8_t* pDataIn, uint8_t len, uint8_t* pDataOut);

#ifdef  __cpluscplus
}
#endif

#endif  // __PARSER_H__
