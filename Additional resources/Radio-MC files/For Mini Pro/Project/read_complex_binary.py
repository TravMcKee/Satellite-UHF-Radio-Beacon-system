
import numpy as np
import _io


def read_complex_binary(fileName, offset=None, valuesToRead=-1, channels = 1):
    """read_complex_binary(fileName, offset=0, valuesToRead=-1)
reads a binary file containing complex numbers stored as 8 bytes (4 real and 4 imag). 
offset is given in values. 
valuesToRead is the number of complex values to read."""

    dt = np.complex64

    if type(fileName)==str:
        with open(fileName,'rb') as f:
            if offset:
                f.seek(offset*channels*np.dtype(dt).itemsize,0)        # 8 bytes pr complex number

                
            data = np.fromfile(f,dtype=dt,count=valuesToRead*channels)
            f.close()
            if channels > 1:
                dataNew = np.empty((channels,int(len(data)/channels)),dtype=dt)
                for i in range(channels):
                    # print(i)
                    dataNew[i] = data[i::channels]
                return dataNew
                    
            return data
        
    elif type(fileName)==_io.BufferedReader:
        if offset:
            fileName.seek(offset*channels*np.dtype(dt).itemsize,1)
        data = np.fromfile(fileName,dtype=dt,count=valuesToRead*channels)
        if channels > 1:
            dataNew = np.empty((channels,int(len(data)/channels)),dtype=dt)
            for i in range(channels):
                dataNew[i] = data[i::channels]
            return dataNew
        return data
    else:
        raise TypeError('fileName must be of type str or _io.BufferedReader')
        
