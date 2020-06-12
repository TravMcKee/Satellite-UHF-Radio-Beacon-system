clear; clc;
load total_CC

figure
plot(Times,(Channel1V-Channel2V)*1000,'lineWidth',1)
hold on; grid minor;
plot(Times1,(Channel1V1-Channel2V1)*1000,'lineWidth',1)
ax=gca;ax.FontSize=16;

title('Satelite beacon total current consumption with Radiohead 2 default settings','fontSize',24);
xlabel('Time (sec)','fontSize',18);
ylabel('Current (mA)','fontSize',18);
legend('Solar panel curent with 3 panels','Solar panel curent with 2 panels','fontSize',12);

