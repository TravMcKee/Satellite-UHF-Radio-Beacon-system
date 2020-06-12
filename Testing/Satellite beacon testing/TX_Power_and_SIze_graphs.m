clear; clc;
load TX_Power_and_Size.mat

figure
subplot(1,2,1)
plot(TX_Power,current*1000,'lineWidth',1)
xlabel('RFM95 TX Power level - dBm','fontSize',20)
ylabel('RFM95 module current - mA','fontSize',20)
grid minor;
title('RFM95 module current usage for TX power','fontSize',20)
subplot(1,2,2)
plot(ByteSize,TXtimeSec,'r','lineWidth',1)
xlabel('Radio transmission packet size - Bytes','fontSize',20)
ylabel('RFM95 module transmission time - Seconds','fontSize',20)
grid minor;
title('RFM module TX time for packet size','fontSize',20)