clear; clc;
load Radio_TX_Power_Checks

figure
plot(Times,Channel2V*1000)
hold on; xlim([-140,120]); grid minor;
plot(Times1,Channel2V1*100)
plot(Times2,Channel2V2*50)
ax=gca;ax.FontSize=16;
title('LoRa radio module transmission power current consumption measurements','fontSize',24);
xlabel('Time, s)','fontSize',18);
ylabel('Current, mA','fontSize',18);
legend('measured using 1\Omega resistor','measured using 10\Omega resistor','measured using 20\Omega resistor','fontSize',12);
