clear; clc; close all;

load total_time_diff_RXdone.mat

figure
plot(abs(total_time_diff_array*1000000),'lineWidth',1)
hold on; grid minor; xlim([1 150])

ax=gca; ax.FontSize = 16;
title('Absolute time difference for RXdone interrupt between two ground stations','fontSize',24)
xlabel('Sample number - n','fontSize',18)
ylabel('Time - microseconds','fontSize',18)
