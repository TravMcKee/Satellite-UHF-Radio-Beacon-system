clear;clc; close all;
load data1_large_sample.mat

edge = [-50:50];

N=length(data1_large_sample);
timing_diff2 = zeros(N-1,1);
for x = 1:N-1
    diff = data1_large_sample(x+1) - data1_large_sample(x);
    timing_diff2(x,1)=diff;
end

% figure
% histogram(timing_diff1,edge)
% title('histogram - small sample size (2692)','fontSize',18);
% 
% total_stddev = std(timing_diff1)
% total_mean = mean(timing_diff1)
% total_samples = length(timing_diff1)
% Z_star = 2.5758 
% low_CI = total_mean-(Z_star*(total_stddev/sqrt(total_samples)))
% high_CI = total_mean+(Z_star*(total_stddev/sqrt(total_samples)))
% 
% [H,P,CI] = ztest(timing_diff1,total_mean,total_stddev,0.01)

figure
histogram(timing_diff2,edge)
title('histogram - large sample size (9758)','fontSize',18);

total_stddev = std(timing_diff2)
total_mean = mean(timing_diff2)
total_median = median(timing_diff2)
total_samples = length(timing_diff2)
Z_star = 2.5758 
low_CI = total_mean-(Z_star*(total_stddev/sqrt(total_samples)))
high_CI = total_mean+(Z_star*(total_stddev/sqrt(total_samples)))

[H,P,CI] = ztest(timing_diff2,total_mean,total_stddev,0.05)

thres_3 = normcdf(10,total_mean,total_stddev)-normcdf(-10,total_mean,total_stddev)

count2 = 0;
for x = 1:length(timing_diff2)
    if (timing_diff2(x) > 3 || timing_diff2(x) < -3)
        count2 = count2+1;
    end
end

figure
plot(timing_diff2)
title('difference in clock cycle measurements between successive GNSS PPS pulses','fontSize',24);
ylabel('Number of clock cycles','fontSize',18)
xlabel('sample number - n','fontSize',18)
yline(3,'--r','3 clock cycles','fontSize',16,'LineWidth',1)
xlim([0 9757])
ax=gca; ax.FontSize=16;