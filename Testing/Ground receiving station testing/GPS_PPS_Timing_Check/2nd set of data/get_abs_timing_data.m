clear;clc; close all;
load data1_small_sample.mat
load data1_large_sample.mat

edge = [-50:50];

N=length(data1_small_sample);
timing_diff1 = zeros(N-1,1);
for x = 1:N-1
    diff = data1_small_sample(x+1) - data1_small_sample(x);
    timing_diff1(x,1)=diff;
end

N=length(data1_large_sample);
timing_diff2 = zeros(N-1,1);
for x = 1:N-1
    diff = data1_large_sample(x+1) - data1_large_sample(x);
    timing_diff2(x,1)=diff;
end

abs_timing_diff = abs(timing_diff2);

figure
histogram(timing_diff2,edge)
title('histogram - large sample size (9758)','fontSize',18);


total_stddev_abs = std(abs(timing_diff2))
total_mean_abs = mean(abs(timing_diff2))
total_median_abs = median(abs(timing_diff2))

[abs_H,abs_P,abs_CI] = ztest(abs(timing_diff2),total_mean_abs,total_stddev_abs,0.01)

count1 = 0;
for x = 1:length(abs_timing_diff)
    if (abs_timing_diff(x) > 3)
        count1 = count1+1;
    end
end

count2 = 0;
for x = 1:length(timing_diff2)
    if (timing_diff2(x) > 3 || timing_diff2(x) < -3)
        count2 = count2+1;
    end
end

figure
plot(timing_diff2)
title('Difference in clock cycle measurements between successive GNSS PPS pulses','fontSize',24);
ylabel('Number of clock cycles','fontSize',18)
xlabel('sample number - n','fontSize',18)
yline(3,'--r','3 clock cycles','fontSize',16,'LineWidth',1)
yline(-3,'--r','-3 clock cycles','fontSize',16,'LineWidth',1,'LabelVerticalAlignment','bottom')
xlim([0 9757])
ax=gca; ax.FontSize=16;

count3=1;

for x = 1:length(abs_timing_diff)
    if (abs_timing_diff(x) < 3)
        abs_timing_no_spike(count3)=abs_timing_diff(x);
        count3 = count3+1;
    end
end

count4=1;
for x = 1:length(timing_diff2)
    if (timing_diff2(x) < 3 && timing_diff2(x)>-3)
        timing_no_spike(count4)=timing_diff2(x);
        count4 = count4+1;
    end
end

NS_stddev_abs = std(abs(abs_timing_no_spike))
NS_mean_abs = mean(abs(abs_timing_no_spike))
NS_median_abs = median(abs(abs_timing_no_spike))
NS_variation_abs = var(abs(abs_timing_no_spike))

NS_stddev = std(timing_no_spike)
NS_mean = mean(timing_no_spike)
NS_median = median(timing_no_spike)
NS_variation = var(timing_no_spike)

[NS_H,NS_P,NS_CI] = ztest(abs(abs_timing_no_spike),NS_mean_abs,NS_stddev_abs,0.01)