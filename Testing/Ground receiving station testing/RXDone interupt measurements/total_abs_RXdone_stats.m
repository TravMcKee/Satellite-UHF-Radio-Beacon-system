clear; clc; close all;

load total_time_diff_RXdone.mat

figure

plot(total_time_diff_array*1000000)
hold on; grid minor; 
plot(abs(total_time_diff_array*1000000))
ax=gca; ax.FontSize = 16;
title('difference in time of RXdone interrupt for 2 ground stations receiving the same TX signal','fontSize',24)
xlabel('sample number - n','fontSize',18)
ylabel('time - microseconds','fontSize',18)
legend('measured time difference between 2 Lora Modules','absolute value of difference','fontSize',12)


Abs_RX_int_mean = mean(abs(total_time_diff_array))
Abs_RX_int_stddev = std(abs(total_time_diff_array))
Abs_RX_int_median = median(abs(total_time_diff_array))
Abs_max_time_diff = max(abs(total_time_diff_array))

[abs_H95,abs_P95,abs_CI95] = ztest(abs(total_time_diff_array),Abs_RX_int_mean,Abs_RX_int_stddev,0.05)
[abs_H97,abs_P97,abs_CI97] = ztest(abs(total_time_diff_array),Abs_RX_int_mean,Abs_RX_int_stddev,0.03)
[abs_H99,abs_P99,abs_CI99] = ztest(abs(total_time_diff_array),Abs_RX_int_mean,Abs_RX_int_stddev,0.01)

figure
histogram(abs(total_time_diff_array),17)

[pdf1] = expcdf([1 2 3 4 5],Abs_RX_int_mean)
