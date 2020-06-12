clear;clc;close all;

load current_data.mat

figure
plot(Times_0,Current_0*1000,'lineWidth',1)
hold on; grid minor; xlim([2 120]); ylim([0 70])
ax = gca; ax.FontSize = 16 %
plot(Times_2,Current_2*1000,'lineWidth',1)
plot(Times_3,Current_3*1000,'lineWidth',1)
xlabel('Time, s','fontSize',18);
ylabel('Current, mA','fontSize',18);
title('Satellite beacon shortened cycle current for default RadioHead radio settings','fontSize',24);
legend ('(0) - Bw125Cr45Sf128','(2) - Bw31.25Cr48Sf512','(3) - Bw125Cr48Sf4096','fontSize',12);