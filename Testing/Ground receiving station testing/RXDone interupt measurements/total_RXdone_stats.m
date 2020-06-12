clear; clc; close all;

load total_time_diff_RXdone.mat

figure

plot(total_time_diff_array*1000000,'lineWidth',1)
grid minor; 
ax=gca; ax.FontSize = 16;
title('Difference in time of RXdone interrupt for 2 LoRa modules receiving the same TX signal','fontSize',24)
xlabel('sample number, n','fontSize',18)
ylabel('Time, \mus','fontSize',18)
legend('measured time difference between 2 Lora Modules','fontSize',12)


RX_int_mean = mean(total_time_diff_array)
RX_int_stddev = std(total_time_diff_array)
RX_int_median = median(total_time_diff_array)
max_time_diff = max(total_time_diff_array)
min_time_diff = min(total_time_diff_array)

% [H95,P95,CI95] = ztest(total_time_diff_array,RX_int_mean,RX_int_stddev,0.05)
% [H97,P97,CI97] = ztest(total_time_diff_array,RX_int_mean,RX_int_stddev,0.03)
% [H99,P99,CI99] = ztest(total_time_diff_array,RX_int_mean,RX_int_stddev,0.01)

thres_3 = normcdf(3,-0.18,2.7966)-normcdf(-3,-0.18,2.7966)
thres_3p5 = normcdf(3.5,-0.18,2.7966)-normcdf(-3.5,-0.18,2.7966)
thres_4 = normcdf(4,-0.18,2.7966)-normcdf(-4,-0.18,2.7966)
thres_4p5 = normcdf(4.5,-0.18,2.7966)-normcdf(-4.5,-0.18,2.7966)
thres_5 = normcdf(5,-0.18,2.7966)-normcdf(-5,-0.18,2.7966)
thres_5 = normcdf(5.5-0.18,2.7966)-normcdf(-5.5,-0.18,2.7966)
thres_6 = normcdf(6,-0.18,2.7966)-normcdf(-6,-0.18,2.7966)

abs_RX_int_mean = mean(abs(total_time_diff_array))
abs_RX_int_stddev = std(abs(total_time_diff_array))
abs_RX_int_median = median(abs(total_time_diff_array))