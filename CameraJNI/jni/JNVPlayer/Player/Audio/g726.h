//speech����Ϊ960
//bitnum=16ʱ��streamSize����120��32ʱ��streamSize����240,40ʱ��streamsize����300
extern "C" void g726_Encode(unsigned char *speech,char *bitstream,int& streamSize,int bitnum);
extern "C" void g726_Decode(char *bitstream,unsigned char *speech,int bitnum);