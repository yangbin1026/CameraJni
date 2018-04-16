//speech长度为960
//bitnum=16时，streamSize返回120，32时，streamSize返回240,40时，streamsize返回300
extern "C" void g726_Encode(unsigned char *speech,char *bitstream,int& streamSize,int bitnum);
extern "C" void g726_Decode(char *bitstream,unsigned char *speech,int bitnum);