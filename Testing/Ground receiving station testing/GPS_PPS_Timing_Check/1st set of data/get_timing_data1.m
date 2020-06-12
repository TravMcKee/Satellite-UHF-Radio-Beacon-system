clear;clc;
load MC_clk_cycles_per_PPS_signal.mat

edge = [-50:50];
edge1 = [-50:0.1:50];

N=length(timingdata1);
timing_diff1 = zeros(N-1,1);
for x = 1:N-1
    diff = timingdata1(x+1) - timingdata1(x);
    timing_diff1(x,1)=diff;
end

N=length(timingdata2);
timing_diff2 = zeros(N-1,1);
for x = 1:N-1
    diff = timingdata2(x+1) - timingdata2(x);
    timing_diff2(x,1)=diff;
end

N=length(timingdata3);
timing_diff3 = zeros(N-1,1);
for x = 1:N-1
    diff = timingdata3(x+1) - timingdata3(x);
    timing_diff3(x,1)=diff;
end

total_time_diff = cat(1,timing_diff1,timing_diff2,timing_diff3);

figure
histogram(timing_diff1,edge)
figure
histogram(timing_diff2,edge)
figure
histogram(timing_diff3,edge)
figure
histogram(total_time_diff)

total_stddev = std(total_time_diff)
total_median = median(total_time_diff)
total_mean = mean(total_time_diff)
total_samples = length(total_time_diff)
Z_star = 2.5758 
low_CI = total_mean-(Z_star*(total_stddev/sqrt(total_samples)))
high_CI = total_mean+(Z_star*(total_stddev/sqrt(total_samples)))

[H,P,CI] = ztest(total_time_diff,total_mean,total_stddev,0.01)
