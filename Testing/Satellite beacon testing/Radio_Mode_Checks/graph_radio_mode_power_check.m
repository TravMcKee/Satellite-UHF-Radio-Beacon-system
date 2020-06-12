clear; clc;
load Radio_Mode_Power_Checks

figure
plot(Times,Channel2V*1000,'lineWidth',1)
hold on; grid minor; ylim([0 130]); xlim([-112 65])
plot(Times1,Channel2V1*100,'lineWidth',1)
plot(Times2,Channel2V2*50,'lineWidth',1)
ax=gca;ax.FontSize=16;
title('LoRa radio module power mode current consumption measurements','fontSize',24);
xlabel('Time, s','fontSize',18);
ylabel('Current, mA','fontSize',18);
legend('measured using 1\Omega resistor','measured using 10\Omega resistor','measured using 20\Omega resistor','fontSize',12);
