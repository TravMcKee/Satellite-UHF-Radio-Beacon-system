clear; clc;
serial_port = 'COM10';
BaudRate = 115200;
count=1;

ButtonHandle = uicontrol('Style', 'PushButton', ...
                         'String', 'Stop loop', ...
                         'Callback', 'delete(gcbf)');

% Open and setup the USB serial port to the Arduino
s = serialport(serial_port,BaudRate);
%s.BaudRate = 115200;
%s.InputBufferSize = 2000;
%open(s);
%fread(s);
while (1)
    if (s.NumBytesAvailable > 0)
       %pause(3)
       msg = read(s,s.NumBytesAvailable,'int16')
       disp(msg)
%        s.flush
%        for x=1:length(msg)
%            GS1(count,x) = msg(x);
%        end
%        count=count+1;
       clear msg
    end
    
    if ~ishandle(ButtonHandle)
        disp('Loop stopped by user');
    break;
    end 
end
