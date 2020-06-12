
import numpy as np


def write_complex_binary(fileName, data,outFmt = np.float32):
    """write_complex_binary(fileName, data,outFmt=np.float32)

    data -- input is expected to be complex
    the rows of data make the channels of the output
    data is stored interleaved as with GNU radio

    outFmt -- is the format for each of the parts of a complex input
    """

    # check orientation of input. lowest dimension is assumed to be the number of channels. 

    dataLen = np.max(data.shape)

    if len(data.shape) > 1:
        if len(data.shape) > 2:
            raise ValueError('Expected data to be 1 or 2 dimensional')

        channelIdxNo = np.argmin(data.shape)
        Nchannels = data.shape[channelIdxNo]

        dataout = np.zeros(2*Nchannels*dataLen,dtype=outFmt)
        for chan in range(Nchannels):
            if channelIdxNo == 0:
                dataout[2*chan::2*Nchannels] = np.real(data[chan]).astype(outFmt)
                dataout[2*chan+1::2*Nchannels] = np.imag(data[chan]).astype(outFmt)
            elif channelIdxNo == 1:
                dataout[2*chan::2*Nchannels] = np.real(data[:,chan]).astype(outFmt)
                dataout[2*chan+1::2*Nchannels] = np.imag(data[:,chan]).astype(outFmt)
            else:
                raise ValueError('Did not expect data to be a 3 dimensional array')
            
        with open(fileName,'wb') as f:
            f.write(dataout)

    else:
        # If data is 1 dimensional it's easy
        with open(fileName,'wb') as f:
            data_complex = np.zeros(2*len(data),dtype=outFmt)
            data_complex[::2] = np.real(data)
            data_complex[1::2] = np.imag(data)
            f.write(data_complex)
    

        
