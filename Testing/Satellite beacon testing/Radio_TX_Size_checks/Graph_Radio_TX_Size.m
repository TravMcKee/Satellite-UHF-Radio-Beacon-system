%% 5dBm graph
clear; clc;
load TX_Size_5dBm

figure
plot(Times,(Channel2V/0.47)*1000,'lineWidth',1)
hold on; ylim([0 25]); xlim([-54,18]); grid minor;
plot(Times1,(Channel2V1)*1000,'lineWidth',1)
plot(Times2,(Channel2V2/10)*1000,'lineWidth',1)
ax=gca;ax.FontSize=16;
title('LoRa module radio packet size current consumption measurements - 5dBm TX power','fontSize',24);
xlabel('Time, s','fontSize',18);
ylabel('Current, mA','fontSize',18);
legend('measured using 0.47\Omega resistor','measured using 1\Omega resistor','measured using 10\Omega resistor','fontSize',12);
%% 10dBm graph
clear; clc;
load TX_Size_10dBm

figure
plot(Times,(Channel2V/0.47)*1000,'lineWidth',1)
hold on; ylim([0 45]); xlim([-54,18]); grid minor;
plot(Times1,(Channel2V1)*1000,'lineWidth',1)
plot(Times2,(Channel2V2/10)*1000,'lineWidth',1)
ax=gca;ax.FontSize=16;
title('LoRa module radio packet size current consumption measurements - 10dBm TX power','fontSize',24);
xlabel('Time, s','fontSize',18);
ylabel('Current, mA','fontSize',18);
legend('measured using 0.47\Omega resistor','measured using 1\Omega resistor','measured using 10\Omega resistor','fontSize',12);
%% 15dBm graph
clear; clc;
load TX_Size_15dBm

figure
plot(Times,(Channel2V/0.47)*1000,'lineWidth',1)
hold on; ylim([0 75]); xlim([-54,18]); grid minor;
plot(Times1,(Channel2V1)*1000,'lineWidth',1)
plot(Times2,(Channel2V2/10)*1000,'lineWidth',1)
ax=gca;ax.FontSize=16;
title('LoRa module radio packet size current consumption measurements - 15dBm TX power','fontSize',24);
xlabel('Time, s','fontSize',18);
ylabel('Current, mA','fontSize',18);
legend('measured using 0.47\Omega resistor','measured using 1\Omega resistor','measured using 10\Omega resistor','fontSize',12);
%% 20dBm graph
clear; clc;
load TX_Size_20dBm

figure
plot(Times,(Channel2V/0.47)*1000,'lineWidth',1)
hold on; ylim([0 95]); xlim([-54,18]); grid minor;
plot(Times1,(Channel2V1)*1000,'lineWidth',1)
ax=gca;ax.FontSize=16;
title('LoRa module radio packet size current consumption measurements - 20dBm TX power','fontSize',24);
xlabel('Time, s','fontSize',18);
ylabel('Current, mA','fontSize',18);
legend('measured using 0.47\Omega resistor','measured using 1\Omega resistor','fontSize',12);