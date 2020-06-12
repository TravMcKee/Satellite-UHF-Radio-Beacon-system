clear; clc; close all;

load total_time_diff_RXdone.mat

figure

plot(total_time_diff_array*1000000)
hold on; grid minor; 
plot(abs(time_diff_array*1000000))
ax=gca; ax.FontSize = 16;
title('difference in time of RXdone interrupt for 2 ground stations receiving the same TX signal','fontSize',24)
xlabel('sample number - n','fontSize',18)
ylabel('time - microseconds','fontSize',18)
legend('measured time difference between 2 Lora Modules','absolute value of difference','fontSize',12)


% RX_int_mean = mean(time_diff_array)
% RX_int_stddev = std(time_diff_array)
% RX_int_median = median(time_diff_array)
% max_time_diff = max(time_diff_array)
% min_time_diff = min(time_diff_array)

Abs_RX_int_mean = mean(abs(time_diff_array))
Abs_RX_int_stddev = std(abs(time_diff_array))
Abs_RX_int_median = median(abs(time_diff_array))
Abs_max_time_diff = max(abs(time_diff_array))

% [H95,P95,CI95] = ztest(time_diff_array,RX_int_mean,RX_int_stddev,0.05)
% [H99,P99,CI99] = ztest(time_diff_array,RX_int_mean,RX_int_stddev,0.01)

[abs_H95,abs_P95,abs_CI95] = ztest(abs(time_diff_array),Abs_RX_int_mean,Abs_RX_int_stddev,0.05)
[abs_H97,abs_P97,abs_CI97] = ztest(abs(time_diff_array),Abs_RX_int_mean,Abs_RX_int_stddev,0.03)
[abs_H99,abs_P99,abs_CI99] = ztest(abs(time_diff_array),Abs_RX_int_mean,Abs_RX_int_stddev,0.01)