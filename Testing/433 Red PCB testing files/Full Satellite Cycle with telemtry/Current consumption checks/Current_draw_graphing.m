clear; clc; close all;
load Current_draw_4Cycles_1Ohm_resistor.mat

figure
plot(Times,TXpower_5dBm*1000);
hold on; grid minor;
plot(Times1,TXpower_10dBm*1000);
plot(Times2,TXpower_15dBm*1000);
plot(Times3,TXpower_20dBm*1000);
plot(Times4,TXpower_23dBm*1000);
xlim([0 950]); ylim([0 100])
title('Full satellite beacon cycle current consumption','fontSize',20);
xlabel('Time (sec)','fontSize',16);
ylabel('Current drawn (Milliamps)','fontSize',16);
legend('TX Power - 5dBm','TX Power - 10dBm','TX Power - 15dBm','TX Power - 20dBm','TX Power - 23dBm')

figure
plot(Times,TXpower_5dBm*1000,'lineWidth',1);
hold on; grid minor;
plot(Times1,TXpower_10dBm*1000,'lineWidth',1);
plot(Times2,TXpower_15dBm*1000,'lineWidth',1);
plot(Times3,TXpower_20dBm*1000,'lineWidth',1);
plot(Times4,TXpower_23dBm*1000,'lineWidth',1);
xlim([0 60]); ylim([0 100])
title('Full satellite beacon cycle current consumption','fontSize',20);
xlabel('Time (sec)','fontSize',16);
ylabel('Current drawn (Milliamps)','fontSize',16);
legend('TX Power - 5dBm','TX Power - 10dBm','TX Power - 15dBm','TX Power - 20dBm','TX Power - 23dBm')