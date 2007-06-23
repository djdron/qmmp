#ifndef __DECODERFLACFACTORY_INTERFACE_H
#define __DECODERFLACFACTORY_INTERFACE_H

class DecoderFLACFactoryInterface
{
public:
    DecoderFLACFactoryInterface() {}
    virtual ~DecoderFLACFactoryInterface() {}


private:
    DecoderFLACFactoryInterface( const DecoderFLACFactoryInterface& source );
    void operator = ( const DecoderFLACFactoryInterface& source );
};


#endif // __DECODERFLACFACTORY_INTERFACE_H
